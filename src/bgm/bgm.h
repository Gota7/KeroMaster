#pragma once

#include <string>
#include <mutex>
#include "raylib.h"
#include "imgui.h"
#include "pxtone/pxtn.h"
#include "pxtone/pxtnService.h"
#include "../gbin/gfile.h"
#include "miniaudio.h"

// Buffer.
constexpr u32 NUM_CHANNELS = 2;
constexpr u32 SAMPLE_RATE = 44100;

// Background music player.
struct BgmPlayer
{
    static std::string rsc;
    static pxtnService pxtn;
    static FILE* handle;
    static bool playing;
    static bool audioInitialized;
    static float prevVolume;
    static std::mutex audioMutex;
    static ma_device device;
    
    static ImVector<char*> songList;
    static char** songNameBuf;
    static int numSongs;
    static float volume;
    static std::string currSong;
    static int currSongInd;

    static void Init(std::string rsc_k);
    static void LoadSongList();
    static void Play(std::string bgmName);
    static void Pause();
    static void Resume();
    static void Stop();
    static int32_t GetPos();
    static int32_t GetEnd();
    static void AudioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
};

int cmpstr2(const void* a, const void* b);