#include "GlobalBufferRegistry.h"

namespace MiniEngine
{
	GlobalBufferRegistry::GlobalBufferRegistry(const Backend::OpenGLDriver* driver)
		: mDriver(driver)
	{
	}
	unsigned int GlobalBufferRegistry::createNewBinding(size_t dataSize, void* data, unsigned int bindIndex)
	{
		auto bindingPoint = mDriver->createUniformBlock(dataSize, data, bindIndex);
		mBindingPoints.push_back(bindingPoint);
		return bindingPoint;
	}
}
