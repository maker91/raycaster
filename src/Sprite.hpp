#pragma once

#include <SFML/Graphics.hpp>
#include "Animation.hpp"

class Sprite {
public:
	Sprite(sf::Texture *tex, const sf::Vector2u &size, const sf::Vector2f &pos, float scale, float floatheight, bool directional=false);
	Sprite(sf::Texture *tex, const Animation<int> &anim, const sf::Vector2u &size, const sf::Vector2f &pos, float scale, float floatheight, bool directional=false);

	virtual ~Sprite();

	void					SetPosition(const sf::Vector2f &pos);
	const sf::Vector2f		&GetPosition() const;
	void					SetForward(const sf::Vector2f &dir);
	const sf::Vector2f		&GetForward() const;

	void					SetScale(float);
	float					GetScale() const;

	void					SetFloatHeight(float);
	float					GetFloatHeight() const;

	const sf::Vector2u		&GetSize() const;	
	void					SetSize(const sf::Vector2u &);

	bool					IsAnimated() const;
	bool					IsDirectional() const;
	virtual bool			IsScripted() const;

	void					SetDirectional(bool b);

	const sf::Texture		*GetTexture() const;
	void					SetTexture(sf::Texture *tex);
	sf::IntRect				GetTextureRect();

	void					SetViewerPosition(const sf::Vector2f &pos);

	virtual void			Tick(float dt);

protected:

	sf::Texture		*m_Texture;
	int				m_Direction;

	float			m_Scale;
	float			m_FloatHeight;
	sf::Vector2f	m_Position;
	sf::Vector2f	m_Forward;
	sf::Vector2u	m_Size;

	bool			m_Animated;
	bool			m_Directional;

	Animation<int>	m_Anim;
};