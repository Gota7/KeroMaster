#include "musicPlayer.h"
#include "editorNew.h"
#include "../bgm/bgm.h"
#include "imgui.h"

MusicPlayer::MusicPlayer(EditorNew* ed)
{
    this->ed = ed;
}

void MusicPlayer::DrawUI()
{
    if (!open) return;

    // Main playing code.
    ImGui::Begin("Music Player", &open, ImGuiWindowFlags_AlwaysAutoResize);
    ed->focus.ObserveFocus();
    ImGui::Combo("Song", &BgmPlayer::currSongInd, BgmPlayer::songs, BgmPlayer::numSongs);
    ImGui::ProgressBar(BgmPlayer::GetPos() / (float)std::max(BgmPlayer::GetEnd(), 1));
    ImGui::SliderFloat("Volume", &BgmPlayer::volume, 0, 1, "%f", 1);
    if (ImGui::Button("Play"))
    {
        BgmPlayer::Play(BgmPlayer::songs[BgmPlayer::currSongInd]);
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause/Resume"))
    {
        if (BgmPlayer::playing)
        {
            BgmPlayer::Pause();
        }
        else
        {
            BgmPlayer::Resume();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        BgmPlayer::Stop();
    }
    ImGui::End();

}