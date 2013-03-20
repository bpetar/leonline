/** 
 * \file Level.h
 * \brief Header file containing CLevel class that is game level.
 * CLevel class manages game objects on the map.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#pragma once

#include <irrlicht.h>
#include "GameObject.h"
#include "../CMonster.h"
#include "../Particles.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CGameManager;

typedef struct
{
	ISceneNode* node;
	vector3df Distance;
	f32 translationTime;
	f32 endTime;
} TranslateGameObject;

typedef struct
{
	ISceneNode* node;
	vector3df angle;
	f32 rotationTime;
	f32 endTime;
} RotateGameObject;

typedef struct
{
	float lifeTime;
	float afterLifeTime;
	bool followPlayer;
	IParticleRotationAffector* rotationAftector;
	IParticleSystemSceneNode* ps;
} TemporaryParticleEffect;

typedef enum EEmiterType
{
	E_EMITERTYPE_BOX,
	E_EMITERTYPE_RING,
	E_EMITERTYPE_SPHERE,
	E_EMITERTYPE_CYLINDER,
	E_EMITERTYPE_POINT

} TEEmiterType;

typedef struct
{
	ISceneNode* node;
	f32 radius;
	bool entered;
} TArea;

class CLevel 
{
public:

	CLevel();
	~CLevel();
	bool Init(CGameManager* gm);
	bool Load(stringc map_filename);
	void Save(stringc map);
	void addTerrainSelector();
	//s32 OnMouseClick();

	/* ISceneUserDataSerializer functions: */
	vector3df GetStartPosition();
	void CleanUp();
	void DeleteActuatorFromQueue(s32 id);
	void DeleteMonsterFromQueue(s32 id);
	void UpdateMonsterHealthBar(s32 id, s32 value);
	void NotifyMonsterOfAttack(s32 id);
	void UpdateMonsters(IVideoDriver* driver, f32 elapsed_time, CPlayerCharacter* pc, IGUIFont* font, ICameraSceneNode* cam);
	void UpdateTranslateGameObject(f32 elapsed_time);
	void UpdateRotateGameObject(f32 elapsed_time);
	void UpdateParticles(f32 elapsed_time);
	ISceneNode* CreateLight(f32 radius);
	IParticleSystemSceneNode* CreateParticles(PARTICLES_EFFECT_TYPE type);
	IParticleSystemSceneNode* InsertParticlesNode(TEEmiterType emiterType, aabbox3df emiterSize, vector3df direction, stringc texture, stringc name, s32 emitRateMin, s32 emitRateMax, s32 angle, bool outlineOnly);
	void AddTranslateGameObject(ISceneNode* node, vector3df translationVectorEndPosition, u32 translationTime);
	void AddRotateGameObject(ISceneNode* node, vector3df rotationVectorEndPosition, u32 rotationTime);
	void ReadSceneNode(IXMLReader* reader);
	void WriteSceneNode(IXMLWriter* writer, ISceneNode* node);
	CGameObject* createPickableGameObject(stringw rootName, s32 id);
	void EnlightAllNodes();
	void DelightAllNodes();
	void RemoveContainerContent(int containerID);
	void RemoveContainerContent(s32 containerID, s32 itemID);
	bool EraseElement(int id);
	void AddContainerItem(int containerID, CGameObject* pick);
	int GetContainerNumberOfItems(int containerID);
	CGameObject* GetContainerItem(int containerID, int itemID);
	ISceneNode* isActionItemUnderMousePointer(); //returns node address != 0 if there is
	ISceneNode* isMonsterUnderMousePointer(); //returns node address != 0 if there is
	CGameObject* getClickedGameObject(position2d<s32> mousePos);
	vector3df getMonsterHealthBarPos(s32 id);
	void CreateParticleEffect(PARTICLES_EFFECT_TYPE type, PARTICLES_EFFECT_COLOR color, stringw target, bool follow_player);
	void CreateLightNode(bool dancing, s32 radius, s32 target);
	bool isActuator(ISceneNode* node);
	bool isObjectContainer(int id);
	bool isObjectPickable(int id);
	bool isObjectTrigger(int id);
	bool isObjectNPC(int id);
	bool isObjectMonster(int id);
	stringw GetObjectScript(int id);
	void SetObjectState(int id, stringw state);
	stringw GetObjectState(int id);
	vector3df GetObjectPosition(int id);
	void AddObjectToScene(CGameObject* pick, vector3df position, bool obstacle);

	stringw GetRootFromPath(stringw path);

	IFileSystem* m_pFS;
	IVideoDriver* m_pDriver;
	ISceneManager* m_SMGR;
	IrrlichtDevice* m_pDevice;
	stringc m_LoadingDir;
	stringc m_MapName;
	s32 m_ID;
	vector3df m_startPos;
	//array of game objects on this level
	list <CGameObject*> m_ListOfGameObjects;
	array <CMonster*> m_ListOfMonsters;
	array <ISceneNode*> m_ListOfActuators;
	array <TArea*> m_ListOfAreas;
	IMetaTriangleSelector* m_LevelMetaTriangleSelector; //used for clicking reaction for all objects on the map
	IMetaTriangleSelector* m_ObstacleMetaTriangleSelector; //used for collision with static objects
	//IMetaTriangleSelector* m_MonstersMetaTriangleSelector; //used for collision among monsters
	bool m_MapMusicThemeLoop;
	bool m_MapAmbientSoundLoop;
	stringc m_MapMusicTheme;
	stringc m_MapAmbientSound;
	stringc m_MapNarationFile;
	array <TranslateGameObject*> m_TranslateGameObject;
	array <RotateGameObject*> m_RotateGameObject;
	bool firstObstacle;
	ISceneNode* rootObstacleNode;

	bool m_SceneEnlighted; //if there are dynamic lights in the scene

	//debug
	core::aabbox3df dWorldBox;
	IAnimatedMeshSceneNode* dNode;

	CGameManager* m_GameManager;
	CGameObject* getGameObjectFromID(int id);

	array<TemporaryParticleEffect> m_ListOfTemporaryParticleEffects;

};