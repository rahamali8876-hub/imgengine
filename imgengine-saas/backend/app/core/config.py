import os

DATABASE_URL = os.getenv(
    "DATABASE_URL", "postgresql://imgengine:imgengine@db:5432/imgengine"
)
