#include <string>
#include <map>

#include "GarrysMod/Lua/Interface.h"

// Not really modeled after anything specific
// These are just the values that we need for the success handler.
struct HTTPResponse {
	long code;
	std::string body;
	std::map<std::string, std::string> headers;
};

class ResultQueueData {
protected:
	int SuccessHandler;
	int FailHandler;
public:
	virtual ~ResultQueueData();
	virtual void run(GarrysMod::Lua::ILuaBase *LUA) = 0;
};

// Data on the success queue
class SuccessQueueData : public ResultQueueData {
private:
	HTTPResponse *response;
public:
	SuccessQueueData(int SuccessHandler, int FailHandler, HTTPResponse *response);
	~SuccessQueueData() override;
	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};

// Data on the success queue
class FailedQueueData : public ResultQueueData {
private:
	std::string reason;
public:
	FailedQueueData(int SuccessHandler, int FailHandler, const std::string& reason);
	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};