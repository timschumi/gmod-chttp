#include <string>
#include "http.h"
#include "method.h"

std::string buildParameters(HTTPRequest request) {
	std::string params = "";

	for (auto const& e : request.parameters) {
		if (params.size() != 0)
			params += "&";

		params += e.first + "=" + e.second;
	}

	return params;
}

std::string buildUrl(HTTPRequest request) {
	std::string url = request.url;

	// Do we need a query string?
	if (!isLikePost(request.method) &&
	    request.parameters.size() != 0) {
		url += "?" + buildParameters(request);
	}

	return url;
}
