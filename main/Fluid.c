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


static DrawBox Fluid_CreateDrawBox(const Fluid fluid)
{
	int x = fluid.X;
	int y = fluid.Y;

	
	if (x < fluid.Radius)
		x = fluid.Radius;
	if (y < fluid.Radius)
		y = fluid.Radius;
	if (x >= DISPLAY_WIDTH - fluid.Radius)
		x = DISPLAY_WIDTH - fluid.Radius;
	if (y >= DISPLAY_HEIGHT - fluid.Radius)
		y = DISPLAY_HEIGHT - fluid.Radius;

	DrawBox drawBox =
	{
		.Top = y - fluid.Radius,
		.Bottom = y + fluid.Radius,
		.Left = x - fluid.Radius,
		.Right = x + fluid.Radius	
	};

	return drawBox;
}


static void Fluid_FillDrawBox(FrameBuffer* frameBuffer, DrawBox drawBox, uint16_t color)
{
	for (int y = drawBox.Top; y < drawBox.Bottom; y++)
	{
		for (int x = drawBox.Left; x < drawBox.Right; x++)
		{
			frameBuffer->Data[y * DISPLAY_WIDTH + x] = color;
		}
	}
}


void Fluid_Render(const Fluid* fluids, size_t length, FrameBuffer* frameBuffer)
{
	for (int i = 0; i < length; i++)
	{
		DrawBox drawBox = Fluid_CreateDrawBox(fluids[i]);
		Fluid_FillDrawBox(frameBuffer, drawBox, 0xFFFFU);
	}
}


void Fluid_Update(Fluid* fluids, size_t length)
{
	for (int i = 0; i < length; i++)
	{
		Fluid* fluid = &fluids[i];
		fluid->X += fluid->VelocityX;
		fluid->Y += fluid->VelocityY;
	}
}