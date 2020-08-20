#include <string>

class HTTPMethod {
public:
	enum Method {
		M_INVALID = 0,
		M_GET,
		M_HEAD,
		M_POST,
		M_PUT,
		M_DELETE,
		M_OPTIONS,
		M_PATCH,
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
