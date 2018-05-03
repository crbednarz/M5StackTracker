#pragma once


#include "InterlacedFrame.hpp"
#include "Gamepad.hpp"


class App
{
public:
	App();

	void update();
	void render(InterlacedFrame& frame);

private:
	Gamepad _gamepad;
	
};
