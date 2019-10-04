#include <string>
#include "method.h"

// Turns a string method into an int
int methodFromString(std::string method) {
	if (method.compare("GET") == 0)
		return METHOD_GET;
	if (method.compare("POST") == 0)
		return METHOD_POST;
	if (method.compare("HEAD") == 0)
		return METHOD_HEAD;
	if (method.compare("PATCH") == 0)
		return METHOD_PATCH;
	if (method.compare("OPTIONS") == 0)
		return METHOD_OPTIONS;

	return METHOD_NOSUPP;
}

// Turns a method int back into a string
std::string methodToString(int method) {
	switch (method) {
	case METHOD_GET:
		return "GET";
	case METHOD_POST:
		return "POST";
	case METHOD_HEAD:
		return "HEAD";
	case METHOD_PUT:
		return "PUT";
	case METHOD_DELETE:
		return "DELETE";
	case METHOD_PATCH:
		return "PATCH";
	case METHOD_OPTIONS:
		return "OPTIONS";
	default:
		return "NOSUPP";
	}
}

bool isLikePost(int method) {
	switch (method) {
	case METHOD_POST:
	case METHOD_PATCH:
		return true;
	default:
		return false;
	}
}
