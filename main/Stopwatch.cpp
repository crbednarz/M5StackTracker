#include "Stopwatch.hpp"
#include <assert.h>


Stopwatch::Stopwatch() :
	_totalElapsedTime(0),
	_lastStartTime(0),
	_recordCount(0),
	_isRecording(false)
{

}


void Stopwatch::start()
{
	// Stopwatch must not be recording in order to call "Start"
	assert(!_isRecording);

	_lastStartTime = xTaskGetTickCount();
	_isRecording = true;
}


void Stopwatch::stop()
{
	// Stopwatch must be recording in order to call "End"
	assert(_isRecording);
	
	
	TickType_t currentTime = xTaskGetTickCount();
	_totalElapsedTime += currentTime - _lastStartTime;
	_recordCount++;
	_isRecording = false;
}


void Stopwatch::reset()
{
	_isRecording = false;
	_totalElapsedTime = 0;
	_recordCount = 0;
}


float Stopwatch::averageTime() const
{
	return (_totalElapsedTime / (float)_recordCount) / configTICK_RATE_HZ;
}