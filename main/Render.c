#include "Render.h"
#include "ILI9341.h"


void Render_DrawFrame(const FrameBuffer* frameBuffer)
{
	Display_WriteCommand(ILI9341_CASET);
	Display_WriteData(0);
	Display_WriteData(0);
	Display_WriteData((DISPLAY_WIDTH >> 8) & 0xFF);
	Display_WriteData(DISPLAY_WIDTH & 0xFF);
	
	
	Display_WriteCommand(ILI9341_PASET);
	Display_WriteData(0);
	Display_WriteData(0);
	Display_WriteData((DISPLAY_HEIGHT >> 8) & 0xFF);
	Display_WriteData(DISPLAY_HEIGHT & 0xFF);
	
	Display_WriteCommand(ILI9341_RAMWR);

	for (size_t y = 0; y < DISPLAY_HEIGHT; y++)
		Display_WriteDataArray((const uint8_t*)&frameBuffer->Data[DISPLAY_WIDTH * y], DISPLAY_WIDTH * DISPLAY_BYTES_PER_PIXEL);
}
