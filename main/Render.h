#ifndef RENDER_H
#define RENDER_H

#include "Display.h"

#define FRAME_BUFFER_SIZE DISPLAY_WIDTH * DISPLAY_HEIGHT
#define FRAME_BUFFER_BYTES (FRAME_BUFFER_SIZE * sizeof(uint16_t))


typedef struct
{
	uint16_t Data[DISPLAY_WIDTH * DISPLAY_HEIGHT];
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

void Render_DrawFrame(const FrameBuffer* frameBuffer);


#endif