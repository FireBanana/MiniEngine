#pragma once
#include <cstdint>
#include <bitset>

class Entity
{

public:

	Entity() : mId(idIncrement++) {  }
	Entity(const Entity& rhs) = delete;
	Entity operator=(Entity&) = delete;
	Entity(Entity&& rhs) = default;
	Entity& operator=(Entity&&) = default;

private:
	static uint32_t idIncrement;
	uint32_t mId;
};