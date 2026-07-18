#include "AirspeedTape.h"
#include <iostream>
#include <cmath>

constexpr float AIRSPEED_RANGE = 100;
constexpr float TICK_INTERVAL = 5;
AirspeedTape::AirspeedTape(const sf::Vector2f& center, float width, float height):
	m_center(center),m_width(width),m_height(height)
{
	// Set antialiasing to 8 for clearer lines
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	// Create an offscreen texture with anti-aliasing so lines are draw into it smooth
    // Offscreen texture for the altitude tape
	m_renderTexture.create(
		static_cast<unsigned int>(std::round(width)),
		static_cast<unsigned int>(std::round(height)),
		settings
	);

	// Initialize font as arial
	if (!m_font.loadFromFile("../../assets/fonts/arial.ttf"))
	{
		std::cout << "Failed to load font";
	}
}

void AirspeedTape::draw(sf::RenderWindow& window, const FlightData& plane) 
{
	m_renderTexture.clear();
	float pixelsPerKnot = m_height / AIRSPEED_RANGE;

	drawTicks(window, plane, pixelsPerKnot); // fill render texture
	drawTape(window);                        // draw tape onto the window
	drawReadout(window, plane);              // draw read out onto window
	drawKts(window);                         // fix overlay on top
}

void AirspeedTape::drawTape(sf::RenderWindow& window)
{
	// Create tape rectangle and set our render texture on top of it
	sf::RectangleShape tape(sf::Vector2f{ m_width, m_height });
	tape.setTexture(&m_renderTexture.getTexture());
	tape.setOutlineColor(sf::Color::White);
	tape.setOutlineThickness(m_width * 0.02f);
	tape.setOrigin(m_width/2.f , m_height/2.f);
	tape.setPosition(m_center);
	window.draw(tape);
}

void AirspeedTape::drawTicks(sf::RenderWindow& window, const FlightData& plane, float pixelsPerKnot)
{
	// Minimum value of 0 since speed can't be negative
	int startSpeed = std::max(0, (static_cast<int>(plane.airspeed - AIRSPEED_RANGE / 2) / 10) * 10);


	for (int airspeed = startSpeed; airspeed<= plane.airspeed + (AIRSPEED_RANGE/2); airspeed+=TICK_INTERVAL)
	{
		// Find distance between plane's airspeed and current airspeed tick
		float offset = (airspeed - plane.airspeed) * pixelsPerKnot;

		// Tick's position based on offset of pixels
		float tickY = (m_height / 2.f) - offset;

		// Skip if outside tape
		if (tickY < 0 || tickY > m_height) continue;

		// Draw a longer tick every 10 knots
		bool isMajor = (airspeed % 10 == 0);
		float tickWidth = isMajor ? m_width * 0.4f : m_width * 0.2f;

		// Scale tick height based on height of speed tape
		float tickHeight = m_height * 0.004f;

		// Create tick mark and put origin halfway through tick
		sf::RectangleShape tick(sf::Vector2f(tickWidth, tickHeight));
		tick.setFillColor(sf::Color::White);
		tick.setOrigin(0.f, tickHeight / 2.f);
		tick.setPosition(0.f, tickY);
		m_renderTexture.draw(tick);

		if (isMajor)
		{
			// Create text label containing airspeed
			sf::Text label;
			unsigned int charSize = static_cast<unsigned int>(m_width * 0.20f);
			label.setFont(m_font);
			label.setCharacterSize(charSize);
			label.setFillColor(sf::Color::White);
			label.setString(std::to_string(airspeed));

			// Create bounds object to get actual measurements of textbox with padding
			sf::FloatRect bounds = label.getLocalBounds();
			float labelOffset = m_width * 0.05f;
			label.setOrigin(0.f, bounds.top + (bounds.height / 2.f));
			label.setPosition(tickWidth + labelOffset, tickY);
			m_renderTexture.draw(label);
		}
	}

	m_renderTexture.display();
}

void AirspeedTape::drawReadout(sf::RenderWindow& window, const FlightData& plane)
{
	// Set width of readout to same size as the tape
	float boxWidth = m_width;
	float boxHeight = m_height * 0.08f;

	sf::RectangleShape readOut(sf::Vector2f{ boxWidth,boxHeight });
	readOut.setFillColor(sf::Color::Black);
	readOut.setOutlineColor(sf::Color::Yellow);
	readOut.setOutlineThickness(m_width * 0.02f);
	readOut.setOrigin(boxWidth / 2.f, boxHeight / 2.f);
	readOut.setPosition(m_center);
	window.draw(readOut);

	// Textbox containing the plane's current airspeed
	sf::Text label;
	label.setFont(m_font);
	unsigned int charSize = static_cast<unsigned int>(m_height * 0.04f);
	label.setCharacterSize(charSize);
	label.setFillColor(sf::Color::Green);
	label.setString(std::to_string(static_cast<int>(plane.airspeed)));

	// Find center of textbox including padding
	sf::FloatRect bounds = label.getLocalBounds();
	label.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
	label.setPosition(m_center);
	window.draw(label);

}

void AirspeedTape::drawKts(sf::RenderWindow& window)
{
	// Create text label to show feet
	sf::Text unitsLabel;
	unitsLabel.setFont(m_font);
	unitsLabel.setCharacterSize(static_cast<unsigned int>(m_width * 0.2f));
	unitsLabel.setFillColor(sf::Color::White);
	unitsLabel.setString("KTS");

	// Find center of text and position it slightly above altitude tape
	sf::FloatRect bounds = unitsLabel.getLocalBounds();
	unitsLabel.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
	unitsLabel.setPosition(m_center.x, (m_center.y - m_height / 2.f) - (m_height * 0.03f));
	window.draw(unitsLabel);
}