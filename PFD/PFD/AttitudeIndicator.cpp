#include "AttitudeIndicator.h"
#include <iostream>
#include <string>

// Constants for attitude indicator
constexpr float PITCH_RANGE = 30.f;
constexpr float SCREEN_SCALE = 3.f;
const sf::Color BLUE = { 0, 153, 204 };
const sf::Color BROWN = { 153, 102, 51 };

AttitudeIndicator::AttitudeIndicator(const sf::Vector2f& center, float radius)
	:m_center(center), m_radius(radius), m_diameter(radius * 2.f)
{
	// Create an offscreen texture with anti-aliasing so lines are draw into it smooth
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	m_renderTexture.create
	(
		static_cast<unsigned int>(std::round(radius * 2)),
		static_cast<unsigned int>(std::round(radius * 2)),
		settings
	);

	// Initialize font as arial
	if (!m_font.loadFromFile("../../assets/fonts/arial.ttf"))
	{
		std::cout << "Failed to load font";
	}
}

void AttitudeIndicator::draw(sf::RenderWindow& window, const FlightData& plane)
{
	// Clear previous texture on canvas
	m_renderTexture.clear();

	// Pixels per degree of pitch
	float pixelsPerPitch = m_radius / PITCH_RANGE;

	// Calculate how much pixel movement based on plane's shift
	float pitchOffset = plane.pitch * pixelsPerPitch;

	// Call helper functions to draw
	drawSkyGround(plane, pitchOffset);
	drawHorizon(plane,  pitchOffset);
	drawPitchLadder(plane, pitchOffset, pixelsPerPitch);
	drawRollIndicator(plane);
	
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
	
	drawAircraftSymbol(window);
	drawRollPointer(window);

}


void AttitudeIndicator::drawSkyGround(const FlightData& plane, float pitchOffset)
{

	// Create sky object larger than canvas to account for turning
	sf::RectangleShape sky(sf::Vector2f{ m_diameter * SCREEN_SCALE, m_diameter * SCREEN_SCALE });

	// Set sky color to blue
	sky.setFillColor(BLUE);

	// Set sky's origin to the center of itself
	sky.setOrigin(m_diameter * 1.5f, m_diameter * 1.5f);

	// Set sky's position to center of the attitude indicator
	sky.setPosition(m_radius, m_radius);

	// Rotate sky by degrees of roll
	sky.setRotation(plane.roll);

	// Create ground object larger than canvas to account for turning
	sf::RectangleShape ground(sf::Vector2f{ m_diameter * SCREEN_SCALE,m_diameter * SCREEN_SCALE });

	// Set ground color to brown
	ground.setFillColor(BROWN);

	// Set ground origin to it's center
	ground.setOrigin(m_diameter * 1.5f, 0.f);

	// Set ground position to center of canvas and move it down based on pitch
	ground.setPosition(m_radius, m_radius + pitchOffset);

	// Rotate ground by degrees of roll
	ground.setRotation(plane.roll);

	// Draw sky and ground to texture
	m_renderTexture.draw(sky);
	m_renderTexture.draw(ground);

}

void AttitudeIndicator::drawHorizon(const FlightData& plane, float pitchOffset)
{
	// Create horizon line
	sf::RectangleShape horizon(sf::Vector2f{ m_diameter * SCREEN_SCALE, m_radius * 0.006f });

	// Set color to white, origin to its center, and position to center of canvas
	horizon.setFillColor(sf::Color::White);
	horizon.setOrigin(m_diameter * 1.5f, m_radius * 0.003f);
	horizon.setPosition(m_radius, m_radius + pitchOffset);
	horizon.setRotation(plane.roll);

	// Draw horizon line to texture
	m_renderTexture.draw(horizon);
}

void AttitudeIndicator::drawPitchLadder(const FlightData& plane, float pitchOffset, float pixelsPerPitch)
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
		float tickWidth = (angle % 10 == 0) ? m_diameter * 0.3f : m_diameter * 0.15f;
		
		// Guarantee tick height is atleast 1 pixel, screen can get too small
		float tickHeight = std::max(m_radius * 0.004f, 1.f);

		// Create tick mark, set color, and set it's origin to it's center
		sf::RectangleShape tick(sf::Vector2f{ tickWidth, tickHeight });
		tick.setFillColor(sf::Color::White);
		tick.setOrigin(tickWidth * 0.5f, m_radius * 0.002f);

		// Calculate position of tick based on where horizon is and pixels per angle
		float tickY = (m_radius + pitchOffset) - (angle * pixelsPerPitch);
		tick.setPosition(m_radius, tickY);

		// Only label tick at increments of 10 degrees
		if (angle % 10 == 0) 
		{
			// Create text object to display the number
			sf::Text label;
			label.setFont(m_font);
			label.setCharacterSize(static_cast<unsigned int>(m_radius * 0.036f));
			label.setFillColor(sf::Color::White);
			label.setString(std::to_string(std::abs(angle)));

			// Struct to hold all values of text box size which is a rectangle
			sf::FloatRect bounds = label.getLocalBounds();

			// To find origin, account for padding from left of the text and top of the text
			label.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);

			// Offset for x positioning based on starting at the center of the tick
			float labelOffset = (tickWidth * 0.5f) + (bounds.width / 2.f) + (m_radius * 0.01f);

			// Draw left label
			label.setPosition(m_radius - labelOffset, tickY);
			m_renderTexture.draw(label, rollTransform);

			// Draw right label
			label.setPosition(m_radius + labelOffset, tickY);
			m_renderTexture.draw(label, rollTransform);
		}

		// Draw tick to be rotated around center
		m_renderTexture.draw(tick,rollTransform);
	}
}

void AttitudeIndicator::drawAircraftSymbol(sf::RenderWindow& window)
{
	// Define wing size variables based on radius
	float wingWidth = m_radius * 0.4f;
	float wingHeight = m_radius * 0.03f;
	float offset = m_radius * 0.25f;
	float verticalOffset = m_radius * 0.05f;
	float tabHeight = m_radius * 0.10f;

	// Create left wing, position according to x offset and slightly above center of ai
	sf::RectangleShape leftWing(sf::Vector2f{ wingWidth, wingHeight });
	leftWing.setFillColor(sf::Color::Yellow);
	leftWing.setOrigin(wingWidth, wingHeight / 2.f);
	leftWing.setPosition(m_center.x - offset, m_center.y - verticalOffset);

	// Create right wing, position according to x offset and slightly above center of ai
	sf::RectangleShape rightWing(sf::Vector2f{ wingWidth, wingHeight });
	rightWing.setFillColor(sf::Color::Yellow);
	rightWing.setOrigin(0.f, wingHeight / 2.f);
	rightWing.setPosition(m_center.x + offset, m_center.y - verticalOffset);

	// Create hollow circle for center circle
	float dotRadius = m_radius * 0.01f;
	sf::CircleShape dot(dotRadius);
	dot.setFillColor(sf::Color::Transparent);
	dot.setOutlineColor(sf::Color::Yellow);
	dot.setOutlineThickness(m_radius * 0.004f);
	dot.setOrigin(dotRadius, dotRadius);
	dot.setPosition(sf::Vector2f{m_center.x,m_center.y - verticalOffset });

	// Left wing tab
	sf::RectangleShape leftTab(sf::Vector2f{ wingHeight, tabHeight });
	leftTab.setFillColor(sf::Color::Yellow);
	leftTab.setOrigin(wingHeight, 0.f);
	leftTab.setPosition(m_center.x - offset, m_center.y - verticalOffset);

	// Right wing tab 
	sf::RectangleShape rightTab(sf::Vector2f{ wingHeight, tabHeight });
	rightTab.setFillColor(sf::Color::Yellow);
	rightTab.setOrigin(0.f, 0.f);
	rightTab.setPosition(m_center.x + offset, m_center.y - verticalOffset);

	// Draw the wings and its tabs directly onto the window
	window.draw(leftWing);
	window.draw(rightWing);
	window.draw(leftTab);
	window.draw(rightTab);
	window.draw(dot);
}

void AttitudeIndicator::drawRollIndicator(const FlightData& plane)
{
	// Create point of rotation to be around center of attitude indicator
	sf::Transform rollTransform;
	rollTransform.rotate(plane.roll, m_radius, m_radius);

	// Follow standard tick mark angle increments
	int tickAngles[] = { -60, -45, -30, -20, -10, 10, 20, 30, 45, 60 };

	
	for (int angle : tickAngles)
	{
		// Convert angle to radians
		float radians = (angle - 90) * (3.14159f / 180.f);

		// Calculate the distance from the center to the edge, then add the center position to it to get x,y coordinates
		float x = m_radius + (m_radius * std::cos(radians));
		float y = m_radius + (m_radius * std::sin(radians));

		// Make the higher angle tickers taller and lower angle ticks smaller
		float tickHeight = (angle >= 30 || angle <= -30) ? m_radius * 0.05f : m_radius * 0.03f;
		float tickWidth = m_radius * 0.004f;

		// Create tick mark, set origin to it's top center 
		sf::RectangleShape tick(sf::Vector2f{ tickWidth, tickHeight });
		tick.setFillColor(sf::Color::White);
		tick.setOrigin(tickWidth / 2.f, 0.f);
		tick.setPosition(x, y);

		// Rotate tick according to angle
		tick.setRotation(angle);

		// Draw tick with rotation onto render texture
		m_renderTexture.draw(tick, rollTransform);
	}
}

void AttitudeIndicator::drawRollPointer(sf::RenderWindow& window)
{
	// Create triangle 
	sf::ConvexShape triangle;
	triangle.setPointCount(3);

	// Do half width as we use 2 points for the base of the triangle
	float halfWidth = m_radius * 0.04f;
	float height = m_radius * 0.05f;

	triangle.setPoint(0, sf::Vector2f{ -halfWidth, -height }); // Top left
	triangle.setPoint(1, sf::Vector2f{ halfWidth, -height });  // Top right
	triangle.setPoint(2, sf::Vector2f{ 0.f,        0.f });     // Tip of triangle
	triangle.setFillColor(sf::Color::White);                     
	triangle.setPosition(m_center.x, m_center.y - m_radius);   // Place right above the center outside ai

	// Draw directly on window as it won't rotate
	window.draw(triangle);

}