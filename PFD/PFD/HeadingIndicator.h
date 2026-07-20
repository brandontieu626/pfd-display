#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"
class HeadingIndicator
{
public:
	HeadingIndicator(const sf::Vector2f& center, float radius);
	void draw(sf::RenderWindow& window, const FlightData& plane);
private:
	void drawArc(sf::RenderWindow& window);
	void drawTicks(sf::RenderWindow& window, const FlightData& plane);
	void drawCenterMarker(sf::RenderWindow& window);

	sf::Vector2f m_center;
	float m_radius;
	sf::Font m_font;
};