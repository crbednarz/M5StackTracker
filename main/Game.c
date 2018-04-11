#include "Game.h"
#include "Render.h"
#include "Fluid.h"
#include "Input.h"
#include <string.h>
#include <stdio.h>

static FrameBuffer Frame;

static void Game_AddFluid(Game* game, int16_t x, int16_t y, int radius)
{
	size_t index = game->FluidCount;
	game->Fluids[index].X = x;
	game->Fluids[index].Y = y;
	game->Fluids[index].VelocityX = 0;
	game->Fluids[index].VelocityY = 0;
	game->Fluids[index].Radius = radius;
	
	game->FluidCount++;
}


void Game_Initialize(Game* game)
{
	memset(game, 0, sizeof(Game));
	Game_AddFluid(game, 20, 20, 5);
}


void Game_Update(Game* game)
{
	Input_Update();

	if (game->FluidCount > 0)
	{
		int velocityX = 0;
		int velocityY = 0;
		if (Input_IsKeyDown(GAMEPAD_LEFT))
			velocityX += 1;
		
		if (Input_IsKeyDown(GAMEPAD_RIGHT))
			velocityX -= 1;

		if (Input_IsKeyDown(GAMEPAD_UP))
			velocityY -= 1;
		
		if (Input_IsKeyDown(GAMEPAD_DOWN))
			velocityY += 1;

		game->Fluids[0].VelocityX = velocityX * 2;
		game->Fluids[0].VelocityY = velocityY * 2;
	}
	Fluid_Update(game->Fluids, game->FluidCount);
}


void Game_Render(const Game* game)
{
	World_Render(&game->World, &Frame);
	Fluid_Render(game->Fluids, game->FluidCount, &Frame);
	Render_DrawFrame(&Frame);
}