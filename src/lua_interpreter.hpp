#ifndef LUA_INTERPRETER_HPP
#define LUA_INTERPRETER_HPP

#include <iostream>
#include <lua5.2/lua.hpp>

class LuaInterpreter
{
    public:
	LuaInterpreter()
	{
		lua_State *state = luaL_newstate();
		lua_close(state);
	}

	void execute(const char *filename);

    private:
	void print_error(lua_State *state);
};

#endif
