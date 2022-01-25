#include "BezierCurve.h"

sf::Vector2f BezierCurve::compute(float t) const
{
	return compute(controlPoints, t);
}

sf::Vector2f BezierCurve::compute(const std::vector<sf::Vector2f>& controlPoints, float t)
{
	if (controlPoints.empty()) throw std::runtime_error("No control points were defined for the Bezier curve.");

	if (controlPoints.size() == 1)
	{
		return controlPoints[0];
	}
	if (controlPoints.size() == 2)
	{
		float x = controlPoints[0].x + (controlPoints[1].x - controlPoints[0].x) * t;
		float y = controlPoints[0].y + (controlPoints[1].y - controlPoints[0].y) * t;

		return { x, y };
	}

	std::vector<sf::Vector2f> tPoints(controlPoints.size() - 1);

	for (int i = 0; i < controlPoints.size() - 1; i++)
	{
		float x = controlPoints[i].x + (controlPoints[i + 1].x - controlPoints[i].x) * t;
		float y = controlPoints[i].y + (controlPoints[i + 1].y - controlPoints[i].y) * t;

		tPoints[i] = { x, y };
	}
	return compute(tPoints, t);
}