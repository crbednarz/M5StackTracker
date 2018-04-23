#ifndef WORLD_H
#define WORLD_H

#include "Render.h"
#include <stdint.h>

#define WORLD_SIZE 64


typedef struct
{
    uint8_t Heights[WORLD_SIZE * WORLD_SIZE];
} World;


void World_Render(const World* world, FrameBuffer* frameBuffer);
	

#endif