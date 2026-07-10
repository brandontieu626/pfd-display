#include "AttitudeIndicator.h"

// Constants for attitude indicator
constexpr float PITCH_RANGE = 30.f;
constexpr float SCREEN_SCALE = 3.f;
const sf::Color BLUE = { 0, 153, 204 };
const sf::Color BROWN = { 153, 102, 51 };

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
	float diameter = m_radius * 2.f; 

	// Pixels per degree of pitch
	float pixelsPerPitch = m_radius / PITCH_RANGE;

	// Calculate how much pixel movement based on plane's shift
	float pitchOffset = plane.pitch * pixelsPerPitch;

	// Call helper functions to draw
	drawSkyGround(plane, diameter,pitchOffset);
	drawHorizon(plane, diameter, pitchOffset);
	drawPitchLadder(plane, diameter, pitchOffset, pixelsPerPitch);
	
	// Set drawn sky, ground, pitch onto render texture
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
	sf::RectangleShape sky(sf::Vector2f{ diameter * SCREEN_SCALE, diameter * SCREEN_SCALE });

	// Set sky color to blue
	sky.setFillColor(BLUE);

	// Set sky's origin to the center of itself
	sky.setOrigin(diameter * 1.5f, diameter * 1.5f);

	// Set sky's position to center of the attitude indicator
	sky.setPosition(m_radius, m_radius);

	// Rotate sky by degrees of roll
	sky.setRotation(plane.roll);

	// Create ground object larger than canvas to account for turning
	sf::RectangleShape ground(sf::Vector2f{ diameter * SCREEN_SCALE,diameter * SCREEN_SCALE });

	// Set ground color to brown
	ground.setFillColor(BROWN);

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
	sf::RectangleShape horizon(sf::Vector2f{ diameter * SCREEN_SCALE, SCREEN_SCALE });

	// Set color to white, origin to its center, and position to center of canvas
	horizon.setFillColor(sf::Color::White);
	horizon.setOrigin(diameter * 1.5f, 1.5f);
	horizon.setPosition(m_radius, m_radius + pitchOffset);
	horizon.setRotation(plane.roll);

	// Draw horizon line to texture
	m_renderTexture.draw(horizon);
}

void AttitudeIndicator::drawPitchLadder(const FlightData& plane, float diameter, float pitchOffset, float pixelsPerPitch)
{
	// Create point of rotation to be around center of attitude indicator
	sf::Transform rollTransform;
	rollTransform.rotate(plane.roll,m_radius,m_radius);

	// Create tick marks between -30 and 30
	for (int angle = -30; angle <= 30; angle += 5)
	{
		// Skip middle tick mark
		if (angle == 0) continue;

		// Make increment of ten tick marks wider
		float tickWidth = (angle % 10 == 0) ? diameter * 0.3f : diameter * 0.15f;

		// Create tick mark, set color, and set it's origin to it's center
		sf::RectangleShape tick(sf::Vector2f{ tickWidth, 2.f });
		tick.setFillColor(sf::Color::White);
		tick.setOrigin(tickWidth * 0.5f, 1.f);

		// Set position of tick based on where the horizon is and pixels per angle
		tick.setPosition(m_radius, (m_radius + pitchOffset) - (angle * pixelsPerPitch));

		// Draw tick to be rotated around center
		m_renderTexture.draw(tick,rollTransform);
	}
}