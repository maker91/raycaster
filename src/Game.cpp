#include <algorithm>
#include <iostream>
#include <cmath>

#include "Game.hpp"
#include "ResourceLoader.hpp"
#include "SoundEngine.hpp"
#include "Animation.hpp"
#include "CurTime.hpp"

#include "Weapons/Shotgun.hpp"
#include "Weapons/Pistol.hpp"

#define TEX_WIDTH 64
#define TEX_HEIGHT 64
#define FOV 65

#define PI 3.14159265359f

Game::Game(sf::RenderWindow *win)
	:	m_Window(win), m_ScreenWidth(win->getSize().x), m_ScreenHeight(win->getSize().y),
		m_Map("Maps/E1M1.rcm", &m_Player),
		m_Player(&m_Map, sf::Vector2f(14.5f, 8.5f), sf::Vector2f(0.f, -1.f), FOV*PI/180.f), 
		m_MouseCaptured(true), m_Paused(false),
		m_Weapon(new Pistol(&m_Player))
{
	// set up weapon ammo types
	Weapon::AmmoTypes["Pistol"] = 100;
	Weapon::AmmoTypes["Shotgun"] = 100;
	m_Player.SetAmmo("Pistol", 10);
	m_Player.SetAmmo("Shotgun", 10);

	// buffers
	m_Buffer.create(m_ScreenWidth, m_ScreenWidth);
	m_DepthBuffer = new float[m_ScreenWidth];

	// test animated sprites
	Animation<int> anim(3);
	anim.InsertFrame(1);
	anim.InsertFrame(2);

	m_Map.AddSprite(new Sprite(ResourceLoader::GetTexture("Images/barrel.png"), anim, sf::Vector2u(23,32), sf::Vector2f(6.5f, 8.5f), 0.4f, 0.f));
	m_Map.AddSprite(new Sprite(ResourceLoader::GetTexture("Images/barrel.png"), anim, sf::Vector2u(23,32), sf::Vector2f(6.5f, 9.5f), 0.4f, 0.f));

	// test directional sprites
	m_Map.AddSprite(new Sprite(ResourceLoader::GetTexture("Images/Monsters/imp.png"), sf::Vector2u(41, 57), sf::Vector2f(10.f, 13.f), 0.65f, 0.f, true));
	
	// test directional animated sprites
	Animation<int> canim(6);
	canim.InsertFrame(1);
	canim.InsertFrame(2);
	canim.InsertFrame(3);
	canim.InsertFrame(4);
	canim.InsertFrame(1);

	m_Map.AddSprite(new Sprite(ResourceLoader::GetTexture("Images/Monsters/cacodemon.png"), canim, sf::Vector2u(76, 78), sf::Vector2f(15.f, 8.f), 0.8f, 0.5f, true));

	// start game music
	m_Music.openFromFile("Music/E1M1.wav");
	m_Music.setRelativeToListener(true);
	m_Music.setPosition(0.f, 0.f, 0.f);
	m_Music.setLoop(true);
	m_Music.setVolume(30.f);
	m_Music.play();
}

Game::~Game() {
	// delete depth buffer
	delete[] m_DepthBuffer;

	// shutdown resource loader
	ResourceLoader::ShutDown();

	// delete weapon
	delete m_Weapon;
}

void Game::SetMouseCaptured(bool b) {
	m_MouseCaptured = b;
}

bool Game::GetMouseCaptured() const {
	return m_MouseCaptured;
}

void Game::Tick(float dt) {
	if (m_Paused)
		return;

	CurTime += dt;

	// player tick
	m_Player.Tick(dt);

	// player rotation
	if (m_MouseCaptured) {
		sf::Vector2i dm = sf::Mouse::getPosition(*m_Window) - sf::Vector2i(m_ScreenWidth/2, m_ScreenHeight/2);
		sf::Mouse::setPosition(sf::Vector2i(m_ScreenWidth/2, m_ScreenHeight/2), *m_Window);

		m_Player.Rotate(0.8f*dm.x*dt);
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		m_Weapon->Shoot();

	// gun tick
	m_Weapon->Tick(dt);

	// map tick
	m_Map.Tick(dt);
}

void Game::Draw() {
	// wall image
	const sf::Image *wall = m_Map.GetWallImage();

	const sf::Vector2f &pos = m_Player.GetPosition();
	const sf::Vector2f &look = m_Player.GetForward();
	const sf::Vector2f &right = m_Player.GetRight();

	int fpHeight = int(256.f*m_Player.GetHeight());

	m_Window->clear(m_Map.GetFloorColor()); // clear to the floor color
	for (int x=0; x<m_ScreenWidth; ++x) { // clear the buffer and depth buffer
		m_DepthBuffer[x] = std::numeric_limits<float>::max();

		for (int y=0; y<m_ScreenHeight; ++y)
			m_Buffer.setPixel(x, y, sf::Color::Transparent);
	}

	// draw the ceiling
	sf::RectangleShape crect(sf::Vector2f((float)m_ScreenWidth, m_ScreenHeight/2.f));
	crect.setFillColor(m_Map.GetCeilingColor());
	crect.setOutlineThickness(0.f);
	m_Window->draw(crect);

	// DDA ray casting (WALL CASTING)
	for (int x = 0; x < m_ScreenWidth; x++) {
		// calculate ray position and direction 
		float cameraX = 2.f*x/float(m_ScreenWidth)-1.f; //x-coordinate in camera space     
		float rayDirX = look.x + right.x*cameraX;
		float rayDirY = look.y + right.y*cameraX;
     
		// cast a ray
		bool hit = false;
		bool side = false; // EASTWEST = false
		WallSide cardinal;
		float perpdist = 0.f;
		float dist = 0.f;
		sf::Vector2f hitpos;

		// which box of the map we're in  
		int mapX = int(pos.x);
		int mapY = int(pos.y);
       
		// length of ray from current position to next x or y-side
		float sideDistX;
		float sideDistY;
       
		// length of ray from one x or y-side to next x or y-side
		float deltaDistX = std::sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
		float deltaDistY = std::sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
       
		// what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;

		// calculate step and initial sideDist
		if (rayDirX < 0) {
			stepX = -1;
			sideDistX = (pos.x - mapX) * deltaDistX;
		} else {
			stepX = 1;
			sideDistX = (mapX + 1.0f - pos.x) * deltaDistX;
		}  
			
		if (rayDirY < 0) {
			stepY = -1;
			sideDistY = (pos.y - mapY) * deltaDistY;
		} else {
			stepY = 1;
			sideDistY = (mapY + 1.0f - pos.y) * deltaDistY;
		}

		// perform DDA
		while (true) {
			// jump to next map square, OR in x-direction, OR in y-direction
			if (sideDistX < sideDistY) {
				sideDistX += deltaDistX;
				mapX += stepX;
				side = false;

				if (stepX < 0)
					cardinal = WallSide::EAST;
				else
					cardinal = WallSide::WEST;
			} else {
				sideDistY += deltaDistY;
				mapY += stepY;
				side = true;

				if (stepY < 0)
					cardinal = WallSide::NORTH;
				else
					cardinal = WallSide::SOUTH;
			}

			// Check if ray is out of bounds
			if (mapX < 0 || mapX >= m_Map.GetWidth() || mapY < 0 || mapY >= m_Map.GetHeight())
				break;

			// Check if ray has hit a wall
			//int type = m_Map.Get(mapX, mapY);
			if (m_Map.IsWall(mapX, mapY)) {
				hit = true;

				if (side) {
					hitpos.x = pos.x + ((mapY - pos.y + (1 - stepY) / 2) / rayDirY) * rayDirX;
					hitpos.y = (float)mapY;

					perpdist += std::abs((mapY - pos.y + (1 - stepY) / 2) / rayDirY);
				} else {
					hitpos.y = pos.y + ((mapX - pos.x + (1 - stepX) / 2) / rayDirX) * rayDirY;
					hitpos.x = (float)mapX;

					perpdist += std::abs((mapX - pos.x + (1 - stepX) / 2) / rayDirX);
				}

				dist += std::sqrt(std::pow(hitpos.x - pos.x, 2.f) + std::pow(hitpos.y - pos.y, 2.f));
				
				// the ray hit a door
				if (m_Map.IsDoor(mapX, mapY)) {
					if (m_Map.IsOpen(mapX, mapY)) {
						hit = false;
						perpdist = 0.f;
						dist = 0.f;
						continue;
					}

					// move ray on by inset
					float inset = 0.5f;

					float dhitposx = hitpos.x;
					float dhitposy = hitpos.y;
					int dmapX = mapX;
					int dmapY = mapY;
					float p;

					if (side) {
						float c = stepX*inset*std::abs(rayDirX/rayDirY);
						p = std::sqrt(inset*inset + c*c);
						dhitposx += c;

						float amount;
						if (m_Map.IsMoving(mapX, mapY, amount) && dhitposx > (float)mapX) {
							dhitposx += amount;
						}

						// flip the texture for back faces
						if (stepY > 0)
							dhitposx = (float)std::floor(mapX) + (1.f - (dhitposx - (float)std::floor(mapX)));

						dmapX = (int)dhitposx;
					} else {
						float c = stepY*inset*std::abs(rayDirY/rayDirX);
						p = std::sqrt(inset*inset + c*c);
						dhitposy += c;

						float amount;
						if (m_Map.IsMoving(mapX, mapY, amount) && dhitposy > (float)mapY) {
							dhitposy += amount;
						}

						// flip the texture for back faces
						if (stepX < 0)
							dhitposy = (float)std::floor(mapY) + (1.f - (dhitposy - (float)std::floor(mapY)));

						dmapY = (int)dhitposy;
					}
					
					// if it is still hitting the door, draw it
					if (dmapX == mapX && dmapY == mapY) {
						hitpos.x = dhitposx;
						hitpos.y = dhitposy;
						mapX = (int)dhitposx;
						mapY = (int)dhitposy;

						// extend perpdist
						perpdist += p*std::cos((x - m_ScreenWidth/2)*((FOV*PI/180.f)/(float)m_ScreenWidth));
					} else { // otherwise, carry on
						hit = false;
						perpdist = 0.f;
						dist = 0.f;
						continue;
					}
				}
				break;
			}	
		}

		if (hit) {
			if (x == m_ScreenWidth/2) {
				m_HitCoords.x = hitpos.x;
				m_HitCoords.y = hitpos.y;

				m_HitSide = cardinal;
			}
       
			// write to depth buffer
			m_DepthBuffer[x] = std::min(m_DepthBuffer[x], perpdist);
        
			// Calculate height of line to draw on screen
			int lineHeight = std::abs(int(m_ScreenHeight / perpdist));

			// calculate lowest and highest pixel to fill in current stripe
			int drawStart = -int(lineHeight*(1-m_Player.GetHeight())) + m_ScreenHeight/2;
			if (drawStart < 0)
				drawStart = 0;

			int drawEnd = int(lineHeight*(m_Player.GetHeight())) + m_ScreenHeight/2;
			if (drawEnd >= m_ScreenHeight)
				drawEnd = m_ScreenHeight - 1;

			// texturing calculations
			int texNum;
			switch (cardinal) {
				case WallSide::NORTH:
					texNum = m_Map.Get((int)hitpos.x, (int)hitpos.y).north - 1; break;
				case WallSide::EAST:
					texNum = m_Map.Get((int)hitpos.x, (int)hitpos.y).east - 1; break;
				case WallSide::SOUTH:
					texNum = m_Map.Get((int)hitpos.x, (int)hitpos.y).south - 1; break;
				case WallSide::WEST:
					texNum = m_Map.Get((int)hitpos.x, (int)hitpos.y).west - 1; break;
				default:
					texNum = 1;
			}

			int texNumX = texNum%3; // N%W
			int texNumY = (texNum - texNumX)/3; // (N-X)/W
       
			// calculate value of wallX
			float hitX = (side ? hitpos.x : hitpos.y);
			float wallX = hitX - std::floor((hitX));
       
			// x coordinate on the texture
			int texX = int(wallX * float(TEX_WIDTH));
			if (!side && rayDirX > 0)
				texX = TEX_WIDTH - texX - 1;
			if (side && rayDirY < 0)
				texX = TEX_WIDTH - texX - 1;

			int mod = int(255.f*std::max(0.f, 1.f - dist/25.f));
			for (int y = drawStart; y < drawEnd; y++) {
				int d = y*256 - m_ScreenHeight*128 + lineHeight*(256 - fpHeight);
				int texY = ((d * TEX_HEIGHT) / lineHeight) / 256;
				sf::Color color = wall->getPixel(texX + TEX_WIDTH*texNumX, texY + TEX_HEIGHT*texNumY);

				// make distant walls darker
				color *= sf::Color(mod, mod, mod);

				// make color darker for y-sides
				if (side)
					color *= sf::Color(170, 170, 170);

				m_Buffer.setPixel(x, y, color);
			}
		} else {
			m_HitCoords.x = -1;
			m_HitCoords.y = -1;
		}
	}

	m_ScreenTexture.loadFromImage(m_Buffer);
	m_Window->draw(sf::Sprite(m_ScreenTexture));

	// SPRITE CASTING
	// sort sprites based on player position
	m_Map.SortSprites(pos);

	// draw sprites
	for (Sprite *sprite : m_Map.GetSprites()) {
		sf::Sprite bspr(*sprite->GetTexture());
		const sf::Vector2u &size = sprite->GetSize();

		float spriteX = sprite->GetPosition().x - pos.x;
		float spriteY = sprite->GetPosition().y - pos.y;

		if (look.x*spriteX + look.y*spriteY > 0) {
			float invDet = 1.0f / (right.x * look.y - look.x * right.y);
			float transformX = invDet * (look.y * spriteX - look.x * spriteY);
			float transformY = invDet * (-right.y * spriteX + right.x * spriteY);

			if (transformY > 0) {            
				float height = (float)std::abs(int(m_ScreenHeight / transformY));
				int spriteScreenX = int((m_ScreenWidth / 2) * (1 + transformX / transformY));
				int spriteScreenY = int(m_ScreenHeight/2 + height*(m_Player.GetHeight() - sprite->GetScale()/2.f - (1.f - sprite->GetScale())*sprite->GetFloatHeight()));
				height *= sprite->GetScale();

				int spriteWidth = int(size.x*(height/size.y));
				int drawStartX = -spriteWidth/2 + spriteScreenX;
				if (drawStartX < 0)
					drawStartX = 0;

				int drawEndX = spriteWidth / 2 + spriteScreenX;
				if (drawEndX >= m_ScreenWidth)
					drawEndX = m_ScreenWidth - 1;

				for (int x=drawStartX; x<drawEndX; ++x) {
					if (x >= 0 && x < m_ScreenWidth && m_DepthBuffer[x] > transformY) {
						int texX = int((x - (-spriteWidth / 2 + spriteScreenX))*size.x/spriteWidth);

						sf::IntRect t = sprite->GetTextureRect();
						bspr.setTextureRect(sf::IntRect(t.left + texX, t.top, 1, size.y));

						bspr.setOrigin(0.f, size.y/2.f);
						bspr.setScale(1.f, height/size.y);
						bspr.setPosition((float)x, (float)spriteScreenY);

						// darken based on distance
						float dist = std::sqrt(std::pow(pos.x - sprite->GetPosition().x, 2.f) + std::pow(pos.y - sprite->GetPosition().y, 2.f));
						int mod = int(255.f*std::max(0.f, 1.f - dist/25.f));
						bspr.setColor(sf::Color(mod, mod, mod));

						m_Window->draw(bspr);
					}
				}
			}
		}
	}

	// draw gun
	m_Weapon->Draw(m_Window);
}

void Game::HandleEvent(const sf::Event &ev) {
	switch (ev.type) {
		case sf::Event::KeyPressed:
			if (ev.key.code == sf::Keyboard::Escape) {
				SetMouseCaptured(!GetMouseCaptured());
				m_Paused = !m_Paused;
				m_Window->setMouseCursorVisible(!GetMouseCaptured());
				sf::Mouse::setPosition(sf::Vector2i(m_ScreenWidth/2, m_ScreenHeight/2), *m_Window);
			} else if (ev.key.code == sf::Keyboard::LControl) {
				m_Player.SetCrouching(true);
				if (m_Player.IsSprinting())
					m_Player.SetSprinting(false);
			} else if (ev.key.code == sf::Keyboard::LShift) {
				if (!m_Player.IsCrouching() && m_Player.IsMoving())
					m_Player.SetSprinting(true);
			} else if (ev.key.code == sf::Keyboard::Num1) {
				delete m_Weapon;
				m_Weapon = new Pistol(&m_Player);
			} else if (ev.key.code == sf::Keyboard::Num2) {
				delete m_Weapon;
				m_Weapon = new Shotgun(&m_Player);
			} else if (ev.key.code == sf::Keyboard::Space) {
				sf::Vector2i mappos = sf::Vector2i((int)m_HitCoords.x, (int)m_HitCoords.y);
				int mapi = mappos.y*m_Map.GetWidth() + mappos.x;

				if (m_Map.IsDoor(mapi) && !m_Map.IsMoving(mapi)) {
					const sf::Vector2f &pos = m_Player.GetPosition();
					float distsqr = std::pow(m_HitCoords.x - pos.x, 2.f) + std::pow(m_HitCoords.y - pos.y, 2.f);
					
					if (distsqr <= 1.6f*1.6f) {
						m_Map.OpenDoor(mapi);
					}
				}
			} else if (ev.key.code == sf::Keyboard::Up) {
				sf::Vector2i mappos = sf::Vector2i((int)m_HitCoords.x, (int)m_HitCoords.y);
				Wall w = m_Map.Get(mappos.x, mappos.y);

				switch (m_HitSide) {
					case WallSide::NORTH:
						w.north = std::max(1u, std::min(54u, w.north+1u)); break;
					case WallSide::EAST:
						w.east = std::max(1u, std::min(54u, w.east+1u)); break;
					case WallSide::SOUTH:
						w.south = std::max(1u, std::min(54u, w.south+1u)); break;
					case WallSide::WEST:
						w.west = std::max(1u, std::min(54u, w.west+1u)); break;
				}

				m_Map.Set(mappos.x, mappos.y, w);
			} else if (ev.key.code == sf::Keyboard::Down) {
				sf::Vector2i mappos = sf::Vector2i((int)m_HitCoords.x, (int)m_HitCoords.y);
				Wall w = m_Map.Get(mappos.x, mappos.y);
				
				switch (m_HitSide) {
					case WallSide::NORTH:
						w.north = std::max(1u, std::min(54u, w.north-1u)); break;
					case WallSide::EAST:
						w.east = std::max(1u, std::min(54u, w.east-1u)); break;
					case WallSide::SOUTH:
						w.south = std::max(1u, std::min(54u, w.south-1u)); break;
					case WallSide::WEST:
						w.west = std::max(1u, std::min(54u, w.west-1u)); break;
				}

				m_Map.Set(mappos.x, mappos.y, w);
			} else if (ev.key.code == sf::Keyboard::C) {
				sf::Vector2i mappos = sf::Vector2i((int)m_HitCoords.x, (int)m_HitCoords.y);

				switch (m_HitSide) {
					case WallSide::NORTH:
						mappos.y += 1; break;
					case WallSide::EAST:
						mappos.x += 1; break;
					case WallSide::SOUTH:
						mappos.y -= 1; break;
					case WallSide::WEST:
						mappos.x -= 1; break;
				}

				Wall w;
				w.flags |= (int)WallFlags::COLLIDE;
				w.north = w.south = w.east = w.west = 1;
				m_Map.Set(mappos.x, mappos.y, w);
			} else if (ev.key.code == sf::Keyboard::X) {
				sf::Vector2i mappos = sf::Vector2i((int)m_HitCoords.x, (int)m_HitCoords.y);
				m_Map.Set(mappos.x, mappos.y, Wall());
			} else if (ev.key.code == sf::Keyboard::M) {
				sf::Vector2i mappos = sf::Vector2i((int)m_HitCoords.x, (int)m_HitCoords.y);
				Wall w = m_Map.Get(mappos.x, mappos.y);

				w.flags ^= (int)WallFlags::DOOR;
				m_Map.Set(mappos.x, mappos.y, w);
			} else if (ev.key.code == sf::Keyboard::F1) {
				m_Map.Reload();
			} else if (ev.key.code == sf::Keyboard::F2) {
				m_Map.Save();
			}

			break;
		case sf::Event::KeyReleased:
			if (ev.key.code == sf::Keyboard::LControl) {
				m_Player.SetCrouching(false);
			} else if (ev.key.code == sf::Keyboard::LShift) {
				m_Player.SetSprinting(false);
			}
			break;
	}
}