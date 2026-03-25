### DEBUGGING

### ImageEngine SaaS: Principal Engineering Roadmap

    This document defines the "Gold Standard" for our request lifecycle, observability, and error-prevention strategy.

## 1. The "Request Pipeline" (A to Z Lifecycle)

    Every request follows this immutable flow to ensure security and traceability.
    Phase Component Responsibility Failure Outcome
    Ingress Nginx / Gateway SSL Termination, Request ID generation. 504 Gateway Timeout
    Identify security.py API Key validation, User context injection. 403 Forbidden
    Throttle limiter.py Rate limiting per User/IP. 429 Too Many Requests
    Validate schemas/*.py Pydantic strict type checking. 422 Validation Error
    Enqueue generate.py DB record creation + Redis task push. 500 Internal Server Error
    Execute tasks.py Background processing (subprocess call). Task Retry / Dead Letter
    Notify Webhooks/Poll Update DB status to completed. Status: failed

## 2. The Three Pillars of Observability (The "Self-Healing" Tech)

    To resolve issues without talking to customers, we must implement:
    A. Structured Logging (JSON)
    Standard: No more plain text. Every log must be a JSON object.
    Context: Every log in the Worker must carry the job_id and user_id.
    Goal: Searchable logs in ELK (Elasticsearch/Logstash/Kibana) or Grafana Loki.
    B. Distributed Tracing (OpenTelemetry)
    Standard: Every request generates a TraceID.
    The Chain: API (TraceID: 123) ?? Redis ?? Worker (TraceID: 123).
    Goal: See the "Gantt Chart" of a single request across different containers.
    C. Automated Error Tracking (Sentry)
    Standard: All 500 errors and Worker Exceptions are sent to Sentry.
    Context: Sentry captures the "Snapshot" (local variables) at the exact moment of the crash.
    Goal: Fix bugs without ever asking the user "What did you do?".

## 3. Resilience Strategy (The "Never Face it Again" Clause)

    Dead Letter Queues (DLQ)
    If a job fails 3 times, it is moved to a failed_jobs queue. We inspect these manually, fix the code, and re-play them. The customer sees a delay, not a failure.
    Health Checks (/health)
    Each service must have a /health endpoint that checks:
    Can I talk to the DB?
    Can I talk to Redis?
    Is the Disk full?
    Regression Testing (CI/CD)
    When a bug is fixed (like the KeyError: 'cols'), a Pytest must be written. The GitHub Action will block any future code that breaks that specific logic again.

## 4. The Implementation Order (Next Steps)

    [ ] Global Exception Handler: Catch all Python errors and return a clean JSON response.
    [ ] Sentry Integration: Connect API and Worker to an error-tracking dashboard.
    [ ] Structured Logger: Replace standard logging with structlog.
    [ ] Correlation IDs: Pass request_id from FastAPI to Celery.
    [ ] Health Monitor: Create a dashboard to see CPU/RAM/Redis status.
