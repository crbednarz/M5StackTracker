#include "Fluid.h"
#include "Gyro.h"
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
{[
	GyroState gyroState = Gyro_ReadState();
	double x = gyroState.X * (1.0 / 32768.0);
	double y = gyroState.Y * (1.0 / 32768.0);
	double z = gyroState.Z * (1.0 / 32768.0);
	
	printf("%f, %f, %f\n", x, y, z);
	fluids[0].Y = (uint16_t)((x * 0.5 + 0.5) * DISPLAY_HEIGHT);
	
	if (fluids[0].Y < 0)
		fluids[0].Y = 0;
	if (fluids[0].Y > DISPLAY_HEIGHT)
		fluids[0].Y = DISPLAY_HEIGHT;
}