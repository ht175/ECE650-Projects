// #include "ringmaster.hpp"
#include "function.hpp"
#include "potato.hpp"

#include <map>
#include <algorithm> 

int main(int argc, char *argv[]){
    //error detection: ringmaster <port_num> <num_players> <num_hops>
    if(argc != 4){
        cerr << "command line parameter number does not match expectaion" << endl;
        return EXIT_FAILURE;
    }
    //parse the command line
    const char * port = argv[1];
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);
    potato_t potato;
    potato.hops = num_hops;
    potato.count = 0;
    // potato.trace= std::vector<int>();
    memset(potato.trace, 0, sizeof(potato.trace)); 
    //only 1 player cannot play game
    if(num_players==1){
        return EXIT_SUCCESS;
    }

    //build a ringmaster sever to listen
    int ringmaster_fd = server_bind_listen(port);
    //print initial information
    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << num_players << endl;
    cout << "Hops = " << num_hops  << endl;
    //creat socket to wait for connect with each players
    vector<int> ringmaster_connection_players;
    int id = 0;
    std::map<int,int> port_map;
    std::map<int,string> ip_map;
    for(int i = 0; i < num_players; i++){
        int player_port;
        string player_ip;
        string temp_port;
        int connection_fd = sever_accept(ringmaster_fd, &player_ip, &temp_port);
        //send id to each player
        send(connection_fd, &id , sizeof(id), 0);
        //send total number of players to each player, they will use to print relvant info
        send(connection_fd, &num_players, sizeof(num_players), 0);
        //recive their player port and store it in order to send neighbor info
        recv(connection_fd, &player_port, sizeof(player_port), 0);
        cout << "Player " << id  << " is ready to play" << endl;
        //store each players fd, port, and ip in order for later use
        //eg: send neighbor info including port and ip to player
        ringmaster_connection_players.push_back(connection_fd);
        port_map[id] = player_port;
        ip_map[id] = player_ip;
        id++;
    }

    //send neighbor info to each players
    for(int i = 0; i < num_players; i++){
        //because it is a ring, so we will just send player' next id to each player
        //id 0 -> get id 1's port and host name
        //id num_Players-1 get id 0' port and host name
        int neighbor_id = i == num_players-1? 0 : i+1;
        int player_port = port_map[neighbor_id];
        char player_ip[100];
        memset(player_ip, 0, sizeof(player_ip));
        strcpy(player_ip, ip_map[neighbor_id].c_str());
        //send neighbor ip 
        //note send string need to convert c_str later
        send(ringmaster_connection_players[i],&player_ip,sizeof(player_ip),0);
        //send neighbor port
        // //note send string need to convert c_str later
        send(ringmaster_connection_players[i],&player_port,sizeof(player_port),0);    
    }
    //now we can start our game
    if(num_hops != 0){
        //random pick a player to send potato
        srand((unsigned int)time(NULL)+num_players);
        int random = rand() % num_players;
        //send potato to player and print info to terminal
        cout << "Ready to start the game, sending potato to player " << random << endl;
        send(ringmaster_connection_players[random],&potato,sizeof(potato), 0);
        //after that, just wait for receive the last potato from player
        //use select to listen to all players
        fd_set read_fd_set;
        FD_ZERO(&read_fd_set);
        //Set the fd_set before passing it to the select call 
        for (int i=0;i < num_players;i++) {
                FD_SET(ringmaster_connection_players[i], &read_fd_set);     
        }
        //Invoke select() and then wait!
        int nfds = *max_element(ringmaster_connection_players.begin(),ringmaster_connection_players.end());
        int ret_val = select(nfds+1, &read_fd_set, NULL, NULL, NULL);
        for(int i=0; i< num_players ; i++){
            if(FD_ISSET(ringmaster_connection_players[i], &read_fd_set)){
                    //receive potato from that user
                    recv(ringmaster_connection_players[i],&potato, sizeof(potato), MSG_WAITALL);
                    break;
            }
            

        }  
      
               
    }
    //shut down the game
    //for num_hops == 0, it will go into game and just immediately shut down
    for(int i =0; i<num_players; i++){
        //send end message to all players
        // potato.hops--;
        send(ringmaster_connection_players[i], &potato, sizeof(potato), 0);
    }
    //print trace
    cout << "Trace of potato:" << endl;
    // if(num_hops != 0){
        
    // }
    for(size_t i = 0; i< potato.count; i++){
         cout << potato.trace[i];
        if (i != potato.count - 1) {
            cout << ",";
        }
        else {
            cout << endl;
        }
    }
    
    //end all player socket
    for(int i=0; i<num_players; i++){
        close(ringmaster_connection_players[i]);
    }
    //close the listen socket
    close(ringmaster_fd);
    return EXIT_SUCCESS;
}




    

