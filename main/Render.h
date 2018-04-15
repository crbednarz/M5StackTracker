#ifndef RENDER_H
#define RENDER_H

#include "Display.h"

#define RENDER_ROW_STRIDE 2
#define FRAME_ROWS (DISPLAY_HEIGHT / RENDER_ROW_STRIDE)
#define FRAME_SIZE (DISPLAY_WIDTH * FRAME_ROWS)
#define FRAME_BYTES (FRAME_SIZE * DISPLAY_BYTES_PER_PIXEL)

typedef struct
{
	uint8_t RowOffset;
	uint16_t Data[FRAME_SIZE];
} FrameBuffer;


inline uint16_t Render_CreateColor(uint8_t r, uint8_t g, uint8_t b)
{
	// Color format is RRRRRGGG_GGGBBBBB (5xR 6xG 5xB)
	r = (r >> 3) & 0b11111;
	g = (g >> 2) & 0b111111;
	b = (b >> 3) & 0b11111;
	
	uint16_t result = ((uint16_t)r << 11) | ((uint16_t)g << 5) | (uint16_t)b;
	result = (result >> 8) | (result << 8);
	return result;
}

FrameBuffer* Render_GetWorkingFrame();

void Render_Initialize();

void Render_SwapBuffers();

void Render_Present();

#endif