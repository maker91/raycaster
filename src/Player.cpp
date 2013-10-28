#include "Player.hpp"
#include <cmath>
#include <algorithm>
#include <SFML/Window.hpp>

#include "SoundEngine.hpp"
#include "Weapon.hpp"

#define PI 3.14159265359f

Player::Player(Map *map)
	: m_Position(sf::Vector2f(5.f, 5.f)), m_Height(0.3f), m_FOV(65*PI/180.f), m_Moving(false), m_Sprinting(false), m_Crouching(false),
	m_Map(map)
{
	sf::Vector2f look(0.f, -1.f);
	float mag = std::sqrt(std::pow(look.x, 2.f) + std::pow(look.y, 2.f));
	m_Forward = look/mag;
	CalculateRight();

	SoundEngine::SetListenerDirection(m_Forward);
}

Player::Player(Map *map, const sf::Vector2f &pos, const sf::Vector2f &look, float fov, float height)
	: m_Position(pos), m_Height(height), m_FOV(fov), m_Moving(false), m_Sprinting(false), m_Crouching(false),
	m_Map(map)
{
	float mag = std::sqrt(std::pow(look.x, 2.f) + std::pow(look.y, 2.f));
	m_Forward = look/mag;
	CalculateRight();

	SoundEngine::SetListenerDirection(m_Forward);
}

void Player::Tick(float dt) {
	m_Moving = false;

	// player movement
	sf::Vector2f d(0.f, 0.f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		d.x += GetForward().x;
		d.y += GetForward().y;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		d.x -= GetForward().x;
		d.y -= GetForward().y;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		d.x -= GetRight().x;
		d.y -= GetRight().y;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		d.x += GetRight().x;
		d.y += GetRight().y;
	}

	float mag = std::sqrt(std::pow(d.x, 2.f) + std::pow(d.y, 2.f));

	if (mag > 0) {
		m_Moving = true;
		d*= 1.3f;

		if (m_Sprinting && !m_Crouching)
			d *= 1.5f;

		if (m_Crouching) {
			d *= 0.5f;
		}

		Move(1.8f*dt*d/mag);
	}
}

void Player::SetPosition(const sf::Vector2f &pos) {
	m_Position = pos;
	SoundEngine::SetListenerPosition(pos);
}

const sf::Vector2f &Player::GetPosition() const {
	return m_Position;
}

const sf::Vector2f &Player::GetForward() const {
	return m_Forward;
}

sf::Vector2f Player::GetRight() const {
	return m_Right;
}

float Player::GetAngle() const {
	return std::atan2(m_Forward.y, m_Forward.x);
}

unsigned int Player::GetAmmo(const std::string &type) const {
	if (m_Ammo.count(type) == 1) {
		return m_Ammo.at(type);
	}

	return 0;
}

void Player::SetAmmo(const std::string &type, unsigned int n) {
	m_Ammo[type] = std::min(std::max(n, 0u), Weapon::AmmoTypes[type]);
}

void Player::AddAmmo(const std::string &type, int d) {
	SetAmmo(type, GetAmmo(type)+d);
}

unsigned int Player::GetHealth() const {
	return m_Health;
}

void Player::SetHealth(unsigned int h) {
	m_Health = std::max(0u, std::min(h, 100u));
}

void Player::AddHealth(int d) {
	SetHealth(GetHealth() + d);
}

void Player::SetHeight(float height) {
	m_Height = height;
}

float Player::GetHeight() const {
	return m_Height;
}

bool Player::IsMoving() const {
	return m_Moving;
}

bool Player::IsSprinting() const {
	return m_Sprinting;
}

bool Player::IsCrouching() const {
	return m_Crouching;
}

void Player::SetCrouching(bool b) {
	m_Crouching = b;
	if (b)
		SetHeight(0.35f);
	else
		SetHeight(0.5f);
}

void Player::SetSprinting(bool b) {
	m_Sprinting = b;
}

void Player::Move(const sf::Vector2f &v) {
	sf::Vector2f npos = m_Position + v;

	if ((int)npos.x >= m_Map->GetWidth() || (int)npos.x < 0 || (int)npos.y >= m_Map->GetHeight() || (int)npos.y < 0 )
		return;

	if (m_Map->GetCollide((int)npos.x, (int)m_Position.y)) {
		npos.x = m_Position.x;
	}

	if (m_Map->GetCollide((int)m_Position.x, (int)npos.y)) {
		npos.y = m_Position.y;
	}

	SetPosition(npos);
}

void Player::Rotate(float ang) {
	sf::Vector2f newlook(m_Forward.x*std::cos(ang) - m_Forward.y*std::sin(ang), m_Forward.x*std::sin(ang) + m_Forward.y*std::cos(ang));
	m_Forward = newlook;
	CalculateRight();

	SoundEngine::SetListenerDirection(m_Forward);
}

void Player::CalculateRight() {
	sf::Vector2f f = GetForward();
	m_Right = sf::Vector2f(-f.y, f.x);

	float mag = std::sqrt(std::pow(m_Right.x, 2.f) + std::pow(m_Right.y, 2.f));
	m_Right *= (std::tan(m_FOV/2.f)/mag);
}

void Player::SetMap(Map *map) {
	m_Map = map;
}