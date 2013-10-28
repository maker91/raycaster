#include "Pistol.hpp"
#include <vector>

#include "../ResourceLoader.hpp"
#include "../SoundEngine.hpp"
#include "../Player.hpp"

Pistol::Pistol(Player *owner)
	: Weapon(owner, ResourceLoader::GetTexture("Images/Weapons/pistol.png"), sf::Vector2u(110, 110), "Pistol", 0.45f)
{
	Animation<sf::Vector2i> anim(16, false);
	anim.InsertFrame(sf::Vector2i(1, 1));
	anim.InsertFrame(sf::Vector2i(1, 2));
	anim.InsertFrame(sf::Vector2i(2, 2));
	anim.InsertFrame(sf::Vector2i(2, 1));

	SetShootAnimation(anim);
	SetShootSound(ResourceLoader::GetSoundBuffer("Sounds/pistol.wav"));
}