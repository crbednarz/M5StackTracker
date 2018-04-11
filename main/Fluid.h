#ifndef FLUID_H
#define FLUID_H


#include <stddef.h>
#include <stdint.h>
#include "Render.h"


typedef struct
{
	int16_t X;
	int16_t Y;
	int16_t VelocityX;
	int16_t VelocityY;
	int16_t Radius;
	
} Fluid;


void Fluid_Render(const Fluid* fluids, size_t length, FrameBuffer* frameBuffer);

void Fluid_Update(Fluid* fluids, size_t length);


#endif