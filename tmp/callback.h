#ifndef CALLBACK_H
#define CALLBACK_H
#include <string>
#include <iostream>
#include <iomanip>
using namespace std;

class Callback {
	public:
	      virtual void run(string a) = 0;
};

class EchoCallback : public Callback {
	public:
	      virtual void run(string a);  
};
#endif
