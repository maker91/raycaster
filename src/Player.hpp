#pragma once

#include <map>

#include <SFML/System/Vector2.hpp>
#include "Map.hpp"

class Player {
public:
	Player(Map *map);
	Player(Map *map, const sf::Vector2f &pos, const sf::Vector2f &look, float fov, float height=0.5f);

	void SetPosition(const sf::Vector2f &pos);
	const sf::Vector2f &GetPosition() const;

	const sf::Vector2f &GetForward() const;
	sf::Vector2f GetRight() const;
	float GetAngle() const;

	unsigned int GetAmmo(const std::string &type) const;
	void SetAmmo(const std::string &type, unsigned int n);
	void AddAmmo(const std::string &type, int d);

	void SetHeight(float height);
	float GetHeight() const;

	unsigned int GetHealth() const;
	void SetHealth(unsigned int h);
	void AddHealth(int d);

	void SetCrouching(bool b);
	void SetSprinting(bool b);

	bool IsMoving() const;
	bool IsSprinting() const;
	bool IsCrouching() const;

	void Move(const sf::Vector2f &v);
	void Rotate(float ang);

	void SetMap(Map *);

	void Tick(float dt);
private:
	void CalculateRight();

private:
	sf::Vector2f	m_Position;
	sf::Vector2f	m_Forward;
	sf::Vector2f	m_Right;

	float			m_FOV;

	Map				*m_Map;
	float			m_Height;

	bool			m_Moving;
	bool			m_Sprinting;
	bool			m_Crouching;

	unsigned int	m_Health;
private:
	std::map<std::string, unsigned int> m_Ammo;
};