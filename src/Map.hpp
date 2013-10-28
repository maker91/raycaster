#pragma once

#include "Sprite.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <set>

enum class WallFlags : int {
	COLLIDE = 0x01,
	DOOR	= 0x02,
	SECRET	= 0x04,
};

struct Wall {
	union {
		unsigned value;

		struct {
			unsigned north	: 6;
			unsigned east		: 6;
			unsigned south		: 6;
			unsigned west		: 6;

			unsigned flags		: 8;
		};
	};

	Wall() : value(0) {};
	Wall(unsigned v) : value(v) {};
};

class Player;

class Map {
public:
	Map(const std::string &filename, Player *player);
	~Map();

	void Tick(float dt);

	Wall Get(int x, int y) const;
	Wall Get(int p) const;
	void Set(int x, int y, Wall value);
	void Set(int p, Wall value);

	bool IsWall(int x, int y) const;
	bool IsWall(int p) const;
	bool IsDoor(int x, int y) const;
	bool IsDoor(int p) const;

	void OpenDoor(int x, int y);
	void OpenDoor(int p);

	bool IsOpen(int x, int y) const;
	bool IsOpen(int p) const;
	bool IsMoving(int x, int y) const;
	bool IsMoving(int p) const;
	bool IsMoving(int x, int y, float &amount) const;
	bool IsMoving(int p, float &amount) const;
	
	bool GetCollide(int x, int y) const;
	bool GetCollide(int p) const;
	void SetCollide(int x, int y);
	void SetCollide(int p);

	int GetWidth() const;
	int GetHeight() const;
	int GetTexWidth() const;
	int GetTexHeight() const;

	const sf::Color &GetFloorColor() const;
	const sf::Color &GetCeilingColor() const;

	sf::Image *GetWallImage() const;

	void AddSprite(Sprite *spr);
	const std::vector<Sprite *> &GetSprites() const;
	void SortSprites(const sf::Vector2f &pos);

	void Save();
	void Save(const std::string &filename);
	void Load(const std::string &filename);
	void Reload();

private:
	int						*m_Array;
	int						m_Width;
	int						m_Height;

	std::string				m_FileName;
	std::string				m_RegionName;
	std::string				m_MapName;

	std::string				m_Texture;
	int						m_TexWidth;
	int						m_TexHeight;

	sf::Color				m_FloorColor;
	sf::Color				m_CeilingColor;

	Player					*m_Player;
	std::vector<Sprite *>	m_Sprites;

	// unsaved values
	std::map<int, float>	m_MovingDoors;
	std::set<int>			m_OpenDoors;
};