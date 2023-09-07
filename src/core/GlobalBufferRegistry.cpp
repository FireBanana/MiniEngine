#include "GlobalBufferRegistry.h"

namespace MiniEngine
{
	GlobalBufferRegistry::GlobalBufferRegistry(const Backend::IDriver* driver)
		: mDriver(driver)
	{
	}

	void GlobalBufferRegistry::createNewBinding(BlockType block, unsigned int bindIndex)
	{
		auto uniformBufferId = mDriver->createUniformBlock(parseBlockSize(block), bindIndex);
		mBindingPoints.insert({ block, uniformBufferId });
	}

	void GlobalBufferRegistry::updateUniformData(BlockType block, unsigned int offset, size_t size, void* data)
	{
		mDriver->updateUniformData(mBindingPoints[block], offset, size, data);
	}

	size_t GlobalBufferRegistry::parseBlockSize(BlockType type)
	{
		switch (type)
		{
			case BlockType::SceneBlock: return sizeof(SceneBlock); break;
			default: throw;
		}
	}
}
