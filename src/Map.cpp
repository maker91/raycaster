#include "Map.hpp"
#include "ResourceLoader.hpp"
#include "SoundEngine.hpp"
#include "Player.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

Map::Map(const std::string &filename, Player *player)
	: m_Array(nullptr), m_Width(0), m_Height(0), m_RegionName("E1"), m_MapName("M1"), m_CeilingColor(sf::Color(56, 56, 56)),
	m_FloorColor(sf::Color(112, 112, 112)), m_Texture("Images/walls.png"),
	m_Player(player)
{
	Load(filename);
}

Map::~Map() {
	// delete sprites
	for (auto sprite : m_Sprites) {
		delete sprite;
	}

	// delete map
	delete[] m_Array;
}

void Map::Tick(float dt) {
	// door sliding
	for (auto itr = m_MovingDoors.begin(); itr != m_MovingDoors.end();) {
		itr->second += dt;

		if (itr->second >= 1.f) {
			m_OpenDoors.insert(itr->first);
			itr = m_MovingDoors.erase(itr);
		} else
			++itr;
	}

	// sprite tick
	for (auto sprite : m_Sprites) {
		if (sprite->IsDirectional())
			sprite->SetViewerPosition(m_Player->GetPosition());
		sprite->Tick(dt);
	}
}

Wall Map::Get(int x, int y) const {
	if (m_Array)
		return m_Array[m_Width*y + x];

	return 0;
}

Wall Map::Get(int p) const {
	if (m_Array)
		return m_Array[p];

	return 0;
}

void Map::Set(int x, int y, Wall value) {
	if (m_Array)
		m_Array[m_Width*y + x] = value.value;
}

void Map::Set(int p, Wall value) {
	if (m_Array)
		m_Array[p] = value.value;
}

bool Map::IsWall(int x, int y) const {
	return Get(x, y).value != 0;
}

bool Map::IsWall(int p) const {
	return Get(p).value != 0;
}

bool Map::IsDoor(int x, int y) const {
	return (Get(x, y).flags & (int)WallFlags::DOOR) == (int)WallFlags::DOOR;
}

bool Map::IsDoor(int p) const {
	return (Get(p).flags & (int)WallFlags::DOOR) == (int)WallFlags::DOOR;
}

void Map::OpenDoor(int x, int y) {
	m_MovingDoors[y*m_Width + x] = 0.f;
	SoundEngine::PlaySound("Sounds/door.wav", sf::Vector2f(x + 0.5f, y+0.5f), 100.f, 1.f);
}

void Map::OpenDoor(int p) {
	int x = p%m_Width;
	int y = (p-x)/m_Width;
	OpenDoor(x, y);
}

bool Map::IsOpen(int x, int y) const {
	return IsOpen(y*m_Width + x);
}

bool Map::IsOpen(int p) const {
	return m_OpenDoors.count(p) > 0;
}

bool Map::IsMoving(int x, int y) const {
	return IsMoving(y*m_Width + x);
}

bool Map::IsMoving(int p) const {
	return m_MovingDoors.count(p) > 0;
}

bool Map::IsMoving(int x, int y, float &amount) const {
	return IsMoving(y*m_Width + x, amount);
}

bool Map::IsMoving(int p, float &amount) const {
	if (m_MovingDoors.count(p) > 0) {
		amount = m_MovingDoors.at(p);
		return true;
	}

	return false;
}

bool Map::GetCollide(int x, int y) const {
	if (IsDoor(x, y) && IsOpen(x, y))
		return false;

	return (Get(x, y).flags & (int)WallFlags::COLLIDE) == (int)WallFlags::COLLIDE;
}

bool Map::GetCollide(int p) const {
	if (IsDoor(p) && IsOpen(p))
		return false;

	return (Get(p).flags & (int)WallFlags::COLLIDE) == (int)WallFlags::COLLIDE;
}

void Map::SetCollide(int x, int y) {
	Wall w = Get(x, y);
	w.flags |= (int)WallFlags::COLLIDE;
}

void Map::SetCollide(int p) {
	Wall w = Get(p);
	w.flags |= (int)WallFlags::COLLIDE;
}

int Map::GetWidth() const {
	return m_Width;
}

int Map::GetHeight() const {
	return m_Height;
}

int Map::GetTexWidth() const {
	return m_TexWidth;
}

int Map::GetTexHeight() const {
	return m_TexHeight;
}

const sf::Color &Map::GetFloorColor() const {
	return m_FloorColor;
}

const sf::Color &Map::GetCeilingColor() const {
	return m_CeilingColor;
}

sf::Image *Map::GetWallImage() const {
	return ResourceLoader::GetImage(m_Texture);
}

void Map::AddSprite(Sprite *spr) {
	m_Sprites.push_back(spr);
}

const std::vector<Sprite *> &Map::GetSprites() const {
	return m_Sprites;
}

void Map::SortSprites(const sf::Vector2f &pos) {
	std::sort(m_Sprites.begin(), m_Sprites.end(), [&pos](const Sprite *a, const Sprite *b) -> bool {
		const sf::Vector2f &apos = a->GetPosition();
		const sf::Vector2f &bpos = b->GetPosition();

		float adist = std::pow(pos.x - apos.x, 2.f) + std::pow(pos.y - apos.y, 2.f);
		float bdist = std::pow(pos.x - bpos.x, 2.f) + std::pow(pos.y - bpos.y, 2.f);

		return adist > bdist;
	});
}

void Map::Save() {
	Save(m_FileName);
}

void Map::Save(const std::string &filename) {
	// open the file
	std::ofstream file(filename);

	// write the magic number
	char magic[] = "RCM";
	file.write(magic, 4);

	// write the region name and map name
	file.write(m_RegionName.c_str(), 20).write(m_MapName.c_str(), 20);

	// write the map width and height
	file.put((char)m_Width).put((char)m_Height);

	// write the floor and ceiling color
	file.write((char *)(&m_FloorColor), 3).write((char *)(&m_CeilingColor), 3);

	// write the wall texture (first 20 bytes)
	file.write(m_Texture.c_str(), 20);

	// write the map data
	file.write((char *)m_Array, m_Width*m_Height*4);
	
	// write the entity count
	unsigned int ec = m_Sprites.size();
	file.write((char *)&ec, 4);

	// write the entity data
	for (auto &sprite : m_Sprites) {
		// write entity id
		file.put(0);

		// write entity position
		file.write((char *)&sprite->GetPosition(), 8);

		// write entity direction
		file.write((char *)&sprite->GetForward(), 8);
	}

	// close the file
	file.close();
}

void Map::Load(const std::string &filename) {
	// open the file
	std::ifstream file(filename);

	char magic[4], region[20], map[20], width, height, tex[20];
	sf::Color fcol, ccol;
	unsigned int ec;

	// read the magic number
	file.read(magic, 4);
	if (std::strcmp(magic, "RCM") != 0)
		throw std::runtime_error("File is not valid");

	m_FileName = filename;

	// read the region and map name
	file.read(region, 20).read(map, 20);

	// read the map width and height
	file.read(&width, 1).read(&height, 1);

	// read the floor and ceiling color
	file.read((char *)&fcol, 3).read((char *)&ccol, 3);

	// read the wall texture
	file.read(tex, 20);
	sf::Image *t = ResourceLoader::GetImage(tex);
	m_TexWidth = t->getSize().x;
	m_TexHeight = t->getSize().y;

	// create the map array and load the data in
	if (m_Array)
		delete[] m_Array;

	m_Array = new int[width*height];
	file.read((char *)m_Array, width*height*4);

	m_CeilingColor = ccol;
	m_FloorColor = fcol;
	m_Height = height;
	m_Width = width;
	m_MapName = std::string(map);
	m_RegionName = std::string(region);
	m_Texture = std::string(tex);

	// read the entity count
	file.read((char *)&ec, 4);

	// read the entity data in
	for (unsigned int i=0; i<ec; ++i) {
		unsigned char id;
		sf::Vector2f pos, dir;

		// read id
		id = file.get();

		// read position and direction
		file.read((char *)&pos, 8).read((char *)&dir, 8);

		std::cout << "Loaded entity (" << (unsigned int)id << ") at (" << pos.x << ", " << pos.y << "), forward: (" << dir.x << ", " << dir.y << ")" << std::endl;
	}

	// clear the door data
	m_MovingDoors.clear();
	m_OpenDoors.clear();
}

void Map::Reload() {
	Load(m_FileName);
}

