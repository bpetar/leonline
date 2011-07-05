/** 
 * \file EDGui.h
 * \brief Header file containing CEditorGUI class used for managing GUI action.
 * CEditorGUI class creates menus and property windows and buttons and handles all the user clicks.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#if !defined(AFX_EDGUICA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_EDGUICA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
#include "GUIWndMaker.h"
#include "GUIMenuHandler.h"
#include "GUIButtonHandler.h"
#include "IGUITreeCtrl.h"
#include "../level/GameObject.h"

class CEditorManager;
class CPickableItem;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


typedef enum EDirection
{
	LE_X_UP,
	LE_X_DOWN,
	LE_Y_UP,
	LE_Y_DOWN,
	LE_Z_UP,
	LE_Z_DOWN,
	LE_LAST
} TDirection;


enum GUI_ID
{
	GUI_ID_BUTTON_ADD_CONTAINER = 100,
	GUI_ID_BUTTON_CONTAINER_OK,
	GUI_ID_BUTTON_CONTAINER_CANCEL,
	GUI_ID_CONTAINER,
	GUI_ID_BUTTON_CONTAINER_ADD_ITEM,
	GUI_ID_BUTTON_CONTAINER_EDIT_PICK_SCRIPT,
	GUI_ID_BUTTON_PICK_ADD,
	GUI_ID_BUTTON_PICK_ADD_ACTION,
	GUI_ID_BUTTON_PICK_ADD_EVENT,
	GUI_ID_BUTTON_PICK_REMOVE_EVENT,
	GUI_ID_BUTTON_PICK_SAVE,
	GUI_ID_BUTTON_PICK_CANCEL,
	GUI_ID_BUTTON_TRIGGER_ADD,
	GUI_ID_BUTTON_TRIGGER_ADD_STATE,
	GUI_ID_BUTTON_TRIGGER_SET_STATE,
	GUI_ID_BUTTON_TRIGGER_REMOVE_STATE,
	GUI_ID_BUTTON_TRIGGER_ADD_ACTION,
	GUI_ID_BUTTON_TRIGGER_SAVE,
	GUI_ID_BUTTON_TRIGGER_CANCEL,
	GUI_ID_BUTTON_NPC_ADD,
	GUI_ID_BUTTON_NPC_ADD_NODE,
	GUI_ID_BUTTON_NPC_REMOVE_NODE,
	GUI_ID_BUTTON_NPC_LINK_NODE,
	GUI_ID_BUTTON_NPC_TOGGLE_ENABLED,
	GUI_ID_BUTTON_NPC_CHANGE_TEXT,
	GUI_ID_BUTTON_NPC_ADD_ACTION,
	GUI_ID_BUTTON_NPC_CLEAR_ACTION,
	GUI_ID_BUTTON_NPC_ADD_TEXT,
	GUI_ID_BUTTON_NPC_WARNING,
	GUI_ID_BUTTON_NPC_OK,
	GUI_ID_BUTTON_NPC_CANCEL,
	GUI_ID_BUTTON_NPC_ADD_CONDITION,
	GUI_ID_BUTTON_MONSTER_ADD,
	GUI_ID_BUTTON_TH_INSERT,
	GUI_ID_BUTTON_TH_HEIGHTMAP_BROWSE,
	GUI_ID_BUTTON_TH_TEXTURE_BROWSE,
	GUI_ID_BUTTON_TH_DETAIL_BROWSE,
	GUI_ID_BUTTON_POSITION_X_DOWN,
	GUI_ID_BUTTON_POSITION_X_UP,
	GUI_ID_BUTTON_POSITION_Y_DOWN,
	GUI_ID_BUTTON_POSITION_Y_UP,
	GUI_ID_BUTTON_POSITION_Z_DOWN,
	GUI_ID_BUTTON_POSITION_Z_UP,
	GUI_ID_BUTTON_ROTATION_X_DOWN,
	GUI_ID_BUTTON_ROTATION_X_UP,
	GUI_ID_BUTTON_ROTATION_Y_DOWN,
	GUI_ID_BUTTON_ROTATION_Y_UP,
	GUI_ID_BUTTON_ROTATION_Z_DOWN,
	GUI_ID_BUTTON_ROTATION_Z_UP,
	GUI_ID_BUTTON_PROPERTY_SET,
	GUI_ID_BUTTON_LEVELMUSIC_MUSIC_BROWSE,
	GUI_ID_BUTTON_LEVELMUSIC_SOUND_BROWSE,
	GUI_ID_BUTTON_LEVELMUSIC_NARATION_BROWSE,
	GUI_ID_BUTTON_LEVELMUSIC_SAVE,
	GUI_ID_BUTTON_PARTICLESYSTEM_INSERT,

	GUI_ID_CHECKBOX_PROPERTY_ANCOR,
	GUI_ID_CHECKBOX_PROPERTY_VISIBLE,
	GUI_ID_CHECKBOX_PROPERTY_ILLUSION,
	GUI_ID_CHECKBOX_PROPERTY_TERRAIN,
	GUI_ID_CHECKBOX_PROPERTY_CONTAINER,
	GUI_ID_CHECKBOX_PROPERTY_PICKABLE,
	GUI_ID_CHECKBOX_PROPERTY_TRIGGER,
	GUI_ID_CHECKBOX_PROPERTY_NPC,
	GUI_ID_CHECKBOX_PROPERTY_MONSTER,
	GUI_ID_CHECKBOX_CONTAINER_DIRECT_PICK,
	GUI_ID_CHECKBOX_PICK_EQUIPABLE,
	GUI_ID_CHECKBOX_PICK_USABLE,

	GUI_ID_TEXT_CONTAINER_PICKABLE_DESC,
	GUI_ID_TEXT_MONSTER_SKILLNAME,

	GUI_ID_EDITBOX_PROPERTY_NAME,
	GUI_ID_EDITBOX_PROPERTY_MODEL,
	GUI_ID_EDITBOX_PROPERTY_ID,
	GUI_ID_EDITBOX_PROPERTY_POS_X,
	GUI_ID_EDITBOX_PROPERTY_POS_Y,
	GUI_ID_EDITBOX_PROPERTY_POS_Z,
	GUI_ID_EDITBOX_PROPERTY_ROT_X,
	GUI_ID_EDITBOX_PROPERTY_ROT_Y,
	GUI_ID_EDITBOX_PROPERTY_ROT_Z,
	GUI_ID_EDITBOX_PROPERTY_DESCRIPTION,
	GUI_ID_EDITBOX_PICK_SCRIPT,
	GUI_ID_EDITBOX_PICK_ACTION_ATTR,
	GUI_ID_EDITBOX_PICK_ACTION_VALUE,
	GUI_ID_EDITBOX_PICK_ACTION_TARGET_ID,
	GUI_ID_EDITBOX_PICK_EVENT_TARGET_ID,
	GUI_ID_EDITBOX_NPC_SELECTED_NODE_ID,
	GUI_ID_EDITBOX_NPC_ACTIONS,
	GUI_ID_EDITBOX_NPC_CHANGE_TEXT,
	GUI_ID_EDITBOX_NPC_ADD_TEXT,
	GUI_ID_EDITBOX_NPC_ACTION_TARGET_ID,
	GUI_ID_EDITBOX_NPC_ACTION_ATTR,
	GUI_ID_EDITBOX_NPC_ACTION_VALUE,
	GUI_ID_EDITBOX_TRIGGER_SCRIPT,
	GUI_ID_EDITBOX_TRIGGER_STATE,
	GUI_ID_EDITBOX_TRIGGER_ACTION_ATTRIBUTE,
	GUI_ID_EDITBOX_TRIGGER_ACTION_VALUE,
	GUI_ID_EDITBOX_TRIGGER_ACTION_TARGET_ID,
	GUI_ID_EDITBOX_TH_HEIGHT_FILE,
	GUI_ID_EDITBOX_TH_TEX_FILE,
	GUI_ID_EDITBOX_TH_DETAIL_FILE,
	GUI_ID_EDITBOX_TH_POS_X,
	GUI_ID_EDITBOX_TH_POS_Y,
	GUI_ID_EDITBOX_TH_POS_Z,
	GUI_ID_EDITBOX_TH_ROT_X,
	GUI_ID_EDITBOX_TH_ROT_Y,
	GUI_ID_EDITBOX_TH_ROT_Z,
	GUI_ID_EDITBOX_TH_SCALE_X,
	GUI_ID_EDITBOX_TH_SCALE_Y,
	GUI_ID_EDITBOX_TH_SCALE_Z,
	GUI_ID_EDITBOX_TH_MAXLOD,
	GUI_ID_EDITBOX_TH_SMOOTH,
	GUI_ID_EDITBOX_TH_PATCH,
	GUI_ID_EDITBOX_LEVELMUSIC_MUSIC_FILE,
	GUI_ID_EDITBOX_LEVELMUSIC_SOUND_FILE,
	GUI_ID_EDITBOX_LEVELMUSIC_NARATION_FILE,


	GUI_ID_LISTBOX_PICK_EVENTS,
	GUI_ID_LISTBOX_TRIGGER_STATES,
	
	
	
	
	
	
	GUI_ID_COMBOBOX_PICK_EVENTS,
	GUI_ID_COMBOBOX_PICK_ACTIONS,
	GUI_ID_COMBOBOX_PICK_EVENT_TARGET,
	GUI_ID_COMBOBOX_PICK_ACTION_TARGET,
	GUI_ID_COMBOBOX_NPC_ACTION_TARGET,
	GUI_ID_COMBOBOX_NPC_ACTIONS,
	GUI_ID_COMBOBOX_NPC_CONDITIONS,
	GUI_ID_COMBOBOX_TRIGGER_ACTIONS,
	GUI_ID_COMBOBOX_TRIGGER_ACTION_TARGET,
	GUI_ID_WINDOW_PROPERTIES,
	GUI_ID_WINDOW_GAME_OBJECTS,
	GUI_ID_WINDOW_CONTAINER,
	GUI_ID_WINDOW_NPC,
	GUI_ID_WINDOW_NPC_ADD_TEXT,
	GUI_ID_WINDOW_MONSTER,
	GUI_ID_WINDOW_TRIGGER,
	GUI_ID_WINDOW_PICKABLE,
	GUI_ID_WINDOW_TH,
	GUI_ID_WINDOW_LEVEL_MUSIC,
	GUI_ID_MENU_NEW_LEVEL,
	GUI_ID_MENU_OPEN_LEVEL,
	GUI_ID_MENU_SAVE_LEVEL,
	GUI_ID_MENU_SAVE_LEVEL_AS,
	GUI_ID_MENU_SAVE_IRR,
	GUI_ID_MENU_LOAD_IRR,
	GUI_ID_MENU_QUIT,
	GUI_ID_MENU_UNDO,
	GUI_ID_MENU_REDO,
	GUI_ID_MENU_SELECT_ALL,
	GUI_ID_MENU_INSERT_CUBE,
	GUI_ID_MENU_INSERT_SPHERE,
	GUI_ID_MENU_INSERT_GRASS,
	GUI_ID_MENU_INSERT_TERRAIN,
	GUI_ID_MENU_INSERT_TREE_ASPEN,
	GUI_ID_MENU_INSERT_TREE_OAK,
	GUI_ID_MENU_INSERT_TREE_PINE,
	GUI_ID_MENU_INSERT_TREE_WILLOW,
	GUI_ID_MENU_INSERT_FLAG,
	GUI_ID_MENU_INSERT_MUSIC,
	GUI_ID_MENU_INSERT_PARTICLE,
	GUI_ID_MENU_HELP_ABOUT,
	GUI_ID_MENU_HELP_HOMEPAGE,
	GUI_ID_MENU_HELP_TUT1,
	GUI_ID_MENU_HELP_TUT2,
	GUI_ID_MENU_HELP_TUT3,
	GUI_ID_MENU_HELP_TUT4,
	GUI_ID_MENU_VIEW_OBJECT_PROPERTIES,
	GUI_ID_MENU_VIEW_OBJECT_PREVIEW,
	GUI_ID_MENU_VIEW_GAME_OBJECTS,
	GUI_ID_TAB_GAME_OBJECTS,
	GUI_ID_TREECTRL_PICKABLE_OBJECTS,
	GUI_ID_MONSTER_RADIUS,
	GUI_ID_MONSTER_MOOD,
	GUI_ID_MONSTER_HP,
	GUI_ID_MONSTER_EXP,
	GUI_ID_MONSTER_SAVE,
	GUI_ID_MONSTER_SAVEFORALL,
	GUI_ID_MONSTER_CANCEL,
	GUI_ID_MONSTER_SOUND_AMBIENT,
	GUI_ID_MONSTER_SOUND_ATTACK,
	GUI_ID_MONSTER_SOUND_HIT,
	GUI_ID_MONSTER_SOUND_WOUND,
	GUI_ID_MONSTER_SOUND_HELLO,
	GUI_ID_MONSTER_SOUND_DIE,
};

#define GUI_ID_MONSTER_SKILL 1000
#define GUI_ID_TREECTRL_GAME_OBJECTS_START 6000
#define GUI_ID_TREECTRL_GAME_OBJECTS_END 6090
#define GUI_ID_TREECTRL_GAME_OBJECTS_SCENE GUI_ID_TREECTRL_GAME_OBJECTS_START + 1

/**
 * \brief CEditorGUI class creates menus and property windows and buttons and handles all the user clicks.
 *
 * \author Petar Bajic \date July, 21 2008.
 */
class CEditorGUI
{

public:
	CEditorGUI();
	~CEditorGUI();
	void OnNewMap();
	bool Init(CEditorManager* edMngr);
	void SetProperties(CGameObject* go);
	void SetPropertiesPosition(f32 x, f32 y, f32 z);
	void SetPropertiesRotation(f32 x, f32 y, f32 z);
	void ClearProperties();
	void SetElementAtHand(const stringw &path, const stringw &name, bool isStatic);
	void SendModelToPhotoSession(const stringw &parent, const stringw &name);
	void SetGameObjectToContainer(const stringw &parent, const stringw &name);
	void WriteNPCDialogScriptAction();
	void SavePickScript(const stringc &file);
	void SaveTriggerScript(const stringc &file);
	void WriteElement(IXMLWriter* xml, TreeNode* node);
	void CheckDialogNodeActions(TreeNode* node, u32 deletedNodeID);
	void SaveDialog(const stringc &file);
	bool OnEvent(const SEvent& event);
	void RemoveNodeFromSceneTree(s32 id);
	void AddNodeToSceneTree(s32 id, const stringw &name);
	void ClearTreeOfSceneNodes();
	void SetSelectedElementInTheTreeofSceneNodes(int id);
	bool GUIWindowOpen();

	CEditorManager* m_EditorManager;

	//Gui manager has a lot of pointers to gui elements:
	IGUIContextMenu* m_TreeSubmenu;
	IGUIContextMenu* m_ViewMenu;
	s32 m_ViewMenuItem_Properties;
	s32 m_ViewMenuItem_GameObjects;
	s32 m_ViewMenuItem_ObjectPreview;
	bool m_TreeSubmenuWasVisible;
	bool m_bGUIFocused; //what is this?
	bool m_bMoveModel;
	bool m_bRotateModel;
	TDirection m_eMoveDirection;
	TDirection m_eRotateDirection;
	u32  m_uClickStartTime;
	IGUITreeCtrl *m_SceneNodesTree;

	//properties window gui elements
	IGUICheckBox* m_PropCheckBox_Anchor;
	IGUICheckBox* m_PropCheckBox_Terrain;
	IGUIEditBox* m_PropEditBox_Name;
	IGUIEditBox* m_PropEditBox_Pos_X;
	IGUIEditBox* m_PropEditBox_Pos_Y;
	IGUIEditBox* m_PropEditBox_Pos_Z;
	IGUIEditBox* m_PropEditBox_Rot_X;
	IGUIEditBox* m_PropEditBox_Rot_Y;
	IGUIEditBox* m_PropEditBox_Rot_Z;
	IGUIEditBox* m_PropEditBox_Model;
	IGUIEditBox* m_PropEditBox_ID;
	IGUIEditBox* m_PropEditBox_Description;
	IGUICheckBox* m_PropCheckBox_Invisible;
	IGUICheckBox* m_PropCheckBox_Illusion;
	IGUICheckBox* m_PropCheckBox_Container;
	IGUICheckBox* m_PropCheckBox_Pickable;
	IGUICheckBox* m_PropCheckBox_Trigger;
	IGUICheckBox* m_PropCheckBox_NPC;
	IGUICheckBox* m_PropCheckBox_Monster;
	IGUIButton* m_ContainerButton;
	IGUIButton* m_PickScriptButton;
	IGUIButton* m_TriggerScriptButton;
	IGUIButton* m_NPCScriptButton;
	IGUIButton* m_MonsterScriptButton;
	IGUIButton* m_PropButton_Pos_X_Up;
	IGUIButton* m_PropButton_Pos_X_Down;
	IGUIButton* m_PropButton_Pos_Y_Up;
	IGUIButton* m_PropButton_Pos_Y_Down;
	IGUIButton* m_PropButton_Pos_Z_Up;
	IGUIButton* m_PropButton_Pos_Z_Down;
	IGUIButton* m_PropButton_Rot_X_Up;
	IGUIButton* m_PropButton_Rot_X_Down;
	IGUIButton* m_PropButton_Rot_Y_Up;
	IGUIButton* m_PropButton_Rot_Y_Down;
	IGUIButton* m_PropButton_Rot_Z_Up;
	IGUIButton* m_PropButton_Rot_Z_Down;
	IGUIButton* m_PropButton_Set;

	//pick script widow gui elements
	IGUIListBox* m_PickListBox_ExistingEvents;
	IGUICheckBox* m_PickCheckBox_Equipable;
	IGUICheckBox* m_PickCheckBox_Usable;
	IGUIComboBox* m_PickComboBox_Actions;
	IGUIComboBox* m_PickComboBox_Events;
	IGUIComboBox* m_PickComboBox_EventTarget;
	IGUIComboBox* m_PickComboBox_ActionTarget;
	IGUIEditBox* m_PickEditBox_ActionTargetID;
	IGUIEditBox* m_PickEditBox_EventTargetID;
	IGUIEditBox* m_PickEditBox_ActionAttribute;
	IGUIEditBox* m_PickEditBox_ActionValue;
	IGUIEditBox* m_PickEditBox_Script;
	IGUIButton* m_PickButton_AddAction;
	IGUIButton* m_PickButton_AddEvent;
	IGUIButton* m_PickButton_RemoveEvent;
	IGUIButton* m_PickButton_Save;
	IGUIButton* m_PickButton_Cancel;

	//Trigger script window
	IGUIComboBox* m_TriggerComboBox_Actions;
	IGUIComboBox* m_TriggerComboBox_ActionTarget;
	IGUIListBox* m_TriggerListBox_States;
	IGUIEditBox* m_TriggerEditBox_State;
	IGUIEditBox* m_TriggerEditBox_Script;
	IGUIButton* m_TriggerButton_AddState;
	IGUIButton* m_TriggerButton_RemoveState;
	IGUIButton* m_TriggerButton_SetState;
	IGUIButton* m_TriggerButton_AddAction;

	//dialog
	IGUIEditBox* m_DialogEditBox_Text;
	IGUIEditBox* m_DialogEditBox_SelectedNodeID;
	IGUIEditBox* m_DialogEditBox_ChangeText;
	IGUIEditBox* m_DialogEditBox_Answer;
	IGUIEditBox* m_DialogEditBox_ActionsText;
	IGUIEditBox* m_DialogEditBox_ActionAttribute;
	IGUIEditBox* m_DialogEditBox_ActionValue;
	IGUIEditBox* m_DialogEditBox_ActionTargetID;
	IGUIComboBox* m_DialogComboBox_Actions;
	IGUIComboBox* m_DialogComboBox_ActionTarget;
	IGUIComboBox* m_DialogComboBox_Conditions;
	IGUIButton* m_DialogButton_AddNode;
	IGUIButton* m_DialogButton_RemoveNode;
	IGUIButton* m_DialogButton_LinkNodes;
	IGUIButton* m_DialogButton_ToggleEnabled;
	IGUIButton* m_DialogButton_ChangeText;
	IGUIButton* m_DialogButton_AddAction;
	IGUIButton* m_DialogButton_ClearAction;
	IGUIButton* m_DialogButton_AddCondition;
	IGUIButton* m_DialogButton_Ok;
	IGUIButton* m_DialogButton_Cancel;
	IGUIButton* m_DialogButton_AddText;
	IGUIButton* m_DialogButton_Warning;
	IGUITreeCtrl* m_DialogTree;
	TreeNode *m_SelectedNode;
	u32 m_NumNodes;
	bool m_bLinkDialogNodes;
	stringc m_DialogFilename;

	//terrain
	IGUIEditBox* m_TH_EditBox_HeightmapFile;
	IGUIButton* m_TH_Button_HeightmapBrowse;
	IGUIEditBox* m_TH_EditBox_TexFile;
	IGUIButton* m_TH_Button_TexBrowse;
	IGUIEditBox* m_TH_EditBox_DetailFile;
	IGUIButton* m_TH_Button_DetailBrowse;
	IGUIEditBox* m_TH_EditBox_PosX;
	IGUIEditBox* m_TH_EditBox_PosY;
	IGUIEditBox* m_TH_EditBox_PosZ;
	IGUIEditBox* m_TH_EditBox_RotX;
	IGUIEditBox* m_TH_EditBox_RotY;
	IGUIEditBox* m_TH_EditBox_RotZ;
	IGUIEditBox* m_TH_EditBox_ScaleX;
	IGUIEditBox* m_TH_EditBox_ScaleY;
	IGUIEditBox* m_TH_EditBox_ScaleZ;
	IGUIEditBox* m_TH_EditBox_MaxLOD;
	IGUIEditBox* m_TH_EditBox_SmoothFactor;
	IGUIComboBox* m_TH_ComboBox_PatchSize;
	IGUIButton* m_TH_Button_Insert;

	//music
	IGUIEditBox* m_LevelMusic_EditBox_MusicFile;
	IGUIEditBox* m_LevelMusic_EditBox_SoundFile;
	IGUIEditBox* m_LevelMusic_EditBox_NarationFile;
	IGUIButton* m_LevelMusic_Button_MusicBrowse;
	IGUIButton* m_LevelMusic_Button_SoundBrowse;
	IGUIButton* m_LevelMusic_Button_NarationBrowse;
	IGUIButton* m_LevelMusic_Button_Save;

	//monsters
	IGUIComboBox* m_Monsters_ComboBox_Mood;
	IGUIEditBox* m_Monsters_EditBox_Health;

	//windows
	IGUIWindow* m_wnd_GameItems;
	IGUIWindow* m_wnd_Properties;
	IGUIWindow* m_wnd_ContainerContent;
	IGUIWindow* m_wnd_AddNPCScriptAction;
	IGUIWindow* m_wnd_AddNPCNewText;
	IGUIWindow* m_wnd_AddPickScriptAction;
	IGUIWindow* m_wnd_AddTriggerScriptAction;
	IGUIWindow* m_wnd_MonsterProperties;
	IGUIWindow* m_wnd_ModelPreview;
	IGUIWindow* m_wnd_InsertTerrainHeightmap;
	IGUIWindow* m_wnd_LevelMusic;
	IGUIWindow* m_wnd_ParticleSystem;
	
	s32 mCenterX;
	s32 mCenterY;

	s32 m_SelectedObjectID;
	stringw m_PickScript;

	IGUITabControl* m_tabGameObjects;

	bool m_bIrrFileSaveDialog;
	bool m_bIrrFileOpenDialog;
	bool m_bMapSaveDialog;
	bool m_bMapOpenDialog;
	bool m_bHeightmapOpenDialog;
	bool m_bTextureOpenDialog;
	bool m_bDetailOpenDialog;
	bool m_bElementPicking;
	bool m_bAddPickDirectly;
	bool m_bLevelMusicOpenDialog;
	bool m_bLevelSoundOpenDialog;
	bool m_bLevelNarationOpenDialog;

	//container GUI
	IGUICheckBox* m_PickCheckBox_DirectPick;
	IGUIButton* m_pContainer_Ok;
	IGUIButton* m_pContainer_Cancel;
	IGUIButton* m_pContainer_EditPickItemScript;
	IGUIButton* m_pContainer_AddPickItemToContainer;
	IGUIStaticText* m_pPickPreviewDescriptionText;
	ITexture* m_slotTex;
	ITexture* m_SelectedPickItemIcon_Texture;
	ITexture* m_ContainerPickPreview_TextureToRenderOn;
	ICameraSceneNode* m_PickCamera;
	CGameObject* m_pPreviewPickableItem;

	//Model Preview window
	ICameraSceneNode* m_PhotoCamera;
	ITexture* m_TextureToRenderOn;
	IGUIStaticText* m_pPreviewDescriptionText;
};


#endif