/** 
 * \file Level.cpp
 * \brief File Level.cpp takes care of level structure - it hold together all game objects on the level.
 *
 * takes care of saving/loading level
 * manages game object arrays (adding deleting game objects)
 * translates events to game objects -> on mouse click, on rotate etc..
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "Level.h"
#include "../GameManager.h"

#define PROCEDURAL_TREE_MESH "procedural_tree_mesh"
#define PARTICLE_GAME_OBJECT "particle_game_object_mesh"

CLevel::CLevel()
{
	m_ID = 0;
	dNode = 0;
	m_startPos = vector3df(0,0,0);
	m_MapMusicTheme = "";
	m_MapAmbientSound = "";
	m_MapNarationFile = "";
	firstObstacle = false;
	rootObstacleNode = 0;
	m_LevelMetaTriangleSelector = 0;
	m_ObstacleMetaTriangleSelector = 0;
}

CLevel::~CLevel()
{
	for (u32 i=0; i < m_ListOfMonsters.size(); i++)
	{
		if(m_ListOfMonsters[i] != 0)
		{
			delete m_ListOfMonsters[i];
		}
	}

	m_ListOfMonsters.clear();
	m_ListOfActuators.clear();
	m_ListOfGameObjects.clear();

	if(m_LevelMetaTriangleSelector != 0)
	{
		m_LevelMetaTriangleSelector->drop();
		m_LevelMetaTriangleSelector = 0;
	}
	if(m_ObstacleMetaTriangleSelector != 0)
	{
		m_ObstacleMetaTriangleSelector->drop();	
		m_ObstacleMetaTriangleSelector = 0;
	}
}

bool CLevel::Init(CGameManager* gm)
{
	m_ID = 0;
	m_pFS = gm->getFS();
	m_pDriver = gm->getDriver();
	m_SMGR = gm->getSceneMngr();
	m_pDevice = gm->getDevice();
	m_GameManager = gm;

	firstObstacle = false;
	rootObstacleNode = 0;

	m_ListOfMonsters.clear();
	m_ListOfActuators.clear();

	return true;
}

/**
 * \brief Returns player start position if flagged on the map.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
vector3df CLevel::GetStartPosition()
{
	return m_startPos;
}

stringw CLevel::GetRootFromPath(stringw path)
{
	s32 position = path.findLastChar(L"/",1);
	stringc filename = path.subString(position+1,path.size()-position);
	position = filename.findLastChar(".",1);
	stringw _root = filename.subString(0,position);
	return _root;
}

void CLevel::Save(stringc map)
{
	stringc workingDirectory = m_pDevice->getFileSystem()->getWorkingDirectory();
	printf("%s",workingDirectory.c_str());

	IXMLWriter* writer = m_pFS->createXMLWriter(map.c_str());
	writer->writeXMLHeader();
	WriteSceneNode(writer, m_SMGR->getRootSceneNode());
	writer->drop();
}

/**
 * \brief Writes a scene node attributes to xml file.
 *
 * And calls itself recursivly.
 * Optimizations can be made to reduce file size:
 * Default attributes (eg. Scale: 1,1,1) can be skipped in saving process, and loaded only if
 * different from default (1,1,1).
 *
 * \author Petar Bajic 
 * \date January, 21 2010.
 */
void CLevel::WriteSceneNode(IXMLWriter* writer, ISceneNode* node)
{
	if (!writer || !node)
		return;

	if (node == m_SMGR->getRootSceneNode())
	{
		writer->writeElement(L"LevelEditorMap", false);
		writer->writeLineBreak();
		writer->writeLineBreak();

		
		//Save Map Sound and Music
		writer->writeElement(L"MapAmbientSound", true, L"filename", stringw(m_MapAmbientSound).c_str());
		writer->writeLineBreak();
		writer->writeElement(L"MapMusicTheme", true, L"filename", stringw(m_MapMusicTheme).c_str());
		writer->writeLineBreak();
		writer->writeElement(L"MapNarationFile", true, L"filename", stringw(m_MapNarationFile).c_str());
		writer->writeLineBreak();
		writer->writeLineBreak();
	}
	else if (node->getID() > -1)
	{
		CGameObject* gameObject = getGameObjectFromID(node->getID());

		if(gameObject)
		{
			writer->writeElement(L"GameObject", false);
			writer->writeLineBreak();

			// write properties
			io::IAttributes* attr = m_pFS->createEmptyAttributes(m_pDriver);
			attr->addString("Name",node->getName());
			attr->addInt("ID",node->getID());
			attr->addString("Mesh",gameObject->mesh.c_str());
			attr->addVector3d("Position",node->getPosition());
			attr->addVector3d("Rotation",node->getRotation());
			attr->addVector3d("Scale",node->getScale());
			if (gameObject->isInvisible)
				//adding attribute isInvisible
				attr->addBool("isInvisible",true);
			if (gameObject->isIllusion)
				//adding attribute isInvisible
				attr->addBool("isIllusion",true);
			if (gameObject->isTrigger)
				//adding atribute isTrigger
				attr->addBool("isTrigger",true);
			if (gameObject->isStatic)
				//adding atribute isStatic
				attr->addBool("isStatic",true);
			if (gameObject->isTerrain)
				//adding atribute isTerrain
				attr->addBool("isTerrain",true);
			if (gameObject->isPickable)
				//adding atribute isPickable
				attr->addBool("isPickable",true);
			if (gameObject->isMonster)
			{
				//adding atribute isMonster
				attr->addBool("isMonster",true);
				//attr->addInt("Health",gameObject->hp);
			}
			if (gameObject->isNPC)
				//adding atribute isNPC
				attr->addBool("isNPC",true);
			if (gameObject->isContainer)
				//adding attribute isContainer
				attr->addBool("isContainer",true);
			if (gameObject->isArea)
				//adding attribute isArea
				attr->addBool("isArea",true);
			if (gameObject->isAnchored)
				//adding attribute isAnchored
				attr->addBool("isAnchored",true);
			if (gameObject->script != stringw(L""))
				attr->addString("Script",gameObject->script.c_str());
			if (gameObject->state != stringw(L""))
				attr->addString("State",gameObject->state.c_str());

			if (gameObject->isMonster)
			{
				//Monster attributes are written only if differ from default values in xml
				if (gameObject->m_Mood != gameObject->m_Mood_Default)
					attr->addInt("Mood", gameObject->m_Mood);
				if (gameObject->m_Radius != gameObject->m_Radius_Default)
					attr->addFloat("Radius", gameObject->m_Radius);
			}

			attr->write(writer);
			//writer->writeLineBreak();
			attr->drop();

			//Monster attributes are written only if differ from xml default values
			if (gameObject->isMonster)
			{
				for(u32 i=0; i<gameObject->m_ListOfAbilities.size(); i++)
				{
					s32 value = gameObject->m_ListOfAbilities[i].value;
					s32 max = gameObject->m_ListOfAbilities[i].max;
					s32 defaultValue = gameObject->m_ListOfAbilities_Default[i].value;
					if(value != defaultValue)
					{
						writer->writeElement(L"Ability",true,L"name",gameObject->m_ListOfAbilities[i].name.c_str(), L"value",stringw(value).c_str(),L"max",stringw((value>max)?value:max).c_str());
						writer->writeLineBreak();
					}
				}

				for(u32 i=0; i<gameObject->m_ListOfSkills.size(); i++)
				{
					s32 min = gameObject->m_ListOfSkills[i].min;
					s32 defaultMin = gameObject->m_ListOfSkills_Default[i].min;
					s32 max = gameObject->m_ListOfSkills[i].max;
					s32 defaultMax = gameObject->m_ListOfSkills_Default[i].max;
					if((min != defaultMin) || (max != defaultMax))
					{
						writer->writeElement(L"Skill",true,L"name",gameObject->m_ListOfSkills[i].name.c_str(), L"min",stringw(min).c_str(),L"max",stringw(max).c_str());
						writer->writeLineBreak();
					}
				}
			}
			
			//Write container content
			if (gameObject->isContainer)
			{
				writer->writeElement(L"Container",false); 
				writer->writeLineBreak();
				for ( s32 index = 0; index < gameObject->GetNumberOfPickableItems(); index++)
				{
					writer->writeElement(L"Pickables",true,L"root",gameObject->GetPickableItemRoot(index).c_str(), L"id", stringw(gameObject->GetPickableItemID(index)).c_str());
					writer->writeLineBreak();
				}
				writer->writeClosingTag(L"Container"); 
				writer->writeLineBreak();
			}
		}
	}

	// write children

	core::list<ISceneNode*>::ConstIterator it = node->getChildren().begin();
	for (; it != node->getChildren().end(); ++it)
		WriteSceneNode(writer, (*it));

	if ((node == m_SMGR->getRootSceneNode())||(node->getID() > -1))
	{
		if (node == m_SMGR->getRootSceneNode())
		{
			writer->writeClosingTag(L"LevelEditorMap");
			writer->writeLineBreak();
		}
		else
		{
			CGameObject* tempGameObject = getGameObjectFromID(node->getID());
			if(tempGameObject)
			{
				writer->writeClosingTag(L"GameObject");
				writer->writeLineBreak();
				writer->writeLineBreak();
			}
		}
	}
}


/**
 * \brief Reads a scene nodes from xml file.
 *
 * \author Petar Bajic 
 * \date May, 21 2009.
 */
void CLevel::ReadSceneNode(IXMLReader* reader)
{
	CGameObject* gameObject = 0;
	stringw rootName;

	//loop through scene nodes in xml
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				if (stringw("attributes") == reader->getNodeName())
				{
					//load node attributes
					IAttributes* attr = m_pFS->createEmptyAttributes(m_pDriver);
					attr->read(reader);
					//get 3d mesh first
					ISceneNode* node = NULL;
					stringc meshPath = attr->getAttributeAsString("Path");
					meshPath += attr->getAttributeAsString("Mesh");

					if(meshPath == stringc(PROCEDURAL_TREE_MESH))
					{
						//recreate procedural mesh here
						//node = createTree(getProceduralTreeTypeFromName(attr->getAttributeAsString("Name")));
					}
					else if(meshPath == stringc(PARTICLE_GAME_OBJECT))
					{
						//recreate particle system here
						// create a particle system
						scene::IParticleSystemSceneNode* ps = m_SMGR->addParticleSystemSceneNode(false);

						scene::IParticleEmitter* em = ps->createBoxEmitter(
							core::aabbox3d<f32>(-20,-2,-20,20,20,20), // emitter size
							core::vector3df(0.0f,0.01f,0.0f),   // initial direction
							80,100,                             // emit rate
							video::SColor(0,55,55,55),       // darkest color
							video::SColor(0,255,255,255),       // brightest color
							3000,12000,0,                         // min and max age, angle
							core::dimension2df(5.f,5.f),         // min size
							core::dimension2df(15.f,15.f));        // max size

						ps->setEmitter(em); // this grabs the emitter
						em->drop(); // so we can drop it here without deleting it

						scene::IParticleAffector* paf = ps->createFadeOutParticleAffector();

						ps->addAffector(paf); // same goes for the affector
						paf->drop();

						gameObject = new CGameObject();

						ps->setMaterialFlag(video::EMF_LIGHTING, false);
						ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
						ps->setMaterialTexture(0, m_pDriver->getTexture("media/particle1.bmp"));
						ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

						node = ps;
					}
					else
					{
						IAnimatedMesh* m = m_SMGR->getMesh(meshPath.c_str());
						if (m)
						{
							//adding new game object to the map
							s32 position = meshPath.findLastChar(".",1);
							stringw _root = meshPath.subString(0,position);
							stringw xmlProperties = _root + L".xml";
							IXMLReader* xml = m_pFS->createXMLReader(stringc(xmlProperties.c_str()).c_str());
							//Create GO
							gameObject = new CGameObject(xml,m_pDriver);
							gameObject->root = _root;
							node = m_SMGR->addAnimatedMeshSceneNode(m);
							node->setMaterialFlag(EMF_LIGHTING, false);
							((IAnimatedMeshSceneNode*)node)->setAnimationSpeed(10);

							// !!?
							//ITriangleSelector *selector = m_SMGR->createTriangleSelector(((IAnimatedMeshSceneNode*)node)->getMesh(), node);
							ITriangleSelector *selector = m_SMGR->createTriangleSelector((IAnimatedMeshSceneNode*)node);
							node->setTriangleSelector(selector);
							//selector->drop();
						}
					}

					node->setName(attr->getAttributeAsString("Name"));
					node->setID(attr->getAttributeAsInt("ID"));
					node->setPosition(attr->getAttributeAsVector3d("Position"));
					node->setRotation(attr->getAttributeAsVector3d("Rotation"));
					node->setScale(attr->getAttributeAsVector3d("Scale"));
					node->setVisible(!attr->getAttributeAsBool("isInvisible"));
						
					gameObject->name = node->getName();
					gameObject->id = node->getID();
					gameObject->mesh = meshPath;
					gameObject->pos = node->getPosition();
					gameObject->rot = node->getRotation();
					gameObject->scale = node->getScale();
					gameObject->script = attr->getAttributeAsStringW("Script");
					gameObject->state = attr->getAttributeAsStringW("State");

					if(stringc(node->getName()) == "Start Flag")
					{
						//record start flag position
						m_startPos = node->getPosition();
						m_startPos.Y += 10; //Above the ground, so that it falls down easily like a feather...
					}

					gameObject->isContainer = attr->getAttributeAsBool("isContainer");
					gameObject->isAnchored = attr->getAttributeAsBool("isAnchored");
					gameObject->isNPC = attr->getAttributeAsBool("isNPC");
					gameObject->isMonster = attr->getAttributeAsBool("isMonster");
					gameObject->isPickable = attr->getAttributeAsBool("isPickable");
					gameObject->isTrigger = attr->getAttributeAsBool("isTrigger");
					gameObject->isStatic = attr->getAttributeAsBool("isStatic");
					gameObject->isTerrain = attr->getAttributeAsBool("isTerrain");
					gameObject->isInvisible = attr->getAttributeAsBool("isInvisible");
					gameObject->isIllusion = attr->getAttributeAsBool("isIllusion");
					gameObject->isArea = attr->getAttributeAsBool("isArea");

					if(attr->existsAttribute("Mood"))
						gameObject->m_Mood = (eMood) attr->getAttributeAsInt("Mood");
					if(attr->existsAttribute("Radius"))
						gameObject->m_Radius = attr->getAttributeAsFloat("Radius");
	
					/*if(gameObject->isTerrain)
					{
						node->setMaterialTexture(0,m_pDriver->getTexture("media/terrain/stones.jpg"));
						node->setMaterialType(EMT_SOLID);
						//node->setMaterialFlag(EMF_WIREFRAME, true);
					}*/

					if(!gameObject->isInvisible)
					{
						m_LevelMetaTriangleSelector->addTriangleSelector(node->getTriangleSelector());
						if(!gameObject->isPickable && !gameObject->isMonster && !gameObject->isNPC && !gameObject->isInvisible  && !gameObject->isIllusion)
						{
							m_ObstacleMetaTriangleSelector->addTriangleSelector(node->getTriangleSelector());
							//add obstacles to children of the root obstacle node (used in collision optimization)
							/*if(firstObstacle)
							{
								rootObstacleNode = node;
								firstObstacle = false;
							}
							else if(rootObstacleNode != 0)
							{
								rootObstacleNode->addChild(node);
								//pos and scale gets fucked up
								//node->setPosition(node->getPosition()-rootObstacleNode->getPosition());
								//node->setScale(node->getScale()*vector3df(1/rootObstacleNode->getScale().X,1/rootObstacleNode->getScale().Y,1/rootObstacleNode->getScale().Z));
							}
							else
							{
								debugPrint("Bad situation in obstacle node linking. First obstacle false, but root obstacle node still null?\n");
							}*/
						}
					}

					if(gameObject->isMonster)
					{
						//Try getting HP from saved Map
						/*if(attr->existsAttribute("Health"))
						{
							gameObject->hp = attr->getAttributeAsInt("Health");
						}
						else
						{
							//If there is no Health attribute stored in the map, take value from xml ability
							gameObject->hp = gameObject->getAbilityValue("Health");
						}*/
						CMonster* monster = new CMonster();
						//Init Monster
						monster->Init(m_SMGR,(IAnimatedMeshSceneNode*)node, gameObject, m_GameManager);
						m_ListOfMonsters.push_back(monster);
					}

					if((gameObject->isTrigger)&&(gameObject->state.size() == 0))
					{
						//load default state if state is not loaded from map file
						stringc temp = m_pDevice->getFileSystem()->getWorkingDirectory();
						m_pDevice->getFileSystem()->changeWorkingDirectoryTo("media/Scripts/Static");
						io::IXMLReader* xml = m_pDevice->getFileSystem()->createXMLReader(gameObject->script.c_str());
						while(xml && xml->read())
						{
							switch(xml->getNodeType())
							{
							case io::EXN_ELEMENT:
								{
									if (stringw("State") == xml->getNodeName())
									{
										gameObject->state = xml->getAttributeValue(L"value");
									}
								}
								break;
							}
						}
						if (xml)
							xml->drop(); // don't forget to delete the xml reader
						m_pDevice->getFileSystem()->changeWorkingDirectoryTo(temp.c_str());
					}

					if(gameObject->isPickable)
					{
						rootName = GetRootFromPath(meshPath);
						stringw icon = rootName + ".png";
						gameObject->m_IconTexture = m_pDriver->getTexture(stringw(L"media/icons/") + icon);
					}

					if(gameObject->isAnimated)
					{
						//set model to idle animation
						((IAnimatedMeshSceneNode*)node)->setFrameLoop(gameObject->getAnimStart("Idle"),gameObject->getAnimEnd("Idle"));
					}
					else
					{
						//Some special cases are not IAnimatedMeshSceneNode, like particles...
						if(!meshPath.equals_ignore_case(PARTICLE_GAME_OBJECT))
						{
							((IAnimatedMeshSceneNode*)node)->setFrameLoop(0,0);
							((IAnimatedMeshSceneNode*)node)->setLoopMode(false);
						}
					}

					if((gameObject->isNPC || gameObject->isContainer || gameObject->isPickable || gameObject->isTrigger)&&(!gameObject->isMonster))
					{
						m_ListOfActuators.push_back(node);
						gameObject->isActuator = true;
					}

					if(gameObject->isArea)
					{
						TArea* area = new TArea;
						area->entered = false;
						area->node = node;
						area->radius = 30;
						m_ListOfAreas.push_back(area);
					}

					m_ListOfGameObjects.push_back(gameObject);
					
				}

				//Loading monster abilities and skills (those that differ from xml default ones)
				if (stringw("Ability") == reader->getNodeName())
				{
					stringc name = reader->getAttributeValue(L"name");
					s32 value = reader->getAttributeValueAsInt(L"value");
					s32 max = reader->getAttributeValueAsInt(L"max");
					s32 min = reader->getAttributeValueAsInt(L"min");
					if(gameObject!=0) gameObject->setAbilityValues(name,value,min,max);
				}
				if (stringw("Skill") == reader->getNodeName())
				{
					stringc name = reader->getAttributeValue(L"name");
					s32 min = reader->getAttributeValueAsInt(L"min");
					s32 max = reader->getAttributeValueAsInt(L"max");
					if(gameObject!=0) gameObject->setSkillValues(name,min,max);
				}

				//Load container content from xml file to GameObjects list of pickable items
				if (stringw("Pickables") == reader->getNodeName())
				{
					//Add pickable game object (PGO) to container game object (CGO)
					stringc pickRoot = reader->getAttributeValue(L"root");
					stringc idStr = reader->getAttributeValue(L"id");
					s32 id = atoi(idStr.c_str());
					stringw xmlProperties = pickRoot + L".xml";
					IXMLReader* xml = m_pFS->createXMLReader(stringc(xmlProperties.c_str()).c_str());
					//Create pickable GO and add it to container GO
					CGameObject* pickGO = new CGameObject(pickRoot,id,xml,m_pDriver);
					pickGO->isPickable = true;
					if(gameObject!=0) gameObject->AddPickableItem(pickGO);
				}

				//Load Map Sound and Music
				if (stringw("MapAmbientSound") == reader->getNodeName())
				{
					m_MapAmbientSound = reader->getAttributeValue(L"filename");
					m_MapAmbientSoundLoop = true; //could be read from map, but no need right now
				}
				if (stringw("MapMusicTheme") == reader->getNodeName())
				{
					m_MapMusicTheme = reader->getAttributeValue(L"filename");
					m_MapMusicThemeLoop = true; //reader->getAttributeAsBool(L"loop");
				}
				if (stringw("MapNarationFile") == reader->getNodeName())
				{
					m_MapNarationFile = reader->getAttributeValue(L"filename");
					//m_MapNarationFileLoop = false; //reader->getAttributeAsBool(L"loop");
				}
			}
			break;
		}
	}
}

void CLevel::CleanUp()
{
	for (u32 i=0; i < m_ListOfMonsters.size(); i++)
	{
		if(m_ListOfMonsters[i] != 0)
		{
			delete m_ListOfMonsters[i];
		}
	}

	m_ListOfGameObjects.clear();
	m_ListOfMonsters.clear();
	m_ListOfActuators.clear();

	if(m_LevelMetaTriangleSelector != 0) 
	{
		m_LevelMetaTriangleSelector->drop();
		m_LevelMetaTriangleSelector = 0;
	}
	if(m_ObstacleMetaTriangleSelector != 0)
	{
		m_ObstacleMetaTriangleSelector->drop();
		m_ObstacleMetaTriangleSelector = 0;
	}

	m_SMGR->clear();
}

bool CLevel::Load(stringc map_filename)
{
	m_ID = 0; //cleaning old map

	//CleanUp();

	//m_ListOfGameObjects.clear();
	//m_ListOfMonsters.clear();
	//m_ListOfActuators.clear();
	//m_SMGR->clear();
	m_MapName = map_filename.subString(map_filename.findLast('/')+1,map_filename.size());

	stringc workingDirectory = m_pDevice->getFileSystem()->getWorkingDirectory();
	printf("%s",workingDirectory.c_str());

	m_LevelMetaTriangleSelector = m_SMGR->createMetaTriangleSelector();
	m_ObstacleMetaTriangleSelector = m_SMGR->createMetaTriangleSelector();
	IXMLReader* reader = m_pFS->createXMLReader(map_filename.c_str());
	if(reader)
	{
		ReadSceneNode(reader);
		reader->drop();
		return true;
	}

	return false;
}

void CLevel::DeleteActuatorFromQueue(s32 id)
{
	for (u32 i=0; i < m_ListOfActuators.size(); i++)
	{
		if(m_ListOfActuators[i]->getID() == id)
		{
			m_ListOfActuators.erase(i);
			return;
		}
	}
}

vector3df CLevel::getMonsterHealthBarPos(s32 id)
{
	for (u32 i=0; i < m_ListOfMonsters.size(); i++)
	{
		if(m_ListOfMonsters[i]->m_ID == id)
		{
			CMonster* monster = m_ListOfMonsters[i];
			vector3df pos = monster->m_Node->getPosition() + vector3df(0,monster->m_maxY+10.0f,0);
			return pos;
		}
	}
	return vector3df(0,0,0);
}

void CLevel::DeleteMonsterFromQueue(s32 id)
{
	for (u32 i=0; i < m_ListOfMonsters.size(); i++)
	{
		if(m_ListOfMonsters[i]->m_ID == id)
		{
			CMonster* monster = m_ListOfMonsters[i];
			monster->CleanAfterKilledMonster();
			m_ListOfMonsters.erase(i);
			delete monster;
			return;
		}
	}
}

void CLevel::NotifyMonsterOfAttack(s32 id)
{
	for (u32 i=0; i < m_ListOfMonsters.size(); i++)
	{
		if(m_ListOfMonsters[i]->m_ID == id)
		{
			m_ListOfMonsters[i]->NotifyOfAttack();
			return;
		}
	}
}

void CLevel::UpdateMonsterHealthBar(s32 id, s32 value)
{
	for (u32 i=0; i < m_ListOfMonsters.size(); i++)
	{
		if(m_ListOfMonsters[i]->m_ID == id)
		{
			if(m_ListOfMonsters[i]->healthBar != 0)
			{
				m_ListOfMonsters[i]->healthBar->changeValueBy(value);
			}
			return;
		}
	}
}

void CLevel::UpdateMonsters(IVideoDriver* driver, f32 elapsed_time, CPlayerCharacter* pc, IGUIFont* font, ICameraSceneNode* cam)
{
	for (u32 i=0; i < m_ListOfMonsters.size(); i++)
	{
		m_ListOfMonsters[i]->update(driver, elapsed_time, pc, font, cam);
		//If distance from pc < 10
		//Move toward PC
		//If distance from PC < 4
		//If not attacking
		//Attack
		
		//Else, idle around

	}
}

void CLevel::UpdateTranslateGameObject(f32 elapsed_time)
{
	for(u32 i=0; i < m_TranslateGameObject.size(); i++)
	{
		f32 currentTime = (f32)m_pDevice->getTimer()->getTime();
		debugPrint("Translating object. id:%d, currentTime:%f, endTime:%f\n", m_TranslateGameObject[i]->node->getID(), currentTime, m_TranslateGameObject[i]->endTime);
		if(currentTime < m_TranslateGameObject[i]->endTime)
		{
			f32 newXPos = m_TranslateGameObject[i]->node->getPosition().X + m_TranslateGameObject[i]->Distance.X*elapsed_time*1000.f/m_TranslateGameObject[i]->translationTime;
			f32 newYPos = m_TranslateGameObject[i]->node->getPosition().Y + m_TranslateGameObject[i]->Distance.Y*elapsed_time*1000.f/m_TranslateGameObject[i]->translationTime;
			f32 newZPos = m_TranslateGameObject[i]->node->getPosition().Z + m_TranslateGameObject[i]->Distance.Z*elapsed_time*1000.f/m_TranslateGameObject[i]->translationTime;
			m_TranslateGameObject[i]->node->setPosition(vector3df(newXPos, newYPos, newZPos));
			debugPrint("Translating object. x:%f, y:%f, z:%f\n",newXPos, newYPos, newZPos);
		}
		else
		{
			if(m_TranslateGameObject[i]->node == m_GameManager->getPC()->getNode())
			{
				m_GameManager->getPC()->m_bTranslated = false;
			}
			m_TranslateGameObject.erase(i);
			i--;
		}
	}
}

void CLevel::AddTranslateGameObject(ISceneNode* node, vector3df translationVectorEndPosition, u32 translationTime)
{
	//TODO: If node not already being translated!
	TranslateGameObject* newTranslator = new TranslateGameObject;
	newTranslator->node = node;
	newTranslator->Distance = translationVectorEndPosition - node->getPosition();
	newTranslator->translationTime = (f32) translationTime;
	newTranslator->endTime = (f32) (m_pDevice->getTimer()->getTime() + translationTime);
	debugPrint("AddTranslateGameObject. Distance.X:%f, Distance.Y:%f, Distance.Z:%f\n",newTranslator->Distance.X, newTranslator->Distance.Y, newTranslator->Distance.Z);
	m_TranslateGameObject.push_back(newTranslator);
}

bool CLevel::EraseElement(int id)
{
	list<CGameObject*>::Iterator it = m_ListOfGameObjects.begin();
	
	for (; it != m_ListOfGameObjects.end(); ++it)
	{
		if((*it)->id == id)
		{
			m_ListOfGameObjects.erase(it);
			return true;
		}
	}

	return false;
}

//used only for pickables now!! double check for other uses...
void CLevel::AddObjectToScene(CGameObject* pick, vector3df position)
{
	CGameObject* go = getGameObjectFromID(pick->id);
	if(go)
	{
		//error! There already exists go with this ID! Unexpected error! Abort! Abort!
		return;
	}
	m_ListOfGameObjects.push_back(pick);
	
	//add to scene
	IAnimatedMesh* m = m_SMGR->getMesh(pick->mesh.c_str());
	if (m)
	{
		//adding new game object to the map
		ISceneNode* node = m_SMGR->addAnimatedMeshSceneNode(m);
		node->setMaterialFlag(EMF_LIGHTING, false);
		((IAnimatedMeshSceneNode*)node)->setAnimationSpeed(10);

		// !!?
		ITriangleSelector *selector = m_SMGR->createTriangleSelector(((IAnimatedMeshSceneNode*)node)->getMesh(), node);
		node->setTriangleSelector(selector);
		m_LevelMetaTriangleSelector->addTriangleSelector(selector);
		selector->drop();

		node->setName(pick->name);
		node->setID(pick->id);
		node->setPosition(position);
		node->setRotation(pick->rot);
		node->setScale(pick->scale);

		pick->pos = position;

		m_ListOfActuators.push_back(node);
	}
}

CGameObject* CLevel::getGameObjectFromID(int id)
{
	list<CGameObject*>::Iterator it = m_ListOfGameObjects.begin();
	
	for (; it != m_ListOfGameObjects.end(); ++it)
	{
		if((*it)->id == id)
		{
			return *it;
		}
	}

	return 0;
}

void  CLevel::RemoveContainerContent(int containerID)
{
	//m_ListOfGameObjects[containerID]->ClearPickableItems();
	getGameObjectFromID(containerID)->ClearPickableItems();
}

void CLevel::RemoveContainerContent(s32 containerID, s32 itemID)
{
	getGameObjectFromID(containerID)->RemovePickableItem(itemID);
}

void CLevel::AddContainerItem(int containerID, CGameObject* pick)
{
	//m_ListOfGameObjects[containerID]->AddPickableItem(itemName.c_str());
	getGameObjectFromID(containerID)->AddPickableItem(pick);
}

int CLevel::GetContainerNumberOfItems(int containerID)
{
	//return m_ListOfGameObjects[containerID]->GetNumberOfPickableItems();
	return getGameObjectFromID(containerID)->GetNumberOfPickableItems();
}

CGameObject* CLevel::GetContainerItem(int containerID, int index)
{
	return getGameObjectFromID(containerID)->GetPickableItem(index);
}

/**
 * \brief Adds triangle selector to the terrain (used on reloading map)
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CLevel::addTerrainSelector()
{
	ITerrainSceneNode* terrain = (ITerrainSceneNode*) m_SMGR->getSceneNodeFromType(ESNT_TERRAIN);
	scene::ITriangleSelector* selector = m_SMGR->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(selector);
	selector->drop();
}

CGameObject* CLevel::getClickedGameObject(position2d<s32> mousePos)
{
	ISceneNode* node = m_SMGR->getSceneCollisionManager()->getSceneNodeFromScreenCoordinatesBB(mousePos);
	if(node)
	{
		CGameObject* go = getGameObjectFromID(node->getID());
		return go;
	}
	return 0;
}

/*ISceneNode* CLevel::isActionItemUnderMousePointer()
{
	position2d<s32> mousePos = m_GameManager->getDevice()->getCursorControl()->getPosition();
	ISceneNode* node = m_SMGR->getSceneCollisionManager()->getSceneNodeFromScreenCoordinatesBB(mousePos);
	if(node)
	{
		CGameObject* go = getGameObjectFromID(node->getID());
		if(go)
			if(go->isContainer || go->isPickable || go->isTrigger || go->isNPC)
				return node;
	}

	return 0;
}*/

bool CLevel::isActuator(ISceneNode* node)
{
	for(u32 i=0; i < m_ListOfActuators.size(); i++)
	{
		ISceneNode* temp = (ISceneNode*)m_ListOfActuators[i];
		if (temp->getID() == node->getID())
			return true;
	}

	return false;
}






/**
 * \brief Checks if Action item (Actuator) is under mouse pointer.
 * Old version of this function used bounding box method. We check actuators from queue now.
 */
ISceneNode* CLevel::isActionItemUnderMousePointer()
{
	line3d<f32> picking_line = m_GameManager->getSceneMngr()->getSceneCollisionManager()->getRayFromScreenCoordinates(m_GameManager->getDevice()->getCursorControl()->getPosition());
	vector3df point;
	triangle3df triangle;
	ISceneNode* pickedNode = m_GameManager->getSceneMngr()->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(picking_line, point, triangle, 0, 0, false);
	if(pickedNode)
	{
		for(u32 i=0; i < m_ListOfActuators.size(); i++)
		{
			ISceneNode* node = (ISceneNode*)m_ListOfActuators[i];
			if(node == pickedNode)
			{
				//node->setDebugDataVisible(EDS_BBOX);
				//dWorldBox = ((IAnimatedMeshSceneNode*)node)->getMesh()->getBoundingBox();
				//dWorldBox = ((IAnimatedMeshSceneNode*)node)->getTransformedBoundingBox();
				//dNode = (IAnimatedMeshSceneNode*)node;
				//((IAnimatedMeshSceneNode*)node)->getAbsoluteTransformation().transformBoxEx(dWorldBox);
				return node;
			}
		}
	}

	return 0;
}

/*
	Old version of this function used bounding box method. We check monsters from queue now.
*/
ISceneNode* CLevel::isMonsterUnderMousePointer()
{
	line3d<f32> picking_line = m_GameManager->getSceneMngr()->getSceneCollisionManager()->getRayFromScreenCoordinates(m_GameManager->getDevice()->getCursorControl()->getPosition());
	for(u32 i=0; i < m_ListOfMonsters.size(); i++)
	{
		//debugPrint("GetClickedMonster: %d\n",i);
		ISceneNode* node = (ISceneNode*)m_ListOfMonsters[i]->getNode();
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
			ISceneNode* obstacle = m_GameManager->getSceneMngr()->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(picking_line, point, triangle, 0, rootObstacleNode, false);
			if(obstacle)
			{
				if(obstacle != node)
				{
					if(point.getDistanceFrom(m_GameManager->getLevelManager()->m_pCamera->getPosition()) < node->getPosition().getDistanceFrom(m_GameManager->getLevelManager()->m_pCamera->getPosition()))
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


bool CLevel::isObjectPickable(int id)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		return go->isPickable;
	}
	printf("ERROR! isObjectPickable: Object with id: %d doesn't exist!",id);
	return false;
}

bool CLevel::isObjectContainer(int id)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		return go->isContainer;
	}
	printf("ERROR! isObjectContainer: Object with id: %d doesn't exist!",id);
	return false;
}

bool CLevel::isObjectTrigger(int id)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		return go->isTrigger;
	}
	printf("ERROR! isObjectTrigger: Object with id: %d doesn't exist!",id);
	return false;
}

bool CLevel::isObjectMonster(int id)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		return go->isMonster;
	}
	printf("ERROR! isObjectMonster: Object with id: %d doesn't exist!",id);
	return false;
}

bool CLevel::isObjectNPC(int id)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		return go->isNPC;
	}
	printf("ERROR! isObjectNPC: Object with id: %d doesn't exist!",id);
	return false;
}

stringw CLevel::GetObjectScript(int id)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		return go->script;
	}
	printf("ERROR! GetObjectScript: Object with id: %d doesn't exist!",id);
	return "";
}

void CLevel::SetObjectState(int id, stringw state)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		go->state = state;
		return;
	}
	printf("ERROR! SetObjectState: Object with id: %d doesn't exist!",id);
}

stringw CLevel::GetObjectState(int id)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		return go->state;
	}
	printf("ERROR! GetObjectState: Object with id: %d doesn't exist!",id);
	return "";
}

vector3df CLevel::GetObjectPosition(int id)
{
	CGameObject* go = getGameObjectFromID(id);
	if(go)
	{
		return go->pos;
	}
	printf("ERROR! GetObjectPosition: Object with id: %d doesn't exist!",id);
	return vector3df(0,10,0);
}

void CLevel::CreateParticleEffect(PARTICLES_EFFECT_TYPE type, PARTICLES_EFFECT_COLOR color, stringw target, bool follow_player)
{
	//m_pLevels[m_LevelIndex]->CreateParticleEffect(type,color,target,follow_player);

	// create a particle system
	IParticleSystemSceneNode* ps = m_SMGR->addParticleSystemSceneNode(false);
	IParticleEmitter* em = 0;

	vector3df particle_position;

	if(follow_player)
	{
		particle_position = m_GameManager->getPC()->getPosition();
	}
	else
	{
		s32 targetID = 0;
		swscanf_s(target.c_str(), L"%d", &targetID);
		particle_position = GetObjectPosition(targetID);
	}

	switch(type)
	{
	case PARTICLES_EFFECT_WHIRL:
		{
			vector3df normal = vector3df(0.f,3.1f,0.f);
			vector3df relative_position = vector3df(0.f,0.f,0.f);
			em = ps->createCylinderEmitter(relative_position, 5, normal, 4, true, vector3df(0.f,0.01f,0.f), 50, 100, 
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					1000,3000,0,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
	}

/*
		em = ps->createBoxEmitter(emiterSize, direction, emitRateMin, emitRateMax,
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	case E_EMITERTYPE_RING:
		{
			em = ps->createRingEmitter(emiterSize.getCenter(), emiterSize.getExtent().X, emiterSize.getExtent().Y, direction, emitRateMin, emitRateMax,
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	case E_EMITERTYPE_SPHERE:
		{
			em = ps->createSphereEmitter(emiterSize.getCenter(), emiterSize.getExtent().X, direction, emitRateMin, emitRateMax, 
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	case E_EMITERTYPE_CYLINDER:
		{
			vector3df normal = vector3df(10.f,10.f,10.f);
			em = ps->createCylinderEmitter(emiterSize.getCenter(), emiterSize.getExtent().X, normal, emiterSize.getExtent().Y, outlineOnly, direction, emitRateMin, emitRateMax, 
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	case E_EMITERTYPE_POINT:
		{
			em = ps->createPointEmitter(direction, emitRateMin, emitRateMax,
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	default:
		{
			em = ps->createBoxEmitter(emiterSize, direction, emitRateMin, emitRateMax,
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
	}
*/

	if(em == 0)
	{
		//Abort! 
	}

	ps->setEmitter(em); // this grabs the emitter
	em->drop(); // so we can drop it here without deleting it

	IParticleAffector* paf = ps->createFadeOutParticleAffector();
	ps->addAffector(paf); // same goes for the affector
	paf->drop();

	ps->setPosition(particle_position);

	paf = ps->createRotationAffector(vector3df(0.0f,300.0f,0.0f), particle_position); //rotation afector
	ps->addAffector(paf);
	paf->drop();

	ps->setScale(core::vector3df(2,2,2));
	ps->setMaterialFlag(video::EMF_LIGHTING, false);
	ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	switch(color)
	{
	case PARTICLES_EFFECT_COLOR_RED:
		{
			ps->setMaterialTexture(0, m_pDriver->getTexture("media/particle_red.bmp"));
		}
	case PARTICLES_EFFECT_COLOR_GREEN:
		{
			ps->setMaterialTexture(0, m_pDriver->getTexture("media/particle_green.bmp"));
		}
	}
	//ps->setMaterialTexture(0, m_pDriver->getTexture(texture));
	ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
	//ps->setID(m_EditorManager->m_ID);
	ps->setName(L"ParticleEffect");

	/*CGameObject* gameObject = new CGameObject();
	gameObject->mesh = PARTICLE_GAME_OBJECT;
	gameObject->name = name;
	
	gameObject->isArea = true; //area can detect players presence - used for teleports
	gameObject->id = m_EditorManager->m_ID;
	gameObject->description = L"Particle System";
	gameObject->script = L"";
	m_ListOfGameObjects.push_back(gameObject);*/

}
