# base image
FROM python:3.11-slim

# install uv
RUN pip install uv

# set working dir
WORKDIR /app

# copy dependency files (cache optimized)
COPY pyproject.toml uv.lock ./

# install deps
RUN uv sync --frozen

# copy app code
COPY . .

# copy binary (FIXED: must be inside context)
COPY imgengine_cli /usr/local/bin/imgengine_cli
RUN chmod +x /usr/local/bin/imgengine_cli

# run server
CMD ["uv", "run", "uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]