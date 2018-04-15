#include "Render.h"
#include "ILI9341.h"
#include <stdbool.h>


static FrameBuffer FrameBuffers[RENDER_ROW_STRIDE];
static FrameBuffer* UploadingFrame;
static FrameBuffer* WorkingFrame;

static volatile bool IsUploadingFrameDirty = false;

static void Render_UploadFrame(const FrameBuffer* frameBuffer)
{
	Display_WriteCommand(ILI9341_CASET);
	uint8_t caset[4] = { 0, 0, (DISPLAY_WIDTH >> 8) & 0xFF, DISPLAY_WIDTH & 0xFF};
	Display_WriteDataArray(caset, 4);
	
	for (int y = 0; y < FRAME_ROWS; y++)
	{
		int displayRow = y * RENDER_ROW_STRIDE + frameBuffer->RowOffset;
		Display_WriteCommand(ILI9341_PASET);
		uint8_t paset[4] = 
		{ 
			(displayRow >> 8) & 0xFF, 
			displayRow & 0xFF, 
			(DISPLAY_HEIGHT >> 8) & 0xFF, 
			DISPLAY_HEIGHT & 0xFF
		};
		Display_WriteDataArray(paset, 4);

		Display_WriteCommand(ILI9341_RAMWR);

		Display_WriteDataArray((uint8_t*)&UploadingFrame->Data[DISPLAY_WIDTH * y], DISPLAY_WIDTH * DISPLAY_BYTES_PER_PIXEL);
	}
}


FrameBuffer* Render_GetWorkingFrame()
{
	return WorkingFrame;
}

void Render_Initialize()
{
	for (int i = 0; i < RENDER_ROW_STRIDE; i++)
	{
		FrameBuffers[i].RowOffset = i;
	}

	UploadingFrame = &FrameBuffers[0];
	WorkingFrame = &FrameBuffers[1];
	IsUploadingFrameDirty = false;
}


void Render_SwapBuffers()
{
	while (IsUploadingFrameDirty)
	{
		
	}

	FrameBuffer* swapTemp = WorkingFrame;
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