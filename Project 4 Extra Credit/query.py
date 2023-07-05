from sqlalchemy.orm import Session, query
from orm import Color, State, Team, Player
def query1(session, use_mpg, min_mpg, max_mpg, use_ppg, min_ppg, max_ppg, use_rpg, min_rpg, max_rpg, use_apg, min_apg, max_apg, use_spg, 
            min_spg, max_spg, use_bpg, min_bpg, max_bpg):
    res = session.query(Player.player_id,Player.team_id,Player.uniform_num, Player.first_name, Player.last_name, Player.mpg, Player.ppg, Player.rpg, Player.apg, Player.spg, Player.bpg)
    min = [min_mpg, min_ppg, min_rpg, min_apg, min_spg, min_bpg]
    max = [max_mpg, max_ppg, max_rpg, max_apg,max_spg, max_bpg]
    names = ["mpg", "ppg", "rpg", "apg", "spg", "bpg"]
    flags = [use_mpg, use_ppg, use_rpg, use_apg, use_spg, use_bpg]
    for i in range(6):
        if(flags[i]==1):
            res=res.filter(getattr(Player, names[i]) >= min[i], getattr(Player, names[i]) <= max[i])
    
    ans = res.all()
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    for i in ans:
        for j in range(10):
            print(i[j], end=" ")
        print(i[10])
    

def query2(session, team_color):
    res = session.query(Team.name).filter(Team.color_id == Color.color_id).filter(Color.name == team_color)
    ans = res.all()
    print("NAME")
    for i in ans:
        print(i[0])

def query3(session, team_name):
    res = session.query(Player.first_name, Player.last_name).filter(Player.team_id == Team.team_id ).filter(Team.name == team_name)
    ans = res.all()
    print("FIRST_NAME LAST_NAME")
    for i in ans:
         print(i[0]," ",i[1])

def query4(session, team_state, team_color):
    # PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.STATE_ID = STATE.STATE_ID AND TEAM.COLOR_ID = COLOR.COLOR_ID";
    #s << " AND STATE.NAME = " << W.quote(team_state) << " AND " << "COLOR.NAME = " << W.quote(team_color) << ";" ;
    res = session.query(Player.uniform_num ,Player.first_name, Player.last_name).filter(Player.team_id == Team.team_id ).filter(Team.state_id == State.state_id).filter(Team.color_id==Color.color_id)
    res = res.filter(State.name == team_state ).filter(Color.name == team_color)
    ans = res.all()
    print("UNIFORM_NUM FIRST_NAME LAST_NAME")
    for i in ans:
         print(i[0]," ",i[1]," ",i[2])

def query5(session, num_wins):
    res = session.query(Player.first_name, Player.last_name, Team.name, Team.wins).filter(Player.team_id== Team.team_id).filter(Team.wins>num_wins)
    ans = res.all()
    print("FIRST_NAME LAST_NAME NAME WINS")
    for i in ans:
        print(i[0]," ",i[1]," ",i[2], "", i[3])

