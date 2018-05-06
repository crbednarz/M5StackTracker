#pragma once

#include "PeripheralConfig.hpp"
#include "rom/lldesc.h"
#include "gsl.hpp"


class Display
{
public:
	Display() = delete;

	static constexpr size_t width();
	static constexpr size_t height();

	static void initialize();
	static void writeCommand(uint8_t command);
	static void writeData(uint8_t data);
	static void writeData(gsl::span<const uint8_t> data);

private:
	static void setupSpi();
	static void setupPins();
	static void sendInitialize();

	enum class MessageMode
	{
		Data = 1,
		Command = 0
	};

	static void write(gsl::span<const uint8_t> data, MessageMode mode);

	static lldesc_t* _dmaDescription;
};


constexpr size_t Display::width()
{
	return DISPLAY_WIDTH;
}


constexpr size_t Display::height()
{
	return DISPLAY_HEIGHT;
}

