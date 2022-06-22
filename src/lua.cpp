#include "lua.h"
#include "Logger.h"

// Builds a LUA table from a map and leaves it on the stack
void mapToLuaTable(GarrysMod::Lua::ILuaBase *LUA, const std::map<std::string, std::string> &map) {
	// Create a new table on the stack
	LUA->CreateTable();

	for (auto const &e : map) {
		// Push key to stack
		LUA->PushString(e.first.c_str());

		// Push value to stack
		LUA->PushString(e.second.c_str());

		// Append both values to the table
		LUA->SetTable(-3);
	}
}

// Transfers values from a LUA Table on the stack (at the given offset)
// into the given map.
void luaTableToMap(GarrysMod::Lua::ILuaBase *LUA, int index, std::map<std::string, std::string> &map) {
	// Query the first entry (we're querying by key, nil = 1st)
	LUA->PushNil();

	// ->Next() gets the last key from the stack and pushes
	// the key-value pair that follows that to the stack.
	// key will now be top-2 and value will be top-1
	while (LUA->Next(index - 1) != 0) {
		// Remove entries with non-string keys
		if (!LUA->IsType(-2, GarrysMod::Lua::Type::String)) {
			Logger::devwarn("Ignoring non-string key in table!");
			goto next;
		}

		// Remove entries with non-string values
		if (!LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
			Logger::devwarn("Ignoring non-string value in table for key '%s'!", LUA->GetString(-2));
			goto next;
		}

		map[LUA->GetString(-2)] = LUA->GetString(-1);

next:
		// Pop value from the stack, key is needed for next iteration
		LUA->Pop();
	}
}

void registerZeroDelayTimer(GarrysMod::Lua::ILuaBase *LUA, GarrysMod::Lua::CFunc function) {
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->GetField(-1, "timer");
	LUA->GetField(-1, "Simple");
	LUA->PushNumber(0);
	LUA->PushCFunction(function);
	LUA->Call(2, 0);
	LUA->Pop();
	LUA->Pop();
}

void registerHook(GarrysMod::Lua::ILuaBase *LUA, char const* event, char const* identifier, GarrysMod::Lua::CFunc function) {
	// We are working on the global table today
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);

	// Get the hook.Add method
	LUA->GetField(-1, "hook");
	LUA->GetField(-1, "Add");

	// Push the new hook data
	LUA->PushString(event);
	LUA->PushString(identifier);
	LUA->PushCFunction(function);

	// Add the hook
	LUA->Call(3, 0);

	// Pop the "hook" table
	LUA->Pop();

	// Pop the global table from the stack again
	LUA->Pop();
}
