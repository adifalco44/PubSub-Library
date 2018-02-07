#include "callback.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/poll.h>

int main(){
//	string m = "hello\n";              /* To be fed from queue */
//	EchoCallback call_it;              /* I believe this is how you would instantiate it */
//	Callback * base_pointer = &call_it;/* Casts virtual function pointer */
//	base_pointer->run(m);              /* Member function call */
	while(true){
		cout << "Polling\n";
		char tmp[BUFSIZ];
		int timeout=10000;
		struct pollfd fd;
		fd.fd = 0;
		fd.events = POLLIN;
		fd.revents = 0;
		int ret = poll(&fd, 1, timeout);
		if (ret > 0 && (fd.events & POLLIN != 0)){
			cout << "File listened on\n";
			cin.getline(tmp,BUFSIZ);
			cout << tmp << endl;
		}else{
			cout << "Timeout" << endl;
		}
		fflush(0);
	}
}
