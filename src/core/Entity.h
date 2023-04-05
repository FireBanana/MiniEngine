#pragma once
#include <cstdint>
#include <bitset>

class Entity
{

public:

	Entity() {  }
	Entity(Entity& rhs) = delete;
	Entity(Entity&& rhs) = default;

private:
	static uint32_t mId;
};