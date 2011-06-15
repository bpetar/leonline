
// this lovely file contains function for handling button events.
//
// When user clicks on some button, EDGui.cpp will pass event to this file.
// This file was created to reduce the size of EDGui.cpp



#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CEditorManager;

bool HandleButtonClick(CEditorManager* editorManager, s32 eventID);

