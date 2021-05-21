#include "bgm.h"

using namespace std;

string BgmPlayer::rsc;
pxtnService BgmPlayer::pxtn;
FILE* BgmPlayer::handle = nullptr;
AudioStream BgmPlayer::strm;
bool BgmPlayer::playing = false;
u8 BgmPlayer::buffer[BUFFER_SIZE];
float BgmPlayer::volume = 1;
float BgmPlayer::prevVolume = 1;

void BgmPlayer::Init(string rsc_k)
{
    rsc = rsc_k;
    pxtnERR pxtn_err = pxtnERR_VOID;
    pxtn_err = pxtn.init();
    if (pxtn_err != pxtnOK)
    {
        printf("PXTONE INIT ERROR!\n");
        return;
    }
    pxtn.set_destination_quality(NUM_CHANNELS, SAMPLE_RATE);
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(BUFFER_SAMPLES);
    strm = InitAudioStream(SAMPLE_RATE, pxtnBITPERSAMPLE, NUM_CHANNELS);
}

void BgmPlayer::Play(string bgmName)
{
    if (handle != nullptr)
    {
        fclose(handle);
        handle = nullptr;
    }
    handle = fopen((rsc + "/bgm/" + bgmName + ".ptcop").c_str(), "r");
    pxtnDescriptor desc;
    int32_t event_num = 0;
    if (!desc.set_file_r(handle))
    {
        printf("PXTONE LOAD ERROR!\n");
        return;
    }
    if (pxtn.read(&desc) != pxtnOK)
    {
        printf("PXTONE LOAD ERROR!\n");
        return;
    }
    if (pxtn.tones_ready() != pxtnOK)
    {
        printf("PXTONE LOAD ERROR!\n");
        return;
    }
    pxtnVOMITPREPARATION prep = {0};
    prep.flags |= pxtnVOMITPREPFLAG_loop;
    prep.start_pos_float = 0;
    prep.master_volume = volume;
    pxtn.moo_preparation(&prep);
    PlayAudioStream(strm);
    playing = true;
}

void BgmPlayer::Pause()
{
    playing = false;
    PauseAudioStream(strm);
}

void BgmPlayer::Resume()
{
    if (handle != nullptr)
    {
        playing = true;
        ResumeAudioStream(strm);
    }
}

void BgmPlayer::Stop()
{
    if (handle != nullptr)
    {
        playing = false;
        StopAudioStream(strm);
        fclose(handle);
    }
}

int32_t BgmPlayer::GetPos()
{
    return pxtn.moo_get_now_clock();
}

int32_t BgmPlayer::GetEnd()
{
    return pxtn.moo_get_end_clock();
}

void BgmPlayer::Update()
{
    if (!playing)
    {
        return;
    }
    if (prevVolume != volume)
    {
        pxtn.moo_set_master_volume(volume);
    }
    prevVolume = volume;
    if (IsAudioStreamProcessed(strm))
    {
        u32 numSamples = BUFFER_SAMPLES;
        if (!pxtn.Moo(buffer, BUFFER_SIZE))
        {
            printf("PXTONE PLAY ERROR!\n");
            StopAudioStream(strm);
            playing = false;
            return;
        }
        UpdateAudioStream(strm, buffer, numSamples);
    }
}