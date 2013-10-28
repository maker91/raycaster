#include "Shotgun.hpp"
#include "../ResourceLoader.hpp"
#include "../Player.hpp"
#include "../SoundEngine.hpp"

#include <vector>

Shotgun::Shotgun(Player *owner)
	: Weapon(owner, ResourceLoader::GetTexture("Images/Weapons/shotgun.png"), sf::Vector2u(500, 160), "Shotgun", 1.15f)
{
	Animation<sf::Vector2i> anim(8, false);
	anim.InsertFrame(sf::Vector2i(1, 1));
	anim.InsertFrame(sf::Vector2i(1, 2));
	anim.InsertFrame(sf::Vector2i(1, 3));
	anim.InsertFrame(sf::Vector2i(1, 4));
	anim.InsertFrame(sf::Vector2i(1, 5));
	anim.InsertFrame(sf::Vector2i(1, 6));
	anim.InsertFrame(sf::Vector2i(1, 5));
	anim.InsertFrame(sf::Vector2i(1, 4));

	SetShootAnimation(anim);
	SetShootSound(ResourceLoader::GetSoundBuffer("Sounds/shotgun.wav"));
}