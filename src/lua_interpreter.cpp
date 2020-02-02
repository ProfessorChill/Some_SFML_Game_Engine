#include "lua_interpreter.hpp"

extern "C" int howdy(lua_State *state)
{
	int args = lua_gettop(state);

	printf("howdy() was called with %d arguments:\n", args);

	for (int n = 1; n <= args; ++n) {
		printf("\targument %d: '%s'\n", n, lua_tostring(state, n));
	}

	lua_pushnumber(state, 123);

	return 1;
}

void LuaInterpreter::execute(const char *filename)
{
	lua_State *state = luaL_newstate();

	luaL_openlibs(state);
	lua_register(state, "howdy", howdy);

	int result;
	result = luaL_loadfile(state, filename);

	if (result != LUA_OK) {
		print_error(state);

		return;
	}

	result = lua_pcall(state, 0, LUA_MULTRET, 0);

	if (result != LUA_OK) {
		print_error(state);

		return;
	}
}

void LuaInterpreter::print_error(lua_State *state)
{
	const char *message = lua_tostring(state, -1);
	std::cout << message << "\n";
	lua_pop(state, 1);
}
