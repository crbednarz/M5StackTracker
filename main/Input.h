#ifndef INPUT_H
#define INPUT_H


#include <stdint.h>
#include <stdbool.h>

typedef enum
{
   GAMEPAD_UP       = 1 << 0,
   GAMEPAD_DOWN     = 1 << 1,
   GAMEPAD_LEFT     = 1 << 2,
   GAMEPAD_RIGHT	= 1 << 3,
   GAMEPAD_A		= 1 << 4,
   GAMEPAD_B		= 1 << 5,
   GAMEPAD_SELECT   = 1 << 6,
   GAMEPAD_START	= 1 << 7 
} InputKey;

void Input_Initialize();

void Input_Update();

bool Input_IsKeyDown(InputKey key);


#endif