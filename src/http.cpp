#include <string>
#include "http.h"
#include "method.h"

std::string buildParameters(const HTTPRequest& request) {
	std::string params;

	for (auto const& e : request.parameters) {
		if (!params.empty())
			params += "&";

		params += e.first + "=" + e.second;
	}

	return params;
}

std::string buildUrl(const HTTPRequest& request) {
	std::string url = request.url;

	// Do we need a query string?
	if (!isLikePost(request.method) && !request.parameters.empty()) {
		url += "?" + buildParameters(request);
	}

	return url;
}
