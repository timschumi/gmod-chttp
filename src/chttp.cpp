#include <string>
#include <curl/curl.h>

#include "GarrysMod/Lua/Interface.h"

#include "lua.h"
#include "threading.h"

/*
 * See https://wiki.facepunch.com/gmod/Global.HTTP for documentation.
 * The function takes a single table argument, based off the HTTPRequest structure.
 * It returns a boolean whether a request was sent or not.
 */
LUA_FUNCTION(CHTTP) {
	if (!LUA->IsType(1, GarrysMod::Lua::Type::Table)) {
		return 0;
	}

	auto *request = new HTTPRequest();
	std::string failreason;

	// Fetch failed handler
	LUA->GetField(1, "failed");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::Function)) {
		request->failed = LUA->ReferenceCreate();
	} else {
		LUA->Pop();
	}

	// Fetch method
	LUA->GetField(1, "method");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
		request->method = HTTPMethod::fromString(LUA->GetString(-1));
	}
	LUA->Pop();
	if (request->method == HTTPMethod::M_INVALID) {
		failreason = "Unsupported request method: " + std::string(LUA->GetString(-1));
		goto exit;
	}

	// Fetch url
	LUA->GetField(1, "url");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
		request->url = LUA->GetString(-1);
	} else {
		failreason = "invalid url";
		goto exit;
	}
	LUA->Pop();

	// Fetch success handler
	LUA->GetField(1, "success");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::Function)) {
		request->success = LUA->ReferenceCreate();
	} else {
		LUA->Pop();
	}

	// Fetch headers
	LUA->GetField(1, "headers");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::Table)) {
		luaTableToMap(LUA, -1, request->headers);
	}
	LUA->Pop();

	// Fetch parameters
	LUA->GetField(1, "parameters");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::Table)) {
		luaTableToMap(LUA, -1, request->parameters);
	}
	LUA->Pop();

	// Fetch type
	LUA->GetField(1, "type");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
		request->headers["Content-Type"] = LUA->GetString(-1);
	}
	LUA->Pop();

	// Fetch body
	LUA->GetField(1, "body");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
		request->body = LUA->GetString(-1);
	}
	LUA->Pop();

	scheduleRequest(request);

exit:
	if (!failreason.empty()) {
		if (request->failed) {
			// Run the fail handler
			LUA->ReferencePush(request->failed);
			LUA->ReferenceFree(request->failed);

			LUA->PushString(failreason.c_str());
			LUA->Call(1, 0);
		}

		delete request;
	}

	LUA->PushBool(true); // Push result to the stack
	return 1; // We are returning a single value
}

LUA_FUNCTION(threadingDoThink) {
	while (!getResultQueue().empty()) {
		ResultQueueData* data = getResultQueue().front();
		getResultQueue().pop();
		data->run(LUA);
		delete data;
	}

	return 0;
}

GMOD_MODULE_OPEN() {
#ifdef WINDOWS_BUILD
	if (curl_global_sslset(CURLSSLBACKEND_SCHANNEL, nullptr, nullptr) != CURLSSLSET_OK) {
		LOG("error: The WinSSL/schannel backend is not available!");
		return 1;
	}
#endif

	// Initialize curl
	curl_global_init(CURL_GLOBAL_ALL);

	// We are working on the global table today
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);

	// Push the function mapping (first is the key/function name,
	// second is the value/actual function)
	LUA->PushString("CHTTP");
	LUA->PushCFunction(CHTTP);

	// SetTable takes the item at the top of the stack (value) and
	// the second item from the top (key) and adds them to the table
	// at the stack offset mentioned in the parameter (again, -1 is the top)
	LUA->SetTable(-3);


	// Get the hook.Add method
	LUA->GetField(-1, "hook");
	LUA->GetField(-1, "Add");

	// Push the new hook data
	LUA->PushString("Think");
	LUA->PushString("__chttpThinkHook");
	LUA->PushCFunction(threadingDoThink);

	// Add the hook
	LUA->Call(3, 0);

	// Pop the "hook" table
	LUA->Pop();

	// Pop the global table from the stack again
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	// Cleanup curl
	curl_global_cleanup();

	return 0;
}
