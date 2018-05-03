#include "RemoteFrame.hpp"
#include "ILI9341.hpp"
#include "Display.hpp"
#include "gsl.hpp"


RemoteFrame::RemoteFrame() :
	_evenFrame(0),
	_oddFrame(1),
	_uploadingFrame(&_oddFrame),
	_workingFrame(&_evenFrame),
	_hasDirtyFrame(false)
{

}


void RemoteFrame::swap()
{
	while (_hasDirtyFrame) 
	{
		// Wait for frame uploading to finish.
	}

	_hasDirtyFrame = true;
	std::swap(_uploadingFrame, _workingFrame);
}


InterlacedFrame& RemoteFrame::workingFrame()
{
	return *_workingFrame;
}


void RemoteFrame::upload()
{
	if (!_hasDirtyFrame)
		return;

	const auto& frame = *_uploadingFrame;

	Display::writeCommand(ILI9341_CASET);
	
	uint8_t caset[4] = { 0, 0, (DISPLAY_WIDTH >> 8) & 0xFF, DISPLAY_WIDTH & 0xFF};
	Display::writeData(gsl::span<const uint8_t>(caset, 4));
	
	for (int y = 0; y < frame.rows() / frame.rowStride(); y++)
	{
		int displayRow = y * frame.rowStride() + frame.rowOffset();
		Display::writeCommand(ILI9341_PASET);
		uint8_t paset[4] = 
		{ 
			static_cast<uint8_t>((displayRow >> 8) & 0xFF), 
			static_cast<uint8_t>(displayRow & 0xFF), 
			(DISPLAY_HEIGHT >> 8) & 0xFF, 
			DISPLAY_HEIGHT & 0xFF
		};

		Display::writeData(gsl::span<const uint8_t>(paset, 4));

		Display::writeCommand(ILI9341_RAMWR);

		Display::writeData(frame.rawRow(displayRow));
	}

	_hasDirtyFrame = false;
}