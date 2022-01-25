#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class BezierCurve
{
public:
	std::vector<sf::Vector2f> controlPoints;

	sf::Vector2f compute(float t) const;

	static sf::Vector2f compute(const std::vector<sf::Vector2f>& controlPoints, float t);
};
