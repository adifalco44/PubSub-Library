#include <iostream>
#include "queue.h"
#include "thread.h"

using namespace std;

class Client {
    public:
        Client(const char *, const char *, const char *); /* constructor */
        void publish(const char *, const char *, size_t);
        void subscribe(const char *, int);              //Second arg is NOT int
        void unsubscribe(const char *);
        void disconnect();
        void run();
        bool shutdown();
        
    private:
        struct hostent *hp;
//        const char* port;
	unsigned short port;
        const char* cid;
        Queue<string> responses_to_process();        // This will be a queue of responses eventually 
        int client_fd;
        FILE *server_stream;
};
