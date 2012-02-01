/** 
 * \file GameGUI.h
 * \brief Header file containing CGameGUI class used for managing GUI action.
 * CGameGUI class creates windows and interface and buttons and sprites 
 * and handles all the user clicks on them.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#if !defined(AFX_GameGUICA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_GameGUICA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
#include "CGUIContainer.h"
#include "CGUIBar.h"
#include "CDialogWindow.h"
#include "CGUITextBox.h"
#include "../Languages.h"

class CGameManager;
class CLevelManager;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


#define GAME_CONST_CONTAINER_ID 1357
#define GAME_CONST_INVENTORY_ID 1358

enum GUI_ID
{
	GUI_ID_MAIN_MENU_COMBOBOX_LANGUAGE = 100,
	GUI_ID_MAIN_MENU_COMBOBOX_RESOLUTION
};

struct SFloatingText
{
	vector3df position;
	stringc text;
	float remainingTime;
	SColor color;

	SFloatingText(vector3df pos, stringc tex, float timeout, SColor _color)
	{
		position = pos;
		text = tex;
		remainingTime = timeout;
		color = _color;
	}
};

typedef struct
{
	stringw text;
	IGUIFont* font;
	recti rectangle;
	SColor color;
} TTextItem;

typedef struct
{
	stringw text;
	u32 height;
	u32 width;
	u32 depth;
} TResolution;
/**
 * \brief CGameGUI class creates windows and interface and buttons and sprites.
 *
 * \author Petar Bajic \date July, 21 2008.
 */
class CGameGUI
{
public:
	CGameGUI();
	~CGameGUI();
	bool Init(CGameManager* gameMngr);
	bool InitGameGUI();
	bool InitMenu();
	bool InitIntroMovie();
	void ClearIntroMovie();
	void ClearMenu();
	//void SetIconAtHand(stringw parent, stringw name);
	bool OnEvent(const SEvent& event);
	bool OnMenuEvent(const SEvent& event);
	bool OnMovieEvent(const SEvent& event);
	void DisplayContainerContent(s32 id, IVideoDriver* driver, IGUIEnvironment* env, CLevelManager* levelManager);
	void DisplayOptionsWindow(IGUIEnvironment* env);
	void DisplayCharacterSheetWindow(IGUIEnvironment* env);
	void DisplaySkillsWindow(IGUIEnvironment* env);
	void DisplayDeathWindow(IGUIEnvironment* env, stringw message);
	void DisplayPickableInfo(IGUIEnvironment* env, CGameObject* go, recti pos);
	bool InventoryFull() {return m_Inventory->isFull();}
	void AddConsoleText(stringw text);
	void AddConsoleText(ELanguageID textID);
	void ClearConsole();
	void AddPickableToInventory(CGameObject* item);
	CGameObject* getItemFromInventory(s32 id);
	void RemovePickableFromInventory(s32 id);
	void StartNPCDialog(stringw dlgFilename, stringw NPCName);
	void ClearInventory();
	void SaveNPCDialogs();
	void LoadNPCDialogs();
	void AddGameDialog(stringc dialog);
	void AddMsgBox(stringw title, stringw message);
	void AddMsgBox(ELanguageID titleID, ELanguageID messageID);
	void DrawFloatingText(vector3df pos, stringc text, u32 timeout, SColor color);
	void renderFloatingTexts(IGUIFont* font, float elapsedTime);
	void drawMenu(float elapsedTime);
	void drawIntroMovie(float elapsedTime);

	CDialogWindow* getDialogManager() { return m_DialogWindow;}

	CGameManager* m_GameManager;

	bool m_bDraggingPickableItem;
	bool m_bFirstTimeContainerClick;
	CGameObject* m_pDraggedPickableItem;
	CGUIBar* healthBar;
	array <SFloatingText*> m_FloatingTexts;
	u32 m_NumberOfSkills;
	bool m_pHoverOverMenuItem;

	CGUIBar* cs_ability_bars[10];
	CGUIBar* cs_skill_bars[10];
	CDialogWindow* m_DialogWindow;
	IGUIWindow* m_wnd_charSheet;
	void LevelUp(bool value) {m_LevelUpButton->setVisible(value);}

private:

	//IGUIWindow* m_wnd_gameItems;
	s32 m_hoveredSlot;
	ITexture* m_PickInfoBackgroundTexture;
	IGUIStaticText* pickInfoDescription;
	IGUIStaticText* pickInfoName;
	IGUIImage* pickInfoImage;
	IGUIFont* fontGaramond12;
	IGUIWindow* m_wnd_Msg;
	IGUIWindow* m_wnd_containerContent;
	IGUIWindow* m_wnd_options;
	IGUIWindow* m_wnd_skills;
	IGUIWindow* m_wnd_death;
	IGUIWindow* m_wnd_pickInfo;
	IGUIContainer* m_Inventory;
	
	//CGUITextBox* m_Console;
	IGUIEditBox* m_Console;

	IGUIEditBox* m_EditBox_Skills_Min;
	IGUIEditBox* m_EditBox_Skills_AvailablePoints;
	IGUIEditBox* m_EditBox_Skills_Max;
	IGUIButton* m_ButtonSkills_Min_Down;
	IGUIButton* m_ButtonSkills_Min_Up;
	IGUIButton* m_ButtonSkills_Max_Down;
	IGUIButton* m_ButtonSkills_Max_Up;
	s32 m_ContainerID;
	s32 m_ActiveDialogIndex;
	IGUIButton* m_ContainerTakeAllButton;
	IGUIButton* m_SettingsButton;
	IGUIButton* m_LevelUpButton;
	IGUIButton* m_CSheetButton;
	IGUIButton* m_ExitButton;
	IGUIButton* m_LoadButton;
	IGUIButton* m_OptionsButton;
	IGUIButton* m_SaveButton;

	int m_OriginalMin[20];
	int m_OriginalMax[20];

	TTextItem m_GameName;
	TTextItem m_MenuNew;
	TTextItem m_MenuLoad;
	TTextItem m_MenuExit;
	TTextItem m_FullscreenText;
	IGUIComboBox* m_langCombo;
	IGUIComboBox* m_resolutionCombo;
	IGUICheckBox* m_fullscreenCheck;

	TTextItem m_MovieText1;
	TTextItem m_MovieText2;
	float m_MovieTextSlidingY;
	IGUIStaticText* m_MovieText3;
	
	IGUIImage* m_IrrLogo;
};

#endif


