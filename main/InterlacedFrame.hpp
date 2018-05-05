#pragma once


#include "PeripheralConfig.hpp"
#include "Color.hpp"
#include "gsl.hpp"
#include <array>


class InterlacedFrame
{
public:
	InterlacedFrame(int rowOffset);

	inline Color& color(int x, int y);
	inline const Color& color(int x, int y) const;

	gsl::span<const uint8_t> rawRow(int y) const;

	void clear(Color color);

	inline int rowOffset() const;

	static constexpr size_t rows();

	static constexpr size_t columns();

	static constexpr size_t rowStride();

private:
	static_assert(DISPLAY_HEIGHT % 2 == 0, "Display height must be a multiple of two to support interlaced frames.");

	int _rowOffset;
	std::array<Color, DISPLAY_WIDTH * DISPLAY_HEIGHT / 2> _colors;
};


Color& InterlacedFrame::color(int x, int y)
{
	return _colors[(y / rowStride()) * DISPLAY_WIDTH + x];
}


const Color& InterlacedFrame::color(int x, int y) const
{
	return _colors[(y / rowStride()) * DISPLAY_WIDTH + x];
}


int InterlacedFrame::rowOffset() const
{
	return _rowOffset;
}


constexpr size_t InterlacedFrame::rows()
{
	return DISPLAY_HEIGHT;
}


constexpr size_t InterlacedFrame::columns()
{
	return DISPLAY_WIDTH;
}


constexpr size_t InterlacedFrame::rowStride()
{
	return 2;
}
