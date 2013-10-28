#pragma once

#include <map>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/Vector2.hpp>

#include "Animation.hpp"

class Player;

class Weapon {
public:
	virtual ~Weapon();

	const std::string &GetAmmoType() const;
	int GetAmmo() const;
	int GetMaxAmmo() const;

	void Tick(float dt);
	void Draw(sf::RenderTarget *rt);
	virtual void Shoot();

	virtual void Think(float dt) {};
	virtual void OnShoot() {};
	virtual void OnEquip() {};
	virtual void OnUnEquip() {};

protected:
	Weapon(Player *owner, sf::Texture *tex, const sf::Vector2u &size, const std::string &ammotype, float firerate);
	void SetShootAnimation(const Animation<sf::Vector2i> &anim);
	void SetShootSound(sf::SoundBuffer *buffer);

protected:
	sf::Texture				*m_Texture;
	sf::Vector2u			m_Size;

	sf::SoundBuffer			*m_ShootSound;
	Player					*m_Owner;

	std::string				m_AmmoType;
	float					m_FireRate;
	float					m_NextFireTime;
	float					m_CurTime;

	bool					m_Animated;
	Animation<sf::Vector2i> m_ShootAnim;

public:
	static std::map<std::string, unsigned int> AmmoTypes;
};