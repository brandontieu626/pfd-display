#include <SFML/Graphics.hpp>
#include "AttitudeIndicator.h"
#include "FlightData.h"
#include "Constants.h"

int main() 
{
	//Create window
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktop, "PFD", sf::Style::Fullscreen);
	//sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);

	//Test plane struct and attitude indicator struct
	FlightData plane{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	//AttitudeIndicator ai{ sf::Vector2f{WINDOW_WIDTH /2.f,WINDOW_HEIGHT / 2.f },
	//	std::min(WINDOW_WIDTH, WINDOW_HEIGHT) * 0.35f};
	AttitudeIndicator ai{ sf::Vector2f{desktop.width / 2.f,desktop.height / 2.f },
	std::min(desktop.width, desktop.height) * 0.35f };

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