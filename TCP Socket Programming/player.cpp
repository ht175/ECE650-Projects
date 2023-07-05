#include "potato.hpp"
#include "function.hpp"
#include <algorithm>

int main(int argc, char *argv[]){
    ////error detection: player <machine_name> <port_num>
    if(argc != 3){
        cerr << "command line parameter number does not match expectaion" << endl;
        return EXIT_FAILURE;
    }
    //parse the command line 
    //to get master hostname and port-num to connect to
    const char * hostname = argv[1];
    const char * port = argv[2];
    int id;
    int num_players;
    //player as a client, connect to ringmaster
    int master_connect_fd=client_connect(hostname, port);
    //get id from ringmaster
    recv(master_connect_fd, &id, sizeof(id), 0);
    //get total number of player from ringmaster
    recv(master_connect_fd, &num_players, sizeof(num_players), 0);
    //after initilization , print ready message 
    //Connected as player <number> out of <number> total players
    cout << "Connected as player " << id <<  " out of " << num_players << " total players"<<endl;
    //build a server in order to accept pre neigbor's connection
    //pre neighbor need this server's port and ip to connect to 
    //thus we need send port to master, as it will assign that information to pre neighbor
    //no specified port
    int negh_server_connect_fd = server_bind_listen("");
    //send port number to ringmaster
    //first get port number of player
    int player_port =get_clinetport(negh_server_connect_fd);
    send(master_connect_fd, &player_port, sizeof(player_port),0);
    //get next neighbor's info from master in order to connect to it
    //receive neighbor ip 
    char next_negh_ip[100];
    recv(master_connect_fd, &next_negh_ip, sizeof(next_negh_ip),0);
    //recive neigbor port
    int next_negh_port;
    recv(master_connect_fd, &next_negh_port, sizeof(next_negh_port),0);
    char port_c[9]; 
    sprintf(port_c, "%d", next_negh_port);
    //now ready to connect to next neighbor
    int next_neigh_fd = client_connect(next_negh_ip, port_c);
    //wait for accept pre neighbor's connection
    string pre_neigh_ip;
    string pre_neigh_port;
    int pre_neigh_fd = sever_accept(negh_server_connect_fd, &pre_neigh_ip, &pre_neigh_port);
    //after setting, ready to play the game
    // int fds_arr [] = {pre_neigh_fd,next_neigh_fd,master_connect_fd};
    // vector<int> fds (fds_arr, fds_arr + sizeof(fds_arr) / sizeof(int));
    vector<int> fds{pre_neigh_fd,next_neigh_fd,master_connect_fd};
    int nfds = *max_element(fds.begin(),fds.end());
    fd_set read_fd_set;
    srand((unsigned int)time(NULL) + id);
    potato_t potato;
    potato.hops = 0;
    memset(potato.trace, 0, sizeof(potato.trace)); 
    potato.count = 0;
    // cout << "personal port " << player_port << endl; 
    // cout << "next neigh port " << next_negh_port << endl;  
    // cout << "pre neigh port " << pre_neigh_port << endl; 
    

    while (1) {
        FD_ZERO(&read_fd_set);
        for (int i=0; i<3; i++) {
            FD_SET(fds[i], &read_fd_set);           
        }
        int ret_val = select(nfds+1, &read_fd_set, NULL, NULL, NULL);
        int len;
        for(int i=0; i< 3 ; i++){
            if(FD_ISSET(fds[i], &read_fd_set)){
                    //receive potato from that user
                    len=recv(fds[i], &potato, sizeof(potato), MSG_WAITALL);
                    potato.hops--;
                    // if(i == 0){
                    //     cout << "potato received" << " hops " << potato.hops << " from prev neigh " << endl;
                    // }
                    // if(i == 1){
                    //     cout << "potato received" << " hops " << potato.hops << " from next neigh " << endl;
                    // }
                    // if(i == 2){
                    //     cout << "potato received" << " hops " << potato.hops << " from master " << endl;
                    // }
                    
                    potato.trace[potato.count] = id;
                    potato.count++;
                    if(len==0){

                    }else{
                        break;
                    }
                    
                }
            }
        
        //if potato greater than 0, then need to randomly choose a neighbor
        if(potato.hops > 0){
            int random = rand() % 2;
            //send to pre neighbor
            if(random == 0){
                send(pre_neigh_fd,&potato,sizeof(potato),0);   
                int pre_neigh_id = id==0 ? num_players-1 : id-1;
                cout << "Sending potato to " << pre_neigh_id << endl;
            }//send to next neighbor
            else{
                send(next_neigh_fd,&potato,sizeof(potato),0);
                int next_neigh_id = id==num_players-1? 0 : id+1;
                cout << "Sending potato to " << next_neigh_id << endl;

            }
        }
        //send back to masterring
        if(potato.hops == 0 ){
            cout << "I'm it" << endl;
            send(master_connect_fd,&potato,sizeof(potato),0);
        }
        //mastering tell the game end
        if(potato.hops <0 ){
            break;
        }
    }
    //Game end, close all socket
    close(pre_neigh_fd);
    close(next_neigh_fd);
    close(master_connect_fd);
    return EXIT_SUCCESS;
}