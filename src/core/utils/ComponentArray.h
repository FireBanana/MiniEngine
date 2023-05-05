#pragma once

#ifndef COMPONENTARRAY_H
#define COMPONENTARRAY_H

#include <assert.h>

template<typename T, size_t S>
class ComponentArray
{
public:

	ComponentArray() : mPtr(0) {}

	//ComponentArray operator=(const Component&) = delete;
	//ComponentArray operator=(const Component&&) = delete;

	inline const size_t size() const { return mPtr; }
	inline T& getLast() { return mArray[mPtr - 1]; }

	void push(T& t) 
	{
		mArray[mPtr++] = t;
	}

	void push(T&& t)
	{
		mArray[mPtr++] = std::move(t);
	}

	const T& operator[] (int index)
	{
		assert(index >= 0 && index < mPtr);
		return mArray[index];
	}

private:

	T mArray[S];
	size_t mPtr;
};

#endif