#include "lua.h"

// Builds a LUA table from a map and leaves it on the stack
void mapToLuaTable(GarrysMod::Lua::ILuaBase *LUA, const std::map<std::string, std::string>& map) {
	// Create a new table on the stack
	LUA->CreateTable();

	for (auto const& e : map) {
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
void luaTableToMap(GarrysMod::Lua::ILuaBase *LUA, int index, std::map<std::string, std::string>& map) {
	// Query the first entry (we're querying by key, nil = 1st)
	LUA->PushNil();

	// ->Next() gets the last key from the stack and pushes
	// the key-value pair that follows that to the stack.
	// key will now be top-2 and value will be top-1
	while (LUA->Next(index - 1) != 0) {
		// Only store things with String keys and String values
		if (LUA->IsType(-2, GarrysMod::Lua::Type::String) && LUA->IsType(-1, GarrysMod::Lua::Type::String))
			map[LUA->GetString(-2)] = LUA->GetString(-1);

		// Pop value from the stack, key is needed for next iteration
		LUA->Pop();
	}
}
