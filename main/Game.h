#ifndef GAME_H
#define GAME_H


#include "World.h"


// Game system acts as the glue to hold together the various subsystem of a single game session.

typedef struct
{	
	World World;
} Game;


void Game_Initialize(Game* game);

void Game_Update(Game* game);

void Game_Render(const Game* game);


#endif