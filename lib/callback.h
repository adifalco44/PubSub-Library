#ifndef CALLBACK_H
#define CALLBACK_H

#include "message.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <cstring>
using namespace std;

class Callback {
	public:
	      virtual void run(Message &a) = 0;
};

class EchoCallback : public Callback {
	public:
	      virtual void run(Message &a);  
};
#endif
