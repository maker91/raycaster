#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#include "Game.hpp"
#include "Map.hpp"

#define MAP_WIDTH 28
#define MAP_HEIGHT 20
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 450

int main(int argc, char *argv[]) {
	sf::RenderWindow win(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Ray Caster");
	win.setVerticalSyncEnabled(false);
	win.setMouseCursorVisible(false);
	win.setKeyRepeatEnabled(false);

	Game game(&win);

	sf::Clock frameclock;
	while (win.isOpen()) {
		sf::Event ev;
		while (win.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed)
				win.close();
			else
				game.HandleEvent(ev);
		}

		float dt = frameclock.restart().asSeconds();

		game.Tick(dt);
		game.Draw();

		win.display();
	}

	return EXIT_SUCCESS;
}