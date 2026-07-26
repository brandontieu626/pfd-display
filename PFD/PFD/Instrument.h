#pragma once
#include <SFML/Graphics.hpp>
#include "FlightData.h"

// Base class for all instruments on the screen
class Instrument
{
public:
	virtual ~Instrument() = default;                                          // proper clean up when deleting through base ptr
	virtual void draw(sf::RenderWindow& window, const FlightData& plane) = 0; // draw method to be implemented by each instrument
};