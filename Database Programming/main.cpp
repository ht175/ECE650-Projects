#include <iostream>
#include <pqxx/pqxx>
#include "query_funcs.h"

#include "exerciser.h"

using namespace std;
using namespace pqxx;

void dropTables(connection *C){
  string s;
  s = "DROP TABLE IF EXISTS PLAYER CASCADE; "  \
      "DROP TABLE IF EXISTS TEAM CASCADE; " \
      "DROP TABLE IF EXISTS STATE CASCADE; "  \
      "DROP TABLE IF EXISTS COLOR CASCADE; ";
  work W(*C);
    /* Execute SQL query */
  W.exec(s);
  W.commit();

}

void createTables(connection *C)
{
  ifstream file("createTable.sql");
  string line, wholeString;

  if (file.is_open())
  {
    while (std::getline(file, line))
    {
      wholeString += line;
    }
    file.close();
  }
  /* Create a transactional object. */
  work W(*C);
  /* Execute SQL query */
  W.exec(wholeString);
  W.commit();
}

void insertPlayer(connection *C)
{
  ifstream file("player.txt");
  string line;
  if (file.is_open())
  {
    while (std::getline(file, line))
    {
      istringstream ss(line);
      int PLAYER_ID, TEAM_ID, UNIFORM_NUM, MPG, PPG, RPG, APG;
      string FIRST_NAME, LAST_NAME;
      double SPG, BPG;
      ss >> PLAYER_ID >> TEAM_ID >> UNIFORM_NUM >> FIRST_NAME >> LAST_NAME >> MPG >> PPG >> RPG >> APG >> SPG >> BPG;
      add_player(C, TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG);
    }
    file.close();
  }
}

void insertTeam(connection *C)
{
  ifstream file("team.txt");
  string line;
  if (file.is_open())
  {
    while (std::getline(file, line))
    {
      istringstream ss(line);
      int TEAM_ID, STATE_ID, COLOR_ID, WINS, LOSSES;
      string NAME;
      ss >> TEAM_ID >> NAME >> STATE_ID >> COLOR_ID >> WINS >> LOSSES;
      add_team(C, NAME, STATE_ID, COLOR_ID, WINS, LOSSES);
    }
    file.close();
  }
}

void insertState(connection *C)
{
  ifstream file("state.txt");
  string line;
  if (file.is_open())
  {
    while (std::getline(file, line))
    {
      istringstream ss(line);
      int STATE_ID;
      string NAME;
      ss >> STATE_ID >> NAME;
      add_state(C, NAME);
    }
    file.close();
  }
}

void insertColor(connection *C)
{
  ifstream file("color.txt");
  string line;
  if (file.is_open())
  {
    while (std::getline(file, line))
    {
      istringstream ss(line);
      int COLOR_ID;
      string NAME;
      ss >> COLOR_ID >> NAME;
      add_color(C, NAME);
    }
    file.close();
  }
}

int main(int argc, char *argv[])
{

  // Allocate & initialize a Postgres connection object
  connection *C;

  try
  {
    // Establish a connection to the database
    // Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open())
    {
      //cout << "Opened database successfully: " << C->dbname() << endl;
    }
    else
    {
      cout << "Can't open database" << endl;
      return 1;
    }
  }
  catch (const std::exception &e)
  {
    cerr << e.what() << std::endl;
    return 1;
  }
  //drop tables that may already exist (e.g. from prior runs of your test) before creating an initializing the tables in the program
  dropTables(C);
  // TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  createTables(C);
  //      load each table with rows from the provided source txt files
  insertColor(C);
  insertState(C);
  insertTeam(C);
  insertPlayer(C);


  exercise(C);

  // Close database connection
  C->disconnect();

  return 0;
}
