#pragma once
#include <memory>

namespace MiniEngine
{
	class Texture
	{
	public:        
		enum class TextureType : int
		{
			Default = 0,
			CubeMap = 1
		};

		Texture() {}
        Texture(int width, int height, int channels);
        ~Texture() { /*free();*/ }

		inline int getHeight() const { return mHeight; }
		inline int getWidth() const { return mWidth; }
		inline unsigned int getId() const { return mId; }

    private:
        static unsigned int sResourceIdCounter;

        int mWidth;
		int mHeight;
		int mChannels;
        unsigned int mId;
    };
}
