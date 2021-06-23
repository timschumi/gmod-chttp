#pragma once

#include <string>
#include <map>
#include <memory>

#include "GarrysMod/Lua/Interface.h"
#include "LuaReference.h"

class LuaTask {
public:
	virtual void run(GarrysMod::Lua::ILuaBase *LUA) = 0;

	virtual ~LuaTask() = default;
};

class RequestCallbackTask : public LuaTask {
protected:
	std::shared_ptr<LuaReference> callback;
public:
	explicit RequestCallbackTask(std::shared_ptr<LuaReference> callback);
};

class SuccessCallbackTask : public RequestCallbackTask {
public:
	long code = 0;
	std::string body;
	std::map<std::string, std::string> headers;
public:
	explicit SuccessCallbackTask(std::shared_ptr<LuaReference> callback);

	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};

class FailCallbackTask : public RequestCallbackTask {
private:
	std::string reason;
public:
	FailCallbackTask(std::shared_ptr<LuaReference> callback, std::string reason);

	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};