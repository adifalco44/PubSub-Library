#include "client_lib.cpp"
using namespace std;

	
void paste_this_elsewhere(){
	Message &m;                            /* To be fed from queue */
	EchoCallback call_it;                  /* I believe this is how you would instantiate it */
	Callback * base_pointer = &call_it;    /* Casts virtual function pointer */
	base_pointer->run(*m);                 /* Member function call */
}


int main(){
       const char *host = "student00.cse.nd.edu";
       const char *port = "400113";
       string username, password, user_content;
       cout  << "Please enter your chat username: ";
       cin   >> username;
       cout << "Please enter your password: ";
       cin  >> password;
       Client instance(host,port,password);
       cout  << "\n\n============================================\n"
	     << "||  ~  ~   Live      Web     Chat   ~  ~  ||\n"
	     << "============================================\n"
	     << "||          To join a chat, type:         ||\n;     
	     << "||              \"JOIN-ROOM_#\"             ||\n"
	     << "||  Where ROOM_# represents the chat room ||\n"
	     << "||           you'd like to join           ||\n"	
	     << "============================================\n"
//	     << "||    To update your chat feed, type      ||\n"
//	     << "||               \"RETRIEVE\"               ||\n"	
//	     << "============================================\n"
	     << "||       To switch chat rooms, type:      ||\n"	
	     << "||    	       \"SWITCH-ROOM_#\"             ||\n"
	     << "||  Where ROOM_# represents the chat room ||\n"
	     << "||       you'd like to switch into        ||\n"
	     << "============================================\n"
	     << "||        To post a message,  type:       ||\n"
	     << "||          \"POST_MESSAGE_COTENT\"         ||\n"
	     << "||  Where MESSAGE-CONTENT represents the  ||\n"
	     << "||         message you'd like to send     ||\n"
	     << "============================================\n"
	     << "||*!* Spaces are only permitted in the *!*||\n"
	     << "||  *!* body of your MESSAGE_CONTENT *!*  ||\n"
	     << "============================================\n"
	     << "||  ~-~             Enjoy             ~-~ ||\n"
	     << "============================================\n\n";
	char to_be_spliced[BUFSIZ] = (char *)user_content.c_str();
	char * token,data,current_chat_room;
	ssize_t size;
	current_chat_room = NULL;
	while (true){
		cout << "Enter your command : ";
		cin >> user_content;
		token = strtok(to_be_spliced,"-");
		if (token !=NULL){
			if (strcmp(token,"JOIN")==0){
				token = strtok(NULL,"-");
				if (token==NULL){
					fprintf(stderr,"Error: command not formatted properly\n");
				}else if (current_chat_room = NULL){
					current_chat_room = token;
					instance.subscribe(current_chat_room)
				}
			}
			if (strcmp(token,"SWITCH")==0){
				token = strtok(NULL,"-");
				if (token==NULL){
					fprintf(stderr,"Error: command not formatted properly\n");
				}else{
					instance.unsubscribe(current_chat_room);
					current_chat_room = token;
					instance.subscribe(current_chat_room);
				}
			}
			if (strcmp(token,"POST")==0){
				token = strtok(NULL,"-");
				if (token==NULL){
					fprintf(stderr,"Error: command not formatted properly\n");
				}else{
					data = token;	
					size = strlen(data)+1;
					instance.publish(current_chat_room,data,size);
				}	
			}
		}
		instance.run();
	}
	instance.disconnect();
	return 0;	
}
