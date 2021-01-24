#include "RetroEngine.hpp"

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; ++i) {
        if (StrComp(argv[i], "UsingCWD"))
            usingCWD = true;
    }
    
    Retro_InitTicks();

    Engine.Init();
#if RETRO_USING_SDL2
    controllerInit(0);
#endif
    Engine.Run();

    return 0;
}
#if RETRO_USING_ALLEGRO4
END_OF_MAIN()
#endif

#if RETRO_PLATFORM == RETRO_UWP
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) { return SDL_WinRTRunApp(main, NULL); }
#endif