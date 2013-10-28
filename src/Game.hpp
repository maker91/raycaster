#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.hpp"
#include "Sprite.hpp"
#include "Map.hpp"
#include "Weapon.hpp"

enum struct WallSide {
	NORTH,
	EAST,
	SOUTH,
	WEST
};

class Game {
public:
	Game(sf::RenderWindow *win);
	~Game();

	void SetMouseCaptured(bool b);
	bool GetMouseCaptured() const;

	void Tick(float dt);
	void Draw();

	void HandleEvent(const sf::Event &);

private:
	Game(const Game &);

private:
	bool					m_Paused;

	Player					m_Player;

	Map						m_Map;
	sf::Music				m_Music;
	sf::Sound				m_Sound;

	sf::RenderWindow		*m_Window;
	int						m_ScreenWidth;
	int						m_ScreenHeight;
	bool					m_MouseCaptured;

	sf::Image				m_Buffer;
	sf::Texture				m_ScreenTexture;
	float					*m_DepthBuffer;

	sf::Vector2f			m_HitCoords;
	WallSide				m_HitSide;

	// TEMPORARY
	Weapon					*m_Weapon;
};