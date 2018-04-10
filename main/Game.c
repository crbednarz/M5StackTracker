#include "Game.h"
#include "Render.h"
#include "Fluid.h"
#include "Input.h"
#include <string.h>
#include <stdio.h>


static void Game_AddFluid(Game* game, int16_t x, int16_t y, int radius)
{
	size_t index = game->FluidCount;
	game->Fluids[index].X = x;
	game->Fluids[index].Y = y;
	game->Fluids[index].Radius = radius;
	
	game->FluidCount++;
}


void Game_Initialize(Game* game)
{
	memset(game, 0, sizeof(Game));
	Game_AddFluid(game, 0, DISPLAY_HEIGHT / 2, 5);
}


void Game_Update(Game* game)
{
	Fluid_Update(game->Fluids, game->FluidCount);
	Input_Update();

	
}


void Game_Render(const Game* game)
{
	Render_ResetFill();
	
	FrameLine frameLine = { 0 };
	
	CompiledFluidRender compiledFluidRender;
	
	Fluid_BuildRender(game->Fluids, game->FluidCount, &compiledFluidRender);
	
	
	for (int y = 0; y < DISPLAY_HEIGHT; y++)
	{
		World_Render(&game->World, &frameLine);
		Fluid_Render(&compiledFluidRender, &frameLine);
		Render_PushLine(&frameLine);
		frameLine.Row++;
	}
}