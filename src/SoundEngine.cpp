#include "SoundEngine.hpp"
#include "ResourceLoader.hpp"
#include <iostream>

sf::Sound SoundEngine::m_SoundObjects[MAX_SOUNDS];

void SoundEngine::PlaySound(sf::SoundBuffer *buffer, const sf::Vector2f &pos, float volume, float pitch) {
	sf::Sound &snd = RequestSoundObject();
	snd.setBuffer(*buffer);
	snd.setPosition(sf::Vector3f(pos.x, 0.f, pos.y));
	snd.setVolume(volume);
	snd.setPitch(pitch);
	snd.setAttenuation(1.f);

	snd.play();
}

void SoundEngine::PlaySound(const std::string &name, const sf::Vector2f &pos, float volume, float pitch) {
	PlaySound(ResourceLoader::GetSoundBuffer(name), pos, volume, pitch);
}

void SoundEngine::PlaySound(sf::SoundBuffer *buffer, float volume, float pitch) {
	sf::Sound &snd = RequestSoundObject();
	snd.setBuffer(*buffer);
	snd.setRelativeToListener(true);
	snd.setPosition(0.f, 0.f, 0.f);
	snd.setVolume(volume);
	snd.setPitch(pitch);

	snd.play();
}

void SoundEngine::PlaySound(const std::string &name, float volume, float pitch) {
	PlaySound(ResourceLoader::GetSoundBuffer(name), volume, pitch);
}

void SoundEngine::SetListenerPosition(const sf::Vector2f &pos) {
	sf::Listener::setPosition(sf::Vector3f(pos.x, 0.f, pos.y));
}

sf::Vector2f SoundEngine::GetListenerPosition() {
	sf::Vector3f pos = sf::Listener::getPosition();
	return sf::Vector2f(pos.x, pos.z);
}

void SoundEngine::SetListenerDirection(const sf::Vector2f &dir) {
	sf::Listener::setDirection(sf::Vector3f(dir.x, 0.f, dir.y));
}

sf::Vector2f SoundEngine::GetListenerDirection() {
	sf::Vector3f dir = sf::Listener::getDirection();
	return sf::Vector2f(dir.x, dir.z);
}

sf::Sound &SoundEngine::RequestSoundObject() {
	for (auto &sound : m_SoundObjects) {
		if (sound.getStatus() == sf::Sound::Status::Stopped) {
			return sound;
		}
	}

	throw std::runtime_error("OUT OF SOUND OBJECTS");
}