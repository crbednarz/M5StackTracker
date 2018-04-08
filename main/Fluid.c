#include "Fluid.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void Fluid_BuildRender(const Fluid* fluids, size_t length, CompiledFluidRender* compiledRender)
{
	compiledRender->Fluids = fluids;
	compiledRender->FluidsLength = length;
	
	for (int i = 0; i < length; i++)
	{
	}
}


void Fluid_Render(const CompiledFluidRender* compiledRender, FrameLine* frameLine)
{
	Fluid fluid;
	uint16_t row = frameLine->Row;
	
	for (int i = 0; i < compiledRender->FluidsLength; i++)
	{
		fluid = compiledRender->Fluids[i];
		if (abs(row - fluid.Y) > fluid.Radius)
			continue;
		
		memset(frameLine->Data, 0xFF, FRAME_LINE_BYTES);
	}
}


void Fluid_Update(Fluid* fluids, size_t length)
{
	for (int i = 0; i < length; i++)
	{
		Fluid* fluid = &fluids[i];

		fluid->Y += 1;

	}
}