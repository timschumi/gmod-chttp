#include "lua.h"

// Builds a LUA table from a map and leaves it on the stack
void mapToLuaTable(Lua::ILuaBase *LUA, std::map<std::string, std::string> map) {
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
// into a map that will be the return value.
std::map<std::string, std::string> mapFromLuaTable(Lua::ILuaBase *LUA, int index) {
	std::map<std::string, std::string> map;

	// Query the first entry (we're querying by key, nil = 1st)
	LUA->PushNil();

	// ->Next() gets the last key from the stack and pushes
	// the key-value pair that follows that to the stack.
	// key will now be top-2 and value will be top-1
	while (LUA->Next(index - 1) != 0) {
		// Only store things with String keys
		if (LUA->IsType(-2, Lua::Type::String))
			map[LUA->GetString(-2)] = LUA->GetString(-1);

		// Pop value from the stack, key is needed for next iteration
		LUA->Pop();
	}

	return map;
}

void printMessage(Lua::ILuaBase *LUA, std::string message) {
	// Push global table to the stack to work on it
	LUA->PushSpecial(Lua::SPECIAL_GLOB);

	// Gets the print function and stores it at the top of the stack (top = -1)
	LUA->GetField(-1, "print");

	// Pushes the argument to the stack
	LUA->PushString(("[CHTTP] " + message).c_str());

	// Calls the function (arg1 = number of arguments, arg2 = number of return values).
	// The function is the top of the stack after arguments have been popped.
	// In this case, we are calling with 1 argument (the message) and no return values (because it's `print`, duh!)
	LUA->Call(1, 0);

	// Pops the last value from the stack (the global table?)
	LUA->Pop();
}
