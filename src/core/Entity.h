#pragma once
#include <cstdint>

class Entity
{

public:

	Entity(uint32_t id) { mId = id; }
	Entity(Entity const& rhs) = delete;
	Entity(Entity&& rhs) = delete;

	inline uint32_t getId() const noexcept { return mId; }
	inline uint32_t setId(uint32_t id) noexcept { mId = id; }

private:

	uint32_t mId;
};