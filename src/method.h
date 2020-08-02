#define METHOD_NOSUPP	-1
#define METHOD_GET	1
#define METHOD_POST	2
#define METHOD_HEAD	3
#define METHOD_PUT	4
#define METHOD_DELETE	5
#define METHOD_PATCH	6
#define METHOD_OPTIONS	7

int methodFromString(std::string method);
std::string methodToString(int method);
bool isLikePost(int method);
