#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"

class VerticalSpeedIndicator
{
public:
    VerticalSpeedIndicator(const sf::Vector2f& center, float width, float height);
    void draw(sf::RenderWindow& window, const FlightData& plane);
private:
    void drawTape(sf::RenderWindow& window);
    void drawPointer(sf::RenderWindow& window, const FlightData& plane);

    sf::Vector2f m_center;
    float m_width;
    float m_height;
    sf::Font m_font;
};
