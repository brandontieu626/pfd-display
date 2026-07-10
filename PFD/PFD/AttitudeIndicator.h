#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"

class AttitudeIndicator
{
public:
	AttitudeIndicator(const sf::Vector2f& center, float radius); 
	void draw(sf::RenderWindow& window, const FlightData& plane); // draws the horizon, sky, and ground
private:
	void drawSkyGround(const FlightData& plane, float diameter, float pitchOffset);
	void drawHorizon(const FlightData& plane, float diameter, float pitchOffset);
	void drawPitchLadder(const FlightData& plane, float diameter, float pitchOffset, float pixelsPerPitch);
	
	sf::Vector2f m_center;	           // center of attitude indicator
	float m_radius;					   // radius of attitude indicator	
	sf::RenderTexture m_renderTexture; // offscreen canvas for the sky and ground
};