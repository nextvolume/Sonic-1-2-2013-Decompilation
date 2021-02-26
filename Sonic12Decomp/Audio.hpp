#ifndef AUDIO_H
#define AUDIO_H

#define TRACK_COUNT (0x10)
#define SFX_COUNT (0x100)
#define CHANNEL_COUNT (0x10) //4 in the original, 16 for convenience

#define MAX_VOLUME (100)

#define ADJUST_VOLUME(s, v) (s = (s * v) / MAX_VOLUME)

#if RETRO_USING_SDL1 || RETRO_USING_SDL2

#if RETRO_USING_SDL2
extern SDL_AudioDeviceID audioDevice;
#endif
extern SDL_AudioSpec audioDeviceFormat;

#define LOCK_AUDIO_DEVICE() SDL_LockAudio();
#define UNLOCK_AUDIO_DEVICE() SDL_UnlockAudio();

#define AUDIO_FREQUENCY (44100)
#define AUDIO_FORMAT    (AUDIO_S16SYS) /**< Signed 16-bit samples */
#define AUDIO_SAMPLES   (0x800)
#define AUDIO_CHANNELS  (2)

#elif RETRO_USING_ALLEGRO4
#define LOCK_AUDIO_DEVICE() ;
#define UNLOCK_AUDIO_DEVICE() ;

#define AUDIO_FREQUENCY (44100)
#define AUDIO_SAMPLES   (0x800)

#else
#define LOCK_AUDIO_DEVICE() ;
#define UNLOCK_AUDIO_DEVICE() ;

#define AUDIO_FREQUENCY (44100)
#define AUDIO_SAMPLES   (0x800)

#endif

#define MIX_BUFFER_SAMPLES (256)

struct TrackInfo {
    char fileName[0x40];
    bool trackLoop;
    uint loopPoint;
};

struct MusicPlaybackInfo {
#if !RETRO_DISABLE_OGGVORBIS
    OggVorbis_File vorbisFile;
    int vorbBitstream;
#endif
#if RETRO_USING_SDL1
    SDL_AudioSpec spec;
#endif
#if RETRO_USING_SDL2
    SDL_AudioStream *stream;
#endif
#if RETRO_USING_ALLEGRO4
#if RETRO_WSSAUDIO
    Sint16 *stream;
#elif RETRO_DOSSOUND
    Sint16 *stream;
    unsigned bufAddr;
    unsigned bufSize;
    unsigned curBufNumAddr;
    unsigned sampleRate;
#else
    AUDIOSTREAM *stream;
#endif
#endif

#if RETRO_OSSAUDIO
    int ossFd;
    Sint16 *stream;
#endif
	
    Sint16 *buffer;
    FileInfo fileInfo;
    bool trackLoop;
    uint loopPoint;
    bool loaded;
};

struct SFXInfo {
    char name[0x40];
    Sint16 *buffer;
    size_t length;
    bool loaded;
};

struct ChannelInfo {
    size_t sampleLength;
    Sint16 *samplePtr;
    int sfxID;
    byte loopSFX;
    sbyte pan;
};

enum MusicStatuses {
    MUSIC_STOPPED = 0,
    MUSIC_PLAYING = 1,
    MUSIC_PAUSED  = 2,
    MUSIC_LOADING = 3,
    MUSIC_READY   = 4,
};

extern int globalSFXCount;
extern int stageSFXCount;

extern int masterVolume;
extern int trackID;
extern int sfxVolume;
extern int bgmVolume;
extern bool audioEnabled;

extern bool musicEnabled;
extern int musicStatus;
extern int musicStartPos;
extern int musicPosition;
extern int musicRatio;
extern TrackInfo musicTracks[TRACK_COUNT];

extern SFXInfo sfxList[SFX_COUNT];
extern char sfxNames[SFX_COUNT][0x40];

extern ChannelInfo sfxChannels[CHANNEL_COUNT];

extern MusicPlaybackInfo musInfo;

//#if RETRO_USING_SDL1 || RETRO_USING_SDL2
//extern SDL_AudioSpec audioDeviceFormat;
//#endif

int InitAudioPlayback();

void ProcessAudioPlayback(void *data, Uint8 *stream, int len);

#if RETRO_USING_SDL1 || RETRO_USING_SDL2

void ProcessMusicStream(Sint32 *stream, size_t bytes_wanted);
void ProcessAudioMixing(Sint32 *dst, const Sint16 *src, int len, int volume, sbyte pan);

#endif

inline void freeMusInfo()
{
    if (musInfo.loaded) {
        LOCK_AUDIO_DEVICE()

#if RETRO_USING_SDL2 || RETRO_USING_SDL1
        if (musInfo.stream)
            SDL_FreeAudioStream(musInfo.stream);
#endif
#if !RETRO_DISABLE_OGGVORBIS
        ov_clear(&musInfo.vorbisFile);
#endif	
#if RETRO_USING_SDL2 || RETRO_USING_SDL1
        if (musInfo.buffer)
            delete[] musInfo.buffer;
	    
	musInfo.buffer    = nullptr;
        musInfo.stream = nullptr;
#endif
        musInfo.trackLoop = false;
        musInfo.loopPoint = 0;
        musInfo.loaded    = false;

        UNLOCK_AUDIO_DEVICE()
    }
}

void SetMusicTrack(const char *filePath, byte trackID, bool loop, uint loopPoint);
void SwapMusicTrack(const char *filePath, byte trackID, uint loopPoint, uint ratio);
bool PlayMusic(int track, int musStartPos);

#if !RETRO_USING_SDL2 && !RETRO_USING_SDL1
void musicFifoReset(void);
#endif

inline void StopMusic()
{
    musicStatus = MUSIC_STOPPED;
#if RETRO_USING_SDL1 || RETRO_USING_SDL2	
    SDL_LockAudio();
#endif
    freeMusInfo();
#if RETRO_USING_SDL1 || RETRO_USING_SDL2
    SDL_UnlockAudio();
#endif
	
#if !RETRO_USING_SDL2 && !RETRO_USING_SDL1
    musicFifoReset();
#endif
}

void LoadSfx(char *filePath, byte sfxID);
void PlaySfx(int sfx, bool loop);
inline void StopSfx(int sfx)
{
    for (int i = 0; i < CHANNEL_COUNT; ++i) {
        if (sfxChannels[i].sfxID == sfx) {
            MEM_ZERO(sfxChannels[i]);
            sfxChannels[i].sfxID = -1;
        }
    }
}
void SetSfxAttributes(int sfx, int loopCount, sbyte pan);

void SetSfxName(const char *sfxName, int sfxID);

//Helper Func
inline bool PlaySFXByName(const char* sfx, sbyte loopCnt) {
    for (int s = 0; s < globalSFXCount + stageSFXCount; ++s) {
        if (StrComp(sfxNames[s], sfx)) {
            PlaySfx(s, loopCnt);
            return true;
        }
    }
    return false;
}

inline void SetMusicVolume(int volume)
{
    if (volume < 0)
        volume = 0;
    if (volume > MAX_VOLUME)
        volume = MAX_VOLUME;
    masterVolume = volume;
}

inline void SetGameVolumes(int bgmVolume, int sfxVolume) {
    //musicVolumeSetting = bgmVolume;
    SetMusicVolume(masterVolume);
    //sfxVolumeSetting = ((sfxVolume << 7) / 100);
}

inline void PauseSound()
{
    if (musicStatus == MUSIC_PLAYING)
        musicStatus = MUSIC_PAUSED;
}

inline void ResumeSound()
{
    if (musicStatus == MUSIC_PAUSED)
        musicStatus = MUSIC_PLAYING;
}


inline void StopAllSfx()
{
    LOCK_AUDIO_DEVICE()
    for (int i = 0; i < CHANNEL_COUNT; ++i) sfxChannels[i].sfxID = -1;
    UNLOCK_AUDIO_DEVICE()
}
inline void ReleaseGlobalSfx()
{
    for (int i = globalSFXCount; i >= 0; --i) {
        if (sfxList[i].loaded) {
            StrCopy(sfxList[i].name, "");
            StrCopy(sfxNames[i], "");
            free(sfxList[i].buffer);
            sfxList[i].length = 0;
            sfxList[i].loaded = false;
        }
    }
    globalSFXCount = 0;
}
inline void ReleaseStageSfx()
{
    for (int i = stageSFXCount + globalSFXCount; i >= globalSFXCount; --i) {
        if (sfxList[i].loaded) {
            StrCopy(sfxList[i].name, "");
            StrCopy(sfxNames[i], "");
            free(sfxList[i].buffer);
            sfxList[i].length = 0;
            sfxList[i].loaded = false;
        }
    }
    stageSFXCount = 0;
}

inline void ReleaseAudioDevice()
{
    StopMusic();
    StopAllSfx();
    ReleaseStageSfx();
    ReleaseGlobalSfx();
}

#if !RETRO_USING_SDL2 && !RETRO_USING_SDL1
uint64_t Resample_s16(const int16_t *input, int16_t *output, int inSampleRate, int outSampleRate, uint64_t inputSize,
                      uint32_t channels);
#endif

#if RETRO_WSSAUDIO
extern int wssSampleRate;
#endif

#endif // !AUDIO_H
