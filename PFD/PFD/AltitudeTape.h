#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"
class AltitudeTape
{
public:
	AltitudeTape(const sf::Vector2f& center, float width, float height);
	void draw(sf::RenderWindow& window, const FlightData& plane);
private:
	void drawTape(sf::RenderWindow& window);
	void drawTicks(const FlightData& plane, float pixelsPerFoot);
	void drawReadout(sf::RenderWindow& window, const FlightData& plane);
	void drawFt(sf::RenderWindow& window);

	sf::Vector2f m_center; // Center of the altitude tape
	float m_width;         // Width of altitude tape
	float m_height;        // Height of altitude tape
	sf::RenderTexture m_renderTexture; // Screen texture for the altitude tape
	sf::Font m_font;                   // Font object
};