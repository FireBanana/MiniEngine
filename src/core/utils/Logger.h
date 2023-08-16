#pragma once

#ifndef _LOGGER_H
#define _LOGGER_H

#include <spdlog/spdlog.h>

namespace MiniEngine
{
namespace Logger
{
	template<typename... T>
	void print(T... mssg)
	{
		spdlog::info(mssg...);
	}

	template<typename... T>
	void wprint(T... mssg)
	{
		spdlog::warn(mssg...);
	}

	template<typename... T>
	void eprint(T... mssg)
	{
		spdlog::error(mssg...);
	}
}
}

#endif