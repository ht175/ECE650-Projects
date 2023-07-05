from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, String, Integer, ForeignKey, Double
Base = declarative_base()
class State(Base):
    __tablename__ = 'state'
    state_id = Column(Integer, primary_key=True)
    name = Column(String(30))
class Color(Base):
    __tablename__ = 'color'
    color_id = Column(Integer, primary_key=True)
    name = Column(String(30))
class Team(Base):
    __tablename__ = 'team'
    team_id = Column(Integer, primary_key=True)
    name = Column(String(30))
    state_id = Column(Integer,  ForeignKey('state.state_id', ondelete="SET NULL",onupdate="CASCADE"))
    color_id = Column(Integer,  ForeignKey('color.color_id', ondelete="SET NULL",onupdate="CASCADE"))
    wins = Column(Integer)
    losses = Column(Integer)

class Player(Base):
     __tablename__ = 'player'
     player_id = Column(Integer, primary_key=True)
     team_id = Column(Integer,   ForeignKey('team.team_id', ondelete="SET NULL",onupdate="CASCADE"))
     uniform_num = Column(Integer)
     first_name = Column(String(30))
     last_name = Column(String(30))
     mpg = Column(Integer)
     ppg = Column(Integer)
     rpg = Column(Integer)
     apg = Column(Integer)
     spg = Column(Double)
     bpg = Column(Double)




