#include <iostream>
#include <string>

using namespace std;

struct Message {
	string type;
	string topic;
	string sender;
	size_t nonce;
	string body;
};
