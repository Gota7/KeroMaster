#pragma once

struct Editor;

// Toolbar for housing all the different tools.
struct Toolbar
{
    Editor* ed; // Base editor.

    // Create a new toolbar.
    Toolbar(Editor* ed);

    // Draw the toolbar UI.
    void DrawUI();

};