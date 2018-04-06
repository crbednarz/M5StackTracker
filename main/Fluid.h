#ifndef FLUID_H
#define FLUID_H


#include <stdint.h>
#include <stddef.h>
#include "Render.h"


typedef struct
{
	int16_t X;
	int16_t Y;
	int16_t Radius;
	
} Fluid;

typedef struct
{
	const Fluid* Fluids;
	size_t FluidsLength;
	
} CompiledFluidRender;


void Fluid_BuildRender(const Fluid* fluids, size_t length, CompiledFluidRender* compiledRender);

void Fluid_Render(const CompiledFluidRender* compiledRender, FrameLine* frameLine);

void Fluid_Update(Fluid* fluids, size_t length);

#endif