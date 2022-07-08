# Kero Master
An editor for Kero Blaster, Pink Hour, and Pink Heaven. Haru To Shura support is planned in the future, but the format is different.

## Downloads
Check out the [releases](https://github.com/Gota7/KeroMaster/releases) page for downloads!

## Usage Manual
This section will teach you everything you need to know about using the editor.

### Getting Started
* Upon first starting the editor, it will prompt you to select the `Resource Path`.
* The easiest way to do this is to hit the `...` button, highlight the target folder, then hitting select.
* Hit the `...` button by `Level To Open` to select the level to edit.
* The editor will remember the last level you are editing, and will show this screen again if the level can not be found.
* You can then customize the editor's controls for which button uses the current tool, and what tool the other buttons of the mouse use.
* The `Use Tile Palette` option will show a palette to select tiles from with the tile brush. It is also possible to open a tileset and select tiles from there to draw.
* Hit `Save And Close` to start editing.

### Editor Navigation
In the top of the editor, you will see entries in the top main menu that will assist you in editing, some of the entries contain keyboard shortcuts:
* File - Manage the level the editor is editing.
* Edit - Edit options, along with settings.
* Window - Show a hidden or closed window.
* Tool - Select the currently selected tool.
* View - Show or hide certain items in the editor. Note that F4 will also toggle fullscreen.
* About - Show info about the editor.

Beneath the main menu at the top, you should see a toolbar that will assist with editing. The level editor window is where most of the level editing occurs.

### Customization
The editor allows for customization from using existing styles, or even custom ones that can be made within the editor.

#### Styling
* `Window->Style Editor` will show the style editor.
* The dropdown box will allow you to change the currently selected style.
* The name box below it will allow you to change the name of the current style.
* The `New Theme` button will add a theme, and the `Delete Theme` button will delete the current theme.
* When editing the current style with the controls below, you can save your changes or revert them with the corresponding buttons.

#### Music
* `Window->Music Player` will show the music player.
* With this, you can select a song to play on repeat from the game you are currently editing.

### Level Editor
* The level editing window contains various aspects of the level that can be edited.
* Hovering over a `?` will explain what the field does.
* The names of tilesets in the `img` folder of the resource path can be adjusted, just make sure `Reload Tilesets` is hit after.
* Opening a tileset will open a tileset editor for that tileset.
* You can also edit the current script for the level, opening a non-existant one will create a new one.
* You can resize the individual map layers and move them as well.
* Layer 0 is the foreground, Layer 1 is the middleground, and Layer 2 is the background.

### Tileset Editor
* Both the palette and tileset editor allow you to drag your mouse to select multiple tiles.
* If the Tile Brush tool is used, you can draw these selected tiles to the screen.
* Viewing attributes will allow you to see collisions for tiles.
* In the tileset editor, you can edit the collision attributes with the button to bring up another editor.
* Saving attributes for a tileset that doesn't exist will create a new attributes file.

### Tools
There are various tools to assist you in editing levels.
* Hand - Move/pan the camera around.
* Tile Brush - Draw selected tiles to the layer.
* Eraser - Erase tiles from the current layer.
* Entity Hand - Move, edit, place, and delete entities. The toolbar is necessary to use to add more. Hit the delete key to delete the currently selected entity.

## Building
In order to build Kero Master, cmake must be installed along with python 3, both accessible from the PATH. Building with `cmake .` should build the project, and place the executeable in the `bin` folder. Note that the object_data folder must be in the same path you are running Kero Master from.

## Editor Source Layout
The editor source composes of multiple subdirectories for various purposes:

* root - Main bootstrapping code along with shortcuts for types.
* bgm - Background music playback utilizing Pixel's pxtone.
* conv - Shift-JIS character conversion code.
* ed - Editor and UI related code.
* gbin - Gota's binary file library.
* px - Reading and writing Studio Pixel's file formats.
* rlImGui - ImGui rendering in Raylib and utilities.
* tinyxml2 - Tiny XML 2 library for reading editor XML files.

## Credits
Kero Master is created by Gota7, Alula, and DrippingYelow.