#include "HTTPMethod.h"

bool HTTPMethod::isLikePost() {
	switch (this->method) {
		case HTTPMethod::POST:
		case HTTPMethod::PUT:
		case HTTPMethod::DELETE:
		case HTTPMethod::PATCH:
			return true;
		default:
			return false;
	}
}

std::string HTTPMethod::toString() {
	switch (this->method) {
		case HTTPMethod::GET:
			return "GET";
		case HTTPMethod::POST:
			return "POST";
		case HTTPMethod::HEAD:
			return "HEAD";
		case HTTPMethod::PUT:
			return "PUT";
		case HTTPMethod::DELETE:
			return "DELETE";
		case HTTPMethod::PATCH:
			return "PATCH";
		case HTTPMethod::OPTIONS:
			return "OPTIONS";
		default:
			return "INVALID";
	}
}

HTTPMethod HTTPMethod::fromString(std::string method) {
	for(auto& c : method) {
		c = std::toupper(c);
	}

	if (method == "GET")
		return HTTPMethod::GET;
	if (method == "POST")
		return HTTPMethod::POST;
	if (method == "HEAD")
		return HTTPMethod::HEAD;
	if (method == "PUT")
		return HTTPMethod::PUT;
	if (method == "DELETE")
		return HTTPMethod::DELETE;
	if (method == "PATCH")
		return HTTPMethod::PATCH;
	if (method == "OPTIONS")
		return HTTPMethod::OPTIONS;

	return HTTPMethod::INVALID;
}
