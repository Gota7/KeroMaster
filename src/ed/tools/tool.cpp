#include "tool.h"
#include "../editor.h"

const char* Tool::toolNames[] =
{
    "Hand",
    "Tile Brush",
    "Eraser",
    "Entity Hand"
};

void Tool::SetActive(Editor* ed, std::vector<int>& buttons)
{

    // If the tool should be released.
    if (toolActive)
    {
        toolActive = false;
        for (int i = 0; i < buttons.size(); i++)
        {
            toolActive |= !IsMouseButtonUp(buttons[i]);
        }
        if (!toolActive) Close(ed);
    }

    // Can use tool if not in modal or other window.
    else if (!ed->focus.mouseInWindow && !ed->focus.isModal)
    {
        toolActive = false;
        for (int i = 0; i < buttons.size(); i++)
        {
            toolActive |= IsMouseButtonPressed(buttons[i]);
        }
        if (toolActive) Init(ed);
    }

}