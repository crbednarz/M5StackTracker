#pragma once

#include <driver/i2c.h>
#include <stddef.h>
#include <stdint.h>


#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

// I2C
#define I2C_PORT I2C_NUM_1
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22

// Display (ILI9341)
#define DISPLAY_MOSI GPIO_NUM_23
#define DISPLAY_CLK GPIO_NUM_18
#define DISPLAY_CS GPIO_NUM_14
#define DISPLAY_DC GPIO_NUM_27
#define DISPLAY_RST GPIO_NUM_33
#define DISPLAY_BCKL GPIO_NUM_32
#define DISPLAY_DMA_CHANNEL 1

// Input (Gameboy FACE)
#define GAMEBOY_ADDRESS 0x88
#define INPUT_INT GPIO_NUM_5