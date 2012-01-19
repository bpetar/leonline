/** 
 * \file GameManager.cpp
 * \brief CGameManager class is the boss. The manager. It creates 3D device (irrlicht of course),
 * creates GUI, and 3D environment. Calls all the initialization functions and goes to the loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include <irrlicht.h>

using namespace irr;

#include "GameManager.h"

#include <iostream>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#define HIT_TIME_DELAY 1.45f
#define PLAYER_HEIGHT 10

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGameManager::CGameManager()
{
	m_ID = 0;
	m_Arrows = 0;
	m_FS = NULL;
	m_pDevice = 0;
	m_bFullscreenPreference = false;
	m_GameGUI = NULL;
	m_pLevelManager = NULL;
	m_pPC = NULL;
	m_PlayerConfigFile = "Player.xml";
	m_WndCaption = "Test";
	m_Resolution.Width = 800;
	m_Resolution.Height = 600;
	m_DesktopResolution.Width = 1280;
	m_DesktopResolution.Height = 1024;
	m_Depth = 32;
	m_WorkingDirectory = ".";
	m_bDoAction = false;
	m_NumberOfMaps = 0;
	m_NewGame = true;
	m_ePCMove = PC_MOVE_NO_MOVING;
	m_fHittingInProgress = -1.0f;
	m_ListOfConditions.clear();
	m_pLanguagePreference = "en";
	m_bRestartDevice = false;
	m_SavedGameAvailable = false;
}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGameManager::~CGameManager()
{
	delete m_pLevelManager;

	getSoundEngine()->drop();
	getDevice()->drop();

}

/**
 * \brief Initialization function. Calls init of all sub classes (GUI, LevelManager, etc..)
 * \author Petar Bajic
 * \date July, 21 2008.
 */
bool CGameManager::Init()
{
	m_bInitOk = false;

	IrrlichtDevice *nulldevice = createDevice(video::EDT_NULL);

	m_FS = nulldevice->getFileSystem();
	//Load data from XML config file
	if (!LoadDataFromXMLConfig("game/game_config.xml"))
		return false;
	
	//Check if there is saved game (to enable Load button)
	stringc playerFile = stringc("save/") + m_PlayerConfigFile;
	if(m_FS->existFile(playerFile.c_str()))
		m_SavedGameAvailable = true;

	IVideoModeList* pVideoList = nulldevice->getVideoModeList() ;
	m_DesktopResolution = pVideoList->getDesktopResolution();
	for (int i = 0 ; i < pVideoList->getVideoModeCount() ; i++)
	{
		TResolution resolution;
		resolution.width = pVideoList->getVideoModeResolution(i).Width;
		resolution.height = pVideoList->getVideoModeResolution(i).Height;
		resolution.depth = pVideoList->getVideoModeDepth(i);
		if(resolution.width < 800) continue;
		if(resolution.width < resolution.height) continue;
		if(resolution.depth < 32) continue;
		resolution.text = stringc(resolution.width) + stringc("x") + stringc(resolution.height) + stringc(", ") + stringc(resolution.depth) + stringc("bit");

		//m_resolutionCombo->addItem(resolution.text.c_str());
		m_listOfResolutions.push_back(resolution);
	}

	nulldevice->drop();

	//create irrlicht device
	CreateDevice(m_bFullscreenPreference, (m_bFullscreenPreference?m_DesktopResolution:m_Resolution), m_Depth);

	//set window caption
	m_pDevice->setWindowCaption(m_WndCaption.c_str());

	//set working dir to whatever XML config tells you
	m_FS->changeWorkingDirectoryTo(m_WorkingDir.c_str());
	m_WorkingDirectory = m_FS->getWorkingDirectory();
	//printf("%s",m_WorkingDirectory.c_str());

	// start the sound engine with default parameters
	m_SoundEngine = createIrrKlangDevice();

	if (!m_SoundEngine)
		return false; // error starting up the engine

	//load languages
	m_pLanguages = new CLanguages(m_FS);
	m_FS->changeWorkingDirectoryTo("media/strings");
	m_pLanguages->Init();
	bool ret = m_pLanguages->setLanguage(m_pLanguagePreference.c_str());
	if(!ret)
	{
		//TODO: display warning message that language is not found!
	}
	backToWorkingDirectory();

	//create level manager
	m_pLevelManager = new CLevelManager();

	//create gui manager
	m_GameGUI = new CGameGUI();

	if (!m_GameGUI->Init(this))
		return false;

	ClearTemp();

	//Init level after GUI creation to send GUI pointer to level manager
	//direct communicatio between level manager and gui frees game manager 
	//from constantly transfering messages back and forth between those two
	m_pLevelManager->Init(this, m_GameGUI);

	//Load First Map
	//if (!m_pLevelManager->OnLoadMap(m_StartMap))
	//	return false;

	//Load Player Character
	//m_pPC = new CPlayerCharacter(this);
	//stringc playerFile = stringc("game/") + m_PlayerConfigFile;
	//if (!m_pPC->Load(m_pDevice,m_pSceneManager,playerFile.c_str()))
	//{
	//	return false;
	//}
	//m_pPC->setPosition(m_pLevelManager->GetStartPosition());

	m_pScriptEngine = new CScript();
	if(!m_pScriptEngine->Init(this, m_pDevice,"media/Scripts/actions.script"))
		return false;

	m_pDevice->getCursorControl()->setVisible(false);
	m_MousePointerTexture = m_pDriver->getTexture("media/Icons/mouse_pointer.png");
	m_MousePointerActionTexture = m_pDriver->getTexture("media/Icons/mouse_action.png");
	m_MousePointerAttackTexture = m_pDriver->getTexture("media/Icons/mouse_attack.png");
	m_MousePointerPickableDraggingTexture = m_pDriver->getTexture("media/Icons/mouse_pick.png");
	if((m_MousePointerTexture == 0)||(m_MousePointerActionTexture == 0)||(m_MousePointerPickableDraggingTexture == 0)||(m_MousePointerAttackTexture == 0))
		return false;

	//m_Arrows = new CGoToArrows(m_pDevice,m_pSceneManager,m_pSceneManager->getRootSceneNode(),-1,SColor(255,150,0,150),0);

	//colision response
	//m_pPC->addAnimator(m_pLevelManager->GetObstacleMetaTriangleSelector());
	//m_pLevelManager->SetMonstersCollisionAnimator();

	InitNPCDialogs();

	//load conditions from file
	LoadConditions("media/Scripts/conditions.script");
	
	m_Font_Garamond14 = m_pGUIEnvironment->getFont("media/Garamond14.xml");//m_pGUIEnvironment->getBuiltInFont();

	LoadMainMenu();
	
	m_GameState = GAME_STATE_MAIN_MENU;

	m_bInitOk = true;

	//stringw message  = "Welcome to game example!\n\nThis game is simple example of what can be made with Level Editor. At the moment this is kind of simple 3rd person rpg adventure. \n\nClick around to move and interact with objects. Adjust camera with right mouse click (press hold and move). \n";
	//m_GameGUI->AddMsgBox(L"Welcome!", message.c_str());

	// -> moved to level manager
	// play some sound stream, looped
	//m_SoundEngine->play2D(m_pLevelManager->GetLevelAmbientSound().c_str(), true);
	//m_SoundEngine->play2D(m_pLevelManager->GetLevelMusicTheme().c_str(), true);
	//m_SoundEngine->play2D(m_pLevelManager->GetLevelNarationFile().c_str(), true);

	return true;
}

void CGameManager::ReCreateDevice()
{
	CreateDevice(m_bFullscreenPreference, (m_bFullscreenPreference?m_DesktopResolution:m_Resolution), m_Depth);
	
	//set window caption
	m_pDevice->setWindowCaption(m_WndCaption.c_str());

	//set working dir to whatever XML config tells you
	m_FS->changeWorkingDirectoryTo(m_WorkingDir.c_str());

	m_pLanguages->setFS(m_FS);

	//recreate resources

	m_pDevice->getCursorControl()->setVisible(false);
	m_MousePointerTexture = m_pDriver->getTexture("media/Icons/mouse_pointer.png");
	m_MousePointerActionTexture = m_pDriver->getTexture("media/Icons/mouse_action.png");
	m_MousePointerAttackTexture = m_pDriver->getTexture("media/Icons/mouse_attack.png");
	m_MousePointerPickableDraggingTexture = m_pDriver->getTexture("media/Icons/mouse_pick.png");

	m_Font_Garamond14 = m_pGUIEnvironment->getFont("media/Garamond14.xml");//m_pGUIEnvironment->getBuiltInFont();
	
	LoadMainMenu();
}

void CGameManager::RestartDevice()
{
	m_bRestartDevice = true;
	//Clearing old device
	m_pDevice->closeDevice();
	//m_pDevice->run(); // Very important to do this here!
	m_pDevice->drop();

}

void CGameManager::LoadMainMenu()
{
	m_GameGUI->InitMenu();
}

void CGameManager::ExitMainMenu()
{
	m_GameGUI->ClearMenu();
}

void CGameManager::PlayIntroMovie()
{
	m_GameState = GAME_STATE_INTRO_MOVIE;
	
	m_GameGUI->InitIntroMovie();

}

void CGameManager::ExitIntroMovie()
{
	m_GameGUI->ClearIntroMovie();
}

bool CGameManager::NewGame()
{
	if (!m_GameGUI->InitGameGUI())
		return false;

	//Load First Map
	if (!m_pLevelManager->OnLoadMap(m_StartMap))
		return false;

	//Load Player Character
	m_pPC = new CPlayerCharacter(this);
	stringc playerFile = stringc("game/") + m_PlayerConfigFile;
	if (!m_pPC->Load(m_pDevice,m_pSceneManager,playerFile.c_str()))
		return false;
	m_pPC->setPosition(m_pLevelManager->GetStartPosition());

	m_Arrows = new CGoToArrows(m_pDevice,m_pSceneManager,m_pSceneManager->getRootSceneNode(),-1,SColor(255,150,0,150),0);

	//colision response
	m_pPC->addAnimator(m_pLevelManager->GetObstacleMetaTriangleSelector());
	m_pLevelManager->SetMonstersCollisionAnimator();

	m_GameState = GAME_STATE_LEVEL;

	return true;
}

/*
 * Load conditions from given file.
 */
void CGameManager::LoadConditions(stringc filename)
{
	IFileSystem* fs = m_pDevice->getFileSystem();
	IXMLReader* xml = fs->createXMLReader(filename.c_str());
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				if (stringw("ScriptConditions") != xml->getNodeName())
				{
					TCondition* condition = new TCondition;
					condition->name = xml->getNodeName();
					condition->value = xml->getAttributeValue(L"default");
					m_ListOfConditions.push_back(condition);
				}
			}
		}
	}
	
	//return true;
}

void CGameManager::ChangeCondition(stringw cond, stringw value)
{
	for(u32 i=0; m_ListOfConditions.size(); i++)
	{
		if(m_ListOfConditions[i]->name.equals_ignore_case(cond))
		{
			m_ListOfConditions[i]->value = value;
			return;
		}
	}

	debugPrint("CGameManager::ChangeCondition - Condition not found!");
}


//Parse all maps and find all NPCs and load all their dialogs... OR
//Load dialogs found in media/Dialogs dir ... OR
//Have user define all dialogs in game config?
//Using first option now.
void CGameManager::InitNPCDialogs()
{
	for(u32 u=0; u<m_NumberOfMaps; u++)
	{
		//Find NPCs on the map
		stringc NPCNames[MAX_NUMBER_OF_NPCS_PER_MAP];
		u32 numberOfNPC = m_pLevelManager->FindNPCsOnMap(stringc("maps/") + m_Maps[u], NPCNames);
		for(u32 i=0; i<numberOfNPC; i++)
		{
			m_GameGUI->AddGameDialog(NPCNames[i] + stringc(".dlg"));
		}
	}
}

bool CGameManager::LoadPC(stringc playerFile)
{
	//Load Player Character
	//m_pPC = new CPlayerCharacter(this);
	if (!m_pPC->Load(m_pDevice,m_pSceneManager,playerFile.c_str()))
		return false;

	return true;
}

void CGameManager::ClearTemp()
{
	//getting all files in dir is platform specific
	//for windows one would use FindFirstFile function and so on...
	//to avoid this platform dependency, we will search for our maps
	//loaded from xml, and ignore other files that end up in temp
	u32 index = 0;
	for(index = 0; index<m_NumberOfMaps; index++)
	{
		stringc mapPath = stringc("save/temp/") + m_Maps[index];
		remove(mapPath.c_str());
	}
}

void CGameManager::ClearSaved()
{
	//getting all files in dir is platform specific
	//for windows one would use FindFirstFile function and so on...
	//to avoid this platform dependency, we will search for our maps
	//loaded from xml, and ignore other files that end up in temp
	u32 index = 0;
	for(index = 0; index<m_NumberOfMaps; index++)
	{
		stringc mapPath = stringc("save/") + m_Maps[index];
		remove(mapPath.c_str());
	}
}

stringc CGameManager::getSavedMap(stringc mapname)
{
	stringc mapPath = stringc("save/temp/") + mapname;

	//find if there is saved map
	if(m_FS->existFile(mapPath.c_str()))
	{
		return mapPath;
	}
	
	if(!m_NewGame)
	{
		mapPath = stringc("save/") + mapname;
	
		if (m_FS->existFile(mapPath.c_str()))
		{
			return mapPath;
		}
	}

	mapPath = stringc("maps/") + mapname;

	return mapPath;
}

stringw CGameManager::getCurrentMapName()
{
	return m_pLevelManager->getCurrentMapName();
}

//Second parameter is used only when there is more then one entry to the map, to specify where the player will show up.
void CGameManager::ChangeLevel(stringc mapname, s32 startPositionID)
{
	//make sure the new level is different then current one.
	//rephrase: make sure we are not loading the level we are currently in.
	if(!getCurrentMapName().equals_ignore_case(mapname))
	{
		//save current level to temp;
		m_pLevelManager->SavePreviousLevelToTemp();

		//BIG TODO!!:
		//I think this is not implemented! This is must, emergency, and not much work, just needs lot of testing
		//m_GameGUI->SaveNPCDialogsToTemp();

		//Release memory before loading new map
		m_pLevelManager->CleanCurrentMap();

		stringc mapPath = getSavedMap(mapname);
		m_pLevelManager->OnLoadMap(mapPath);

		m_pPC->ReinitModel(m_pSceneManager);
		if(startPositionID > 0)
		{
			vector3df startPos = m_pLevelManager->GetObjectPosition(startPositionID);
			m_pPC->setPosition(startPos);
			m_pLevelManager->SetCameraPos(startPos);
		}
		else
		{
			m_pPC->setPosition(m_pLevelManager->GetStartPosition());
		}
		m_pPC->removeAnimators();
		m_pPC->addAnimator(m_pLevelManager->GetObstacleMetaTriangleSelector());

		resetArrows();
	}
	else
	{
		//We are trying to load level we are currently in! This means only one thing! Teleport!
		//Lets just move player to new position !
		if(startPositionID > 0)
		{
			vector3df startPos = m_pLevelManager->GetObjectPosition(startPositionID);
			m_pPC->setPosition(startPos);
			m_pLevelManager->SetCameraPos(startPos);

			m_pPC->m_bTranslated = false;
			m_pPC->removeAnimators();
			m_pPC->addAnimator(m_pLevelManager->GetObstacleMetaTriangleSelector());
		}
	}
}

void CGameManager::Restart()
{
	//load latest saved level
	ReLoadGame(true);
}

void CGameManager::ClearInventory()
{
	//clear inventory
	m_GameGUI->ClearInventory();
}

//Load game from main menu. 
//First we initialize game gui, player class, arrows and other utils, then we load game.
void CGameManager::LoadGame(bool restart)
{
	m_NewGame = false;
	//Init Game GUI
	m_GameGUI->InitGameGUI();
	//Load Player Character
	m_pPC = new CPlayerCharacter(this);
	//clear temp
	ClearTemp();
	//Clear Inventory
	ClearInventory();
	//get latest level and load
	stringc playerFile = stringc("save/") + m_PlayerConfigFile;
	if(m_FS->existFile(playerFile.c_str())) {
		//m_LoadedMapName is loaded with PC
		LoadPC(playerFile);
		//Load Level (m_LoadedMapName is loaded with PC)
		m_pLevelManager->OnLoadMap(stringc("save/") + m_LoadedMapName);
		//Adjust player to new Level
		m_pPC->ReinitModel(m_pSceneManager);
		m_pPC->SetLoadedPosition();
		m_pPC->removeAnimators();
		m_pPC->addAnimator(m_pLevelManager->GetObstacleMetaTriangleSelector());
		//Move Camera above PC
		m_pLevelManager->MoveCamera(m_pPC->getPosition());
		//Load arrows
		m_Arrows = new CGoToArrows(m_pDevice,m_pSceneManager,m_pSceneManager->getRootSceneNode(),-1,SColor(255,150,0,150),0);
		//Load Dialogs
		m_GameGUI->LoadNPCDialogs();
		//Set HealthBar
		m_GameGUI->healthBar->setBarValue(m_pPC->getAbilityValue("Health"));
		//Colision response
		m_pLevelManager->SetMonstersCollisionAnimator();
		//Console info + clear console
		m_GameGUI->ClearConsole();
		m_GameGUI->AddConsoleText(E_LANG_STRING_LEVEL_CONSOLE_GAME_LOAD);
	}
	else
	{
		//just restart the game if there is no saved game
		if(restart)
		{
			m_GameGUI->AddConsoleText(E_LANG_STRING_LEVEL_CONSOLE_GAME_RESTART);
			stringc playerFile = stringc("game/") + m_PlayerConfigFile;
			if(m_FS->existFile(playerFile.c_str())) {
				//m_LoadedMapName is loaded with PC
				LoadPC(playerFile);
				//Load First Map
				if (!m_pLevelManager->OnLoadMap(m_StartMap)) {
					//Display error to message box
					m_GameGUI->AddMsgBox(E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING, E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING_MSG2);
					return;
				}
				//Adjust player to new Level
				m_pPC->ReinitModel(m_pSceneManager);
				m_pPC->SetLoadedPosition();
				m_pPC->removeAnimators();
				m_pPC->addAnimator(m_pLevelManager->GetObstacleMetaTriangleSelector());
				//Move Camera above PC
				m_pLevelManager->MoveCamera(m_pPC->getPosition());
				//Reset arrows
				resetArrows();
				m_GameGUI->LevelUp(false);
				//Load Dialogs
				m_GameGUI->LoadNPCDialogs();
				//Set HealthBar
				m_GameGUI->healthBar->setBarValue(m_pPC->getAbilityValue("Health"));
				m_GameGUI->healthBar->setMaxBarValue(m_pPC->getAbilityValue("Health"));
				//Console info + clear console
				m_GameGUI->ClearConsole();
			}
		}
		else
		{
			//Display error to console
			m_GameGUI->AddMsgBox(E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING, E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING_MSG1);
		}
	}

	m_GameState = GAME_STATE_LEVEL;

}

//Load game called from level. 
//This game reloading skips some intialization that is already done.
void CGameManager::ReLoadGame(bool restart)
{
	m_NewGame = false;
	//clear temp
	ClearTemp();
	//Clear Inventory
	ClearInventory();
	//get latest level and load
	stringc playerFile = stringc("save/") + m_PlayerConfigFile;
	if(m_FS->existFile(playerFile.c_str())) {
		//m_LoadedMapName is loaded with PC
		LoadPC(playerFile);
		//Release memory before loading new map
		m_pLevelManager->CleanCurrentMap();
		//Load Level (m_LoadedMapName is loaded with PC)
		m_pLevelManager->OnLoadMap(stringc("save/") + m_LoadedMapName);
		//Adjust player to new Level
		m_pPC->ReinitModel(m_pSceneManager);
		m_pPC->SetLoadedPosition();
		m_pPC->removeAnimators();
		m_pPC->addAnimator(m_pLevelManager->GetObstacleMetaTriangleSelector());
		//Move Camera above PC
		m_pLevelManager->MoveCamera(m_pPC->getPosition());
		//Reset arrows
		resetArrows();
		//Load Dialogs
		m_GameGUI->LoadNPCDialogs();
		//Set HealthBar
		m_GameGUI->healthBar->setBarValue(m_pPC->getAbilityValue("Health"));
		//Console info + clear console
		m_GameGUI->ClearConsole();
		m_GameGUI->AddConsoleText(E_LANG_STRING_LEVEL_CONSOLE_GAME_LOAD);
	}
	else
	{
		//just restart the game if there is no saved game
		if(restart)
		{
			m_GameGUI->AddConsoleText(E_LANG_STRING_LEVEL_CONSOLE_GAME_RESTART);
			stringc playerFile = stringc("game/") + m_PlayerConfigFile;
			if(m_FS->existFile(playerFile.c_str())) {
				//m_LoadedMapName is loaded with PC
				LoadPC(playerFile);
				//Load First Map
				if (!m_pLevelManager->OnLoadMap(m_StartMap)) {
					//Display error to message box
					m_GameGUI->AddMsgBox(E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING, E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING_MSG2);
					return;
				}
				//Adjust player to new Level
				m_pPC->ReinitModel(m_pSceneManager);
				m_pPC->SetLoadedPosition();
				m_pPC->removeAnimators();
				m_pPC->addAnimator(m_pLevelManager->GetObstacleMetaTriangleSelector());
				//Move Camera above PC
				m_pLevelManager->MoveCamera(m_pPC->getPosition());
				//Reset arrows
				resetArrows();
				m_GameGUI->LevelUp(false);
				//Load Dialogs
				m_GameGUI->LoadNPCDialogs();
				//Set HealthBar
				m_GameGUI->healthBar->setBarValue(m_pPC->getAbilityValue("Health"));
				m_GameGUI->healthBar->setMaxBarValue(m_pPC->getAbilityValue("Health"));
				//Console info + clear console
				m_GameGUI->ClearConsole();
			}
		}
		else
		{
			//Display error window
			m_GameGUI->AddMsgBox(E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING, E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING_MSG1);
		}
	}
}

/**
 * \brief Saves game position.
 * Important function for every game. It was a bitch to make it.
 * Saves state of allllll objects on alllll maps player visited (I hope).
 */
void CGameManager::SaveGame()
{
	if(m_NewGame)
	{
		//Playing from start, want to save? remove existing saved levels...
		ClearSaved();
	}

	m_NewGame = false; //??

	//Save Player Character Stats
	m_pPC->Save();

	//Save Scene
	m_pLevelManager->SaveLevels();

	//Save NPC conversation
	m_GameGUI->SaveNPCDialogs();

	//Notify
	m_GameGUI->AddConsoleText(E_LANG_STRING_LEVEL_CONSOLE_GAME_SAVE);
}

/**
 * \brief Recreating Terrain click arrows.
 * Arrows are drawn when user clicks on terrain. 
 * They are animated and serve as 'eyekandy' and better differentiator of terrain click.
 * Here, they are deleted and recreated for smgr/map reloading purposes.
 */
void CGameManager::resetArrows()
{
	if(m_Arrows)
	{
		delete m_Arrows;
		m_Arrows = 0;
		m_Arrows = new CGoToArrows(m_pDevice,m_pSceneManager,m_pSceneManager->getRootSceneNode(),-1,SColor(255,150,0,150),0);
	}
}

void CGameManager::ExitGame()
{
	getDevice()->closeDevice();
}

/**
 * \brief Inform user he failed miserably, and offer him to restart the game.
 * It is hard to handle death. But we try here anyway. 
 */
void CGameManager::HandleDeath(stringw message)
{
	m_pPC->Dies();

	//Player dies, monsters should stop attacking.
	//In case of multiple players, or NPCs accompaning PC, fight might continue with surviving characters.
	//In that case, function should be called "Look for Other enemies, or Relax".
	//There might be even need to record which monster is fighting wich player...
	m_pLevelManager->SetMonstersMood(Relaxed);

	m_GameGUI->DisplayDeathWindow(m_pGUIEnvironment, message);
}

/**
 * \brief This function works with PC Attributes, but they are configurable in xml file. 
 * Here, 'Health' and 'Experience' are hardcoded, because they impact game dynamics directly.
 * If you don't have them, you have to code something else here...
 */
void CGameManager::PCChangeAbility(stringw name, s32 value)
{
	if(name.equals_ignore_case("Health"))
	{
		PCChangeHealth(value, "You successfully reduced your Health below 0!");
	}
	else if (name.equals_ignore_case("Experience"))
	{
		AddPCExperience(value);
	}
	else
	{
		//ability not yet implemented
	}
}

/**
 * \brief Add/remove health points to PC. If player less then 0, update GUI notification, cause you are dead.
 * Please note that 'Health' is one of Players configurable attributes, so this API is hardcoded over flexibile attribute. 
 * This will not work if 'Health' is removed from list of attributes, and this code may even crash. But it works for this example...
 */
void CGameManager::PCChangeHealth(s32 value, stringw deathReason)
{
	int hp = m_pPC->getAbilityValue("Health");
	hp += value;

	if(m_pPC->isAlive())
	{
		m_pPC->setAbilityValue("Health",hp);
		m_GameGUI->healthBar->changeValueBy(value);

		//we could do: if (hp <= m_pPC->getAbilityMin("Health"))
		if(hp <= 0)
		{
			m_GameGUI->AddConsoleText(E_LANG_STRING_LEVEL_CONSOLE_GAME_PLAYER_DIED);
			HandleDeath(deathReason);
		}
		else
		{
			//Play PC Hit sound cry
			//Play Blood Wound
		}
	}
}

/**
 * \brief Add experience points to PC. If player has enough for next level update GUI notification.
 * Please note that experience is one of Players configurable attributes, so this API is hardcoded over flexibile attribute. 
 * This will not work if 'Experience' is removed from list of attributes, and this code may even crash. But it works for this example...
 */
void CGameManager::AddPCExperience(s32 experience)
{
	m_pPC->AddExperience(experience);
	
	if(m_pPC->m_ReachedNextLevel)
	{
		//Display Upgrade Icon +
		m_GameGUI->LevelUp(true);
		m_pPC->m_ReachedNextLevel = false;
	}
}

/**
 * \brief When PC arrives to desired place (object) this callback is called.
 * This initiates attack if clicked on distant monster, 
 * or general action if clicked on other action object.
 */
void CGameManager::PCMoveEnd()
{
	m_ePCMove = PC_MOVE_STOPS_MOVING;

	if(m_AttackTargetID>0)
	{
		//Player clicked on distant monster, approach has ended, now attack!
		m_fHittingInProgress = HIT_TIME_DELAY;
		m_pPC->attack(CCharacter::EByteAttack);
		m_SoundEngine->play3D(m_pPC->GetSound(EAttack).c_str(), m_pPC->getPosition(),false);
		m_AttackTargetID = 0;
		return;
	}

	m_bDoAction = true;

}

/**
 * \brief When player clicks on trigger, this function will animate it
 * Trigger objects can have action script that says object should be animated
 * for example doors opening upon click.
 * when such action is executed from Script.cpp, this function is called.
 */
void CGameManager::AnimateTrigger(s32 id, u32 keyStart, u32 keyEnd, f32 animationSpeed)
{
	m_pLevelManager->AnimateTrigger(id, keyStart, keyEnd, false, animationSpeed);
}

/**
 * \brief When player clicks on trigger, this function will animate it
 * Trigger objects can have action script that says object should be animated
 * for example doors opening upon click.
 * when such action is executed from Script.cpp, this function is called.
 */
void CGameManager::TranslateGameObject(s32 id, vector3df translationVectorEndPosition, u32 translationTime)
{
	ISceneNode* node = m_pLevelManager->m_pLevels[m_pLevelManager->m_LevelIndex]->m_SMGR->getSceneNodeFromId(id);
	m_pLevelManager->TranslateGameObject(node, translationVectorEndPosition, translationTime);
}

/**
 * \brief When player needs to be moved around by some force, this function will translate it
 * translationVector is relative, translationTime is in miliseconds.
 */
void CGameManager::TranslatePlayer(vector3df translationVector, u32 translationTime)
{
	//Player is being moved - disable all user interaction
	m_pPC->m_bTranslated = true;
	m_pPC->removeAnimators();
	m_pLevelManager->TranslateGameObject(m_pPC->getNode(), m_pPC->getPosition() + translationVector, translationTime);
}

/**
 * Display monster damage in floating text rising above (damaged) monster
 */
void CGameManager::DisplayMonsterDamage(vector3df pos, int dmg)
{
	u32 timeout = 5;
	if(dmg>0)
		m_GameGUI->DrawFloatingText(pos, stringc("hit: ") + stringc(dmg) + stringc("hp"), timeout, SColor(255,255,255,255));
	else
		m_GameGUI->DrawFloatingText(pos, "miss", timeout, SColor(255,255,40,40));
}


/**
 * \brief Plays 2D sound. Background music or ambient sounds.
 */
ISound* CGameManager::Play2DSound(stringc soundfile, bool loop)
{
	return m_SoundEngine->play2D(soundfile.c_str(),loop, false, true);
}

/**
 * \brief Plays 3D sound. At given position. Used for SFX.
 */
void CGameManager::Play3DSound(stringc soundfile, vector3df position, bool loop)
{
	m_SoundEngine->play3D(soundfile.c_str(), position, loop);
}

/**
 * \brief This callback is called when PC attack animation ends.
 * This is when hit/damage is calculated and consequences to the hit.
 */
void CGameManager::PCAttackEnd()
{
	m_bDoAction = true;
	m_ePCMove = PC_MOVE_STOPS_MOVING;
}

/**
 * \brief Game drawing loop. Draws GUI (2D) and current level (3D).
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGameManager::Update(f32 elapsed_time)
{
	if(elapsed_time > 5) 
	{
		elapsed_time = 1;
	}

	m_pDriver->beginScene(true, true, SColor(255,0,0,0));

	m_pSceneManager->drawAll();
	m_pGUIEnvironment->drawAll();

	// display frames per second in window title
	int fps = m_pDriver->getFPS();
	stringw str = "FPS: ";
	str += fps;
	m_Font_Garamond14->draw(str, recti(80,30,140,50),SColor(255,255,255,255));

	if(m_GameState == GAME_STATE_LEVEL)
	{

		if(m_bDoAction)
		{
			debugPrint("m_bDoAction: %d\n",m_ClickedNodeID);
			m_pLevelManager->Action(m_ClickedNodeID);
			m_bDoAction = false;
			return;
		}

		if(m_fHittingInProgress > 0)
		{
			m_fHittingInProgress -= elapsed_time;
		}
		
		if (m_pPC && m_pPC->isAlive())
		{
			m_pPC->update(elapsed_time);
			if(m_ePCMove == PC_MOVE_STARTS_MOVING)
			{
				m_pLevelManager->MoveCamera(m_pPC->getPosition());
				m_SoundEngine->setListenerPosition(m_pPC->getPosition() + vector3df(0,+10,0), vector3df(0,-1,0));
				//check if player approached some activation area
				m_pLevelManager->CheckAreaApproach(m_pPC->getPosition());
			}
			if(m_pPC->m_bTranslated)
			{
				m_pPC->idle();
				m_pLevelManager->MoveCamera(m_pPC->getPosition());
				m_SoundEngine->setListenerPosition(m_pPC->getPosition() + vector3df(0,+10,0), vector3df(0,-1,0));
				//check if player approached some activation area
				m_pLevelManager->CheckAreaApproach(m_pPC->getPosition());
			}

			if(m_pPC->getPosition().Y < -50)
			{
				//you fell off the world?
				HandleDeath("You fell off the edge of the world?");
			}
		}

		//update and move some objects 'manually'
		m_pLevelManager->Update(m_pDriver, m_pDevice, elapsed_time, m_pPC, m_Font_Garamond14);

		m_GameGUI->healthBar->renderGUIBars(m_pDriver, m_Font_Garamond14);
		m_GameGUI->renderFloatingTexts(m_Font_Garamond14, elapsed_time);

		//draw cursor hand depending on situation
		if(m_GameGUI->m_bDraggingPickableItem)
		{
			video::SColor color = video::SColor(125,255,255,255);
			if(m_pLevelManager->m_pHoverOverActionNode)
			{
				color = video::SColor(255,255,255,255);
			}
			m_pGUIEnvironment->getVideoDriver()->draw2DImage(m_MousePointerPickableDraggingTexture,
				m_pDevice->getCursorControl()->getPosition()+position2di(-15, -15),
					core::rect<s32>(0,0,82,78), 0, 
					color, true);
			m_pGUIEnvironment->getVideoDriver()->draw2DImage(m_GameGUI->m_pDraggedPickableItem->m_IconTexture,m_pDevice->getCursorControl()->getPosition()+position2di(-25, -25));
		}
		else if(m_pLevelManager->m_pHoverOverMonsterNode)
		{
			if(m_fHittingInProgress <= 0)
			{
				m_pGUIEnvironment->getVideoDriver()->draw2DImage(m_MousePointerAttackTexture,
					m_pDevice->getCursorControl()->getPosition(),
						core::rect<s32>(0,0,50,50), 0, 
						video::SColor(255,255,255,255), true);
			}
			else
			{
				m_pGUIEnvironment->getVideoDriver()->draw2DImage(m_MousePointerAttackTexture,
					m_pDevice->getCursorControl()->getPosition(),
						core::rect<s32>(0,0,50,50), 0, 
						video::SColor(115,255,255,255), true);
			}
		}
		else if (m_pLevelManager->m_pHoverOverActionNode)
		{
			m_pGUIEnvironment->getVideoDriver()->draw2DImage(m_MousePointerActionTexture,
				m_pDevice->getCursorControl()->getPosition(),
					core::rect<s32>(0,0,50,50), 0, 
					video::SColor(255,255,255,255), true);
		}
		else
		{
			m_pGUIEnvironment->getVideoDriver()->draw2DImage(m_MousePointerTexture,
				m_pDevice->getCursorControl()->getPosition(),
					core::rect<s32>(0,0,50,50), 0, 
					video::SColor(255,255,255,255), true);
		}
	}
	else if(m_GameState == GAME_STATE_MAIN_MENU)
	{
		//draw menu
		m_GameGUI->drawMenu(elapsed_time);

		//draw pointer
		if (m_GameGUI->m_pHoverOverMenuItem)
		{
			m_pGUIEnvironment->getVideoDriver()->draw2DImage(m_MousePointerActionTexture,
				m_pDevice->getCursorControl()->getPosition(),
					core::rect<s32>(0,0,50,50), 0, 
					video::SColor(255,255,255,255), true);
		}
		else
		{
			m_pGUIEnvironment->getVideoDriver()->draw2DImage(m_MousePointerTexture,
				m_pDevice->getCursorControl()->getPosition(),
					core::rect<s32>(0,0,50,50), 0, 
					video::SColor(255,255,255,255), true);
		}

	}
	else if (m_GameState == GAME_STATE_INTRO_MOVIE)
	{
		//draw movie
		m_GameGUI->drawIntroMovie(elapsed_time);
	}


	/*if(m_GameGUI->m_wnd_charSheet)
	{
		//Im sorry for this code TODO: make guiBars gui elements!
		int x = m_GameGUI->m_wnd_charSheet->getAbsolutePosition().UpperLeftCorner.X;
		int y = m_GameGUI->m_wnd_charSheet->getAbsolutePosition().UpperLeftCorner.Y;
		for(u32 i=0; i<m_pPC->m_ListOfAbilities.size(); i++)
		{
			if(m_GameGUI->cs_ability_bars[i])
			{
				m_GameGUI->cs_ability_bars[i]->setPos(x + 20, y+60+i*30, x + 120, y+80+i*30);
				m_GameGUI->cs_ability_bars[i]->renderGUIBars(m_pDriver, font);
			}
		}
		for(u32 i=0; i<m_pPC->m_ListOfSkills.size(); i++)
		{
			if(m_GameGUI->cs_skill_bars[i])
			{
				m_GameGUI->cs_skill_bars[i]->setPos(x + 20, y+270+i*30, x + 120, y+290+i*30);
				m_GameGUI->cs_skill_bars[i]->renderGUIBars(m_pDriver, font);
			}
		}
		//m_GameGUI->cs_health->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_str->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_exp->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_dex->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_int->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_attack->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_defence->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_magic->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_gardenning->renderGUIBars(m_pDriver, font);
		//m_GameGUI->cs_level->renderGUIBars(m_pDriver, font);
	}*/

	m_pDriver->endScene();
}

void CGameManager::DisplayError()
{
	m_pDriver->beginScene(true, true, SColor(255,100,101,140));
	m_pSceneManager->drawAll();
	m_pGUIEnvironment->drawAll();
	m_pDriver->endScene();
}

void CGameManager::TransferPickableFromNPCToPlayer(s32 NPCid, s32 itemID)
{
	CGameObject* NPCgo = m_pLevelManager->getGameObjectFromID(NPCid);

	if(NPCgo)
	{
		CGameObject* itemGO = NPCgo->GetPickableItemFromID(itemID);

		if(itemGO)
		{
			//remove item from NPC inventory
			m_pLevelManager->RemoveContainerContent(NPCid, itemID);

			//add item to players inventory
			m_GameGUI->AddPickableToInventory(itemGO);

			//display notification "item received"
			m_GameGUI->AddConsoleText(m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_GAME_ITEM_GAIN) + itemGO->name);
			m_GameGUI->DrawFloatingText(m_pPC->node->getPosition()+vector3df(0,PLAYER_HEIGHT,0), m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_GAME_ITEM_GAIN) + itemGO->name, 5, SColor(255,40,240,40));
		}
	}
}

stringw CGameManager::getRootNameFromPathName(stringw meshName)
{
	stringw rootName = meshName.subString(meshName.findLast('/')+1,meshName.size());
	rootName = rootName.subString(0,rootName.findLast('.'));
	return rootName;
}

stringc CGameManager::getRootNameFromPathName(stringc meshName)
{
	stringc rootName = meshName.subString(meshName.findLast('/')+1,meshName.size());
	rootName = rootName.subString(0,rootName.findLast('.'));
	return rootName;
}

/**
 * \brief Creates the Irrlicht device and get pointers to the main subsytems
 * for later use, the Game manager is the central interface point to the rendering engine
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGameManager::CreateDevice(bool fullscreen, dimension2d<u32> resolution, u32 depth)
{
	//create irrlicht device and send "this" as event receiver meaning this class should 
	//implement OnEvent function and handle its own events...
	m_pDevice = createDevice( video::EDT_DIRECT3D9, resolution, depth, fullscreen, false, false, this);
	//m_pDevice->setResizable(true);

	m_WindowWidth = resolution.Width;
	m_WindowHeight = resolution.Height;

   	m_pDriver = m_pDevice->getVideoDriver();
    m_pSceneManager = m_pDevice->getSceneManager();
	m_pGUIEnvironment = m_pDevice->getGUIEnvironment();
	//change to m_WorkingDirectory
	IGUIFont* font = m_pGUIEnvironment->getFont("media/Garamond14.xml");
	m_pGUIEnvironment->getSkin()->setFont(font);

	//we gonna use filesystem everywhere so lets store the pointer
	m_FS = m_pDevice->getFileSystem();
}


/**
 * \brief Save config data to given XML file
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGameManager::StoreDataToXMLConfig(stringc filename)
{
	io::IXMLWriter* xml = m_FS->createXMLWriter(filename.c_str());

	if (!xml)
	{
		//Display error message and exit
		return false;
	}

	xml->writeXMLHeader();

	xml->writeElement(L"Config",false); 
	xml->writeLineBreak();
	
	xml->writeElement(L"MapStart",true,L"filename",stringw(m_StartMap).c_str()); 
	xml->writeLineBreak();

	xml->writeElement(L"PlayerConfig",true,L"filename",stringw(m_PlayerConfigFile).c_str()); 
	xml->writeLineBreak();

	xml->writeElement(L"WorkingDir",true,L"filename",stringw(m_WorkingDir).c_str()); 
	xml->writeLineBreak();

	xml->writeElement(L"WindowCaption",true,L"text",stringw(m_WndCaption).c_str()); 
	xml->writeLineBreak();

	xml->writeElement(L"Language",true,L"name",stringw(m_pLanguages->m_Language->name).c_str(),L"value",stringw(m_pLanguages->m_Language->value).c_str()); 
	xml->writeLineBreak();

	xml->writeElement(L"Fullscreen",true,L"value",(m_bFullscreenPreference?L"true":L"false")); 
	xml->writeLineBreak();

	xml->writeElement(L"Resolution",true,L"width",stringw(m_Resolution.Width).c_str(),L"height",stringw(m_Resolution.Height).c_str(),L"depth",stringw(m_Depth).c_str()); 
	xml->writeLineBreak();
	
	xml->writeElement(L"Maps",false); 
	xml->writeLineBreak();
	
	for(u32 i=0; i< m_NumberOfMaps; i++)
	{
		xml->writeElement(L"Level",true,L"id",stringw(i).c_str(),L"filename",stringw(m_Maps[i]).c_str()); 
		xml->writeLineBreak();
	}

	xml->writeClosingTag(L"Maps");
	xml->writeLineBreak();

	xml->writeClosingTag(L"Config");
	xml->writeLineBreak();

	xml->drop(); // don't forget to delete the xml writer

	return true;
}

/**
 * \brief Loads config data from given XML file
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGameManager::LoadDataFromXMLConfig(stringc filename)
{
	io::IXMLReader* xml = m_FS->createXMLReader(filename.c_str());

	if (!xml)
	{
		//Display error message and exit

		return false;
	}

	while(xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo;
				if (core::stringw("MapStart") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"filename");
					m_StartMap = figo.c_str();
				}
				else if (core::stringw("PlayerConfig") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"filename");
					m_PlayerConfigFile = figo.c_str();
				}
				else if (core::stringw("WorkingDir") == xml->getNodeName())
				{
					//New Folder, create node and step in.
					figo = xml->getAttributeValue(L"filename");
					m_WorkingDir = figo.c_str();
				}
				else if (core::stringw("Language") == xml->getNodeName())
				{
					//Chosen Language.
					figo = xml->getAttributeValue(L"value");
					m_pLanguagePreference = figo.c_str();
				}
				else if (core::stringw("Resolution") == xml->getNodeName())
				{
					//Chosen Resolution.
					u32 height = xml->getAttributeValueAsInt(L"height");
					u32 width = xml->getAttributeValueAsInt(L"width");
					u32 depth = xml->getAttributeValueAsInt(L"depth");
					m_Resolution.Width = width;
					m_Resolution.Height = height;
					m_Depth = depth;
				}
				else if (core::stringw("Fullscreen") == xml->getNodeName())
				{
					//Chosen Language.
					figo = xml->getAttributeValue(L"value");
					if(figo.equals_ignore_case("true"))
					{
						m_bFullscreenPreference = true;
					}
					else
					{
						m_bFullscreenPreference = false;
					}
				}
				else if (core::stringw("WindowCaption") == xml->getNodeName())
				{
					//New Folder, create node and step in.
					figo = xml->getAttributeValue(L"text");
					m_WndCaption = figo.c_str();
				}
				else if (stringw("Level") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"filename");
					m_Maps[m_NumberOfMaps] = figo;
					m_NumberOfMaps++;
				}
			}
			break;
		}
	}

	xml->drop(); // don't forget to delete the xml reader

	return true;
}

/**
 * \brief Returns a pointer to the Irrlicht Device subsystem
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
IrrlichtDevice* CGameManager::getDevice()
{
	return m_pDevice;
}

/**
 * \brief Returns a pointer to the Irrlicht Driver subsystem
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
IVideoDriver* CGameManager::getDriver()
{
	return m_pDriver;
}

/**
 * \brief Changes working directory to default one.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGameManager::backToWorkingDirectory()
{
	m_FS->changeWorkingDirectoryTo(m_WorkingDirectory.c_str());
}

/**
 * \brief Returns a pointer to the Irrlicht SceneManager subsystem
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
ISceneManager* CGameManager::getSceneMngr()
{
	return m_pSceneManager;
}

/**
 * \brief Returns a pointer to the Irrlicht GUI subsystem
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
IGUIEnvironment* CGameManager::getGUIEnvironment()
{
	return m_pGUIEnvironment;
}

/**
 * \brief Displays GUI Window with content of container object with given id
 * \author Petar Bajic 
 * \date July, 21 2008.
 
void CGameManager::DisplayContainerContent(s32 id)
{
	m_GameGUI->DisplayContainerContent(id, m_pDriver, m_pGUIEnvironment, m_pLevelManager);
}*/

/**
 * \brief Main event handler derived from IEventHandler, this will be passed down 
 * to the current states keyboard handler. The state controls its own keyboard events
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGameManager::OnEvent(const SEvent& event)
{
	if (!m_pDriver || !m_bInitOk)
		return false;

	if(m_GameState == GAME_STATE_LEVEL)
	{
		//Gui Handles its own events here
		if(m_GameGUI->OnEvent(event))
			return false; //false will let irrlicht handle events too
		
		//3D level environment handles events here
		if (m_pLevelManager->OnEvent(event))
			return false; //false will let irrlicht handle events too

		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			// Pass input down to the specific game state mouse handler
			switch(event.MouseInput.Event)
			{
				case EMIE_LMOUSE_PRESSED_DOWN:
					{
						//If player is moved by some force, ignore user input for the time being
						if(m_pPC->m_bTranslated)
							return false;

						//Player clicked on something, lets fined out what
						m_AttackTargetID = 0;
						m_ClickedNodeID = 0;
					
						//If mouse pointer was above monster we don't have to calculate again... its monster he clicked on!
						if(m_pLevelManager->m_pHoverOverMonsterNode)
						{
							if(_isNodeClose(m_pPC->node->getPosition(),m_pLevelManager->m_pHoverOverMonsterNode->getPosition()))
							{
								//Click on the monster. Include animations and hit delays.
								if(m_fHittingInProgress <= 0)
								{
									m_fHittingInProgress = HIT_TIME_DELAY;
									m_pPC->attackMonster(m_pLevelManager->m_pHoverOverMonsterNode);
									m_SoundEngine->play3D(m_pPC->GetSound(EAttack).c_str(), m_pPC->getPosition(),false);
								}
								m_ClickedNodeID = m_AttackTargetID = m_pLevelManager->m_pHoverOverMonsterNode->getID();
								debugPrint("click on monster %d\n",m_ClickedNodeID);						}
							else
							{
								//move pc to approach hit node as action object
								m_pPC->setTargetNode(m_pLevelManager->m_pHoverOverMonsterNode);
								m_ePCMove = PC_MOVE_STARTS_MOVING;
								if(m_fHittingInProgress <= 0)
								{
									m_ClickedNodeID = m_AttackTargetID = m_pLevelManager->m_pHoverOverMonsterNode->getID();
								}
							}
						}
						//If mouse pointer was hovering above action item (pickable, NPC, trigger...)
						else if (m_pLevelManager->m_pHoverOverActionNode)
						{
							if(_isNodeClose(m_pPC->node->getPosition(),m_pLevelManager->m_pHoverOverActionNode->getPosition()))
							{
									//skip moving, interact right away
									m_ClickedNodeID = m_pLevelManager->m_pHoverOverActionNode->getID();
									m_bDoAction = true;
							}
							else
							{
								//move pc to approach hit node as action object
								m_ClickedNodeID = m_pLevelManager->m_pHoverOverActionNode->getID();
								m_pPC->setTargetNode(m_pLevelManager->m_pHoverOverActionNode);
								m_ePCMove = PC_MOVE_STARTS_MOVING;
							}
						}
						// User clicked on terrain or some silly object
						else
						{
							const ISceneNode* hitNode = 0;
							vector3df instersection_point; //intersection of terrain and mouse click
							triangle3df instersection_triangle; //not used, but needed as parameter for function call
							line3d<f32> picking_line = getSceneMngr()->getSceneCollisionManager()->getRayFromScreenCoordinates(getDevice()->getCursorControl()->getPosition());
							IMetaTriangleSelector* selector = m_pLevelManager->GetLevelMetaTriangleSelector();
							getSceneMngr()->getSceneCollisionManager()->getCollisionPoint(picking_line,selector,instersection_point,instersection_triangle,hitNode);					

							if ((hitNode != 0)&&(hitNode->getID() >= 0)) 
							{
								m_ClickedNodeID = hitNode->getID();
								//User clicked on terrain?
								if(m_pLevelManager->isNodeTerrain(hitNode))
								{
									if(m_GameGUI->m_bDraggingPickableItem)
									{
										//drop pickable item on terrain
										m_GameGUI->m_bDraggingPickableItem = false;
										m_pLevelManager->DropPickableToMap(m_GameGUI->m_pDraggedPickableItem,instersection_point + vector3df(0,1,0));
										m_GameGUI->m_pDraggedPickableItem = 0;
									}
									else
									{
										//move pc to point where clicked on the terrain.
										m_pPC->setTargetPosition(instersection_point); 
										m_Arrows->Play(instersection_point + vector3df(0,5,0));
										m_ePCMove = PC_MOVE_STARTS_MOVING;
									}
								}
								else 
								{
									//click on the non terrain non action item
									m_GameGUI->AddConsoleText(E_LANG_STRING_LEVEL_CONSOLE_GAME_EMPTY_CLICK);
								}
							}
						}
					}
					break;
			}
		}
	}
	else if(m_GameState == GAME_STATE_MAIN_MENU)
	{
		//Gui Handles its own events here
		if(m_GameGUI->OnMenuEvent(event))
			return false; //false will let irrlicht handle events too
	}
	else if(m_GameState == GAME_STATE_INTRO_MOVIE)
	{
		//Gui Handles its own events here
		if(m_GameGUI->OnMovieEvent(event))
			return false; //false will let irrlicht handle events too
	}


	return false;
}

bool CGameManager::_isNodeClose(vector3df pos1, vector3df pos2)
{
	if(pos1.getDistanceFrom(pos2) < NEAR_OBJECT_DISTANCE)
	{
		return true;
	}
	return false;
}

void CGameManager::CreateParticleEffect(PARTICLES_EFFECT_TYPE type, PARTICLES_EFFECT_COLOR color, stringw target, bool follow_player)
{
	m_pLevelManager->CreateParticleEffect(type, color, target, follow_player);
}

void CGameManager::CreateLightNode(bool dancing, s32 radius, s32 target)
{
	m_pLevelManager->CreateLightNode(dancing, radius, target);
}
