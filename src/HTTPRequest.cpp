#include <curl/curl.h>

#include "HTTPRequest.h"

HTTPRequest::HTTPRequest() {
	curl_version_info_data *info = curl_version_info(CURLVERSION_NOW);
	this->headers["User-Agent"] = (std::string) "curl/" + info->version + " gmod-chttp/1.3.2";
}

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
