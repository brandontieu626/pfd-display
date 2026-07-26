#include "AirspeedTape.h"
#include "Band.h"
#include <iostream>
#include <cmath>

constexpr float AIRSPEED_RANGE = 100;
constexpr float AIRSPEED_MAX   = 400;
constexpr float TICK_INTERVAL  = 5;
constexpr float SPEED_STALL    =  60.f;
constexpr float SPEED_CAUTION  = 250.f;
constexpr float SPEED_MAX      = 300.f;

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

	// Clamp the scroll so 400 never goes above the top of the tape.
	// displayPlane is used only for tick/band positioning , readout still uses real plane.
	FlightData displayPlane = plane;
	displayPlane.airspeed = std::min(plane.airspeed, AIRSPEED_MAX - AIRSPEED_RANGE / 2.f);

	drawColorBands(pixelsPerKnot, displayPlane);
	drawTicks(window, displayPlane, pixelsPerKnot); // fill render texture
	drawTape(window);                               // draw tape onto the window
	drawReadout(window, plane);                     // draw read out onto window
	drawKts(window);                                // fix overlay on top
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

	// Ensures that ticks aren't drawn past 400 knots (the max)
	int endSpeed = std::min(static_cast<int>(plane.airspeed + AIRSPEED_RANGE / 2), static_cast<int>(AIRSPEED_MAX));
	for (int airspeed = startSpeed; airspeed<= endSpeed; airspeed+=TICK_INTERVAL)
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

void AirspeedTape::drawColorBands(float pixelsPerKnot, const FlightData& plane)
{
	float bandWidth = m_width * 0.08f;

	// Create array of bands for each speed band
	Band bands[] = {
		{   0.f,        SPEED_STALL,   sf::Color::Red                   },
		{ SPEED_STALL,  SPEED_CAUTION, sf::Color{0, 200, 0, 255}        },
		{ SPEED_CAUTION,SPEED_MAX,     sf::Color{255, 200, 0, 255}      },
		{ SPEED_MAX,    400.f,         sf::Color::Red                   },
	};

	for (auto& b : bands)
	{
		// Convert the band's knot boundaries to Y positions in the render texture.
		// Formula: center - (bandSpeed - currentSpeed) * pixelsPerKnot
		//   - (b.high - airspeed): how many knots above current speed the top boundary is
		//   - multiplying by pixelsPerKnot converts that to pixels
		//   - subtracting from center: positive offset moves UP (smaller Y) since Y increases downward
		// b.high goes into yTop because faster speeds sit higher on the tape (smaller Y)
		// b.low goes into yBottom because slower speeds sit lower on the tape (larger Y)
		float yTop    = (m_height / 2.f) - (b.high - plane.airspeed) * pixelsPerKnot;
		float yBottom = (m_height / 2.f) - (b.low  - plane.airspeed) * pixelsPerKnot;

		// Clamp to the texture bounds — a band edge may calculate outside [0, m_height]
		// if that speed is off the visible tape. max/min pins it to the tape edge.
		yTop    = std::max(0.f, yTop);
		yBottom = std::min(m_height, yBottom);

		// If both edges clamped to the same side the band is fully off-screen — skip it
		if (yBottom <= yTop) continue;

		sf::RectangleShape band(sf::Vector2f(bandWidth, yBottom - yTop));
		band.setFillColor(b.color);
		band.setPosition(0.f, yTop);
		m_renderTexture.draw(band);
	}
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