#pragma once

class Color
{
public:

	Color() : mRed(0), mGreen(0), mBlue(0), mAlpha(0) {	}
	Color(float r, float g, float b, float a)
	{
		mRed = r;
		mBlue = b;
		mGreen = g;
		mAlpha = a;
	}

	inline float r() const noexcept { return mRed; }
	inline float g() const noexcept { return mGreen; }
	inline float b() const noexcept { return mBlue; }
	inline float a() const noexcept { return mAlpha; }

private:

	float mRed;
	float mGreen;
	float mBlue;
	float mAlpha;

};