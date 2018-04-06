#ifndef RENDER_H
#define RENDER_H

#include "Display.h"

#define RGB_TO_DISPLAY_BYTES(R, G, B) ((((R) >> 3) << 11) | (((G) >> 2) << 5) | ((B) >> 2))
#define FRAME_LINE_SIZE DISPLAY_WIDTH
#define FRAME_LINE_BYTES (FRAME_LINE_SIZE * sizeof(uint16_t))

typedef struct
{
	uint16_t Row;
	uint16_t Data[FRAME_LINE_SIZE];
	
} FrameLine;


/// Resets render position and prepares for pushing frame lines.
void Render_ResetFill();

/// Pushes the frame line to the display, ignoring the current Row value.
void Render_PushLine(const FrameLine* frameLine);

#endif