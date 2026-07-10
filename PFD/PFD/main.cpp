#include <SFML/Graphics.hpp>
#include "AttitudeIndicator.h"
#include "FlightData.h"
#include "Constants.h"

int main() 
{
	//Create window
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);

	//Test plane struct and attitude indicator struct
	FlightData plane{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	AttitudeIndicator ai{ sf::Vector2f{400,300},300 };

	while (window.isOpen())
	{
		sf::Event event;
		
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color::Black);
		ai.draw(window, plane);
		window.display();
	}
	return 0;
}