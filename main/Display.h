#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stddef.h>

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320
#define DISPLAY_BYTES_PER_PIXEL 2
#define DISPLAY_SIZE DISPLAY_WIDTH * DISPLAY_HEIGHT
#define DISPLAY_BYTES (DISPLAY_SIZE * DISPLAY_BYTES_PER_PIXEL)


void Display_BeginWrite();

void Display_EndWrite();

void Display_Initialize();

void Display_WriteCommand(uint8_t command);

void Display_WriteData(uint8_t data);

void Display_WriteDataArray(const uint8_t* data, size_t length);


#endif