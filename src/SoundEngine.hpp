#pragma once

#include <SFML/Audio.hpp>

#define MAX_SOUNDS 150

class SoundEngine {
public:
	static void PlaySound(sf::SoundBuffer *buffer, const sf::Vector2f &pos, float volume=100.f, float pitch=1.f);
	static void PlaySound(const std::string &name, const sf::Vector2f &pos, float volume=100.f, float pitch=1.f);
	static void PlaySound(sf::SoundBuffer *buffer, float volume=100.f, float pitch=1.f);
	static void PlaySound(const std::string &name, float volume=100.f, float pitch=1.f);

	static void SetListenerPosition(const sf::Vector2f &);
	static void SetListenerDirection(const sf::Vector2f &);
	static sf::Vector2f GetListenerPosition();
	static sf::Vector2f GetListenerDirection();

private:
	static sf::Sound m_SoundObjects[MAX_SOUNDS];
	static sf::Sound &RequestSoundObject();
};