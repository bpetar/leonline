/** 
 * \file EditorManager.h
 * \brief CEditorManager class is the boss. The manager. It creates 3D device (irrlicht of course),
 * creates GUI, and 3D environment so called Level Editor. Calls all the initialization functions and 
 * goes to the loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#if !defined(AFX_GAMEMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_GAMEMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_


#include <irrlicht.h>
#include "level/EDLevel.h"
#include "gui/EDGui.h"
#include "Script.h"
#include "UndoActions.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define SOUND_DIR "media/sounds/"
#define MUSIC_DIR "media/music/"
#define ICON_DIR L"media/icons/"
#define MODEL_DIR L"media/models/"
#define OBJECT_DIR L"media/gameobjects/"
#define OBJECT_DIR_C8 "media/gameobjects/"
#define SCRIPT_DIR L"media/scripts/"
#define DIALOG_DIR "media/dialogs/"
#define TERRAIN_DIR "media/terrain/"
#define TREES_DIR "media/trees/"
#define PICKABLES_DIR L"media/gameobjects/Pickables/"
#define PICKABLES_DIR_C8 "media/gameobjects/Pickables/"
#define MONSTERS_DIR_C8 "media/gameobjects/Monsters/"
#define STATIC_DIR L"media/gameobjects/Static/"
#define PICKABLES_SCRIPTS_DIR "media/scripts/pickables/"
#define STATIC_SCRIPTS_DIR "media/scripts/static/"

#define ACTIONS_SCRIPT_FILE "media/scripts/actions.script"
#define CONDITIONS_SCRIPT_FILE "media/scripts/conditions.script"
#define FONT_FILE "media/Garamond14.xml"
#define IRRLOGO_FILE "media/irrlichtlogo2.png"

#define PROCEDURAL_TREE_MESH "procedural_tree_mesh"
#define PARTICLE_GAME_OBJECT "particle_game_object_mesh"
#define LIGHT_GAME_OBJECT "light_game_object_mesh"

/**
 * \brief CEditorManager class is the boss. The manager. It creates 3D device (irrlicht of course),
 * creates GUI, and 3D environment so called Level Editor. Calls all the initialization functions and 
 * goes to the loop.
 *
 * \author Petar Bajic \date July, 21 2008.
 */
class CEditorManager : public IEventReceiver
{
public:
	CEditorManager();
	~CEditorManager();
	void Update();
	bool isLoaded() {return true;}
	void Init();
	IrrlichtDevice* getDevice();
	IVideoDriver* getDriver();
	ISceneManager* getSceneMngr();
	IGUIEnvironment* getGUIEnvironment();
	CEditorGUI* getGUIManager() {return m_pGuiManager;}
	CEditorLevel* getEdiLevel() {return m_pEdiLevel;}
	CScript* getScriptEngine() {return m_ScriptEngine;}
	IFileSystem* getFS() {return m_FS;}
	void backToWorkingDirectory();
	virtual bool OnEvent(const SEvent& event);
	void AddGameObjectToLevel(CGameObject* go);
	void RemoveGameObjectFromMap(CGameObject* go);

	void AddUndoAction(TUndoAction undoAction);
	void Undo();

	s32 m_ID;

	stringw GetObjectParameter_State(s32 objectID);
	stringw GetObjectParameter_Name(s32 objectID);
	stringw GetObjectParameter_RootName(s32 objectID);
	stringw GetObjectParameter_Script(s32 objectID);
	stringw GetObjectParameter_Mesh(s32 objectID);
	void ObjectClearPickableItems(s32 objectID);
	void ObjectAddPickableItem(s32 objectID, stringc rootname);
	s32 GetSelectedObjectParameter_ID();
	s32 GetObjectNumberOfPickableItems(s32 id);
	CGameObject* GetObjectPickableItem(s32 id, s32 i);
	void SetObjectParameter_State(s32 objectID, stringw state);
	void SetSelectedObjectParameter_isContainer(bool isContainer);
	void SetSelectedObjectParameter_isPickable(bool isPickable);
	void SetSelectedObjectParameter_isTrigger(bool isTrigger);
	void SetSelectedObjectParameter_isNPC(bool isNPC);
	void SetSelectedObjectParameter_isMonster(bool isMonster);
	CGameObject* GetSelectedGameObject();
	bool SingleObjectSelected();
	void OnNewMap();

	stringc m_WorkingDirectory;

protected:
	

private:
	void CreateDevice();
	IrrlichtDevice* m_pDevice;  
	IVideoDriver* m_pDriver;
	ISceneManager* m_pSceneManager;
    IGUIEnvironment* m_pGUIEnvironment; //irrlichts default gui environment
	IFileSystem* m_FS;

	CEditorGUI* m_pGuiManager; //our homemade gui manager
	CEditorLevel* m_pEdiLevel;
	CScript* m_ScriptEngine;
	/*irr::gui::IGUIFont* m_pFont;*/
	CUndoActions* m_UndoActionsManager;
	
	int lastFPS;
};
#endif

