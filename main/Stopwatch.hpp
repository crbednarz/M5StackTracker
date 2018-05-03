#pragma once


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>


class Stopwatch
{
public:
	Stopwatch();

	void start();
	
	void stop();
	
	void reset();

	float averageTime() const;

	inline int recordCount() const;

private:
	TickType_t _totalElapsedTime;
	TickType_t _lastStartTime;
	uint16_t _recordCount;
	bool _isRecording;

};


int Stopwatch::recordCount() const
{
	return _recordCount;
}