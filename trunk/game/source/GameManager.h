/** 
 * \file GameManager.h
 * \brief CGameManager class is the boss. The manager. It creates 3D device (irrlicht of course),
 * creates GUI, and 3D environment so called Level Game. Calls all the initialization functions and 
 * goes to the loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#if !defined(AFX_GAMEMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_GAMEMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#define MAX_NUMBER_OF_LEVELS 10
#define MAX_NUMBER_OF_NPCS_PER_MAP 100
#define NEAR_OBJECT_DISTANCE 60

#include <irrlicht.h>
#include "level/LevelManager.h"
#include "PlayerCharacter.h"
#include "gui/GameGUI.h"
#include "Script.h"
#include "level/CGoToArrows.h"
#include "Particles.h"
#include "Languages.h"


#include <irrKlang.h>

using namespace irrklang;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CLevelManager;

#define debugPrint printf

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
#define FONT_FILE "media/Garamond14.xml"
#define IRRLOGO_FILE "media/irrlichtlogo2.png"

#define SKILLPOINTSPERLEVEL 5

enum EPCMove {
	PC_MOVE_STARTS_MOVING,
	PC_MOVE_STOPS_MOVING,
	PC_MOVE_NO_MOVING
};

enum EGameState {
	GAME_STATE_MAIN_MENU,
	GAME_STATE_INTRO_MOVIE,
	GAME_STATE_LEVEL
};


/**
 * \brief CGameManager class is the boss. The manager. It creates 3D device (irrlicht of course),
 * creates GUI, and 3D environment so called Level Game. Calls all the initialization functions and 
 * goes to the loop.
 *
 * \author Petar Bajic \date July, 21 2008.
 */
class CGameManager : public IEventReceiver
{
public:
	CGameManager();
	~CGameManager();
	void Update(f32 elapsed_time);
	bool Init();
	void LoadMainMenu();
	void ExitMainMenu();
	void PlayIntroMovie();
	void ExitIntroMovie();
	bool NewGame();
	bool LoadDataFromXMLConfig(stringc filename);
	bool StoreDataToXMLConfig(stringc filename);
	IrrlichtDevice* getDevice();
	IVideoDriver* getDriver();
	ISceneManager* getSceneMngr();
	IGUIEnvironment* getGUIEnvironment();
	void DisplayError();
	void ExitGame();
	CGameGUI* getGameGUI() {return m_GameGUI;}
	CLevelManager* getLevelManager() {return m_pLevelManager;}
	CPlayerCharacter* getPC() {return m_pPC;}
	IFileSystem* getFS() {return m_FS;}
	void backToWorkingDirectory();
	stringw getRootNameFromPathName(stringw meshName);
	stringc getRootNameFromPathName(stringc meshName);
	virtual bool OnEvent(const SEvent& event);
	void CreateParticleEffect(PARTICLES_EFFECT_TYPE type, PARTICLES_EFFECT_COLOR color, stringw target, bool follow_player);
	void CreateLightNode(bool dancing, s32 radius, s32 target);
	bool LoadPC(stringc playerFile);
	void resetArrows();
	void PCMoveEnd();
	void DisplayMonsterDamage(vector3df pos, int dmg);
	ISound* Play2DSound(stringc soundfile, bool loop);
	void Play3DSound(stringc soundfile, vector3df position, bool loop);
	void PCAttackEnd();
	void LoadConditions(stringc filename);
	void PCChangeAbility(stringw name, s32 value);
	void PCChangeHealth(s32 value, stringw deathReason);
	void AddPCExperience(s32 experience);
	void AnimateTrigger(s32 id, u32 keyStart, u32 keyEnd, f32 animationSpeed);
	void TranslateGameObject(s32 id, vector3df translationVectorEndPosition, u32 translationTime);
	void TranslatePlayer(vector3df translationVectorEndPosition, u32 translationTime);
	void ChangeLevel(stringc mapname, s32 startPositionFlagID);
	void ChangeCondition(stringw cond, stringw value);
	void Restart();
	void TransferPickableFromNPCToPlayer(s32 NPCID, s32 itemID);
	stringc getSavedMap(stringc mapname);
	stringw getCurrentMapName();
	void SaveGame();
	void LoadGame(bool restart);
	void ReLoadGame(bool restart);
	void ClearTemp();
	void ClearInventory();
	void ClearSaved();
	void InitNPCDialogs();
	bool _isNodeClose(vector3df pos1, vector3df pos2);
	ISoundEngine* getSoundEngine() { return m_SoundEngine;}

	s32 m_ID;
	s32 m_ClickedNodeID;
	bool m_bInitOk;
	stringc m_StartMap;
	stringc m_PlayerConfigFile;
	stringc m_WorkingDir;
	stringw m_WndCaption;
	stringw m_LoadedMapName;
	stringc m_Maps[MAX_NUMBER_OF_LEVELS];
	u32 m_NumberOfMaps;
	CScript* m_pScriptEngine;
	CLanguages* m_pLanguages;
	stringc m_pLanguagePreference;

	array<TCondition*> m_ListOfConditions; //All game conditions in one array.

	CPlayerCharacter* m_pPC;
	EPCMove m_ePCMove; //move to bool?
	ISoundEngine* m_SoundEngine;

	void CreateDevice(bool fullscreen, dimension2d<u32> resolution, u32 depth);
	void ReCreateDevice();
	void RestartDevice();
	bool m_bFullscreenPreference;
	dimension2d<u32> m_Resolution; // chosen resolution
	dimension2d<u32> m_DesktopResolution;
	u32 m_Depth;
	//IVideoModeList* m_pVideoList;
	array <TResolution> m_listOfResolutions;

	bool m_bRestartDevice;
	bool m_SavedGameAvailable;
	u32 m_WindowWidth;
	u32 m_WindowHeight;
	
private:
	void HandleDeath(stringw message);
	IrrlichtDevice* m_pDevice;  
	IVideoDriver* m_pDriver;
	ISceneManager* m_pSceneManager;
    IGUIEnvironment* m_pGUIEnvironment; //irrlichts default gui environment
	IFileSystem* m_FS;
	stringc m_WorkingDirectory;
	float m_fHittingInProgress;
	int m_AttackTargetID;
	bool m_bDoAction;
	bool m_NewGame;

	IGUIFont* m_Font_Garamond14;

	EGameState m_GameState;

	//CGameGUI* m_pGuiManager; //our homemade gui manager
	CLevelManager* m_pLevelManager;
	CGameGUI* m_GameGUI;
	ITexture* m_MousePointerTexture;
	ITexture* m_MousePointerPickableDraggingTexture;
	ITexture* m_MousePointerActionTexture;
	ITexture* m_MousePointerAttackTexture;
	/*irr::gui::IGUIFont* m_pFont;*/

	CGoToArrows* m_Arrows;
	
};
#endif

