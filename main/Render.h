#ifndef RENDER_H
#define RENDER_H

#include "Display.h"

#define RGB_TO_DISPLAY_BYTES(R, G, B) ((((R) >> 3) << 11) | (((G) >> 2) << 5) | ((B) >> 2))
#define FRAME_BUFFER_SIZE DISPLAY_WIDTH * DISPLAY_HEIGHT
#define FRAME_BUFFER_BYTES (FRAME_BUFFER_SIZE * sizeof(uint16_t))


typedef struct
{
	uint16_t Data[DISPLAY_WIDTH * DISPLAY_HEIGHT];
} FrameBuffer;


void Render_DrawFrame(const FrameBuffer* frameBuffer);


#endif