#include "HeadingIndicator.h"
#include "Constants.h"
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
    drawCenterMarker(window);
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
            float radians = (i + 180.f) * (PI / 180.f);
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
    constexpr float ARC_RANGE     = 180.f;
    constexpr float TICK_INTERVAL = 5.f;

    // Start at the first 5° heading below the left edge of the visible range
    float startHeading = std::floor((plane.heading - HEADING_RANGE / 2.f) / TICK_INTERVAL) * TICK_INTERVAL;

    // Iterate from first left heading to last right heading
    for (float tickHeading = startHeading; tickHeading <= plane.heading + HEADING_RANGE / 2.f; tickHeading += TICK_INTERVAL)
    {
        // Fractional offset of this tick from current heading — drives arc position
        float headingOffset = tickHeading - plane.heading;

        // Wrap offset to +-180 so crossing the 0/360 boundary is handled correctly
        if (headingOffset >  180.f) headingOffset -= 360.f;
        if (headingOffset < -180.f) headingOffset += 360.f;

        if (headingOffset < -HEADING_RANGE / 2.f || headingOffset > HEADING_RANGE / 2.f) continue;

        // Normalize absolute tick heading to 0-360 for label/major check
        // Use a while loop because it can be more than one full rotation out of range
        float displayHeading = tickHeading;
        while (displayHeading <    0.f) displayHeading += 360.f;
        while (displayHeading >= 360.f) displayHeading -= 360.f;

        // Round due to floating arithmetic errors and check if it's a major tick
        int h = static_cast<int>(std::round(displayHeading));
        bool isMajor = (h % 10 == 0);

        // Arc position
        // Stretch the 80 degree heading range in a semicircle
        float arcOffset = headingOffset * (ARC_RANGE / HEADING_RANGE);

        // Convert to radians
        float radians   = (arcOffset + 270.f) * (PI / 180.f);

        // Calculate actual screen coordinates using radius
        float x = m_center.x + m_radius * std::cos(radians);
        float y = m_center.y + m_radius * std::sin(radians);


        float tickLength = isMajor ? m_radius * 0.08f : m_radius * 0.04f;
        float tickWidth  = m_radius * 0.004f;

        sf::RectangleShape tick(sf::Vector2f{ tickWidth, tickLength });
        tick.setFillColor(sf::Color::White);
        tick.setOrigin(tickWidth / 2.f, 0.f);
        tick.setPosition(x, y);
        tick.setRotation(arcOffset);
        window.draw(tick);

        if (isMajor)
        {
            std::string labelStr;
            sf::Color directionColor = sf::Color::White;
            switch (h)
            {
            case 0:
            case 360: labelStr = "N"; directionColor = sf::Color::Red; break;
            case 90:  labelStr = "E"; break;
            case 180: labelStr = "S"; break;
            case 270: labelStr = "W"; break;
            default:  labelStr = std::to_string(h); break;
            }

            sf::Text label;
            label.setFont(m_font);
            label.setCharacterSize(static_cast<unsigned int>(m_radius * 0.08f));
            label.setFillColor(directionColor);
            label.setString(labelStr);

            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);

            float labelOffset = tickLength + m_radius * 0.08f;
            label.setPosition(
                x - labelOffset * std::cos(radians),
                y - labelOffset * std::sin(radians)
            );
            window.draw(label);
        }
    }
}
void HeadingIndicator::drawCenterMarker(sf::RenderWindow& window)
{
    sf::ConvexShape triangle;
    triangle.setPointCount(3);

    float halfWidth = m_radius * 0.04f;
    float height    = m_radius * 0.05f;

    triangle.setPoint(0, sf::Vector2f{ -halfWidth, height }); // bottom-left (base)
    triangle.setPoint(1, sf::Vector2f{  halfWidth, height }); // bottom-right (base)
    triangle.setPoint(2, sf::Vector2f{  0.f,       0.f   }); // tip pointing up toward labels
    triangle.setFillColor(sf::Color::Blue);
    // Position inside the arc, below the heading labels (labels sit ~0.84 * radius from center)
    triangle.setPosition(m_center.x, m_center.y - m_radius * 0.76f);

    window.draw(triangle);
}