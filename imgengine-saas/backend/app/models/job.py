# backend/models/job.py

from sqlalchemy import Column, String, DateTime
from datetime import datetime
from app.core.db import Base


class Job(Base):
    __tablename__ = "jobs"

    id = Column(String, primary_key=True, index=True)
    input = Column(String)
    output = Column(String)

    status = Column(String, default="queued")
    created_at = Column(DateTime, default=datetime.utcnow)
