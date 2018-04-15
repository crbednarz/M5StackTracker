#include "World.h"
#include <string.h>


void World_Render(const World* world, FrameBuffer* frameBuffer)
{
	memset(frameBuffer->Data, 0, FRAME_BYTES);
}
