#pragma once

struct EditorNew;

// Toolbar for housing all the different tools.
struct Toolbar
{
    EditorNew* ed; // Base editor.

    // Create a new toolbar.
    Toolbar(EditorNew* ed);

    // Draw the toolbar UI.
    void DrawUI();

};