#include "Gamepad.hpp"
#include "PeripheralConfig.hpp"
#include "I2CDevice.hpp"


Gamepad::Gamepad() :
	_keyStates(0)
{
	gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = BIT(INPUT_INT);
    gpioConfig.mode         = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en   = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type    = gpio_int_type_t(GPIO_PIN_INTR_DISABLE);
    gpio_config(&gpioConfig);
}


void Gamepad::poll()
{
	I2CDevice gamepad(GAMEBOY_ADDRESS);

	if (gpio_get_level(INPUT_INT) == 0)
    {
        gamepad.read(gsl::span<uint8_t>(&_keyStates, 1));
        _keyStates = ~_keyStates;
    }
}


bool Gamepad::isKeyDown(Key key)
{
	return (_keyStates & static_cast<uint8_t>(key)) != 0;
}
