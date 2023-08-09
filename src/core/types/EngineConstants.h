#pragma once

#ifndef MINIENGINE_CONSTANTS_H
#define MINIENGINE_CONSTANTS_H

#include "EnumExtension.h"
#include "EngineTypes.h"

namespace MiniEngine
{
	constexpr int MAX_TEXTURE_TYPES = EnumExtension::elementCount<MiniEngine::Types::TextureType>();
};

#endif