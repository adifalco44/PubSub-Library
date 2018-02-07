// client_lib.cpp: Producer / Consumer

#include "queue.h"
#include "thread.h"
#include "client_lib.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <string>
#include <vector>
#define MAX_LINE 256 // max number of bytes we can get at once
#define HOST "student00.cse.nd.edu"
//#define PORT 41001 // the port c
using namespace std;

Client::Client(const char *host_in, const char *port_in, const char *cid_in){
    struct hostent *hp;
    struct sockaddr_in server;
    port = (unsigned short)strtoul(port_in,NULL,0);

    /* translate host name into IP address */
    hp = gethostbyname(HOST);
    if (!hp) {
            fprintf(stderr, "Error: unknown host: %s\n", HOST);
            exit(1);
    }

    /* build address data structure */
    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;
    //server.sin_socktype = SOCK_STREAM;
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    server.sin_port = htons(port);
    size_t server_len = sizeof(server.sin_addr) + sizeof(server.sin_family);


    // Create socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server, server_len) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Open stream from server
    server_stream = fdopen(client_fd, "r+");
    if (server_stream == NULL) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }
    cid = cid_in;
}


// Main execution

int main(int argc, char *argv[]) {

    Client tmp("host", "port", "cid");

    return 0;
}
