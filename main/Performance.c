#include "Performance.h"
#include <assert.h>

void Stopwatch_Start(Stopwatch* stopwatch)
{
	// Stopwatch must not be recording in order to call "Start"
	assert(!stopwatch->IsRecording);

	stopwatch->LastStartTime = xTaskGetTickCount();
	stopwatch->IsRecording = true;
}


void Stopwatch_End(Stopwatch* stopwatch)
{
	// Stopwatch must be recording in order to call "End"
	assert(stopwatch->IsRecording);
	
	
	TickType_t currentTime = xTaskGetTickCount();
	stopwatch->TotalElapsedTime += currentTime - stopwatch->LastStartTime;
	stopwatch->RecordCount++;
	stopwatch->IsRecording = false;
}


float Stopwatch_AverageTime(const Stopwatch* stopwatch)
{
	return (stopwatch->TotalElapsedTime / (float)stopwatch->RecordCount) / configTICK_RATE_HZ;
}

void Stopwatch_Reset(Stopwatch* stopwatch)
{
	stopwatch->IsRecording = false;
	stopwatch->TotalElapsedTime = 0;
	stopwatch->RecordCount = 0;
}