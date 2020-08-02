#include <string>
#include <cctype>
#include "method.h"

// Turns a string method into an int
int methodFromString(std::string method) {
	for(auto& c : method) {
		c = std::toupper(c);
	}
	if (method == "GET")
		return METHOD_GET;
	if (method == "POST")
		return METHOD_POST;
	if (method == "HEAD")
		return METHOD_HEAD;
	if (method == "PUT")
		return METHOD_PUT;
	if (method == "DELETE")
		return METHOD_DELETE;
	if (method == "PATCH")
		return METHOD_PATCH;
	if (method == "OPTIONS")
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
	case METHOD_PUT:
	case METHOD_DELETE:
	case METHOD_PATCH:
		return true;
	default:
		return false;
	}
}
