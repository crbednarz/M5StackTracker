#include "App.hpp"


App::App()
{

}


void App::update()
{

}


void App::render(InterlacedFrame& frame)
{
	for (int y = 0; y < frame.rows(); y += frame.rowStride())
	{
		for (int x = 0; x < frame.columns(); x++)
		{
			frame.color(x, y) = Color();
		}
	}
}