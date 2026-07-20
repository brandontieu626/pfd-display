#include "VerticalSpeedIndicator.h"
#include <iostream>
#include <algorithm>

constexpr float VSI_RANGE = 4000.f; // total span: -2000 to +2000 fpm
constexpr float TICK_INTERVAL = 250.f;

VerticalSpeedIndicator::VerticalSpeedIndicator(const sf::Vector2f& center, float width, float height)
    : m_center(center), m_width(width), m_height(height)
{
    if (!m_font.loadFromFile("../../assets/fonts/arial.ttf"))
    {
        std::cout << "Failed to load font";
    }
}

void VerticalSpeedIndicator::draw(sf::RenderWindow& window, const FlightData& plane)
{
    drawTape(window);
    drawPointer(window, plane);
}

void VerticalSpeedIndicator::drawTape(sf::RenderWindow& window)
{
    // Outline rectangle
    sf::RectangleShape tape(sf::Vector2f{ m_width, m_height });
    tape.setFillColor(sf::Color::Black);
    tape.setOutlineColor(sf::Color::White);
    tape.setOutlineThickness(m_width * 0.02f);
    tape.setOrigin(m_width / 2.f, m_height / 2.f);
    tape.setPosition(m_center);
    window.draw(tape);

    for (float fpm = -(VSI_RANGE / 2.f); fpm <= (VSI_RANGE / 2.f); fpm += TICK_INTERVAL)
    {
        // Map fpm to y position: +fpm goes up (negative y in SFML)
        float tickY = (m_height / 2.f) - (fpm / (VSI_RANGE / 2.f)) * (m_height / 2.f);

        // Wider tick mark every 500 fpm
        bool isMajor = (static_cast<int>(fpm) % 500 == 0);
        float tickWidth = isMajor ? m_width * 0.5f : m_width * 0.25f;
        float tickHeight = m_height * 0.004f;

        // Create tick mark and set it to it's tickY position
        sf::RectangleShape tick(sf::Vector2f{ tickWidth, tickHeight });
        tick.setFillColor(sf::Color::White);
        tick.setOrigin(tickWidth, tickHeight / 2.f);
        tick.setPosition(m_center.x + m_width / 2.f, m_center.y - m_height / 2.f + tickY);
        window.draw(tick);

        if (isMajor && fpm > 0)
        {
            sf::Text label;
            label.setFont(m_font);
            label.setCharacterSize(static_cast<unsigned int>(m_width * 0.25f));
            label.setFillColor(sf::Color::White);
            label.setString(std::to_string(static_cast<int>(fpm)));

            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(0.f, bounds.top + bounds.height / 2.f);
            label.setPosition(m_center.x + (m_width / 2.f + m_width * 0.1f),
                m_center.y - m_height / 2.f + tickY);
            window.draw(label);
        }
    }

}

void VerticalSpeedIndicator::drawPointer(sf::RenderWindow& window, const FlightData& plane)
{
    // Clamp vspeed to displayable range
    float vspeed = std::clamp(plane.vspeed, -(VSI_RANGE / 2.f), VSI_RANGE / 2.f);

    // Map vspeed to screen y � same formula as ticks
    float pointerScreenY = m_center.y - (vspeed / (VSI_RANGE / 2.f)) * (m_height / 2.f);

    // Bar grows from the center line (0 fpm) toward the pointer position
    float barHeight = std::abs(m_center.y - pointerScreenY);
    float originY = (vspeed >= 0.f) ? barHeight : 0.f;
    float barWidth = m_width * 0.4f;

    sf::Color barColor = (vspeed >= 0.f) ? sf::Color::Green : sf::Color{ 255, 140, 0 };

    sf::RectangleShape bar(sf::Vector2f{ barWidth, barHeight });
    bar.setFillColor(barColor);
    bar.setOrigin(barWidth * 0.5f, originY);
    bar.setPosition(m_center);
    window.draw(bar);

    // Draw center line on top of bar so it always intersects cleanly
    float centerLineHeight = m_height * 0.004f;
    sf::RectangleShape centerLine(sf::Vector2f{ m_width, centerLineHeight });
    centerLine.setFillColor(sf::Color::White);
    centerLine.setOrigin(m_width / 2.f, centerLineHeight / 2.f);
    centerLine.setPosition(m_center);
    window.draw(centerLine);
}