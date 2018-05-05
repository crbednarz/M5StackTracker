#pragma once


#include "InterlacedFrame.hpp"
#include "Gamepad.hpp"
#include "TrackingVisualizer.hpp"


class App
{
public:
	App();

	void update();
	void render(InterlacedFrame& frame);

private:
	Gamepad _gamepad;
	TrackingVisualizer _visualizer;
	
};
