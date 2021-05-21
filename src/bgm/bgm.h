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
constexpr u32 BUFFER_SAMPLES = 512;
//constexpr u32 BUFFER_SAMPLES = SAMPLE_RATE * 300 / 1000;
constexpr u32 BUFFER_SIZE = 4096;
//constexpr u32 BUFFER_SIZE = BUFFER_SAMPLES * NUM_CHANNELS * sizeof(u16);

// Background music player.
struct BgmPlayer
{
    static std::string rsc;
    static pxtnService pxtn;
    static FILE* handle;
    static bool playing;
    static bool audioInitialized;
    static u8 buffer[BUFFER_SIZE];
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