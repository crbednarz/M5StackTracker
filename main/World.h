#ifndef WORLD_H
#define WORLD_H

#include "Render.h"
#include <stdint.h>

#define WORLD_WIDTH 64
#define WORLD_HEIGHT 64


typedef struct
{
    uint8_t Heights[WORLD_WIDTH * WORLD_HEIGHT];
} World;


void World_Render(const World* world, FrameBuffer* frameBuffer);
	

#endif