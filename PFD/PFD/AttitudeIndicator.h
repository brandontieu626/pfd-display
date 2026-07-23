#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"

class AttitudeIndicator
{
public:
	AttitudeIndicator(const sf::Vector2f& center, float radius); 
	void draw(sf::RenderWindow& window, const FlightData& plane);
private:
	void drawSkyGround(const FlightData& plane, float pitchOffset);
	void drawHorizon(const FlightData& plane, float pitchOffset);
	void drawPitchLadder(const FlightData& plane, float pitchOffset, float pixelsPerPitch);
	void drawAircraftSymbol(sf::RenderWindow& window);
	void drawRollIndicator(const FlightData& plane);
	void drawRollArc(sf::RenderWindow& window);
	void drawRollPointer(sf::RenderWindow& window);
	void drawFlightPathMarker(sf::RenderWindow& window, const FlightData& plane);

	sf::Vector2f m_center;	           // center of attitude indicator
	float m_radius;					   // radius of attitude indicator	
	float m_diameter;
	sf::RenderTexture m_renderTexture; // offscreen canvas for the sky and ground
	sf::Font m_font;                   // font used for the tick marks
};