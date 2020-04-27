#ifndef UTILS_HPP
#define UTILS_HPP

#include <type_traits>

namespace utils
{
template <typename E>
constexpr auto to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}
}; // namespace utils

#endif
