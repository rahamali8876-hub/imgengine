from celery import Celery
from app.services.engine_runner import run_engine
from app.core.db import SessionLocal
from app.models.job import Job


celery = Celery(
    "imgengine",
    broker="redis://redis:6379/0",
    backend="redis://redis:6379/0",
)


@celery.task(bind=True)
def process_image(self, job: dict):
    db = SessionLocal()

    db_job = db.query(Job).filter(Job.input == job["input"]).first()

    try:
        result = run_engine(job)

        if result["returncode"] != 0:
            db_job.status = "failed"
            db.commit()
            return

        db_job.status = "completed"
        db.commit()

    finally:
        db.close()
