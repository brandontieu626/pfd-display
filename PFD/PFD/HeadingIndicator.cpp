#include "HeadingIndicator.h"
#include <iostream>
#include <cmath>

HeadingIndicator::HeadingIndicator(const sf::Vector2f& center, float radius)
    : m_center(center), m_radius(radius)
{
    if (!m_font.loadFromFile("../../assets/fonts/arial.ttf"))
    {
        std::cout << "Failed to load font";
    }
}

void HeadingIndicator::draw(sf::RenderWindow& window, const FlightData& plane)
{
    drawArc(window);
    drawTicks(window, plane);
    //drawCenterMarker(window);
}

void HeadingIndicator::drawArc(sf::RenderWindow& window)
{
    // Scale thickness based on radius 
    float thickness = m_radius * 0.008f;

    // Draw multiple arcs centered on radius for a thicker line
    for (float r = m_radius - thickness / 2.f; r <= m_radius + thickness / 2.f; r += 0.5f)
    {
        sf::VertexArray arc(sf::LineStrip, 181);

        for (int i = 0; i <= 180; i++)
        {
            // Step from 180 to 360 degrees to draw the upper semicircle (left -> top -> right)
            float radians = (i + 180.f) * (3.14159f / 180.f);
            arc[i].position = sf::Vector2f{
                m_center.x + r * std::cos(radians),
                m_center.y + r * std::sin(radians)
            };
            arc[i].color = sf::Color::White;
        }

        window.draw(arc);
    }
}

void HeadingIndicator::drawTicks(sf::RenderWindow& window, const FlightData& plane)
{
    constexpr float HEADING_RANGE = 80.f;
    constexpr float ARC_RANGE = 180.f;
    constexpr float TICK_INTERVAL = 5.f;

    for (float headingOffset = - (HEADING_RANGE / 2.f); headingOffset <= HEADING_RANGE / 2.f; headingOffset += TICK_INTERVAL)
    {
        // Calculate heading for that tick
        float heading = plane.heading + headingOffset;


        // Wrap heading between 0 and 360
        if (heading < 0) heading += 360.f;
        if (heading >= 360.f) heading -= 360.f;

        // Convert heading offset to arc position by scaling to fit the full arc range
        float arcOffset = headingOffset * (ARC_RANGE / HEADING_RANGE);
        // Add 270 to shift reference point to 12 o'clock position (top of arc)
        float radians = (arcOffset + 270.f) * (3.14159f / 180.f);
        float x = m_center.x + m_radius * std::cos(radians);
        float y = m_center.y + m_radius * std::sin(radians);

        // Longer tick mark if increment of 10
        bool isMajor = (static_cast<int>(heading) % 10 == 0);
        float tickLength = isMajor ? m_radius * 0.08f : m_radius * 0.04f;

        // Draw tick pointing inward
        float tickWidth = m_radius * 0.004f;
        sf::RectangleShape tick(sf::Vector2f{ tickWidth, tickLength });
        tick.setFillColor(sf::Color::White);
        tick.setOrigin(tickWidth / 2.f, 0.f);
        tick.setPosition(x, y);
        tick.setRotation(arcOffset);
        window.draw(tick);

        if (isMajor)
        {
            int h = static_cast<int>(heading);
            std::string labelStr;
            switch (h)
            {
            case 0:
            case 360:   labelStr = "N"; break;
            case 90:    labelStr = "E"; break;
            case 180:   labelStr = "S"; break;
            case 270:   labelStr = "W"; break;
            default:    labelStr = std::to_string(h); break;
            }

            // Label with angle or direction
            sf::Text label;
            label.setFont(m_font);
            label.setCharacterSize(static_cast<unsigned int>(m_radius * 0.08f));
            label.setFillColor(sf::Color::White);
            label.setString(labelStr);

            // Find center of text box
            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);

            // Position label inward from arc edge by pushing opposite to the outward direction
            float labelOffset = tickLength + m_radius * 0.08f;
            label.setPosition(
                x - labelOffset * std::cos(radians),
                y - labelOffset * std::sin(radians)
            );
            window.draw(label);
        }
    }
}
