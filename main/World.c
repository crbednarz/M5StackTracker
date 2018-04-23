#include "World.h"
#include <string.h>
#include <stdbool.h>


void World_Render(const World* world, FrameBuffer* frameBuffer)
{
	const int TILE_SIZE = 32;
	
	memset(frameBuffer->Data, 0, FRAME_BYTES);


	/*
	for (int y = frameBuffer->RowOffset; y < DISPLAY_HEIGHT; y += RENDER_ROW_STRIDE)
	{
		for (int x = 0; x < DISPLAY_WIDTH; x++)
		{
			int tileX = (y * 2 + x) / (2 * TILE_SIZE);
			int tileY = (y * 2 - x) / (2 * TILE_SIZE);

			bool shouldFill = false;

			shouldFill = (tileX + tileY) % 2 == 0;

			
			if (shouldFill)
				frameBuffer->Data[Render_FrameDataIndex(x, y)] = Render_CreateColor(0xAF, 0, 0xFF);
		}
	}*/
}
