#pragma once

#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class ResourceLoader {
public:
	static sf::Texture		*GetTexture(const std::string &name);
	static sf::Image		*GetImage(const std::string &name);
	static sf::SoundBuffer	*GetSoundBuffer(const std::string &name);

	static void ShutDown();

private:
	static std::map<std::string, sf::Texture *>		m_Textures;
	static std::map<std::string, sf::Image *>		m_Images;
	static std::map<std::string, sf::SoundBuffer *> m_Sounds;
};