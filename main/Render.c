#include "Render.h"
#include "ILI9341.h"


void Render_ResetFill()
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
}


void Render_PushLine(const FrameLine* frameLine)
{
	Display_WriteDataArray((uint8_t*)frameLine->Data, FRAME_LINE_BYTES);
}
