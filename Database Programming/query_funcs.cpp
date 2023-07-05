#include "query_funcs.h"
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision

void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
    stringstream s;
    work W(*C);
    s << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG,"
         "RPG, APG, SPG, BPG) VALUES ("
      << team_id << ", " << jersey_num << ", " << W.quote(first_name) << ", " << W.quote(last_name) << ", "
      << mpg << ", " << ppg << ", " << rpg << ", " << apg << ", "
      << spg << ", " << bpg << " );";
    
    /* Execute SQL query */
    W.exec(s.str());
    W.commit();
}

void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses)
{
    stringstream s;
    work W(*C);
    //cout<< name << color_id;
    s << "INSERT INTO TEAM ( NAME, STATE_ID, COLOR_ID, WINS, LOSSES)"
         "VALUES ("
      << W.quote(name) << ", " << state_id << ", " << color_id << ", " << wins << ", "
      << losses << " );";
   // cout << s.str() << endl;
    /* Execute SQL query */
    W.exec(s.str());
    W.commit();
}

void add_state(connection *C, string name)
{
    stringstream s;
    work W(*C);
    s << "INSERT INTO STATE (NAME) "
         "VALUES ("
      << W.quote(name) << ");";
    
    /* Execute SQL query */
    W.exec(s.str());
    W.commit();
}

void add_color(connection *C, string name)
{
    stringstream s;
    work W(*C);
    //cout << name <<endl;
    s << "INSERT INTO COLOR (NAME )"
         "VALUES ("
      << W.quote(name) << " );";
    //cout << s.str() << endl;
    /* Execute SQL query */
    W.exec(s.str());
    W.commit();
}

/*
 * All use_ params are used as flags for corresponding attributes
 * a 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
 * a 0 for a use_ param means this attribute is disabled
 */

// how all attributes of each player with average statistics that fall between the
// min and max (inclusive) for each enabled statistic

void query1(connection *C,
            int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg)
{
    stringstream s;
    s << "SELECT * FROM PLAYER";
    int enabled_num = 0;
    int flags[6] = {use_mpg, use_ppg, use_rpg, use_apg, use_spg, use_bpg};
    int min_int[4] = {min_mpg, min_ppg, min_rpg, min_apg};
    int max_int[4] = {max_mpg, max_ppg, max_rpg, max_apg};
    double min_double[2] = {min_spg, min_bpg};
    double max_double[2] = {max_spg, max_bpg};
    string names[6] = {"MPG", "PPG", "RPG", "APG", "SPG", "BPG"};
    for (int i = 0; i < 6; i++)
    {
        if (flags[i] == 1)
        {
            if (enabled_num == 0)
            {
                s << " WHERE ";
                if (i < 4)
                {
                    s << names[i] << " BETWEEN " << min_int[i] << " AND " << max_int[i];
                }
                else
                {
                    // 4-4=0 5-4=1
                    s << names[i] << " BETWEEN " << min_double[i - 4] << " AND " << max_double[i - 4];
                }

                enabled_num++;
            }
            else
            {
                if (i < 4)
                {
                    s << " AND " << names[i] << " BETWEEN " << min_int[i] << " AND " << max_int[i];
                }
                else
                {
                    s << " AND " << names[i] << " BETWEEN " << min_double[i - 4] << " AND " << max_double[i - 4];
                }
            }
        }
    }
    s << ";";
    /* Create a non-transactional object. */
    nontransaction N(*C);
    /* Execute SQL query */
    result R(N.exec(s.str()));
    //show result
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
      cout << c[0].as<int>() << " " << c[1].as<int>() << " " << c[2].as<int>() << " " 
      << c[3].as<string>() << " "<<  c[4].as<string>() << " "
      << c[5].as<int>() << " " << c[6].as<int>() << " " << c[7].as<int>() << " " << c[8].as<int>() << " "
      <<fixed << setprecision(1)<< c[9].as<double>() << " " << c[10].as<double>() << " " << endl;
    }
}

//query2(): show the name of each team with the indicated uniform color
void query2(connection *C, string team_color)
{
    stringstream s;
    work W(*C);
    s << "SELECT TEAM.NAME FROM TEAM, COLOR";
    s << " WHERE TEAM.COLOR_ID = COLOR.COLOR_ID AND COLOR.NAME = ";
    s << W.quote(team_color) << ";";
    //cout << s.str() << endl;
    W.commit();
    nontransaction N(*C);
    /* Execute SQL query */
    result R(N.exec(s.str()));
    cout << "NAME" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << endl;
    }
}
//show the first and last name of each player that plays for the indicated team,
//ordered from highest to lowest ppg (points per game)
void query3(connection *C, string team_name)
{
    stringstream s;
    work W(*C);
    s << "SELECT FIRST_NAME, LAST_NAME FROM PLAYER, TEAM";
    s << " WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.NAME = ";
    s << W.quote(team_name);
    s << " ORDER BY PPG DESC;";
    W.commit();
    nontransaction N(*C);
    /* Execute SQL query */
    result R(N.exec(s.str()));
    cout << "FIRST_NAME LAST_NAME" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << " " << c[1].as<string>() << endl;
    }
}
//show how uniform number, first name and last name of each player that plays in
//the indicated state and wears the indicated uniform color
void query4(connection *C, string team_state, string team_color)
{
    stringstream s;
    work W(*C);
    s << "SELECT UNIFORM_NUM, FIRST_NAME, LAST_NAME FROM PLAYER, STATE, COLOR, TEAM";
    s << " WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.STATE_ID = STATE.STATE_ID AND TEAM.COLOR_ID = COLOR.COLOR_ID";
    s << " AND STATE.NAME = " << W.quote(team_state) << " AND " << "COLOR.NAME = " << W.quote(team_color) << ";" ;
    W.commit();
    nontransaction N(*C);
    /* Execute SQL query */
    result R(N.exec(s.str()));
    cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<int>() << " " << c[1].as<string>() << " " << c[2].as<string>() << endl;
    }

}
//show first name and last name of each player, and team name and number of
//wins for each team that has won more than the indicated number of games
void query5(connection *C, int num_wins)
{
    stringstream s;
    work W(*C);
    s << "SELECT FIRST_NAME, LAST_NAME, NAME, WINS  FROM PLAYER, TEAM";
    s << " WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID ";
    s << " AND WINS > " << W.quote(num_wins) << ";" ;
    W.commit();
    nontransaction N(*C);
    /* Execute SQL query */
    result R(N.exec(s.str()));
    cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << " " << c[1].as<string>() << " " << c[2].as<string>() <<  " " << c[3].as<int>()  << endl;
    }
}
