#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
 #include <arpa/inet.h>
#include <unistd.h>
using namespace std;
int server_bind_listen(const char *port );
int sever_accept(int sever_fd, string * ip, string * port);
int client_connect(const char *hostname, const char *port);
int get_clinetport(int clientfd);
