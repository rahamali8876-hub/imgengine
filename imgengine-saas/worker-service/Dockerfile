FROM python:3.11-slim

WORKDIR /app

RUN pip install celery redis

COPY tasks.py .
COPY engine_runner.py .
COPY imgengine_cli /usr/local/bin/imgengine_cli

RUN chmod +x /usr/local/bin/imgengine_cli

CMD ["celery", "-A", "tasks", "worker", "--loglevel=info"]