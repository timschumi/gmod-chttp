#include <string>

class HTTPMethod {
public:
	enum Method {
		INVALID = 0,
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		OPTIONS,
		PATCH,
	};

	HTTPMethod() = default;
	constexpr HTTPMethod(Method m) : method(m) {};

	operator Method() const {
		return method;
	}

	explicit operator bool() = delete;

	bool isLikePost();
	std::string toString();
	static HTTPMethod fromString(std::string method);

private:
	Method method;
};
