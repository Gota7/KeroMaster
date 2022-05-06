#include "tools.h"

#include "hand.h"
#include "zoom.h"
#include "../editorNew.h"

const char* Tools::toolNames[] =
{
    "Hand",
    "Tile Brush",
    "Eraser",
    "Entity Hand",
    "Zoom"
};

const ToolBasicFunction Tools::initFunctions[] =
{
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

const ToolActivateFunction Tools::activateFunctions[] =
{
    HandToolActivate,
    nullptr,
    nullptr,
    nullptr,
    ZoomToolActivate
};

const ToolBasicFunction Tools::updateFunctions[] =
{
    HandToolUpdate,
    nullptr,
    nullptr,
    nullptr,
    ZoomToolUpdate
};

const ToolBasicFunction Tools::closeFunctions[] =
{
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

void Tools::Init(EditorNew* ed, int toolItem)
{
    if (toolItem >= 0 && toolItem < (int)ToolItem::NumTools && initFunctions[toolItem] != nullptr)
    {
        initFunctions[toolItem](ed);
    }
}

void Tools::Activate(EditorNew* ed, int toolItem, Vector2 pos1, Vector2 pos2)
{
    if (toolItem >= 0 && toolItem < (int)ToolItem::NumTools && activateFunctions[toolItem] != nullptr)
    {
        activateFunctions[toolItem](ed, pos1, pos2);
    }
}

void Tools::Update(EditorNew* ed, int toolItem)
{
    if (toolItem >= 0 && toolItem < (int)ToolItem::NumTools && updateFunctions[toolItem] != nullptr)
    {
        updateFunctions[toolItem](ed);
    }
}

void Tools::Close(EditorNew* ed, int toolItem)
{
    if (toolItem >= 0 && toolItem < (int)ToolItem::NumTools && closeFunctions[toolItem] != nullptr)
    {
        closeFunctions[toolItem](ed);
    }
}

void Tools::SetActive(EditorNew* ed, int toolItem, std::vector<int>& buttons)
{

    // Safety check.
    if (toolItem < 0 || toolItem >= (int)ToolItem::NumTools) return;

    // If the tool should be released.
    if (toolActive[toolItem])
    {
        toolActive[toolItem] = false;
        for (int i = 0; i < buttons.size(); i++)
        {
            toolActive[toolItem] |= IsMouseButtonDown(buttons[i]);
        }
        if (!toolActive[toolItem]) Close(ed, toolItem);
    }

    // Can use tool if not in modal or other window.
    else if (!ed->focus.mouseInWindow && !ed->focus.isModal)
    {
        toolActive[toolItem] = false;
        for (int i = 0; i < buttons.size(); i++)
        {
            toolActive[toolItem] |= IsMouseButtonPressed(buttons[i]);
        }
        if (toolActive[toolItem]) Init(ed, toolItem);
    }

}

void Tools::Update(EditorNew* ed)
{
    for (int i = 0; i < (int)ToolItem::NumTools; i++)
    {
        if (toolActive[i]) Update(ed, i);
    }
}