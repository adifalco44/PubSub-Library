// client_lib.cpp: Producer / Consumer

#include "client_lib.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctime>

#include <string>
#include <vector>
#define MAX_LINE 256 // max number of bytes we can get at once
#define HOST "student04.cse.nd.edu"
#define PORT 40112 //the port c

pthread_mutex_t mutex;
pthread_cond_t sent_cond, rec_cond, call_cond;
int ready_to_send, ready_to_recieve, ready_to_process;
int recieved;
int finished_sending;
int finished_recieving;
string USER = "ophelan1";
size_t NONCE = 222; 


using namespace std;

struct send_msg_args {
    std::queue<string> *a;
    FILE* b;
    std::queue<Message> *c;
};

//void print_message(Message *msg){
//	cout << "MESSAGE:\n\ttype: " << msg->type << "\n\ttopic: " << msg->topic << "\n\tsender: " << msg->sender;
//	cout << "\n\tnonce: " << msg->nonce << "\n\tbody: " << msg->body << endl << flush;
//};
 
Message build_msg(char* response_msg){
    string tmp_str;
	Message tmp;
    tmp.type = "UNSPECIFIED\0";
    tmp.topic = "N/A\0";
    tmp.sender = "N/A\0";
    tmp.nonce = NONCE;
    tmp.body = &response_msg[4];

    if(response_msg[0] == '4'){
		tmp.type = "ERROR\0";
	}
	else if(response_msg[0] == 'M'){
		tmp.type = "RETRIEVE";
		tmp_str = &response_msg[0];
		int tmp_len = strlen(response_msg)-1;
		while (tmp_len > 0){
			if(response_msg[tmp_len-1] == ' '){
				tmp.body = tmp_str.substr(tmp_len, strlen(response_msg)-1); // Sets the value of "body" to what the buffer size should be, so it can be accessed by the calling function
				break;
			}
			tmp_len--;
		}
		int tmp_len2 = tmp_len - 8;
		while(tmp_len2 > 0){
			if(response_msg[tmp_len2-1] == ' '){
				tmp.sender = tmp_str.substr(tmp_len2, tmp_len-8-tmp_len2);
				tmp.topic = tmp_str.substr(8, tmp_len2-14);
				break;
			}
			tmp_len2--;
		}

	}
	else if(response_msg[4] == 'I'){
		tmp.type = "IDENTIFY";
	}
	else if(response_msg[4] == 'S'){
		tmp.type = "SUBSCRIBE";
	}
	else if(response_msg[4] == 'U'){
		tmp.type = "UNSUBSCRIBE";
	}
	else if(response_msg[4] == 'P'){
		tmp.type = "PUBLISH";
	}
	else if(response_msg[4] == 'D'){
		tmp.type = "DISCONNECT";
	}
	return tmp;

};


void* send_messages(void *arg) {
    struct send_msg_args *args = (struct send_msg_args *)arg;
    std::queue<string> *q;
    FILE* stream;
    q = args->a;
    stream = args->b;
    char buffer[BUFSIZ];
    string tmp;

    while(!q->empty()){
	cout << q->front() << endl;
        tmp = q->front();
	cout << "gets to end of send_messages" << endl;
        sleep(10); 
	q->pop();
        // Aquire Lock 
        int rc;
        Pthread_mutex_lock(&mutex);
        while(ready_to_send == 0)
            Pthread_cond_wait(&sent_cond, &mutex);
	// Code To Run once the lock is Aquired 
	fputs(&tmp[0], stream);  
        // Set Control Variables and Release the Lock  
        ready_to_send = 0;
	ready_to_recieve = 1;
        if(q->empty()){
		finished_sending = 1;
	}		
        Pthread_cond_signal(&rec_cond);
        Pthread_mutex_unlock(&mutex);
    }

    return 0;
}

void* get_messages(void* arg) {
	struct send_msg_args *args = (struct send_msg_args *)arg;

	FILE* stream;
	stream = args->b;
	std::queue<Message> *msg_q = args->c;
	char buffer[BUFSIZ];
	string tmp;

    while(true){
	// GET RESPONSE FROM SERVER 
        // Aquire Lock 
        int rc;
        Pthread_mutex_lock(&mutex);
	while(ready_to_recieve == 0){
        	Pthread_cond_wait(&rec_cond, &mutex);
	}

	// Code To Run once the lock is Aquired 
        if(fgets(buffer,BUFSIZ,stream)){
		Message tmpMsg = build_msg(buffer);
		if(tmpMsg.type[0] == 'R'){
			fgets(buffer, atoi(&tmpMsg.body[0])+1, stream);
			tmpMsg.body = &buffer[0];
		}
		msg_q->push(tmpMsg);
		ready_to_recieve = 0;
		ready_to_process = 1;
	}
	
	if(finished_sending == 1){
		finished_recieving = 1;
		ready_to_process = 1;
		Pthread_cond_signal(&call_cond);
		Pthread_mutex_unlock(&mutex);
		break;
	} 
	
	Pthread_cond_signal(&call_cond);
        Pthread_mutex_unlock(&mutex);
    }
	cout << "gets to end of get_messages" << endl;

    return 0;
}

void* signal_callbacks(void* arg){
    struct send_msg_args *args = (struct send_msg_args *)arg;
    std::queue<Message> *msg_q = args->c;
    
    while(true){
	int rc;
	//Message tmpMsg = msg_q -> pop();
	Pthread_mutex_lock(&mutex);
	while(ready_to_process == 0){
		Pthread_cond_wait(&call_cond, &mutex);
	}
	if(!msg_q->empty()){
		Message tmpMsg = msg_q->front();
		msg_q->pop();
		EchoCallback call_it;             
		Callback * base_pointer = &call_it;
		base_pointer->run(tmpMsg);    
		ready_to_process = 0;
		ready_to_send  = 1;
	}
	if(finished_recieving == 1){
		Pthread_mutex_unlock(&mutex);
		break;
	}
	Pthread_cond_signal(&sent_cond);
	Pthread_mutex_unlock(&mutex);
    }
	cout << "gets to end of signal_callback" << endl;

    return 0;
}

Client::Client(const char *host_in, const char *port_in, const char *cid_in){
    struct hostent *hp;
    struct sockaddr_in server;

    /* translate host name into IP address */
    hp = gethostbyname(HOST);
    if (!hp) {
            fprintf(stderr, "Error: unknown host: %s\n", HOST);
            exit(1);
    }

    /* build address data structure */
    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    server.sin_port = htons(PORT);
    size_t server_len = sizeof(server) + sizeof(server.sin_family);

    // Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server, server_len) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

   /*######################################################################
     Client Variables
     ######################################################################*/
    port = *port_in;
    cid = cid_in;
    user_name = "ophelan1";
    nonce = 222;
    // Open stream from server
    server_stream = fdopen(client_fd, "r+");
    if (server_stream == NULL) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

}

void Client::publish(const char *topic, const char *message, size_t length){
    stringstream ss;
    ss << "PUBLISH " << topic << " " << length << "\n" << message; 
    string test = ss.str();
    processes_for_server.push(test);
}

void Client::disconnect() {
    string tmp = "DISCONNECT ophelan1 222\n";
    fputs(&tmp[0], server_stream);  
    char buffer[BUFSIZ];
    fgets(buffer, BUFSIZ, server_stream);               // Get the Response
    Message tmpMsg = build_msg(buffer);
    //print_message(&tmpMsg);                              // Print the response 
    fclose(server_stream);
    close(client_fd);
    exit(0);
	
}

bool Client::shutdown() {
	if (processes_for_server.empty()) {
		return true;
	} else {
		return false;
	}	
}

void Client::subscribe(const char *topic){     
    stringstream ss;

    ss << "SUBSCRIBE " << topic << "\n";
    string test = ss.str();
    processes_for_server.push(test);
}


void Client::unsubscribe(const char *topic){
    stringstream ss;

    ss << "UNSUBSCRIBE " << topic << "\n";
    string test = ss.str();
    processes_for_server.push(test);
}



void Client::retrieve(){
	stringstream ss;

	ss << "RETRIEVE " << USER << "\n";
	string test = ss.str();
	processes_for_server.push(test);
}

void Client::run(){
    Thread threads[3];

    struct send_msg_args *arg  = new(send_msg_args);
    //Queue<Message> MsgQueue;
    arg->a = &processes_for_server;
    arg->b = server_stream; 
    arg->c = &messages_for_callback;    
    threads[0].start(send_messages, arg);
    threads[1].start(get_messages, arg);
    threads[2].start(signal_callbacks, arg);

    size_t result;

    threads[0].join((void **)&result);
    threads[1].join((void **)&result);
    threads[2].join((void **)&result);

    delete(arg);
	cout << "deleted argument\n";
}


FILE * Client::svr_rtr(){
	return server_stream;
}
// Main execution

int main(int argc, char *argv[]) {
    // Initialize Conditional & Control Variables 
    int rc;
    Pthread_mutex_init(&mutex, NULL);
    Pthread_cond_init(&sent_cond, NULL);    
    Pthread_cond_init(&rec_cond, NULL);
    Pthread_cond_init(&call_cond, NULL);
    ready_to_send = 1;
    ready_to_recieve = 0;
    ready_to_process = 0;
    finished_sending = 0;
    finished_recieving = 0;

    const char* username;
    string recipient;
    // Get Username and Initialize Client
    cout << "Please Enter YOUR Username: ";
    cin >> USER;
    username = (const char*) &USER[0];
    cout << "Please Enter YOUR CHAT BUDDIE'S Username: ";
    cin >> recipient;    
    Client tmp("host", "port", "cid");

    //IDENTIFY Message Sent During Construction 
    char buffer[BUFSIZ];
    char identify_msg[BUFSIZ];

    // generate nonce
    srand(time(0));
    int r = rand() % (300 - 200) + 100;

    // create message
    int n = sprintf(identify_msg,"IDENTIFY %s %d\n",username, r);
    fputs(identify_msg, tmp.svr_rtr());                 // Send the Identify Msg
    fgets(buffer, BUFSIZ, tmp.svr_rtr());               // Get the Response
    Message tmpMsg = build_msg(buffer);

    // have user pick retrieve or send
    cout << "Type R to Retrieve messages, type S to send messages" << endl;
    string answer;
    cin >> answer;
    string message;
    if (answer == "R") {
    	tmp.retrieve();
    } else if (answer == "S") {
	cin >> message;
	// do something with message
	size_t length = strlen(message.c_str()) + 1;
    	tmp.publish(&username[0], message.c_str(), length);
    }
    tmp.run();


    // Disconnect From the Client   
    tmp.disconnect();

    return 0;
}
