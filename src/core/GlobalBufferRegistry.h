#pragma once

#include <vector>

#include "../backend/OpenGLDriver.h"

namespace MiniEngine
{
	class GlobalBufferRegistry
	{
	public:

		GlobalBufferRegistry(const Backend::OpenGLDriver* driver);

		unsigned int createNewBinding(size_t dataSize, void* data, unsigned int bindIndex);

	private:

		const Backend::OpenGLDriver* mDriver;
		std::vector<unsigned int> mBindingPoints;

	};
}