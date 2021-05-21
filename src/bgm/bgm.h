#pragma once

#include <string>
#include "raylib.h"
#include "pxtone/pxtn.h"
#include "pxtone/pxtnService.h"
#include "../gbin/gfile.h"

// Buffer.
const u32 NUM_CHANNELS = 1;
const u32 SAMPLE_RATE = 44100;
const u32 BUFFER_SAMPLES = 10000;
const u32 BUFFER_SIZE = pxtnBITPERSAMPLE / 8 * NUM_CHANNELS * BUFFER_SAMPLES;

// Background music player.
struct BgmPlayer
{
    static std::string rsc;
    static pxtnService pxtn;
    static FILE* handle;
    static AudioStream strm;
    static bool playing;
    static u8 buffer[BUFFER_SIZE];
    static float prevVolume;
    
    static float volume;

    static void Init(std::string rsc_k);
    static void Play(std::string bgmName);
    static void Pause();
    static void Resume();
    static void Stop();
    static int32_t GetPos();
    static int32_t GetEnd();
    static void Update();
};