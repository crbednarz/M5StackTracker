#include "Fluid.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


typedef struct
{
	int Top;
	int Bottom;
	int Left;
	int Right;
} DrawBox;

static DrawBox Fluid_CreateDrawBox(int x, int y, int radius)
{
	DrawBox drawBox =
	{
		.Top = y - radius,
		.Bottom = y + radius + 1,
		.Left = x - radius,
		.Right = x + radius + 1	
	};

	if (drawBox.Left < 0)
		drawBox.Left = 0;
	
	if (drawBox.Right > DISPLAY_WIDTH)
		drawBox.Right = DISPLAY_WIDTH;
	
	if (drawBox.Top < 0)
		drawBox.Top = 0;

	if (drawBox.Bottom > DISPLAY_HEIGHT)
		drawBox.Bottom = DISPLAY_HEIGHT;


	return drawBox;
}


static void Fluid_DrawFluid(FrameBuffer* frameBuffer, int fluidX, int fluidY, int radius, uint16_t color)
{
	DrawBox drawBox = Fluid_CreateDrawBox(fluidX, fluidY, radius);
	int radiusSquared = radius * radius;
	for (int y = (drawBox.Top & ~frameBuffer->RowOffset) + frameBuffer->RowOffset; y < drawBox.Bottom; y += RENDER_ROW_STRIDE)
	{
		for (int x = drawBox.Left; x < drawBox.Right; x++)
		{
			int xDiff = x - fluidX;
			int yDiff = y - fluidY;
			if (radiusSquared < (xDiff * xDiff + yDiff * yDiff))
				continue;
			int drawY = y / RENDER_ROW_STRIDE;
			frameBuffer->Data[drawY * DISPLAY_WIDTH + x] = color;
		}
	}
}


void Fluid_Initialize()
{

}


void Fluid_Render(const Fluid* fluids, size_t length, FrameBuffer* frameBuffer)
{
	for (int i = 0; i < length; i++)
		Fluid_DrawFluid(frameBuffer, fluids[i].X, fluids[i].Y, fluids[i].Radius, Render_CreateColor(0x70, 0x70, 0x70));

	
	for (int i = 0; i < length; i++)
		Fluid_DrawFluid(frameBuffer, fluids[i].X, fluids[i].Y - 3, fluids[i].Radius * 7 / 8, Render_CreateColor(0xA0, 0xA0, 0xA0));


	for (int i = 0; i < length; i++)
		Fluid_DrawFluid(frameBuffer, fluids[i].X, fluids[i].Y - 5, fluids[i].Radius * 6 / 8, Render_CreateColor(0xC0, 0xC0, 0xC0));
	
	
	for (int i = 0; i < length; i++)
		Fluid_DrawFluid(frameBuffer, fluids[i].X, fluids[i].Y - 7, fluids[i].Radius * 4 / 8, Render_CreateColor(0xD8, 0xD8, 0xD8));
	
}


void Fluid_Update(Fluid* fluids, size_t length)
{
	for (int i = 0; i < length; i++)
	{
		Fluid* fluid = &fluids[i];
		fluid->X += fluid->VelocityX / 8;
		fluid->Y += fluid->VelocityY / 8;

		if (fluid->X < fluid->Radius)
		{
			fluid->X = fluid->Radius;
			fluid->VelocityX = 0;
		}

		if (fluid->Y < fluid->Radius)
		{
			fluid->Y = fluid->Radius;
			fluid->VelocityY = 0;
		}

		if (fluid->X + fluid->Radius > DISPLAY_WIDTH)
		{
			fluid->X = DISPLAY_WIDTH - fluid->Radius;
			fluid->VelocityX = 0;
		}
		
		if (fluid->Y + fluid->Radius > DISPLAY_HEIGHT)
		{
			fluid->Y = DISPLAY_HEIGHT - fluid->Radius;
			fluid->VelocityY = 0;
		}
	}
}