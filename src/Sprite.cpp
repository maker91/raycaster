#include "Sprite.hpp"
#include <iostream>

#define PI 3.14159265359f

Sprite::Sprite(sf::Texture *tex, const sf::Vector2u &size, const sf::Vector2f &pos, float scale, float floatheight, bool directional)
	: m_Position(pos), m_Scale(scale), m_FloatHeight(floatheight), m_Animated(false), m_Texture(tex),
	m_Size(size), m_Directional(directional), m_Direction(0), m_Forward(sf::Vector2f(0.f, -1.f))
{
	
}

Sprite::Sprite(sf::Texture *tex, const Animation<int> &anim, const sf::Vector2u &size, const sf::Vector2f &pos, float scale, float floatheight, bool directional)
	: m_Position(pos), m_Scale(scale), m_FloatHeight(floatheight), m_Animated(true), m_Texture(tex),
	m_Anim(anim), m_Size(size), m_Directional(directional), m_Direction(0), m_Forward(sf::Vector2f(0.f, -1.f))
{
	m_Anim.Play();
}

Sprite::~Sprite() {

}

void Sprite::Tick(float dt) {
	m_Anim.Tick(dt);
}

void Sprite::SetPosition(const sf::Vector2f &pos) {
	m_Position = pos;
}

const sf::Vector2f &Sprite::GetPosition() const {
	return m_Position;
}

void Sprite::SetForward(const sf::Vector2f &dir) {
	m_Forward = dir/(std::sqrt(dir.x*dir.x + dir.y*dir.y));
}

const sf::Vector2f &Sprite::GetForward() const {
	return m_Forward;
}

void Sprite::SetScale(float s) {
	m_Scale = s;
}

float Sprite::GetScale() const {
	return m_Scale;
}

void Sprite::SetFloatHeight(float h) {
	m_FloatHeight = h;
}

float Sprite::GetFloatHeight() const {
	return m_FloatHeight;
}

const sf::Vector2u &Sprite::GetSize() const {
	return m_Size;
}

void Sprite::SetSize(const sf::Vector2u &s) {
	m_Size = s;
}

bool Sprite::IsAnimated() const {
	return m_Animated;
}

bool Sprite::IsDirectional() const {
	return m_Directional;
}

bool Sprite::IsScripted() const {
	return false;
}

void Sprite::SetDirectional(bool b) {
	m_Directional = true;
}

const sf::Texture *Sprite::GetTexture() const {
	return m_Texture;
}

void Sprite::SetTexture(sf::Texture *t) {
	m_Texture = t;
}

sf::IntRect Sprite::GetTextureRect() {
	sf::IntRect r(0, 0, m_Size.x, m_Size.y);

	if (m_Animated)
		r.left = m_Size.x*(m_Anim.GetCurrentFrame()-1);

	if (m_Directional)
		r.top = m_Size.y*m_Direction;

	return r;
}

void Sprite::SetViewerPosition(const sf::Vector2f &pos) {
	if (m_Directional) {
		sf::Vector2f d = pos - m_Position;
		float mag = std::sqrt(std::pow(d.x, 2.f) + std::pow(d.y, 2.f))+0.0000001f;

		float dot = (d.x*m_Forward.x + d.y*m_Forward.y)/mag;
		float cross = d.x*m_Forward.y - d.y*m_Forward.x;
		cross = -cross/std::abs(cross);

		float ang = cross*std::acos(dot) + PI;
		m_Direction = int(8.f*(ang+PI/8)/(2.f*PI))%8;
	}
}
