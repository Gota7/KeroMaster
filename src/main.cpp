#include "ed/editorNew.h"

int main(int argc, char* argv[])
{
    EditorNew editor;
    editor.Init();
    return editor.EditorLoop();
}