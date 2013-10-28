#include "Player.hpp"
#include "Weapon.hpp"
#include "SoundEngine.hpp"

#include <iostream>

std::map<std::string, unsigned int> Weapon::AmmoTypes;

Weapon::Weapon(Player *owner, sf::Texture *tex, const sf::Vector2u &size, const std::string &ammotype, float firerate)
	: m_Owner(owner), m_Texture(tex), m_AmmoType(ammotype), m_FireRate(firerate), m_CurTime(0.f), m_Size(size),
	m_Animated(false), m_ShootSound(nullptr), m_NextFireTime(0.f)
{

}

Weapon::~Weapon() {

}

void Weapon::Tick(float dt) {
	m_CurTime += dt;
	m_ShootAnim.Tick(dt);
}

void Weapon::Draw(sf::RenderTarget *rt) {
	sf::Sprite gunspr(*m_Texture);

	if (m_Animated) {
		const sf::Vector2i &anim = m_ShootAnim.GetCurrentFrame();
		gunspr.setTextureRect(sf::IntRect((anim.x-1)*m_Size.x, (anim.y-1)*m_Size.y, m_Size.x, m_Size.y));
	}

	gunspr.setOrigin(m_Size.x/2.f, (float)m_Size.y);
	gunspr.setScale(1.5f, 1.5f);

	sf::Vector2u scr = rt->getSize();

	if (m_Owner->IsMoving()) {
		if (m_Owner->IsSprinting())
			gunspr.setPosition(scr.x/2.f + 10.f*std::sin(7.f*m_CurTime), scr.y + 4.f + 4.f*std::cos(14.f*m_CurTime));
		else if (m_Owner->IsCrouching())
			gunspr.setPosition(scr.x/2.f + 10.f*std::sin(3.f*m_CurTime), scr.y + 4.f + 4.f*std::cos(6.f*m_CurTime));
		else
			gunspr.setPosition(scr.x/2.f + 10.f*std::sin(5.f*m_CurTime), scr.y + 4.f + 4.f*std::cos(10.f*m_CurTime));
	} else
		gunspr.setPosition(scr.x/2.f, (float)scr.y + 4.f);

	rt->draw(gunspr);
}

void Weapon::SetShootAnimation(const Animation<sf::Vector2i> &anim) {
	m_ShootAnim = anim;
	m_Animated = true;
}

void Weapon::SetShootSound(sf::SoundBuffer *buffer) {
	m_ShootSound = buffer;
}

const std::string &Weapon::GetAmmoType() const {
	return m_AmmoType;
}

int Weapon::GetAmmo() const {
	return 0;
}

int Weapon::GetMaxAmmo() const {
	return 0;
}

void Weapon::Shoot() {
	if (m_CurTime >= m_NextFireTime) {
		if (m_Owner->GetAmmo(m_AmmoType) <= 0) {
			SoundEngine::PlaySound("Sounds/click.wav");
			m_NextFireTime = m_CurTime + 0.5f;
			return;
		}

		m_ShootAnim.Play();
		if (m_ShootSound)
			SoundEngine::PlaySound(m_ShootSound);

		// shoot bullets
		this->OnShoot();
		m_Owner->AddAmmo(m_AmmoType, -1);

		m_NextFireTime = m_CurTime + m_FireRate;
	}
}