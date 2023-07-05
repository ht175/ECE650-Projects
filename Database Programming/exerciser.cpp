#include "exerciser.h"

void exercise(connection *C)
{
    // void query1(connection *C,int use_mpg, int min_mpg, int max_mpg,
    // int use_ppg, int min_ppg, int max_ppg,
    //  int use_rpg, int min_rpg, int max_rpg,
    //  int use_apg, int min_apg, int max_apg,
    //  int use_spg, double min_spg, double max_spg,
    //  int use_bpg, double min_bpg, double max_bpg);
    //mpg ppg rpg apg
    //  cout<< "query 1 *******************" << endl;
     query1(C, 1, 35, 40, 0, 5, 19, 0, 0, 20, 0, 2, 4, 0, 0.8, 1.6, 0, 0.4, 3.5);
    //  cout<< "query 2 *******************" << endl;
     query2(C, "Green");
    //  cout<< "query 3 *******************" << endl;
     query3(C, "Duke");
    //  cout<< "query 4 *******************" << endl;
     query4(C, "KY", "LightBlue");
    // 
     query5(C, 6);
    
}
