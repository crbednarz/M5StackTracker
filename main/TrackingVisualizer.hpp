#pragma once


#include "InterlacedFrame.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


class TrackingVisualizer
{
public:
	void update();

	void draw(InterlacedFrame& frame);

private:
	void drawPoint(InterlacedFrame& frame, glm::ivec2 position, Color color);

};
