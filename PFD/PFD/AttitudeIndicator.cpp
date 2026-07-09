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

	// Call helper functions to draw
	drawSkyGround(plane, diameter,pitchOffset);
	drawHorizon(plane, diameter, pitchOffset);
	drawPitchLadder(plane, diameter, pitchOffset);
	
	
	m_renderTexture.display();

	// Create circle for attitude indicator screen
	sf::CircleShape ai_screen(m_radius);
	
	// Increase pixel count
	ai_screen.setPointCount(128);

	// Wrap render texture onto attitude indicator screen
	ai_screen.setTexture(&m_renderTexture.getTexture());
	
	// Set origin to center of itself
	ai_screen.setOrigin(m_radius, m_radius);

	// Set position center of screen
	ai_screen.setPosition(m_center);

	// Draw it onto the window
	window.draw(ai_screen);

}


void AttitudeIndicator::drawSkyGround(const FlightData& plane, float diameter, float pitchOffset)
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

void AttitudeIndicator::drawHorizon(const FlightData& plane, float diameter, float pitchOffset)
{
	// Create horizon line
	sf::RectangleShape horizon(sf::Vector2f{ diameter * 3.f, 3.f });

	// Set color to white, origin to its center, and position to center of canvas
	horizon.setFillColor(sf::Color::White);
	horizon.setOrigin(diameter * 1.5f, 1.5f);
	horizon.setPosition(m_radius, m_radius + pitchOffset);
	horizon.setRotation(plane.roll);

	// Draw horizon line to texture
	m_renderTexture.draw(horizon);
}

void AttitudeIndicator::drawPitchLadder(const FlightData& plane, float diameter, float pitchOffset)
{
	for (int angle = -30; angle <= 30; angle += 5)
	{
		if (angle == 0) continue;

		float tickWidth = (angle % 10 == 0) ? diameter * 0.3f : diameter * 0.15f;
		float pixelsPerDegree = m_radius / 30.f;
		sf::RectangleShape tick(sf::Vector2f{ tickWidth, 2.f });
		tick.setFillColor(sf::Color::White);
		tick.setOrigin(tickWidth * 0.5f, 1.f);
		tick.setPosition(m_radius, (m_radius + pitchOffset) - (angle * pixelsPerDegree));
		tick.setRotation(plane.roll);

		m_renderTexture.draw(tick);
	}
}