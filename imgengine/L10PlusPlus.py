#!/usr/bin/env python3
"""
L10++ Architecture Analyzer
Kernel-Grade Static Analysis for imgengine

Enforces:
  - Layer DAG (no upward dependencies)
  - Hot path rules (no malloc, no locks, no printf)
  - ABI contracts (kernel fn signatures)
  - Zero-copy rules
  - Duplicate symbol detection
  - File/layer cycle detection
  - Build order enforcement
"""

import os
import re
import sys
from collections import defaultdict


# ============================================================
# ABI CONTRACT TABLE
#
# Single source of truth for all kernel function type ABIs.
# Any violation detected in source is a hard build error.
#
# Format:
#   type_name → (expected_signature_regex, description)
#
# DO NOT REORDER. DO NOT REMOVE. MAJOR version bump required
# before any modification to these contracts.
# ============================================================
ABI_CONTRACTS = {
    "img_kernel_fn": {
        "description": "jump table ops — (img_ctx_t*, img_buffer_t*, void*)",
        "params":      ["img_ctx_t", "img_buffer_t", "void"],
        "param_count": 3,
        "must_not_use_as": ["img_single_kernel_fn", "img_fused_kernel_fn"],
    },
    "img_single_kernel_fn": {
        "description": "fused single-image — (img_ctx_t*, img_buffer_t*) — params via ctx->fused_params",
        "params":      ["img_ctx_t", "img_buffer_t"],
        "param_count": 2,
        "must_not_use_as": ["img_kernel_fn", "img_fused_kernel_fn"],
    },
    "img_fused_kernel_fn": {
        "description": "fused batch — (img_ctx_t*, img_batch_t*, void*)",
        "params":      ["img_ctx_t", "img_batch_t", "void"],
        "param_count": 3,
        "must_not_use_as": ["img_kernel_fn", "img_single_kernel_fn"],
    },
}

# ============================================================
# TYPEDEF SIGNATURES (what the typedef must expand to)
# Checked in header files only.
# ============================================================
ABI_TYPEDEF_PATTERNS = {
    "img_kernel_fn": re.compile(
        r"typedef\s+void\s*\(\s*\*\s*img_kernel_fn\s*\)\s*"
        r"\(\s*img_ctx_t\s*\*\s*,\s*img_buffer_t\s*\*\s*,\s*void\s*\*\s*\)"
    ),
    "img_single_kernel_fn": re.compile(
        r"typedef\s+void\s*\(\s*\*\s*img_single_kernel_fn\s*\)\s*"
        r"\(\s*img_ctx_t\s*\*\s*,\s*img_buffer_t\s*\*\s*\)"
    ),
    "img_fused_kernel_fn": re.compile(
        r"typedef\s+void\s*\(\s*\*\s*img_fused_kernel_fn\s*\)\s*"
        r"\(\s*img_ctx_t\s*\*\s*,\s*img_batch_t\s*\*\s*,\s*void\s*\*\s*\)"
    ),
}

# Pattern to detect illegal casts between kernel fn types
ABI_ILLEGAL_CAST_PATTERNS = [
    # (img_single_kernel_fn)some_fused_kernel_fn_var
    (
        re.compile(r"\(\s*img_single_kernel_fn\s*\)\s*\w*(?:fused|batch)\w*"),
        "cast of batch/fused fn to img_single_kernel_fn",
    ),
    (
        re.compile(r"\(\s*img_fused_kernel_fn\s*\)\s*\w*(?:single|scalar|avx)\w*"),
        "cast of single-image fn to img_fused_kernel_fn",
    ),
    (
        re.compile(r"\(\s*img_kernel_fn\s*\)\s*\w*(?:fused|batch)\w*"),
        "cast of batch/fused fn to img_kernel_fn",
    ),
]

# Pattern: function defined with kernel-like signature, check it uses correct type
# Detects: fn(ctx, buf) where fn is declared as img_fused_kernel_fn (needs batch)
ABI_CALL_MISMATCH = re.compile(
    r"(img_fused_kernel_fn\s+\w+)\s*=.*\n.*\1\s*\(.*img_buffer_t"
)


class L10PlusPlus:
    def __init__(self):
        self.failed = False

        # Layer DAG: dependencies must only point DOWNWARD (low index → high index)
        self.layers = [
            "core",
            "memory",
            "security",
            "arch",
            "pipeline",
            "runtime",
            "plugins",
            "observability",
            "api",
            "io",
            "cmd",
        ]

        self.roots = ["include", "src", "api"]

        self.file_graph  = defaultdict(set)
        self.layer_graph = defaultdict(set)

        # ABI tracking
        self.abi_typedef_locations = defaultdict(list)  # type_name → [file]
        self.abi_violations         = []

        self.RED    = "\033[0;31m"
        self.GREEN  = "\033[0;32m"
        self.YELLOW = "\033[1;33m"
        self.CYAN   = "\033[0;36m"
        self.BOLD   = "\033[1m"
        self.NC     = "\033[0m"

    # ============================================================
    # LOGGING
    # ============================================================

    def error(self, msg):
        print(f"{self.RED}❌ ERROR:{self.NC} {msg}")
        self.failed = True

    def warn(self, msg):
        print(f"{self.YELLOW}⚠️  WARN:{self.NC} {msg}")

    def suggest(self, msg):
        print(f"{self.YELLOW}💡 FIX:{self.NC} {msg}")

    def abi_error(self, msg):
        print(f"{self.RED}🔒 ABI VIOLATION:{self.NC} {msg}")
        self.failed = True
        self.abi_violations.append(msg)

    def info(self, msg):
        print(f"{self.CYAN}ℹ️  INFO:{self.NC} {msg}")

    # ============================================================
    # PATH → LAYER
    # ============================================================

    def get_layer(self, path):
        rel = os.path.relpath(path, ".")
        for root in self.roots:
            if rel.startswith(root + os.sep):
                rel = rel[len(root) + 1:]
                break
        layer = rel.split(os.sep)[0]
        return layer if layer in self.layers else "unknown"

    def layer_index(self, layer):
        return self.layers.index(layer) if layer in self.layers else -1

    # ============================================================
    # ABI CONTRACT ENFORCEMENT
    # ============================================================

    def check_abi_typedef_correctness(self, file_path, content):
        """
        Rule: Every typedef of a kernel fn type MUST match the canonical signature.
        A wrong signature (e.g. img_fused_kernel_fn taking img_buffer_t instead of
        img_batch_t) is a hard ABI violation — it silently breaks hot path dispatch.
        """
        if not file_path.endswith(".h"):
            return

        for type_name, pattern in ABI_TYPEDEF_PATTERNS.items():
            # File contains this typedef name
            if type_name in content:
                # Check if it's a typedef declaration
                if f"typedef" in content and f"*{type_name}" in content.replace(" ", ""):
                    if not pattern.search(content.replace("\n", " ")):
                        self.abi_error(
                            f"{file_path}: typedef {type_name} has wrong signature\n"
                            f"          Expected: {ABI_CONTRACTS[type_name]['description']}"
                        )
                        self.suggest(
                            f"Correct signature:\n"
                            f"  img_kernel_fn:        typedef void (*img_kernel_fn)(img_ctx_t *, img_buffer_t *, void *);\n"
                            f"  img_single_kernel_fn: typedef void (*img_single_kernel_fn)(img_ctx_t *, img_buffer_t *);\n"
                            f"  img_fused_kernel_fn:  typedef void (*img_fused_kernel_fn)(img_ctx_t *, img_batch_t *, void *);"
                        )
                    else:
                        self.abi_typedef_locations[type_name].append(file_path)

    def check_abi_illegal_casts(self, file_path, content):
        """
        Rule: Never cast between kernel fn types.
        These types are structurally incompatible — a cast hides a real ABI bug.
        The compiler may not warn on fn pointer casts even when args are wrong.
        """
        for pattern, description in ABI_ILLEGAL_CAST_PATTERNS:
            if pattern.search(content):
                self.abi_error(
                    f"{file_path}: illegal kernel fn cast — {description}"
                )
                self.suggest(
                    "Kernel fn types are ABI-incompatible. Never cast between:\n"
                    "  img_kernel_fn / img_single_kernel_fn / img_fused_kernel_fn\n"
                    "Declare the correct type at the call site instead."
                )

    def check_abi_call_sites(self, file_path, content):
        """
        Rule: img_fused_kernel_fn must be called with (ctx, batch, params).
              img_single_kernel_fn must be called with (ctx, buf).
              img_kernel_fn must be called with (ctx, buf, params).

        Detects the most common mistake: calling a batch fn with a single buffer.
        Pattern: variable declared as img_fused_kernel_fn, then called as fn(ctx, buf)
        """
        # Find all variables declared as a kernel fn type
        for type_name, contract in ABI_CONTRACTS.items():
            # Match: img_fused_kernel_fn fn = ...; then fn(ctx, buf[^,)] — missing args
            decl_pattern = re.compile(
                rf"{re.escape(type_name)}\s+(\w+)\s*[=;]"
            )
            for m in decl_pattern.finditer(content):
                var_name = m.group(1)
                expected_argc = contract["param_count"]

                # Find all call sites of this variable
                call_pattern = re.compile(
                    rf"\b{re.escape(var_name)}\s*\(([^;{{}}]*)\)"
                )
                for call in call_pattern.finditer(content):
                    args_str = call.group(1).strip()
                    if not args_str:
                        actual_argc = 0
                    else:
                        # rough arg count (handles nested parens poorly but good enough)
                        actual_argc = args_str.count(",") + 1

                    if actual_argc != expected_argc:
                        self.abi_error(
                            f"{file_path}: {type_name} '{var_name}' called with "
                            f"{actual_argc} arg(s), expected {expected_argc}\n"
                            f"          ABI: {contract['description']}"
                        )
                        self.suggest(
                            f"Check call site: {var_name}({args_str})\n"
                            f"Expected args:   {contract['params']}"
                        )

    def check_abi_typedef_uniqueness(self):
        """
        Rule: Each kernel fn typedef must be defined in exactly ONE header.
        Multiple definitions risk divergence — one file gets the wrong signature.
        """
        for type_name, files in self.abi_typedef_locations.items():
            if len(files) > 1:
                self.abi_error(
                    f"typedef {type_name} defined in multiple headers: {files}\n"
                    f"          Single source of truth: include/pipeline/fused_kernel.h"
                )
                self.suggest(
                    f"Remove typedef {type_name} from all files except fused_kernel.h.\n"
                    "Other headers must use a forward declaration or include fused_kernel.h."
                )

    def print_abi_contract_table(self):
        """Print the ABI contract table at startup — visible in CI logs."""
        print(f"\n{self.BOLD}{'=' * 60}{self.NC}")
        print(f"{self.BOLD}  ABI CONTRACT TABLE (DO NOT BREAK){self.NC}")
        print(f"{self.BOLD}{'=' * 60}{self.NC}")
        for type_name, contract in ABI_CONTRACTS.items():
            print(f"  {self.CYAN}{type_name}{self.NC}")
            print(f"    → {contract['description']}")
        print(f"{self.BOLD}{'=' * 60}{self.NC}\n")

    # ============================================================
    # EXISTING RULES
    # ============================================================

    def check_layer_violation(self, file_path, layer, layer_idx, inc):
        inc_layer = inc.split("/")[0]
        inc_idx   = self.layer_index(inc_layer)
        if inc_idx == -1:
            return
        self.layer_graph[layer].add(inc_layer)
        if inc_idx > layer_idx:
            self.error(f"{file_path} → {inc} (layer violation)")
            self.suggest("Move dependency DOWN or introduce interface abstraction")

    def check_hot_path(self, file_path, content):
        if "hot" not in file_path:
            return
        if re.search(r"\b(malloc|calloc|realloc)\b", content):
            self.error(f"{file_path}: malloc/calloc/realloc in HOT path")
            self.suggest("Use img_slab_alloc() or img_arena_alloc()")
        if "printf" in content:
            self.error(f"{file_path}: printf in HOT path")
            self.suggest("Use IMG_LOG_* macros (lock-free ring buffer)")
        if "pthread_mutex" in content:
            self.error(f"{file_path}: pthread_mutex in HOT path")
            self.suggest("Use lock-free atomics (__atomic_* or _Atomic)")

    def check_security(self, file_path, content):
        if "api" in file_path and file_path.endswith(".c"):
            if "img_security_validate_request" not in content:
                self.error(f"{file_path}: missing input validation at API entry")
                self.suggest("Call img_security_validate_request() before processing")

    def check_zero_copy(self, file_path, content):
        if "plugins" in file_path:
            if "memcpy" in content:
                self.warn(f"{file_path}: memcpy detected in plugin")
                self.suggest(
                    "Ensure zero-copy (operate in-place on slab buffer). "
                    "If memcpy is intentional, add /* zero-copy-exempt: reason */ comment."
                )

    def check_lock_free(self, file_path, content):
        if "queue" in file_path:
            if "pthread_mutex" in content:
                self.error(f"{file_path}: pthread_mutex in lock-free queue")
                self.suggest("Use _Atomic / __atomic_* operations only")

    def check_type_ownership(self, file_path, content):
        if "struct img_buffer" in content and "buffer.h" not in file_path:
            self.error(f"{file_path}: illegal 'struct img_buffer' definition outside buffer.h")
            self.suggest("Move to include/core/buffer.h — single source of truth")

        if "img_pipeline_desc_t" in content:
            has_include = (
                '#include "pipeline/pipeline_types.h"' in content or
                '#include "core/pipeline/pipeline_types.h"' in content
            )
            # Only enforce in .c files — .h files may forward-declare
            if file_path.endswith(".c") and not has_include:
                self.error(
                    f"{file_path}: uses img_pipeline_desc_t but missing "
                    "#include \"pipeline/pipeline_types.h\""
                )

    def check_slab_usage(self, file_path, content):
        if "img_slab_free(" in content:
            if not re.search(r"img_slab_free\s*\(\s*[^,]+,\s*[^)]+\)", content):
                self.error(f"{file_path}: invalid img_slab_free() call signature")
                self.suggest("Correct usage: img_slab_free(pool, ptr)")

    # ============================================================
    # SCAN FILE
    # ============================================================

    def scan_file(self, file_path):
        try:
            with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()
        except Exception:
            return

        layer     = self.get_layer(file_path)
        layer_idx = self.layer_index(layer)
        includes  = re.findall(r'#include\s+"(.+?)"', content)

        for inc in includes:
            self.file_graph[file_path].add(inc)
            self.check_layer_violation(file_path, layer, layer_idx, inc)

        # Architecture rules
        self.check_hot_path(file_path, content)
        self.check_security(file_path, content)
        self.check_zero_copy(file_path, content)
        self.check_lock_free(file_path, content)
        self.check_type_ownership(file_path, content)
        self.check_slab_usage(file_path, content)

        # ABI contract enforcement
        self.check_abi_typedef_correctness(file_path, content)
        self.check_abi_illegal_casts(file_path, content)
        self.check_abi_call_sites(file_path, content)

    # ============================================================
    # DUPLICATE SYMBOL DETECTOR
    # ============================================================

    def detect_duplicate_symbols(self):
        symbols = defaultdict(list)

        for file in list(self.file_graph.keys()):
            try:
                with open(file, "r", encoding="utf-8", errors="ignore") as f:
                    content = f.read()
            except Exception:
                continue

            # Match non-static function definitions (static is file-scoped, OK to duplicate)
            funcs = re.findall(
                r"^(?!static\s)(?:void|int|uint\w*|size_t|img_\w+)\s+(\w+)\s*\(",
                content,
                re.MULTILINE,
            )
            for fn in funcs:
                symbols[fn].append(file)

        for fn, files in symbols.items():
            if len(files) > 1:
                self.error(f"Duplicate non-static symbol: '{fn}' in {files}")
                self.suggest(
                    f"Make '{fn}' static if file-local, "
                    "or move shared definition to a single .c file"
                )

    # ============================================================
    # CYCLE DETECTION
    # ============================================================

    def detect_file_cycles(self):
        visited = set()
        stack   = set()

        def dfs(node, path):
            if node in stack:
                self.error(f"FILE CYCLE: {' -> '.join(path + [node])}")
                self.suggest("Break cycle using interface/forward-declaration split")
                return
            if node in visited:
                return
            visited.add(node)
            stack.add(node)
            for neighbor in self.file_graph.get(node, []):
                dfs(neighbor, path + [node])
            stack.remove(node)

        for node in list(self.file_graph.keys()):
            dfs(node, [])

    def detect_layer_cycles(self):
        visited = set()
        stack   = set()

        def dfs(node):
            if node in stack:
                self.error(f"LAYER CYCLE DETECTED at '{node}'")
                return
            if node in visited:
                return
            visited.add(node)
            stack.add(node)
            for neighbor in self.layer_graph.get(node, []):
                dfs(neighbor)
            stack.remove(node)

        for node in list(self.layer_graph.keys()):
            dfs(node)

    # ============================================================
    # BUILD ORDER ENFORCEMENT
    # ============================================================

    def enforce_build_order(self):
        for src, targets in self.layer_graph.items():
            for tgt in targets:
                if self.layer_index(src) > self.layer_index(tgt):
                    self.error(
                        f"Build order violation: '{src}' (idx {self.layer_index(src)}) "
                        f"depends on '{tgt}' (idx {self.layer_index(tgt)})"
                    )
                    self.suggest(
                        "Layer dependency must point downward only:\n"
                        f"  {' → '.join(self.layers)}"
                    )

    # ============================================================
    # GRAPH EXPORT
    # ============================================================

    def export_graph(self):
        with open("dependency_graph.dot", "w") as f:
            f.write("digraph G {\n")
            f.write('  rankdir=LR;\n')
            f.write('  node [shape=box fontname="monospace"];\n')
            for src, targets in self.file_graph.items():
                for tgt in targets:
                    f.write(f'  "{src}" -> "{tgt}";\n')
            f.write("}\n")
        os.system("dot -Tpng dependency_graph.dot -o dependency_graph.png 2>/dev/null")

    def export_layer_graph(self):
        with open("layer_graph.dot", "w") as f:
            f.write("digraph L {\n")
            f.write('  rankdir=TB;\n')
            f.write('  node [shape=ellipse fontname="monospace" style=filled fillcolor=lightyellow];\n')
            for src, targets in self.layer_graph.items():
                for tgt in targets:
                    f.write(f'  "{src}" -> "{tgt}";\n')
            f.write("}\n")
        os.system("dot -Tpng layer_graph.dot -o layer_graph.png 2>/dev/null")

    # ============================================================
    # ABI SUMMARY REPORT
    # ============================================================

    def print_abi_summary(self):
        print(f"\n{self.BOLD}ABI TYPEDEF LOCATIONS{self.NC}")
        for type_name in ABI_CONTRACTS:
            files = self.abi_typedef_locations.get(type_name, [])
            if files:
                print(f"  {self.CYAN}{type_name}{self.NC} → {files[0]}")
            else:
                self.warn(f"typedef {type_name} not found in any scanned header")
                self.suggest(
                    f"Define typedef {type_name} in include/pipeline/fused_kernel.h"
                )

        # Uniqueness check runs after all files scanned
        self.check_abi_typedef_uniqueness()

        if self.abi_violations:
            print(f"\n{self.RED}ABI VIOLATIONS SUMMARY ({len(self.abi_violations)} total):{self.NC}")
            for i, v in enumerate(self.abi_violations, 1):
                print(f"  [{i}] {v}")

    # ============================================================
    # RUN
    # ============================================================

    def run(self):
        self.print_abi_contract_table()

        print("🔍 L10++ ARCHITECTURE ANALYZER STARTING...\n")

        for root, dirs, files in os.walk("."):
            dirs[:] = [d for d in dirs if d not in ("build", ".git", "__pycache__")]
            for file in files:
                if file.endswith((".c", ".h")):
                    self.scan_file(os.path.join(root, file))

        print("\n📊 Generating dependency graphs...")
        self.export_graph()
        self.export_layer_graph()

        print("\n🔍 Checking file cycles...")
        self.detect_file_cycles()

        print("\n🔍 Checking layer cycles...")
        self.detect_layer_cycles()

        print("\n🔍 Checking duplicate symbols...")
        self.detect_duplicate_symbols()

        print("\n🔍 Enforcing build order...")
        self.enforce_build_order()

        print("\n🔒 ABI contract summary...")
        self.print_abi_summary()

        print("\n" + "=" * 60)

        if self.failed:
            print(f"{self.RED}{self.BOLD}🚨 BUILD BLOCKED — L10++ VIOLATIONS DETECTED{self.NC}")
            print(f"{self.RED}   Fix all errors above before merging.{self.NC}")
            sys.exit(1)
        else:
            print(f"{self.GREEN}{self.BOLD}✅ ARCHITECTURE CLEAN — L10++ PASSED{self.NC}")
            sys.exit(0)


if __name__ == "__main__":
    L10PlusPlus().run()