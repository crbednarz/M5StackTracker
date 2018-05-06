#pragma once


#include "InterlacedFrame.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "MpuSystem.hpp"

class TrackingVisualizer
{
public:
	TrackingVisualizer();

	void update();

	void draw(InterlacedFrame& frame);

private:
	void drawPoint(InterlacedFrame& frame, glm::ivec2 position, Color color);

	MpuSystem _mpu;
};
