#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"
class AirspeedTape
{
public:
	AirspeedTape(const sf::Vector2f& center,float width, float height);
	void draw(sf::RenderWindow& window, const FlightData& plane);
private:
	void drawTape(sf::RenderWindow& window);
	void drawTicks(sf::RenderWindow& window, const FlightData& plane, float pixelsPerKnot);
	void drawReadout(sf::RenderWindow& window, const FlightData& plane);
	void drawKts(sf::RenderWindow& window);

	sf::Vector2f m_center;              // Center of airspeed tape
	float m_width;                      // Width of airspeed tape
	float m_height;                     // Height of airspeed tape
	sf::RenderTexture m_renderTexture;  // Texture for airspeed tape
	sf::Font m_font;                    // Font for airspeed tape
};