#include "InterlacedFrame.hpp"


InterlacedFrame::InterlacedFrame(int rowOffset) :
	_rowOffset(rowOffset)
{

}


gsl::span<const uint8_t> InterlacedFrame::rawRow(int y) const
{
	const uint8_t* rowData = reinterpret_cast<const uint8_t*>(&_colors[DISPLAY_WIDTH * (y / 2)]);
	return gsl::span<const uint8_t>(rowData, DISPLAY_WIDTH * sizeof(Color));
}


void InterlacedFrame::clear(Color color)
{
	for (int i = 0; i < _colors.size(); i++)
		_colors[i] = color;
}