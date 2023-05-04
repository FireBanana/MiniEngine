#pragma once
#include <memory>

class Texture
{
public:

	Texture(const char* path);
	~Texture();

	inline const int getHeight() const { return mHeight; }
	inline const int getWidth() const { return mWidth; }

	void free();

private:

	int mWidth;
	int mHeight;
	int mCompression;

};