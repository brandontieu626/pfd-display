#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"
class AltitudeTape
{
public:
	AltitudeTape(const sf::Vector2f& center, float width, float height);
	void draw(sf::RenderWindow& window, const FlightData& plane);
private:
	void drawTape(const FlightData& plane, float pixelsPerFoot);
	void drawReadout(sf::RenderWindow& window, const FlightData& plane);

	sf::Vector2f m_center;
	float m_width;
	float m_height;
	sf::RenderTexture m_renderTexture;
	sf::Font m_font;
};