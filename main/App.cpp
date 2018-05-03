#include "App.hpp"


App::App()
{

}


void App::update()
{
	_gamepad.poll();
}


void App::render(InterlacedFrame& frame)
{
	uint8_t color = 0;
	for (int i = 0; i < 8; i++)
		if (_gamepad.isKeyDown(static_cast<Gamepad::Key>(1 << i)))
			color |= (1 << i);

	for (int y = 0; y < frame.rows(); y += frame.rowStride())
	{
		for (int x = 0; x < frame.columns(); x++)
		{
			frame.color(x, y) = Color(color, color, color);
		}
	}
}