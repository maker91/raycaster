CC			= g++
CFLAGS		= -Wall -Wextra -pedantic -std=c++11 -g
LDFLAGS		= -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system
DEFINES		= -D SFML_STATIC
SOURCES		= src/main.cpp src/CurTime.cpp src/Game.cpp src/Map.cpp src/Player.cpp src/ResourceLoader.cpp src/SoundEngine.cpp src/Sprite.cpp src/Weapon.cpp src/Weapons/Pistol.cpp src/Weapons/Shotgun.cpp
OBJECTS		= $(patsubst %.cpp, %.o, $(patsubst src/%, obj/%, ${SOURCES}))
EXECUTABLE	= bin/raytracer

all: $(EXECUTABLE)
	@echo "done!"
	
$(EXECUTABLE): bin obj $(OBJECTS)
	@echo "buiding $(EXECUTABLE)"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	
obj/%.o : src/%.cpp
	@echo "compiling $<"
	@$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@
	
obj:
	@mkdir obj
	@mkdir obj/Weapons
	
bin:
	@mkdir bin
	
clean:
	@echo "cleaning project"
	@rm -rf obj/
	@echo "done!"
