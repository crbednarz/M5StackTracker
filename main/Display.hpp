#pragma once

#include "PeripheralConfig.hpp"


class Display
{
public:
	Display() = delete;
	
	static constexpr size_t width();
	static constexpr size_t height();

private:

};


size_t Display::width()
{
	return DISPLAY_WIDTH;
}


size_t Display::height()
{
	return DISPLAY_HEIGHT;
}

