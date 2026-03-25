# backend/app/main.py


from fastapi import FastAPI
from app.api.routes.generate import router as generate_router
from app.core.db import Base, engine

from app.core.limiter import limiter


Base.metadata.create_all(bind=engine)

app = FastAPI()

app.include_router(generate_router)

app.state.limiter = limiter
