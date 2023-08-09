#pragma once

#include <utility>
#include <iostream>

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace EnumExtension
{
	template<typename E, E V>
	constexpr bool isValid()
	{
		auto name = __PRETTY_FUNCTION__;
		auto val = name;
		int i = 0;

		while (*val != '\0')
		{
			++i;
			++val;
		}

		for (; i >= 0; --i)
		{
			if (name[i] == '>') break;
		}

		char c1 = name[i - 2];
		char c2 = name[i - 3];

		if (c1 == 'x' || c2 == 'x') return false;

		return true;
	}

	template<typename E, E A, E... B>
	constexpr int countValid()
	{
		constexpr bool isV = isValid<E, A>();
		return countValid<E, B...>() + (int)isV;
	}

	template<typename E>
	constexpr int countValid()
	{
		return 0;
	}

	template<typename E, int... I>
	constexpr int internalElementCount(std::integer_sequence<int, I...> unused)
	{
		return countValid<E, (E)I...>();
	}

	template<typename E>
	constexpr int elementCount()
	{
		return internalElementCount<E>(std::make_integer_sequence<int, 32>());
	}

	// === DEBUG ===

	template <typename E, E V>
	constexpr void _print_func_name()
	{
		std::cout << __PRETTY_FUNCTION__ <<std::endl;
	}

	template <typename E, E V>
	constexpr void _print_enum_name()
	{
		auto name = __PRETTY_FUNCTION__;
		auto val = name;
		int count = 0;

		while (*val != '\0')
		{
			++count;
			++val;
		}

		bool found = false;
		std::string res;

		for (int i = 0; i < count; ++i)
		{
			if (name[i] == '>') break;
			if (found) res += name[i];
			if (name[i] == ',') found = true;
		}

		std::cout << res << std::endl;
	}
}