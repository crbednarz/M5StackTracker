#include "App.hpp"


App::App()
{

}


void App::update()
{
	_gamepad.poll();
	_visualizer.update();
}



void App::render(InterlacedFrame& frame)
{
	frame.clear(Color(0, 0, 0));
	_visualizer.draw(frame);
}