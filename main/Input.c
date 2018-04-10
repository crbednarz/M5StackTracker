#include "Input.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "PeripheralConfig.h"
#include "driver/i2c.h"
#include "I2C.h"
#include <stdio.h>


#define GAMEBOY_ADDRESS 0x88

static uint8_t KeyState;


void Input_Initialize()
{
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = BIT(INPUT_PIN_INT);
    gpioConfig.mode         = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en   = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type    = GPIO_PIN_INTR_DISABLE;
    gpio_config(&gpioConfig);

}


void Input_Update()
{
    if (gpio_get_level(INPUT_PIN_INT) == 0)
    {
        I2C_ReadBytes(GAMEBOY_ADDRESS, &KeyState, 1);
        KeyState = ~KeyState;
    }
}


bool Input_IsKeyDown(InputKey key)
{
    return (KeyState & key) != 0;
}