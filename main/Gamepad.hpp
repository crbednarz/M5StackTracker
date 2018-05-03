#pragma once


#include <stdint.h>


class Gamepad
{
public:
	enum class Key : uint8_t
	{
		UP      = 1 << 0,
		DOWN    = 1 << 1,
		LEFT    = 1 << 2,
		RIGHT	= 1 << 3,
		A		= 1 << 4,
		B		= 1 << 5,
		SELECT  = 1 << 6,
		START	= 1 << 7 
	};

	Gamepad();

	void poll();

	bool isKeyDown(Key key);

private:
	uint8_t _keyStates;

};
