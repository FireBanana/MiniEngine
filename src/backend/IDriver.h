#pragma once

namespace MiniEngine
{
namespace Backend
{

	class IDriver
	{
	public:

		virtual void initialize();
		virtual void setImguiInterface();

	private:
	};

}
}