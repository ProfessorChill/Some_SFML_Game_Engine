#ifndef COMMON_FUNCTIONS_HPP
#define COMMON_FUNCTIONS_HPP

#include <limits.h>
#include <string>
#include <unistd.h>

std::string get_selfpath()
{
	char buff[PATH_MAX];
	ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
	if (len != -1) {
		buff[len] = '\0';
		return std::string(buff);
	}

	return "";
}

#endif
