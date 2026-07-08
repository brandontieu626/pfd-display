#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"

class AttitudeIndicator
{
public:
	AttitudeIndicator(const sf::Vector2f& center, float radius); 
	void draw(sf::RenderWindow& window, const FlightData& data); // draws the horizon, sky, and ground according to pitch

private:
	sf::Vector2f m_center;	           // center of attitude indicator
	float m_radius;					   // radius of attitude indicator	
	sf::RenderTexture m_renderTexture; // offscreen canvas for the sky and ground
};