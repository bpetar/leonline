/** 
 * \file LevelManager.h
 * \brief Header file containing CLevelManager class used for managing game levels.
 * CLevelManager class handles saving and loading maps, and handles map related user events.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#if !defined(AFX_EDLEVELICA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_EDLEVELICA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_


#include <irrlicht.h>
#include "RTSCamera.h"
#include "Level.h"
#include "../GameManager.h"
#include "../Particles.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CGameManager;
class CGameGUI;

/**
 * \brief CLevelManager class manages game levels.
 *
 * It takes care of init/saving/loading levels 
 * holds array of CLevel objects m_pLevels, which are dynamically loaded as user transfers from one level to another
 * serves as public API provider from GameManager to Clevel.
 *
 * \author Petar Bajic \date July, 21 2008.
 */
class CLevelManager
{

public:
	CLevelManager();
	~CLevelManager();
	bool Init(CGameManager* gameMngr, CGameGUI* gameGUI);
	bool OnEvent(const SEvent& event);
	void CleanCurrentMap();
	bool OnLoadMap(stringc map_filename);
	void SaveLevels();
	void SavePreviousLevelToTemp();
	void CreateCamera(vector3df position);
	void SetCameraPos(vector3df position);
	void SetMovieCamera(vector3df position, vector3df target);
	vector3df GetStartPosition();
	void SetMonstersMood(eMood mood);
	void Update(IVideoDriver* driver, IrrlichtDevice* pDevice, f32 elapsed_time, CPlayerCharacter* pc, IGUIFont* font);
	ISceneNode* GetClickedMonster(line3d<f32> picking_line);
	void SetMonstersCollisionAnimator();
	void AnimateTrigger(s32 id, u32 keyStart, u32 keyEnd, bool loop, f32 animationSpeed);
	void TranslateGameObject(ISceneNode* node, vector3df translationVectorEndPosition, u32 translationTime);
	void RotateGameObject(ISceneNode* node, vector3df rotationVectorEndPosition, u32 rotationTime);
	void  RemoveContainerContent(int containerID);
	void RemoveContainerContent(s32 containerID, s32 itemID);
	void  AddContainerItem(int containerID, CGameObject* pick);
	int GetContainerNumberOfItems(int containerID);
	CGameObject* GetContainerItem(int containerID, int itemID);
	IMetaTriangleSelector* GetLevelMetaTriangleSelector() { return m_pLevels[m_LevelIndex]->m_LevelMetaTriangleSelector;}
	IMetaTriangleSelector* GetObstacleMetaTriangleSelector() { return m_pLevels[m_LevelIndex]->m_ObstacleMetaTriangleSelector;}
	bool isNodeTerrain(const ISceneNode* node);
	bool isNodeActionObject(const ISceneNode* node);
	bool isObjectContainer(int id);
	bool isObjectPickable(int id);
	bool isObjectTrigger(int id);
	bool isObjectNPC(int id);
	bool isObjectMonster(int id);
	bool isNudgedMonsterColliding(s32 nudgerId, float nudgerRadius, s32 id, vector3df pos, vector3df targetPos);
	bool isMonsterColliding(s32 id, float nudgerRadius, vector3df pos, vector3df targetPos, vector3df* obstaclePos);
	void CreateParticleEffect(PARTICLES_EFFECT_TYPE type, PARTICLES_EFFECT_COLOR color, stringw target, bool follow_player);
	void CreateLightNode(bool dancing, s32 radius, s32 target);
	stringc GetLevelAmbientSound();
	stringc GetLevelMusicTheme();
	stringc GetLevelNarationFile();
	bool Action(s32 id);
	void CheckAreaApproach(vector3df playerPosition);
	stringw GetObjectScript(int id);
	void SetObjectState(int id, stringw state);
	stringw GetObjectState(int id);
	vector3df GetObjectPosition(int id);
	CGameObject* getGameObjectFromID(int id);
	void MoveCamera(vector3df pos);
	void DropPickableToMap(CGameObject* pick, vector3df position, bool obstacle);
	void StaticToPickable(s32 id);
	void DisintegrateObjectFromLevel(s32 objectID);
	stringw getCurrentMapName();
	u32 _GetLevelIndex(stringc map_filename);
	u32 FindNPCsOnMap(stringc mapname, stringc NPCNames[]);

	CGameManager* m_GameManager;
	CGameGUI* m_GameGUI;
	CLevel* m_pLevels[MAX_NUMBER_OF_LEVELS];
	s32 m_LevelIndex;
	ISceneNode* m_pHoverOverMonsterNode;
	ISceneNode* m_pHoverOverActionNode;
	u32 m_NumberOfLoadedLevels;
	ISound* m_LevelMusicTheme;
	ISound* m_LevelAmbientSound;
	ISound* m_LevelNarationFile;
	RTSCamera* m_pCamera;
	vector3df m_CurrentZoom;
	vector3df m_instersection_point;
};

#endif