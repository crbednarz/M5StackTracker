#include "Render.h"
#include "ILI9341.h"
#include <stdbool.h>


static FrameBuffer FrameBuffers[1];
static FrameBuffer* UploadingFrame;
static FrameBuffer* WorkingFrame;

static volatile bool IsUploadingFrameDirty = false;

static void Render_UploadFrame(const FrameBuffer* frameBuffer)
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

	Display_WriteDataArray((const uint8_t*)frameBuffer->Data, FRAME_BUFFER_BYTES);
}


FrameBuffer* Render_GetWorkingFrame()
{
	return WorkingFrame;
}

void Render_Initialize()
{
	UploadingFrame = &FrameBuffers[0];
	WorkingFrame = &FrameBuffers[0];
	IsUploadingFrameDirty = false;
}


void Render_SwapBuffers()
{
	while (IsUploadingFrameDirty)
	{
		
	}

	FrameBuffer* swapTemp = UploadingFrame;
	WorkingFrame = UploadingFrame;
	UploadingFrame = swapTemp;
	IsUploadingFrameDirty = true;
}


void Render_Present()
{
	if (!IsUploadingFrameDirty)
		return;

	Render_UploadFrame(UploadingFrame);
	IsUploadingFrameDirty = false;
}