
// this lovely file contains declarations of function for creating GUI windows.
//
// there are several GUI windows in level editor and all are created here
// Game Object window
// Properties window
// Container Content window (pops up when you want to add content to container)



#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CEditorManager;
class CGameObject;

void PopulateTreeNodeFromXML(CEditorManager* editorManager, stringc dir_filename, IGUIElement *parent, core::rect<s32> position, s32 id);
IGUIWindow* MakeContainerContentWindow(CEditorManager* editorManager, s32 id);
IGUIWindow* MakeGameObjectsWindow(CEditorManager* editorManager);
IGUIWindow* MakePropertiesWindow(CEditorManager* editorManager);
IGUIWindow* MakePickScriptWindow(CEditorManager* editorManager, stringw script);
IGUIWindow* MakeTriggerScriptWindow(CEditorManager* editorManager, s32 id, stringw filename);
IGUIWindow* MakeNPCScriptWindow(CEditorManager* editorManager, stringc filename);
IGUIWindow* MakeTerrainHeightmapWindow(CEditorManager* editorManager);
IGUIWindow* MakeMusicWindow(CEditorManager* editorManager);
IGUIWindow* MakeParticleWindow(CEditorManager* editorManager);
IGUIWindow* MakeMonsterScriptWindow(CEditorManager* editorManager, CGameObject* go);

