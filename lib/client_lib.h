#include <iostream>
#include "callback.h"
#include "queue.h"
#include "thread.h"

using namespace std;

class Client {
    public:
        Client(const char *, const char *, const char *); /* constructor */
        void publish(const char *, const char *, size_t);
        void subscribe(const char *topic);      // til we figure out callback use this function
        void unsubscribe(const char *);
        void disconnect();
        void run();
        bool shutdown();
        void retrieve(const char *); 
    private:
	unsigned short port;
        const char* cid;
        std::queue <Message> messages_for_callback;
	std::queue <string> processes_for_server;
        int client_fd;
        FILE *server_stream;
        size_t nonce;
	string user_name;
};
