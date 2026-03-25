# worker-service/tasks.py

from celery import Celery
from engine_runner import run_engine
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
import os
import traceback
from datetime import datetime


DATABASE_URL = os.getenv(
    "DATABASE_URL", "postgresql://imgengine:imgengine@db:5432/imgengine"
)


engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(bind=engine)


celery = Celery(
    "worker",
    broker="redis://redis:6379/0",
    backend="redis://redis:6379/0",
)


@celery.task(
    bind=True,
    autoretry_for=(Exception,),
    retry_backoff=5,
    retry_kwargs={"max_retries": 3},
)
def process_image(self, job: dict):

    logger.info(f"Processing job: {job['job_id']}")
    logger.error(result["stderr"])

    db = SessionLocal()

    try:
        db_job = db.query(job).filter(job.id == job["job_id"]).first()

        # 🔥 MARK STARTED
        db_job.status = "processing"
        db_job.updated_at = datetime.utcnow()
        db.commit()

        # 🔥 RUN ENGINE
        result = run_engine(job)

        db_job.logs = result["stdout"]

        if result["returncode"] != 0:
            db_job.status = "failed"
            db_job.error = result["stderr"]
            db.commit()
            return

        # ✅ SUCCESS
        db_job.status = "completed"
        db_job.updated_at = datetime.utcnow()
        db.commit()

    except Exception as e:
        # 🔥 AUTO RETRY TRIGGER
        db_job.status = "retrying"
        db_job.error = traceback.format_exc()
        db.commit()

        raise self.retry(exc=e)

    finally:
        db.close()
