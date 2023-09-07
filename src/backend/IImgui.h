#pragma once

#include <functional>

namespace MiniEngine
{
	namespace Backend
	{

		class IImgui
		{
		public:

			virtual void createSliderPanel(const char* name, float* value, float min, float max, std::function<void()> cb) = 0;
			virtual void createBooleanPanel(const char* name, bool& flag, std::function<void()> cb) = 0;

		private:
		};

	}
}