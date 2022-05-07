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
    static std::string rsc; // Resource path.
    static pxtnService pxtn; // PxTone service for playback.
    static FILE* handle; // Current song file handle.
    static bool playing; // If the audio is playing.
    static bool audioInitialized; // If audio has been initialized.
    static float prevVolume; // Previous volume.
    static std::mutex audioMutex; // Mutex for locking audio resources.
    static ma_device device; // Audio output device.
    
    static char** songs; // Song list.
    static int numSongs; // How many songs there are.
    static float volume; // Current song volume.
    static std::string currSong; // Name of the current song.
    static int currSongInd; // Current song index.

    // Initialize the BGM player given a resource path.
    static void Init(std::string rsc_k);

    // Load the song listing.
    static void LoadSongList();

    // Play a song.
    static void Play(std::string bgmName);

    // Pause the song.
    static void Pause();

    // Resume playback of a song.
    static void Resume();

    // Stop playback of a song.
    static void Stop();
    
    // Get position in song.
    static int32_t GetPos();

    // Get how long a song is.
    static int32_t GetEnd();

    // Song buffer feeding.
    static void AudioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

};