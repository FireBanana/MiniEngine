#pragma once

#ifndef SPAN_H
#define SPAN_H

#include <assert.h>

template<typename T>
struct Span
{
	const T* start;
	const size_t length;

	Span(const T* s, const size_t l) : start(s), length(l) {  }

	const T& operator[] (int index)
	{
		assert(index >= 0 && index < length);
		return start[index];
	}
};

#endif