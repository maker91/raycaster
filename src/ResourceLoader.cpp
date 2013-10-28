#include "ResourceLoader.hpp"

std::map<std::string, sf::Texture *>	ResourceLoader::m_Textures;
std::map<std::string, sf::Image *>	ResourceLoader::m_Images;
std::map<std::string, sf::SoundBuffer *> ResourceLoader::m_Sounds;

sf::Texture *ResourceLoader::GetTexture(const std::string &name) {
	if (m_Textures.count(name) == 1) {
		return m_Textures.at(name);
	}

	sf::Texture *t = new sf::Texture;
	t->loadFromFile(name);
	m_Textures[name] = t;

	return t;
}

sf::Image *ResourceLoader::GetImage(const std::string &name) {
	if (m_Images.count(name) == 1) {
		return m_Images.at(name);
	}

	sf::Image *t = new sf::Image;
	t->loadFromFile(name);
	m_Images[name] = t;

	return t;
}

sf::SoundBuffer *ResourceLoader::GetSoundBuffer(const std::string &name) {
	if (m_Sounds.count(name) == 1) {
		return m_Sounds.at(name);
	}

	sf::SoundBuffer *t = new sf::SoundBuffer;
	t->loadFromFile(name);
	m_Sounds[name] = t;

	return t;
}

void ResourceLoader::ShutDown() {
	for (auto &pair : m_Images) {
		delete pair.second;
	}

	for (auto &pair : m_Textures) {
		delete pair.second;
	}

	for (auto &pair : m_Sounds) {
		delete pair.second;
	}
}