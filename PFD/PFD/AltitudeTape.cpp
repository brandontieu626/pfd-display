#include "AltitudeTape.h"
#include <iostream>
#include <cmath>

constexpr float ALTITUDE_RANGE = 1000.f;
constexpr float TICK_INTERVAL = 100.f;

AltitudeTape::AltitudeTape(const sf::Vector2f& center, float width, float height)
    : m_center(center), m_width(width), m_height(height)
{
    // Create an offscreen texture with anti-aliasing so lines are draw into it smooth
    // Offscreen texture for the altitude bar
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
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

void AltitudeTape::draw(sf::RenderWindow& window, const FlightData& plane)
{
    m_renderTexture.clear(sf::Color::Black);

    float pixelsPerFoot = m_height / ALTITUDE_RANGE;

    drawTape(plane, pixelsPerFoot);

    m_renderTexture.display();
    sf::RectangleShape tape(sf::Vector2f{ m_width, m_height });
    tape.setTexture(&m_renderTexture.getTexture());
    tape.setOrigin(m_width / 2.f, m_height / 2.f);
    tape.setPosition(m_center);
    window.draw(tape);

}

void AltitudeTape::drawTape(const FlightData& plane, float pixelsPerFoot)
{
    // Move half way from plane altitude and round down to nearest 100 to find starting altitude
    int startAltitude = (static_cast<int>(plane.altitude - ALTITUDE_RANGE / 2) / 100) * 100;

    
    for (int altitude = startAltitude; altitude <= plane.altitude + (ALTITUDE_RANGE / 2); altitude += TICK_INTERVAL)
    {
        // Find distance between plane's altitude current altitude by pixels
        float offset = (altitude - plane.altitude) * pixelsPerFoot;

        // Calculate where the tick is from center based on the offset
        float tickY = (m_height / 2.f) - offset;

        // Skip if outside the tape
        if (tickY < 0 || tickY > m_height) continue;

        // Draw a longer tick every 500 ft
        bool isMajor = (altitude % 500 == 0);
        float tickWidth = isMajor ? m_width * 0.4f : m_width * 0.2f;
        // Scale tick height based on height of altitude tape
        float tickHeight = m_height * 0.004f;

        // Create tick and set origin to halfway through tick
        sf::RectangleShape tick(sf::Vector2f{ tickWidth, tickHeight });
        tick.setFillColor(sf::Color::White);
        tick.setOrigin(0.f, tickHeight/2.f);
        tick.setPosition(0.f, tickY);
        m_renderTexture.draw(tick);

        // Draw text label if it is an increment of 500
        if (isMajor)
        {
            // Create text label containing altitude
            sf::Text label;
            unsigned int charSize = static_cast<unsigned int>(m_width * 0.3f);
            label.setFont(m_font);
            label.setCharacterSize(charSize);
            label.setFillColor(sf::Color::White);
            label.setString(std::to_string(altitude));

            // Create bounds object to get measurements of textbox
            sf::FloatRect bounds = label.getLocalBounds();
            // x position offset for font
            float labelOffset = m_width * 0.05f;
            label.setOrigin(0.f, bounds.top + (bounds.height / 2.f));
            label.setPosition(tickWidth + (m_width * 0.05f), tickY);
            m_renderTexture.draw(label);
        }
    }

}
