#include "callback.h"
using namespace std;

void Callback::run(Message &a){}

void EchoCallback::run(Message &a){
	if (a.type=="IDENTIFY"){
		cout << "IDENTIFY " << a.sender << " " << a.nonce  << endl;
	}
	if (a.type == "SUBSCRIBE"){
		cout << "SUBSCRIBE " << a.topic << endl;
	}
	if (a.type == "UNSUBSCRIBE"){
		cout << "UNSUBSCRIBE " << a.topic << endl;
	}
	if (a.type == "PUBLISH"){
		cout << "PUBLISH " << a.topic << " " << strlen(a.body.c_str()) + 1 << endl;
		cout << a.body << endl;
	}
	if (a.type == "RETRIEVE"){
		cout << "RETRIEVE " << a.sender << endl;
	}
	if (a.type == "DISCONNECT"){
		cout << "DISCONNECT " << a.sender << " " << a.nonce << endl;
	}
}
