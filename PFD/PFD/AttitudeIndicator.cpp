#include "AttitudeIndicator.h"

AttitudeIndicator::AttitudeIndicator(const sf::Vector2f& center, float radius)
	:m_center(center), m_radius(radius)
{
	//Create an offscreen texture for holding the sky and ground
	m_renderTexture.create
	(
		static_cast<unsigned int>(std::round(radius * 2)),
		static_cast<unsigned int>(std::round(radius * 2))
	);
}

void AttitudeIndicator::draw(sf::RenderWindow& window, const FlightData& plane)
{
	// Clear previous texture on canvas
	m_renderTexture.clear();

	// Diameter of renderTexture
	float diameter = static_cast<float>(m_renderTexture.getSize().x); 

	// Shift the horizon based on pitch, 10 degrees x 3 = 30 pixels shift
	float pitchOffset = plane.pitch * 3.f;

	drawSkyGround(window, plane, diameter,pitchOffset);
	drawHorizon(window, plane, diameter, pitchOffset);

	m_renderTexture.display();

	sf::CircleShape ai_screen(m_radius);
	ai_screen.setPointCount(128);
	ai_screen.setTexture(&m_renderTexture.getTexture());
	ai_screen.setOrigin(m_radius, m_radius);
	ai_screen.setPosition(m_center);

	window.draw(ai_screen);

}


void AttitudeIndicator::drawSkyGround(sf::RenderWindow& window, const FlightData& plane, float diameter, float pitchOffset)
{

	// Create sky object larger than canvas to account for turning
	sf::RectangleShape sky(sf::Vector2f{ diameter * 3.f, diameter * 3.f });

	// Set sky color to blue
	sky.setFillColor(sf::Color{ 0,153,204 });

	// Set sky's origin to the center of itself
	sky.setOrigin(diameter * 1.5f, diameter * 1.5f);

	// Set sky's position to center of the attitude indicator
	sky.setPosition(m_radius, m_radius);

	// Rotate sky by degrees of roll
	sky.setRotation(plane.roll);

	// Create ground object larger than canvas to account for turning
	sf::RectangleShape ground(sf::Vector2f{ diameter * 3.f,diameter * 3.f });

	// Set ground color to brown
	ground.setFillColor(sf::Color{ 153,102,51 });

	// Set ground origin to it's center
	ground.setOrigin(diameter * 1.5f, 0.f);

	// Set ground position to center of canvas and move it down based on pitch
	ground.setPosition(m_radius, m_radius + pitchOffset);

	// Rotate ground by degrees of roll
	ground.setRotation(plane.roll);

	// Draw sky and ground to texture
	m_renderTexture.draw(sky);
	m_renderTexture.draw(ground);

}

void AttitudeIndicator::drawHorizon(sf::RenderWindow& window, const FlightData& plane, float diameter, float pitchOffset)
{
	sf::RectangleShape horizon(sf::Vector2f{ diameter * 3.f, 3.f });
	horizon.setFillColor(sf::Color::White);
	horizon.setOrigin(diameter * 1.5f, 1.5f);
	horizon.setPosition(m_radius, m_radius + pitchOffset);
	horizon.setRotation(plane.roll);

	// Draw horizon line to texture
	m_renderTexture.draw(horizon);
}