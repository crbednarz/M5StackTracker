#ifndef PERFORMANCE_H
#define PERFORMANCE_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdint.h>


typedef struct
{
	TickType_t TotalElapsedTime;
	TickType_t LastStartTime;
	uint16_t RecordCount;
	bool IsRecording;
} Stopwatch;


void Stopwatch_Start(Stopwatch* stopwatch);

void Stopwatch_End(Stopwatch* stopwatch);

float Stopwatch_AverageTime(const Stopwatch* stopwatch);

void Stopwatch_Reset(Stopwatch* stopwatch);

#endif