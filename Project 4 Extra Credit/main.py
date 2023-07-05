from sqlalchemy import create_engine
from sqlalchemy.orm import Session, sessionmaker
from query import *


if __name__ == "__main__":
    engine = create_engine("postgresql+psycopg2://postgres:passw0rd@0.0.0.0:5432/ACC_BBALL", future=True)
    DBSession = sessionmaker(bind=engine)
    session = DBSession()
    query1(session, 1, 35, 40, 1, 5, 19, 1, 0, 20, 1, 2, 4, 1, 0.8, 1.6, 1, 0.4, 3.5)
    query2(session, "Green")
    query3(session, "Duke")
    query4(session, "KY", "LightBlue")
    query5(session, 6)