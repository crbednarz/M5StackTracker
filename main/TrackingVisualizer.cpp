#include "TrackingVisualizer.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

void TrackingVisualizer::update()
{

}

void TrackingVisualizer::draw(InterlacedFrame& frame)
{
	glm::mat4 projection = glm::perspective(
		glm::quarter_pi<float>(), 
		static_cast<float>(DISPLAY_WIDTH) / DISPLAY_HEIGHT, 
		25.0f - 8.0f, 
		25.0f + 8.0f);

	glm::mat4 model(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
	//model = glm::rotate(model, rotation, glm::vec3(1.0f, 1.0f, 1.0f));
	
	for (int x = -5; x < 5; x++)
	{
		for (int y = -5; y < 5; y++)
		{
			for (int z = -5; z < 5; z++)
			{
				auto point = glm::project(
					glm::vec3(x, y, z),
					model,
					projection,
					glm::vec4(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT));

				drawPoint(
					frame, 
					glm::ivec2(point.x, point.y), 
					Color((x + 5) * 25, (y + 5) * 25, (z + 5) * 25));
			}
		}
	}

}



void TrackingVisualizer::drawPoint(InterlacedFrame& frame, glm::ivec2 position, Color color)
{
	// Interestingly enough, if we simply round to the nearest row for the interlaced frame,
	// the result looks considerably smoother. Since single pixel draws don't look very good using
	// interlaced rendering.

	//if ((position.y % frame.rowStride()) != frame.rowOffset())
	//	return;

	if (position.x < 0 || position.y < 0 || position.x >= DISPLAY_WIDTH || position.y >= DISPLAY_HEIGHT)
		return;

	frame.color(position.x, position.y) = color;
}
	