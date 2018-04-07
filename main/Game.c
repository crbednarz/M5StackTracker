#include "Game.h"
#include "Render.h"
#include "Fluid.h"
#include <string.h>


Fluid fluid;
void Game_Initialize(Game* game)
{
	memset(game, 0, sizeof(Game));
	fluid.Y = (DISPLAY_HEIGHT / 2);
}


void Game_Update(Game* game)
{
		
}


void Game_Render(const Game* game)
{
	Render_ResetFill();
	
	FrameLine frameLine = { 0 };
	
	CompiledFluidRender compiledFluidRender;
	fluid.Radius = 5;
	
	Fluid_Update(&fluid, 1);
	Fluid_BuildRender(&fluid, 1, &compiledFluidRender);
	
	
	for (int y = 0; y < DISPLAY_HEIGHT; y++)
	{
		World_Render(&game->World, &frameLine);
		Fluid_Render(&compiledFluidRender, &frameLine);
		Render_PushLine(&frameLine);
		frameLine.Row++;
	}
}