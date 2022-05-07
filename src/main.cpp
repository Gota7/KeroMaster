#include "ed/editor.h"

int main(int argc, char* argv[])
{
    Editor editor;
    editor.Init();
    return editor.EditorLoop();
}