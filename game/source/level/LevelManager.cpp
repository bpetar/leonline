/** 
 * \file LevelManager.cpp
 * \brief File LevelManager.cpp takes care of all game levels.
 *
 * takes care of saving/loading levels
 * manages list of levels
 * tightly communicates with GUI environment. 
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "LevelManager.h"
#include "../GameManager.h"
#include "../gui/GameGUI.h"

#define CAMERA_OFFSET vector3df(50,180,50)

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CLevelManager::CLevelManager()
{
	m_GameManager = NULL;
	m_CurrentZoom = vector3df(1.0f,1.0f,1.0f);
	m_NumberOfLoadedLevels = 0;
	m_pHoverOverMonsterNode = 0;
	m_pHoverOverActionNode = 0;

	m_LevelMusicTheme = 0;
	m_LevelAmbientSound = 0;
	m_LevelNarationFile = 0;
}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CLevelManager::~CLevelManager()
{
	for(u32 i=0; i<MAX_NUMBER_OF_LEVELS; i++)
	{
		if(m_pLevels[i] != 0)
		{
			delete m_pLevels[i];
		}
	}

	if(m_LevelMusicTheme)
	{
		m_LevelMusicTheme->stop();
		m_LevelMusicTheme->drop();
		m_LevelMusicTheme = 0;
	}

	if(m_LevelAmbientSound)
	{
		m_LevelAmbientSound->stop();
		m_LevelAmbientSound->drop();
		m_LevelAmbientSound = 0;
	}

	if(m_LevelNarationFile)
	{
		m_LevelNarationFile->stop();
		m_LevelNarationFile->drop();
		m_LevelNarationFile = 0;
	}
}

/**
 * helper function: draw the object aligned bounding box
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void drawObjectBoundingBox(scene::ISceneNode* node,
                                      video::IVideoDriver* driver,
									  aabbox3df selectedBox,
                                      video::SColor color = video::SColor(255, 0, 255, 0))
{
	if(node)
	{
   video::SMaterial matl;
   matl.Lighting = false;
   driver->setMaterial(matl);
   driver->setTransform(video::ETS_WORLD, node->getAbsoluteTransformation());
   driver->draw3DBox(((IAnimatedMeshSceneNode*)node)->getBoundingBox(), color);
	}
   	/*Vitek said:
	Be careful with getTransformedBoundingBox(). It doesn't rotate the box properly so you will occasionally get boxes that don't actually enclose the object. This can be very noticeable when you have an oblong object that is rotated.
	The best is to use matrix4::transformBoxEx() to do something like this...

	core::aabbox3df b1 = cube->getBoundingBox();
	cube->getAbsoluteTransformation().transformBoxEx(b1);
	core::aabbox3df b2 = sphere->getBoundingBox();
	sphere->getAbsoluteTransformation().transformBoxEx(b2);
	*/
}


/**
 * \brief Update monsters actions and behavior
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CLevelManager::Update(IVideoDriver* driver, IrrlichtDevice* pDevice, f32 elapsed_time, CPlayerCharacter* pc, IGUIFont* font)
{
	m_pLevels[m_LevelIndex]->UpdateMonsters(driver, elapsed_time, pc, font, m_pCamera);
	m_pLevels[m_LevelIndex]->UpdateTranslateGameObject(elapsed_time);
	m_pLevels[m_LevelIndex]->UpdateParticles(elapsed_time);

	//debug draw bounding box:
	//drawObjectBoundingBox(m_pLevels[m_LevelIndex]->dNode,driver,m_pLevels[m_LevelIndex]->dWorldBox);
}

bool CLevelManager::isNudgedMonsterColliding(s32 nudgerId, float nudgerRadius, s32 id, vector3df nudgerPos, vector3df targetPos)
{
	for(u32 i=0; i < m_pLevels[m_LevelIndex]->m_ListOfMonsters.size(); i++)
	{
		ISceneNode* node = (ISceneNode*)m_pLevels[m_LevelIndex]->m_ListOfMonsters[i]->getNode();
		float neighborRadius = m_pLevels[m_LevelIndex]->m_ListOfMonsters[i]->m_GO->m_Radius;
		if((node->getID() != id)&&(node->getID() != nudgerId))
		{
			vector3df neighborPos = node->getPosition();
			neighborPos.Y = 0;
			nudgerPos.Y = 0;
			if(neighborPos.getDistanceFrom(nudgerPos) < nudgerRadius+neighborRadius)
			{
				return true;
			}
		}
	}

	return false;
}

bool CLevelManager::isMonsterColliding(s32 id, float nudgerRadius, vector3df pos, vector3df targetPos, vector3df* obstaclePos)
{
	for(u32 i=0; i < m_pLevels[m_LevelIndex]->m_ListOfMonsters.size(); i++)
	{
		ISceneNode* neighborNode = (ISceneNode*)m_pLevels[m_LevelIndex]->m_ListOfMonsters[i]->getNode();
		float neighborRadius = m_pLevels[m_LevelIndex]->m_ListOfMonsters[i]->m_GO->m_Radius;
		if(neighborNode->getID() != id)
		{
			vector3df neighborPos = neighborNode->getPosition();
			neighborPos.Y = 0;
			pos.Y = 0;
			if(neighborPos.getDistanceFrom(pos) < nudgerRadius + neighborRadius)
			{
				if(neighborPos.getDistanceFrom(targetPos) < pos.getDistanceFrom(targetPos))
				{
					*obstaclePos = neighborPos;
					m_pLevels[m_LevelIndex]->m_ListOfMonsters[i]->Nudge(id, pos);
					return true;
				}
			}
		}
		//ISceneNodeAnimator* anim = m_GameManager->getSceneMngr()->createCollisionResponseAnimator(m_pLevels[m_LevelIndex]->m_MonstersMetaTriangleSelector, node, vector3df(10,5,10),vector3df(0,-6,0),vector3df(0,1,0));
		//node->addAnimator(anim);
		//anim->drop();  // Drop the animator when we're done referring to it.
	}

	return false;
}

/**
 * \brief Set collision animator for each monster
 * \author Petar Bajic 
 * \date July, 21 2010.
 */
void CLevelManager::SetMonstersCollisionAnimator()
{
	for(u32 i=0; i < m_pLevels[m_LevelIndex]->m_ListOfMonsters.size(); i++)
	{
		ISceneNode* node = (ISceneNode*)m_pLevels[m_LevelIndex]->m_ListOfMonsters[i]->getNode();
		//ISceneNodeAnimator* anim = m_GameManager->getSceneMngr()->createCollisionResponseAnimator(m_pLevels[m_LevelIndex]->m_MonstersMetaTriangleSelector, node, vector3df(10,5,10),vector3df(0,-6,0),vector3df(0,1,0));
		//node->addAnimator(anim);
		//anim->drop();  // Drop the animator when we're done referring to it.
	}
}

/**
 * \brief Return first monster pierced by this picking line
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
ISceneNode* CLevelManager::GetClickedMonster(line3d<f32> picking_line)
{
	for(u32 i=0; i < m_pLevels[m_LevelIndex]->m_ListOfMonsters.size(); i++)
	{
		//debugPrint("GetClickedMonster: %d\n",i);
		ISceneNode* node = (ISceneNode*)m_pLevels[m_LevelIndex]->m_ListOfMonsters[i]->getNode();
		const core::aabbox3df & objectBox = node->getBoundingBox();
		core::aabbox3df worldBox(objectBox);
		node->getAbsoluteTransformation().transformBox(worldBox);
		if (worldBox.intersectsWithLine(picking_line))
		{
			//debugPrint("Monster under pointer: %d\n",node->getID());
			//we have problem here in case more of them monsters are on the ray path...
			//it may not return the closest one.
			//but we decide to ignore this issue because of 3rd person view game...
			//we look on monsters down from the top, and they are not stacked on top of each other...
			
			//if some obstacle is in the way, then no monster is picked
			vector3df point;
			triangle3df triangle;
			ISceneNode* obstacle = m_GameManager->getSceneMngr()->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(picking_line, point, triangle, 0, m_pLevels[m_LevelIndex]->rootObstacleNode, false);
			if(obstacle)
			{
				if(obstacle != node)
				{
					if(point.getDistanceFrom(m_pCamera->getPosition()) < node->getPosition().getDistanceFrom(m_pCamera->getPosition()))
					{
						//debugPrint("Obstacle before monster: %d\n",obstacle->getID());
						return 0;
					}
				}
			}
			
			return node;
		}
	}

	return 0;
}

/**
 * \brief Init function stores pointer to CGameManager and creates initial level.
 * \author Petar Bajic
 * \date July, 21 2008.
 */
bool CLevelManager::Init(CGameManager* gameMngr, CGameGUI* gameGUI)
{
	m_GameManager = gameMngr;

	m_LevelIndex = -1;
	
	//what is this for?
	m_GameManager->getDriver()->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	// add irrlicht logo
	m_GameManager->getGUIEnvironment()->addImage(m_GameManager->getDriver()->getTexture(IRRLOGO_FILE), core::position2d<s32>(50,50));

	m_GameGUI = gameGUI;

	for(u32 i=0; i<MAX_NUMBER_OF_LEVELS; i++)
	{
		m_pLevels[i] = 0;
	}

	return true;
}

/**
 * \brief Animates an object with given keyframes
 * \author Petar Bajic 
 * \date July, 21 2011.
 */
void CLevelManager::AnimateTrigger(s32 id, u32 keyStart, u32 keyEnd, bool loop, f32 animationSpeed)
{
	IAnimatedMeshSceneNode* node = (IAnimatedMeshSceneNode*) m_pLevels[m_LevelIndex]->m_SMGR->getSceneNodeFromId(id);
	if(node)
	{
		node->setAnimationSpeed(animationSpeed);
		node->setLoopMode(false);
		node->setFrameLoop(keyStart,keyEnd);
	}
	else
	{
		printf("scene node id defined in script doesn't exist on this map!");
	}
	//node->setAnimationEndCallback(this);

	//for obstacle triggers only:
	//we don't have nice physics engine, so our player has fixed triangle surroundings.
	//in case of animated doors, first we have closed door obstacle, and then here they are removed 
	//from obstacles and player can go through them as soon as they start opening.
	//we can make them stay in obstacle list once opened (on animation end), but physics engine is proper solution here.
	//m_pLevels[m_LevelIndex]->m_ObstacleMetaTriangleSelector->removeTriangleSelector(node->getTriangleSelector());
	//m_pLevels[m_LevelIndex]->m_LevelMetaTriangleSelector->removeTriangleSelector(node->getTriangleSelector());

}

/**
 * \brief Animates an object with given keyframes
 * \author Petar Bajic 
 * \date July, 21 2011.
 */
void CLevelManager::TranslateGameObject(ISceneNode* node, vector3df translationVectorEndPosition, u32 translationTime)
{
	
	if(node != m_GameManager->getPC()->getNode())
	{
		//remove from obstacles (for sliding platforms we would surely want to have obstacle triangle selector move with the plaform, but like above, physic engine is proper solution for that.
		m_pLevels[m_LevelIndex]->m_ObstacleMetaTriangleSelector->removeTriangleSelector(node->getTriangleSelector());
	}
	m_pLevels[m_LevelIndex]->AddTranslateGameObject(node,translationVectorEndPosition,translationTime);
}


/**
 * \brief Creates camera in initial position (used for loading map)
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CLevelManager::CreateCamera(vector3df position)
{
	m_pCamera = new RTSCamera(m_GameManager->getDevice(),m_pLevels[m_LevelIndex]->m_SMGR->getRootSceneNode(),m_GameManager->getSceneMngr(),-1,100.0f,10.0f,100.0f);
	m_pCamera->setPosition(position + CAMERA_OFFSET);
	m_pCamera->setTarget(position);
}

/**
 * \brief Sets camera position
 * \author Petar Bajic 
 * \date February 10, 2010.
 */
void CLevelManager::SetCameraPos(vector3df position)
{
	m_pCamera->setPosition(position + CAMERA_OFFSET);
	m_pCamera->setTarget(position);
}

stringw CLevelManager::getCurrentMapName()
{
	return m_pLevels[m_LevelIndex]->m_MapName;
}

/**
 * \brief returns initial player position if marked on the map
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
vector3df CLevelManager::GetStartPosition()
{
	return m_pLevels[m_LevelIndex]->GetStartPosition();
}

/**
 * \brief saves level to temp dir. 
 *
 * When player moves from level to level, and happens to return to previous one, 
 * it has to be saved as he left it, even if player haven't explicitly saved the game.
 * That is what temp saved levels are for.
 *
 * \author Petar Bajic 
 * \date January, 24 2010.
 */
void CLevelManager::SavePreviousLevelToTemp()
{
	m_pLevels[m_LevelIndex]->Save(stringc("save/temp/") + m_pLevels[m_LevelIndex]->m_MapName);
}

void CLevelManager::SaveLevels()
{
	int c;
	//FILE *in,*out;
	for(u32 i=0; i<m_NumberOfLoadedLevels; i++)
	{
		stringc tempname = stringc("save/temp/") + m_pLevels[i]->m_MapName;
		stringc savename = stringc("save/") + m_pLevels[i]->m_MapName;
		if(m_GameManager->getFS()->existFile(tempname.c_str()))
		{
			remove(savename.c_str());
			c = rename(tempname.c_str(),savename.c_str());
			if(c) perror(0);
		}
		else
		{
			m_pLevels[i]->Save(stringc("save/") + m_pLevels[i]->m_MapName);
		}
	}
}

/**
 * \brief Determines if this map was already loaded, and returns new index if not.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
u32 CLevelManager::_GetLevelIndex(stringc map_filename)
{
	u32 index = 0;
	stringc MapName = map_filename.subString(map_filename.findLast('/')+1,map_filename.size());

	for(index=0; index<m_NumberOfLoadedLevels; index++)
	{
		if(m_pLevels[index]->m_MapName == MapName)
		{
			return index;
		}
	}

	return index;
}

void CLevelManager::CleanCurrentMap()
{
	m_pLevels[m_LevelIndex]->CleanUp();
}

/**
 * \brief Manages Map Loading
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CLevelManager::OnLoadMap(stringc map_filename)
{
	//Cleanup first
	m_pHoverOverMonsterNode = 0;
	m_pHoverOverActionNode = 0;

	if(m_LevelMusicTheme)
	{
		m_LevelMusicTheme->stop();
		m_LevelMusicTheme->drop();
		m_LevelMusicTheme = 0;
	}

	if(m_LevelAmbientSound)
	{
		m_LevelAmbientSound->stop();
		m_LevelAmbientSound->drop();
		m_LevelAmbientSound = 0;
	}

	if(m_LevelNarationFile)
	{
		m_LevelNarationFile->stop();
		m_LevelNarationFile->drop();
		m_LevelNarationFile = 0;
	}
	
	//level index is tied to map, if level is to be loaded again, his old index will be waiting for him.
	m_LevelIndex = _GetLevelIndex(map_filename);

	//Create new level if map is loaded for the first time
	if(!m_pLevels[m_LevelIndex])
	{
		m_pLevels[m_LevelIndex] = new CLevel();
		m_pLevels[m_LevelIndex]->Init(m_GameManager);
		m_NumberOfLoadedLevels++;
	}
	else
	{
		//Shouldn't we clean this map that we already visited, and now returning to it?
		//m_pLevels[m_LevelIndex]->CleanUp();
	}

	//Load map scene from file
	if (m_pLevels[m_LevelIndex]->Load(map_filename))
	{
		//back to working dir
		m_GameManager->backToWorkingDirectory();
		CreateCamera(GetStartPosition());

		m_LevelMusicTheme = m_GameManager->Play2DSound(GetLevelMusicTheme(),true);
		m_LevelAmbientSound = m_GameManager->Play2DSound(GetLevelAmbientSound(),true);
		m_LevelNarationFile = m_GameManager->Play2DSound(GetLevelNarationFile(),false);

		return true;
	}

	//Display error to console
	stringw message  = "Map: ";
	message += map_filename;
	message += " can not be loaded!";
	m_GameManager->getGUIEnvironment()->addMessageBox(L"Error Loading Map", message.c_str());

	return false;
}

u32 CLevelManager::FindNPCsOnMap(stringc mapname, stringc NPCNames[])
{
	u32 numNPCs = 0;
	IXMLReader* reader = m_GameManager->getFS()->createXMLReader(mapname.c_str());

	if(!reader)
	{
		//map doesn't exist
		stringw message  = "Map: ";
		message += mapname;
		message += " can not be loaded! It is listed in game_config.xml and I can't find it!";
		m_GameManager->getGUIEnvironment()->addMessageBox(L"Error Loading Map", message.c_str());
		return 0;
	}

	while(reader->read())
	{
		//loop through scene nodes in xml
		switch(reader->getNodeType())
		{
			case io::EXN_ELEMENT:
			{
				if (stringw("attributes") == reader->getNodeName())
				{
					//load node attributes
					IAttributes* attr = m_GameManager->getFS()->createEmptyAttributes(m_GameManager->getDriver());
					attr->read(reader);
					if(attr->getAttributeAsBool("isNPC"))
					{
						stringc meshPath = attr->getAttributeAsString("Mesh");
						NPCNames[numNPCs] = m_GameManager->getRootNameFromPathName(meshPath);
						numNPCs++;
					}
				}
				break;
			}
		}
	}
	return numNPCs;
}

void CLevelManager::RemoveContainerContent(int containerID)
{
	m_pLevels[m_LevelIndex]->RemoveContainerContent(containerID);
}

void CLevelManager::RemoveContainerContent(s32 containerID, s32 itemID)
{
	m_pLevels[m_LevelIndex]->RemoveContainerContent(containerID, itemID);
}

void CLevelManager::AddContainerItem(int containerID, CGameObject* pick)
{
	m_pLevels[m_LevelIndex]->AddContainerItem(containerID, pick);
}

int CLevelManager::GetContainerNumberOfItems(int containerID)
{
	return m_pLevels[m_LevelIndex]->GetContainerNumberOfItems(containerID);
}

CGameObject* CLevelManager::GetContainerItem(int containerID, int itemID)
{
	return m_pLevels[m_LevelIndex]->GetContainerItem(containerID, itemID);
}

bool CLevelManager::isObjectContainer(int id)
{
	return m_pLevels[m_LevelIndex]->isObjectContainer(id);
}

bool CLevelManager::isObjectPickable(int id)
{
	return m_pLevels[m_LevelIndex]->isObjectPickable(id);
}

bool CLevelManager::isObjectTrigger(int id)
{
	return m_pLevels[m_LevelIndex]->isObjectTrigger(id);
}

bool CLevelManager::isObjectNPC(int id)
{
	return m_pLevels[m_LevelIndex]->isObjectNPC(id);
}

bool CLevelManager::isObjectMonster(int id)
{
	return m_pLevels[m_LevelIndex]->isObjectMonster(id);
}

stringw CLevelManager::GetObjectScript(int id)
{
	return m_pLevels[m_LevelIndex]->GetObjectScript(id);
}

void CLevelManager::SetObjectState(int id, stringw state)
{
	m_pLevels[m_LevelIndex]->SetObjectState(id,state);
}

stringw CLevelManager::GetObjectState(int id)
{
	return m_pLevels[m_LevelIndex]->GetObjectState(id);
}

vector3df CLevelManager::GetObjectPosition(int id)
{
	return m_pLevels[m_LevelIndex]->GetObjectPosition(id);
}

CGameObject* CLevelManager::getGameObjectFromID(int id)
{
	return m_pLevels[m_LevelIndex]->getGameObjectFromID(id);
}

bool CLevelManager::isNodeTerrain(const ISceneNode* node)
{
	CGameObject* go = m_pLevels[m_LevelIndex]->getGameObjectFromID(node->getID());
	if(go)
	{
		if(go->isTerrain)
		{
			return true;
		}
	}

	return false;
}

bool CLevelManager::isNodeActionObject(const ISceneNode* node)
{
	CGameObject* go = m_pLevels[m_LevelIndex]->getGameObjectFromID(node->getID());
	if(go)
	{
		if(go->isContainer || go->isMonster || go->isNPC || go->isPickable || go->isTrigger)
		{
			return true;
		}
	}

	return false;
}

void CLevelManager::SetMonstersMood(eMood mood)
{
	for(u32 i=0; i < m_pLevels[m_LevelIndex]->m_ListOfMonsters.size(); i++)
	{
		m_pLevels[m_LevelIndex]->m_ListOfMonsters[i]->m_GO->m_Mood = mood;
	}
}

stringc CLevelManager::GetLevelAmbientSound()
{
	return m_pLevels[m_LevelIndex]->m_MapAmbientSound;
}

stringc CLevelManager::GetLevelMusicTheme()
{
	return m_pLevels[m_LevelIndex]->m_MapMusicTheme;
}

stringc CLevelManager::GetLevelNarationFile()
{
	return m_pLevels[m_LevelIndex]->m_MapNarationFile;
}

void CLevelManager::CheckAreaApproach(vector3df playerPosition)
{
	for(u32 i=0; i< m_pLevels[m_LevelIndex]->m_ListOfAreas.size(); i++)
	{
		ISceneNode* node = m_pLevels[m_LevelIndex]->m_ListOfAreas[i]->node;
		vector3df areaCenter = node->getPosition();
		f32 distance = playerPosition.getDistanceFrom(areaCenter);
		if((distance < m_pLevels[m_LevelIndex]->m_ListOfAreas[i]->radius) && (!m_pLevels[m_LevelIndex]->m_ListOfAreas[i]->entered) )
		{
			CGameObject* go = m_pLevels[m_LevelIndex]->getGameObjectFromID(node->getID());
			m_GameManager->m_pScriptEngine->OnEvent(SCRIPT_EVENT_AREA_ONAPPROACH, go->script, go->id);
			m_pLevels[m_LevelIndex]->m_ListOfAreas[i]->entered = true;
			return;
		}
		if((distance > m_pLevels[m_LevelIndex]->m_ListOfAreas[i]->radius) && (m_pLevels[m_LevelIndex]->m_ListOfAreas[i]->entered) )
		{
			CGameObject* go = m_pLevels[m_LevelIndex]->getGameObjectFromID(node->getID());
			m_GameManager->m_pScriptEngine->OnEvent(SCRIPT_EVENT_AREA_ONLEAVE, go->script, go->id);
			m_pLevels[m_LevelIndex]->m_ListOfAreas[i]->entered = false;
			return;
		}
	}
}

bool CLevelManager::Action(s32 id)
{
	//perform action of game object
	CGameObject* go = m_pLevels[m_LevelIndex]->getGameObjectFromID(id);

	if (go)
	{
		//nonexclusive list of "ifs" allows objects to be manything
		//object can be both container and pickable 
		//Update: Id say no, not exactly
		if (go->isPickable)
		{
			if(!m_GameManager->getGameGUI()->InventoryFull())
			{
				m_GameManager->getGameGUI()->AddConsoleText(go->name + m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_PICKED));
				m_GameManager->getGameGUI()->DrawFloatingText(go->pos+vector3df(0,15,0), m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_ITEM_GAIN) + go->name, 5, SColor(255,40,240,40));
				//Move item to inventory automatically
				m_GameManager->getGameGUI()->AddPickableToInventory(go);
				//Remove item from PC targets
				m_GameManager->getPC()->targetRemoved();
				m_pHoverOverMonsterNode = 0;
				//Plus do the pickup script
				m_GameManager->m_pScriptEngine->OnEvent(SCRIPT_EVENT_ONPICK, go->script, go->id);
				//Remove object from the scene
				m_pLevels[m_LevelIndex]->EraseElement(id);
				m_pLevels[m_LevelIndex]->DeleteActuatorFromQueue(id);
				ISceneNode* pickedNode = m_GameManager->getSceneMngr()->getSceneNodeFromId(id);
				m_pLevels[m_LevelIndex]->m_LevelMetaTriangleSelector->removeTriangleSelector(pickedNode->getTriangleSelector());
				//I think pickables are not added to Obstacles in the first place so commenting out...
				//m_pLevels[m_LevelIndex]->m_ObstacleMetaTriangleSelector->removeTriangleSelector(pickedNode->getTriangleSelector());
				pickedNode->remove();
			}
			else
			{
				m_GameManager->getGameGUI()->AddConsoleText(E_LANG_STRING_LEVEL_GUI_MSGBOX_INVENTORY_FULL);
			}
			//return is needed here because there is a situation when action causes level change (MoveToMap)
			//and rest of the code (isNPC,isTrigger) can not be performed on new level cause it crashes naturally.
			return true;
		}
		if (go->isTrigger)
		{
			if(m_GameManager->getGameGUI()->m_bDraggingPickableItem)
			{
				m_GameManager->m_pScriptEngine->OnEvent(SCRIPT_EVENT_ONUSEAGAINST, m_GameManager->getGameGUI()->m_pDraggedPickableItem->script, go->id);
			}
			else
			{
				m_GameManager->m_pScriptEngine->OnEvent(SCRIPT_EVENT_TRIGGER_ONCLICK, go->script, go->id);
			}
			//return is needed here because there is a situation when action causes level change (MoveToMap)
			//and rest of the code (isNPC check) can not be performed on new level cause it crashes naturally.
			return true;
		}
		if (go->isNPC)
		{
			stringw NPCName = m_GameManager->getRootNameFromPathName(go->mesh);
			NPCName += ".dlg";
			m_GameManager->getGameGUI()->StartNPCDialog(NPCName, go->name);
			return true;
		}
		if (go->isMonster)
		{
			//Attack the beast!
			//notify the monster that hes being attacked
			m_pLevels[m_LevelIndex]->NotifyMonsterOfAttack(go->id);

			//calculate
			int PCAttackMin = m_GameManager->getPC()->getSkillMin("Attack");
			int PCAttackMax = m_GameManager->getPC()->getSkillMax("Attack");
			int MonsterMinDefence = go->getSkillMin("Defense");
			int MonsterMaxDefence = go->getSkillMax("Defense");
			int PCDamageMin = m_GameManager->getPC()->getSkillMin("Damage");
			int PCDamageMax = m_GameManager->getPC()->getSkillMax("Damage");

			int randomPCAttack = 0;
			if(PCAttackMax != PCAttackMin)
			{
				randomPCAttack = rand()%(PCAttackMax-PCAttackMin+1) + PCAttackMin;
			}
			else
			{
				randomPCAttack = PCAttackMin;
			}
			int randomMonsterDefence = 0;
			if(MonsterMaxDefence != MonsterMinDefence)
			{
				randomMonsterDefence = rand()%(MonsterMaxDefence-MonsterMinDefence+1) + MonsterMinDefence;
			}
			else
			{
				randomMonsterDefence = MonsterMinDefence;
			}

			if(randomPCAttack>=randomMonsterDefence)
			{
				//Hit!
				int dmg = 0;
				if(PCDamageMax != PCDamageMin)
				{
					dmg = rand()%(PCDamageMax-PCDamageMin+1) + PCDamageMin;
				}
				else
				{
					dmg = PCDamageMin;
				}

				//Display Floating damage
				m_GameManager->DisplayMonsterDamage(m_pLevels[m_LevelIndex]->getMonsterHealthBarPos(id),dmg);
				//Display Blod Stain

				go->changeAbilityValueBy("Health",-dmg);
				m_pLevels[m_LevelIndex]->UpdateMonsterHealthBar(go->id, -dmg);

				m_GameManager->getGameGUI()->AddConsoleText(m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_YOU_HIT) + go->name + m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_WITH) + stringw(dmg) + m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_DAMAGE) + stringw(" (") + stringw(randomPCAttack) + stringw(" > ") + stringw(randomMonsterDefence) + stringw(")"));
				if(go->getAbilityValue("Health") <= 0)
				{
					//Monster Dies!
					m_GameManager->getGameGUI()->AddConsoleText(go->name + m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_DEAD) + stringw(go->getAbilityValue("Experience")) + m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_EXP_GAIN));
				
					debugPrint("Monster dies: %d\n",go->id);

					//DisplayBlodEffect();

					//PlayDyingAnimation();

					//Add Experiance
					m_GameManager->AddPCExperience(go->getAbilityValue("Experience"));

					//Remove monster from PC targets
					m_GameManager->getPC()->targetRemoved();
					
					ISceneNode* monstrousNode = m_GameManager->getSceneMngr()->getSceneNodeFromId(id);

					//PlayDyingSound
					m_GameManager->Play3DSound(go->m_Sound_Die,monstrousNode->getPosition(),false);

					//Drop pickables from inventory
					for(u32 i = 0; i< go->m_ListOfPickableItems.size(); i++)
					{
						DropPickableToMap(go->m_ListOfPickableItems[i], monstrousNode->getPosition());
					}

					//Remove object from the scene
					m_pLevels[m_LevelIndex]->EraseElement(id);
					m_pLevels[m_LevelIndex]->m_LevelMetaTriangleSelector->removeTriangleSelector(monstrousNode->getTriangleSelector());
					//I think monsters are not added to Obstacles in the first place so commenting out...
					//m_pLevels[m_LevelIndex]->m_ObstacleMetaTriangleSelector->removeTriangleSelector(monstrousNode->getTriangleSelector());
					m_pHoverOverMonsterNode = 0;
					
					//DeleteMonsterFromQueue();
					m_pLevels[m_LevelIndex]->DeleteMonsterFromQueue(id);

				}
				else
				{
					//Monster takes damage but lives!
					//PlayWoundedSound
					m_GameManager->Play3DSound(go->m_Sound_Wound,m_GameManager->getSceneMngr()->getSceneNodeFromId(id)->getPosition(),false);
				}
			}
			else
			{
				//Miss!
				m_GameManager->DisplayMonsterDamage(m_pLevels[m_LevelIndex]->getMonsterHealthBarPos(id),0); //this will display "miss" on screen...
				m_GameManager->getGameGUI()->AddConsoleText(m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_CONSOLE_MISSED) + go->name + stringw(" (") + stringw(randomPCAttack) + stringw(" < ") + stringw(randomMonsterDefence) + stringw(")"));
			}
			return true;
		}
		if (go->isContainer)
		{
			m_GameManager->m_pScriptEngine->OnEvent(SCRIPT_EVENT_ONCONTAINERCLICK, go->script, go->id);
			return true;
		}

	}
	return false;
}

/**
 * \brief Mouse and Keyboard events are handled here. 
 * Every mouse click on game object is caught, interaction with the map is handled:
 * doors lead to another map, containers display their content, NPCs start talking...
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CLevelManager::OnEvent(const SEvent& ovent)
{
	if(ovent.EventType == EET_MOUSE_INPUT_EVENT)
	{
		switch(ovent.MouseInput.Event)
		{
			case EMIE_MOUSE_MOVED:
				{
					m_pHoverOverMonsterNode = m_pLevels[m_LevelIndex]->isMonsterUnderMousePointer();
						
					if(m_pHoverOverMonsterNode == 0)
						m_pHoverOverActionNode = m_pLevels[m_LevelIndex]->isActionItemUnderMousePointer();
				}
				break;
			default:
				break;
		}
	}
	
	//hehe, 'Uvent'
	m_pCamera->OnUvent(ovent);

	return false;
}

void CLevelManager::DisintegrateObjectFromLevel(s32 objectID)
{
	//Remove object from the scene
	m_pLevels[m_LevelIndex]->EraseElement(objectID);
	m_pLevels[m_LevelIndex]->DeleteActuatorFromQueue(objectID);
	ISceneNode* doomedNode = m_GameManager->getSceneMngr()->getSceneNodeFromId(objectID);
	m_pLevels[m_LevelIndex]->m_LevelMetaTriangleSelector->removeTriangleSelector(doomedNode->getTriangleSelector());
	m_pLevels[m_LevelIndex]->m_ObstacleMetaTriangleSelector->removeTriangleSelector(doomedNode->getTriangleSelector());
	doomedNode->remove();
}

void CLevelManager::DropPickableToMap(CGameObject* pick, vector3df position)
{
	m_pLevels[m_LevelIndex]->AddObjectToScene(pick, position);
}

void CLevelManager::MoveCamera(vector3df pos)
{
	vector3df move = pos - m_pCamera->getTarget();
	m_pCamera->setTarget(pos);
	m_pCamera->setPosition(m_pCamera->getPosition()+move);
}

void CLevelManager::CreateParticleEffect(PARTICLES_EFFECT_TYPE type, PARTICLES_EFFECT_COLOR color, stringw target, bool follow_player)
{
	m_pLevels[m_LevelIndex]->CreateParticleEffect(type,color,target,follow_player);
}

void CLevelManager::CreateLightNode(bool dancing, s32 radius, s32 target)
{
	m_pLevels[m_LevelIndex]->CreateLightNode(dancing, radius, target);
}