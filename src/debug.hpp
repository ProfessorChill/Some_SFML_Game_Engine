#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <ostream>

#include <tinyxml2.h>

namespace dbg
{
enum Code {
	FG_RED = 31,
	FG_GREEN = 32,
	FG_YELLOW = 33,
	FG_BLUE = 34,
	FG_DEFAULT = 39,
};

enum Urgency {
	DEFAULT = 0,
	WARNING,
	ERROR,
};

class Modifier
{
	Code code;

public:
	Modifier(Code pCode) : code(pCode)
	{
	}

	friend std::ostream &operator<<(std::ostream &os, const Modifier &mod)
	{
		return os << "\033[" << mod.code << "m";
	}
};

void printMessage(const char *msg, Urgency urgency);
tinyxml2::XMLError *handleXMLError(tinyxml2::XMLError err);
}; // namespace dbg

#endif
