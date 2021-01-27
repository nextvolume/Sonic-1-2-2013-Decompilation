SOURCES = Sonic12Decomp/Animation.cpp     \
          Sonic12Decomp/Audio.cpp         \
          Sonic12Decomp/Collision.cpp     \
          Sonic12Decomp/Debug.cpp         \
          Sonic12Decomp/Drawing.cpp       \
          Sonic12Decomp/Ini.cpp           \
          Sonic12Decomp/Input.cpp         \
          Sonic12Decomp/main.cpp          \
          Sonic12Decomp/Math.cpp          \
          Sonic12Decomp/Network.cpp       \
          Sonic12Decomp/Object.cpp        \
          Sonic12Decomp/Palette.cpp       \
          Sonic12Decomp/PauseMenu.cpp     \
          Sonic12Decomp/Reader.cpp        \
          Sonic12Decomp/RetroEngine.cpp   \
          Sonic12Decomp/RetroGameLoop.cpp \
          Sonic12Decomp/Scene.cpp         \
          Sonic12Decomp/Scene3D.cpp       \
          Sonic12Decomp/Script.cpp        \
          Sonic12Decomp/Sprite.cpp        \
          Sonic12Decomp/String.cpp        \
          Sonic12Decomp/Text.cpp          \
          Sonic12Decomp/Userdata.cpp      \

ifneq ($(USE_ALLEGRO4),)
	ifneq ($(DOS),)
		CXXFLAGS_ALL = -DBASE_PATH='"$(BASE_PATH)"'  \
		-DRETRO_USING_ALLEGRO4 -DRETRO_DOS $(CXXFLAGS)
		LDFLAGS_ALL = $(LDFLAGS)
		LIBS_ALL = -lvorbisfile  -lvorbis -logg -lalleg $(LIBS)
	else
		CXXFLAGS_ALL = $(shell pkg-config --cflags vorbisfile vorbis) $(shell allegro-config --cppflags) \
		-DBASE_PATH='"$(BASE_PATH)"' -DRETRO_USING_ALLEGRO4 $(CXXFLAGS)
		LDFLAGS_ALL = $(LDFLAGS)
		LIBS_ALL =  $(shell pkg-config --libs vorbisfile vorbis) $(shell allegro-config --libs) $(LIBS)	
	endif	
else
	CXXFLAGS_ALL = $(shell pkg-config --cflags --static sdl2 vorbisfile vorbis) \
               -DBASE_PATH='"$(BASE_PATH)"' $(CXXFLAGS)
	LDFLAGS_ALL = $(LDFLAGS)
	LIBS_ALL = $(shell pkg-config --libs --static sdl2 vorbisfile vorbis) -pthread $(LIBS)
endif

ifneq ($(FORCE_CASE_INSENSITIVE),)
	CXXFLAGS_ALL += -DFORCE_CASE_INSENSITIVE
	SOURCES += Sonic12Decomp/fcaseopen.cpp
endif

objects/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $^ -o $@ -c

bin/sonic2013: $(SOURCES:%=objects/%.o)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)

install: bin/sonic2013
	install -Dp -m755 bin/sonic2013 $(prefix)/bin/sonic2013

clean:
	rm -r -f bin && rm -r -f objects
