#include "bgm.h"
#include "../rlImGui/utils.h"

#include <algorithm>

std::string BgmPlayer::rsc;
pxtnService BgmPlayer::pxtn;
bool BgmPlayer::playing = false;
bool BgmPlayer::audioInitialized = false;
std::mutex BgmPlayer::audioMutex;
AudioStream BgmPlayer::stream;
float BgmPlayer::volume = 1;
float BgmPlayer::prevVolume = 1;
char **BgmPlayer::songs = nullptr;
int BgmPlayer::numSongs = 0;
std::string BgmPlayer::currSong;
int BgmPlayer::currSongInd = 0;

void BgmPlayer::Init(std::string rsc_k)
{

    // Initialize pxtone.
    rsc = rsc_k;
    pxtnERR pxtn_err = pxtnERR_VOID;
    pxtn_err = pxtn.init();
    if (pxtn_err != pxtnOK)
    {
        printf("PXTONE INIT ERROR!\n");
        return;
    }
    pxtn.set_destination_quality(NUM_CHANNELS, SAMPLE_RATE);

    stream = LoadAudioStream(SAMPLE_RATE, 16, NUM_CHANNELS);
    SetAudioStreamCallback(stream, BgmPlayer::AudioCallback);
    PlayAudioStream(stream);
    audioInitialized = true;
}

void BgmPlayer::Dispose()
{
    if (audioInitialized)
    {
        StopAudioStream(stream);
        UnloadAudioStream(stream);
        audioInitialized = false;
    }
}

// Load the song listing.
void BgmPlayer::LoadSongList()
{

    // Delete if needed.
    if (!songs)
        DelImGuiStringList(songs, numSongs);

    // Get a new song listing.
    std::vector<std::string> songList = ReadFilesFromDir(rsc + "/bgm");
    std::sort(songList.begin(), songList.end());
    songs = GenImGuiStringList(songList, &numSongs);

    // Find current song.
    currSongInd = 0;
    for (int i = 0; i < songList.size(); i++)
    {
        if (currSong == songList[i])
        {
            currSongInd = i;
            break;
        }
    }
}

void BgmPlayer::Play(std::string bgmName)
{
    // Open file and lock data.
    audioMutex.lock();
    currSong = bgmName;
    auto ptcopPath = rsc + "/bgm/" + bgmName + ".ptcop";
    auto handle = fopen(ptcopPath.c_str(), "r");
    GFile gfile(handle); // RAII takes care of closing
    playing = false;

    pxtnDescriptor desc;
    int32_t event_num = 0;
    if (!desc.set_file_r(handle))
    {
        printf("PXTONE LOAD ERROR!\n");
        audioMutex.unlock();
        return;
    }
    if (pxtn.read(&desc) != pxtnOK)
    {
        printf("PXTONE LOAD ERROR!\n");
        audioMutex.unlock();
        return;
    }
    if (pxtn.tones_ready() != pxtnOK)
    {
        printf("PXTONE LOAD ERROR!\n");
        audioMutex.unlock();
        return;
    }
    pxtnVOMITPREPARATION prep = {0};
    prep.flags |= pxtnVOMITPREPFLAG_loop;
    prep.start_pos_float = 0;
    prep.master_volume = volume;
    pxtn.moo_preparation(&prep);
    playing = true;

    audioMutex.unlock();
}

void BgmPlayer::Pause()
{
    playing = false;
}

void BgmPlayer::Resume()
{
    playing = true;
}

void BgmPlayer::Stop()
{
    playing = false;
}

int32_t BgmPlayer::GetPos()
{
    return pxtn.moo_get_now_clock();
}

int32_t BgmPlayer::GetEnd()
{
    return pxtn.moo_get_end_clock();
}

void BgmPlayer::AudioCallback(void *pOutput, unsigned int frameCount)
{
    bool lock = audioMutex.try_lock();
    if (!lock)
        return;

    if (!playing)
    {
        memset(pOutput, 0, frameCount * NUM_CHANNELS * sizeof(s16));
        audioMutex.unlock();
        return;
    }

    if (prevVolume != volume)
    {
        pxtn.moo_set_master_volume(volume);
    }
    prevVolume = volume;

    if (!pxtn.Moo(pOutput, frameCount * NUM_CHANNELS * sizeof(s16)))
    {
        printf("PXTONE PLAY ERROR!\n");
        playing = false;
    }

    audioMutex.unlock();
}