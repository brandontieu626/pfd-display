#include <SFML/Graphics.hpp>
#include "AttitudeIndicator.h"
#include "AltitudeTape.h"
#include "AirspeedTape.h"
#include "HeadingIndicator.h"
#include "VerticalSpeedIndicator.h"
#include "PlaneController.h"
#include "Instrument.h"
#include <vector>
#include <memory>
int main()
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::ContextSettings windowSettings;
	windowSettings.antialiasingLevel = 8;
	sf::RenderWindow window(desktop, "PFD", sf::Style::Fullscreen, windowSettings);
	window.setFramerateLimit(60);

	std::vector<std::unique_ptr<Instrument>> instruments;

	instruments.push_back(std::make_unique<AttitudeIndicator>(
			sf::Vector2f{ desktop.width / 2.f, desktop.height * 0.42f },
			std::min(desktop.width, desktop.height) * 0.35f));

	instruments.push_back(std::make_unique<AltitudeTape>(
			sf::Vector2f{ desktop.width * 0.75f, desktop.height * 0.42f }, 
			desktop.width * 0.05f, 
			desktop.height * 0.6f));

	instruments.push_back(std::make_unique<AirspeedTape>(
		sf::Vector2f{ desktop.width * 0.25f, desktop.height * 0.42f }, 
		desktop.width * 0.05f,
		desktop.height * 0.6f));

	instruments.push_back(std::make_unique<HeadingIndicator>(
		sf::Vector2f{ desktop.width * 0.5f, (float)desktop.height }, 
		desktop.height * 0.18f));

	instruments.push_back(std::make_unique<VerticalSpeedIndicator>(
		sf::Vector2f{ desktop.width * 0.90f, desktop.height * 0.42f }, 
		desktop.width * 0.04f,
		desktop.height * 0.7f));



	PlaneController controller;
	sf::Clock deltaClock;

	/*AttitudeIndicator ai{ sf::Vector2f{desktop.width / 2.f, desktop.height * 0.42f},
		std::min(desktop.width, desktop.height) * 0.35f };
	float tapeWidth = desktop.width * 0.05f;
	float tapeHeight = desktop.height * 0.6f;
	AltitudeTape at{ sf::Vector2f{desktop.width * 0.75f, desktop.height * 0.42f}, tapeWidth, tapeHeight };

	float speedWidth = desktop.width * 0.05f;
	float speedHeight = desktop.height * 0.6f;
	AirspeedTape st{ sf::Vector2f{desktop.width * 0.25f, desktop.height * 0.42f}, speedWidth, speedHeight };

	float hiRadius = desktop.height * 0.18f;
	HeadingIndicator hi{ sf::Vector2f{desktop.width * 0.5f, (float)desktop.height}, hiRadius };

	float vsiWidth = desktop.width * 0.04f;
	float vsiHeight = desktop.height * 0.7f;
	VerticalSpeedIndicator vsi{ sf::Vector2f{desktop.width * 0.90f, desktop.height * 0.42f}, vsiWidth, vsiHeight };*/

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
		}

		// dt = seconds elapsed since last frame — scales all movement to real time, not frame rate
		float dt = deltaClock.restart().asSeconds();
		controller.update(dt);

		window.clear(sf::Color::Black);

		for (auto& instrument : instruments)
		{
			instrument->draw(window,controller.getPlane());
		}
		window.display();
	}
	return 0;
}
