#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stddef.h>

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define DISPLAY_BYTES_PER_PIXEL 2

void Display_Initialize();

void Display_WriteCommand(uint8_t command);

void Display_WriteData(uint8_t data);

void Display_WriteDataArray(const uint8_t* data, size_t length);


#endif