#include "HTTPRequest.h"

std::string HTTPRequest::buildQueryString() {
	std::string params;

	for (auto const& e : this->parameters) {
		if (!params.empty())
			params += "&";

		params += e.first + "=" + e.second;
	}

	return params;
}

std::string HTTPRequest::buildURL() {
	std::string fullurl = this->url;

	// Do we need a query string?
	if (!this->method.isLikePost() && !this->parameters.empty()) {
		fullurl += "?" + this->buildQueryString();
	}

	return fullurl;
}