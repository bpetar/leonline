/** 
 * \file EDLevel.h
 * \brief Header file containing CEditorLevel class used for managing 3D action.
 * CEditorLevel class handles 3D models, saving and loading map, rotating/scaling/trans objects etc...
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#if !defined(AFX_EDLEVELICA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_EDLEVELICA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
#include "../gui/EDGui.h"
#include "RTSCamera.h"
#include "GameObject.h"
#include "CGridSceneNode.h"
#include "CTreeGenerator.h"
#include "CTreeSceneNode.h"
#include "CGrassPatchSceneNode.h"
#include "IWindGenerator.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define LE_AXIS_X 1
#define LE_AXIS_Y 2
#define LE_AXIS_Z 3

#define LE_MOVE_STEP 2

typedef enum EEmiterType
{
	E_EMITERTYPE_BOX,
	E_EMITERTYPE_RING,
	E_EMITERTYPE_SPHERE,
	E_EMITERTYPE_CYLINDER,
	E_EMITERTYPE_POINT

} TEEmiterType;



class CEditorManager;

/**
 * \brief CEditorLevel class manages 3D game objects.
 *
 * It takes care of saving/loading levels 
 * manages game object arrays (adding deleting game objects)
 * allows translation, rotation and scaling of models.
 * implements undo/redo functionality
 * translates events to game objects -> on mouse click, on rotate etc..
 * tightly communicates with GUI environment, for example: 
 *  - selecting different game object causes changes in GUI window Properties
 *  - changed game object properties must be saved to map file
 *
 * \author Petar Bajic \date July, 21 2008.
 */
class CEditorLevel : public ISceneUserDataSerializer 
{
public:
	CEditorLevel();
	~CEditorLevel();
	bool Init(CEditorManager* edMngr);
	bool OnEvent(const SEvent& event);
	void InitLevel();
	void OnNewMap();
	void OnLoadMap(stringc filename);
	void OnLoadMap_IrrScene(stringc filename);
	void OnSaveMap_IrrScene(stringc mapname);
	void InitListOfGameObjects();
	void ReadSceneNode(IXMLReader* reader);
	void WriteSceneNode(IXMLWriter* writer, ISceneNode* node);
	void OnSaveMap();
	CTreeSceneNode* createTree(PROCEDURAL_TREE_TYPE treeType);
	void InsertLight();
	void InsertParticles(TEEmiterType emiterType, aabbox3df emiterSize, vector3df direction, stringc texture, stringc name, s32 emitRateMin, s32 emitRateMax, s32 angle, bool outlineOnly);
	void InsertTree(PROCEDURAL_TREE_TYPE treeType);
	void InsertTerrainGrass(int param);
	void InsertFlag();
	void CreateCamera(vector3df position);
	void addTerrainSelector();
	void AddGameObjectToLevel(CGameObject* go);
	void RemoveGameObjectFromMap(CGameObject* go);
	vector3df GetIntersectionPoint(f32 height);
	bool SetElementAtHand(CGameObject* go);
	bool SendModelToPhotoSession(CGameObject* go);
	bool SendModelToPickSession(CGameObject* go);
	void SetElementAnchored(bool anchored);
	void SetElementTerrain(bool terrain);
	void SetElementVisible(bool visible);
	void SetElementIllusion(bool visible);
	void SetElementSelected(int elementID);
	void SetModelProperties(vector3df pos, vector3df rot, stringw name);
	void SetSelectedElement_Name(stringw name);
	void RemovePhotoSessionModel();
	void RemovePickSessionModel();
	void SetMapName(stringc mapname);
	void SetLoadingDir(stringc mapname);
	bool SingleObjectSelected();
	void MoveSelectedElement(int axis, int amount);
	void SetPositionSelectedElement(int axis, float value);
	void RotateSelectedElement(int axis, int amount);
	void SetRotationSelectedElement(int axis, float value);
	bool MapNameExists();
	void OnRender();
	void EnlightAllNodes();
	/* ISceneUserDataSerializer functions: */
	IAttributes* createUserData (irr::scene::ISceneNode *forSceneNode);
	void OnReadUserData (irr::scene::ISceneNode *forSceneNode, irr::io::IAttributes *userData);
	virtual void OnCreateNode(ISceneNode* node);

	void CEditorLevel::getPickedNodeBBB(ISceneNode* root, const core::line3df& ray, s32 bits, bool bNoDebugObjects, f32& outbestdistance, ISceneNode*& outbestnode);
	ISceneNode* CEditorLevel::getSceneNodeFromRayBBB(core::line3d<f32> ray,s32 idBitMask,bool bNoDebugObjects, ISceneManager* smgr);
	ISceneNode* CEditorLevel::getSceneNodeFromScreenCoordinatesBBB(core::position2d<s32> pos, ISceneManager* smgr);

	CEditorManager* m_EditorManager;
	//we need list of game objects here
	ISceneNode* m_SelectedGameObject;
	ISceneNode* m_PreviewGameObject;
	ISceneNode* m_PreviewPickGameObject;
	bool m_bSavedState;
	bool m_bMoveSelectedNode;
	bool m_bNodeBeingMoved; //used for undo move action
	bool m_bElementAtHand;
	bool m_bShiftPressed;
	bool m_bCtrlPressed;
	bool m_bKeyXPressed;
	bool m_bKeyZPressed;
	
	bool m_bRotateXSelectedNode;
	bool m_bRotateYSelectedNode;
	bool m_bRotateZSelectedNode;
	
	bool m_MultiSelectStart;
	vector3df m_MultiSelectPosStart;
	vector3df m_MultiSelectPosEnd;
	vector3df m_MoveOldPosition;

	stringc m_MapName;
	stringc m_LoadingDir;
	s32 m_NodeRotationX;
	s32 m_NodeRotationY;
	list <CGameObject*> m_ListOfGameObjects;
	vector3df m_CurrentZoom;
	vector3df m_CurrentRotationOfSelectedNode;
	vector3df m_objectMoveOffset;
	RTSCamera* m_LevelCamera;
	aabbox3df m_SelectedBox;
	IMetaTriangleSelector * m_LevelMetaTriangleSelector; //used for collision with static objects
	s32 m_TileHeight;
	s32 m_WallHeight;
	CGridSceneNode* m_Grid;
	ITerrainSceneNode* terrain;

	stringw m_LevelMusic_MusicFile;
	stringw m_LevelMusic_SoundFile;
	stringw m_LevelMusic_NarationFile;

	CTreeGenerator* m_AspenTreeGenerator;
	CTreeGenerator* m_OakTreeGenerator;
	CTreeGenerator* m_PineTreeGenerator;
	CTreeGenerator* m_WillowTreeGenerator;
	CTreeSceneNode* m_Tree;
	video::ITexture* m_AspenTreeTexture;
    video::ITexture* m_AspenLeafTexture;
	video::ITexture* m_OakTreeTexture;
    video::ITexture* m_OakLeafTexture;
	video::ITexture* m_PineTreeTexture;
    video::ITexture* m_PineLeafTexture;
	video::ITexture* m_WillowTreeTexture;
    video::ITexture* m_WillowLeafTexture;

	void AddTerrainFromHeightmap(stringc heightmapFilename,
								 stringc textureFilename,
								 stringc detailsFilename,
								 vector3df position,
								 vector3df rotation,
								 vector3df scale,
								 s32 maxLOD,
								 s32 smoothFactor,
								 E_TERRAIN_PATCH_SIZE patchSize);

	CGameObject* _getGameObjectFromID(s32 id);
	void _eraseGameObject(s32 id);
	void ObjectClearPickableItems(s32 objectID);
	void ObjectAddPickableItem(s32 objectID, stringc rootname);
	CGameObject* GetObjectPickableItem(s32 objectID, s32 index);
	stringw GetObjectParameter_State(s32 objectID);
	stringw GetObjectParameter_Name(s32 objectID);
	stringw GetObjectParameter_RootName(s32 objectID);
	stringw GetObjectParameter_Mesh(s32 objectID);
	stringw GetObjectParameter_Script(s32 objectID);
	s32 GetObjectNumberOfPickableItems(s32 objectID);
	s32 GetSelectedObjectParameter_ID();
	void SetObjectParameter_State(s32 objectID, stringw state);
	void SetSelectedObjectParameter_isContainer(bool isContainer);
	void SetSelectedObjectParameter_isPickable(bool isPickable);
	void SetSelectedObjectParameter_isTrigger(bool isTrigger);
	void SetSelectedObjectParameter_isNPC(bool isNPC);
	void SetSelectedObjectParameter_isMonster(bool isMonster);

	
	
};

#endif