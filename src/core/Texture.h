#pragma once
#include <memory>

class Texture
{
public:

	Texture() {}
	Texture(int width, int height, int compression, unsigned int id);
	~Texture() { /*free();*/ }

	inline const int getHeight() const { return mHeight; }
	inline const int getWidth() const { return mWidth; }
	inline const int getId() const { return mId; }

private:

	int mWidth;
	int mHeight;
	int mCompression;
	unsigned int mId;

};