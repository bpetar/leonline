/** 
 * \file GameObject.cpp
 * \brief GameObject class is used for storing Game object attributes and data.
 * CGameObject class handles mesh name, id, and parameters like isContainer, isNPC, etc...
 * it also handles list of pickable items (if the object is container) of the object.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "GameObject.h"
#include "../EditorManager.h"
#include "../Utils.h"

CGameObject::CGameObject()
{
	isContainer = false;
	isPickable = false;
	isTrigger = false;
	isStatic = false;
	isTerrain = false;
	isTile = false;
	isWall = false;
	isNPC = false;
	isMonster = false;
	isArea = false;
	isAnchored = false;
	isAnimated = false;
	isInvisible = false;
	isIllusion = false;
	hasTrajectoryPath = false;
	isTrajectoryNode = false;
	m_Radius = 20.0f;
	m_HP = 10;
	m_Exp = 50;
	m_Sound_Ambient = "";
	m_Sound_Attack = "";
	m_Sound_Hit = "";
	m_Sound_Hello = "";
	m_Sound_Wound = "";
	m_Sound_Die = "";
	nameID = 0;
	animations.clear();
	m_ListOfAbilities_Default.clear();
	m_ListOfSkills_Default.clear();
	m_ListOfTrajectoryPaths.clear();
	trajectoryPathFile = "";
	trajectoryParent = NULL;
}
/**
 * Advanced constructor.
 */
CGameObject::CGameObject(stringw _path, stringw _name, bool _static, IVideoDriver* driver)
{
	animations.clear();
	m_ListOfAbilities_Default.clear();
	m_ListOfSkills_Default.clear();
	m_ListOfTrajectoryPaths.clear();
	isAnimated = false;
	name = _name;
	path = _path;
	root = _name;
	id = -1;
	isContainer = false;
	isMonster = false;
	isArea = false;
	isAnchored = false;
	isNPC = false;
	isPickable = false;
	isTrigger = false;
	isInvisible = false;
	isIllusion = false;
	isStatic = false;
	isTerrain = false;
	isTile = false;
	isWall = false;
	hasTrajectoryPath = false;
	isTrajectoryNode = false;
	m_IconTexture = NULL;
	if (_static) isStatic = true;
	description = L"No description specified";
	script = _name + ".script"; //default, but can be different
	icon = _name + ".png"; //default, but can be different
	m_Radius = 20.0f;
	m_HP = 10;
	m_Exp = 50;
	m_Sound_Ambient = "";
	m_Sound_Attack = "";
	m_Sound_Hit = "";
	m_Sound_Hello = "";
	m_Sound_Wound = "";
	m_Sound_Die = "";
	trajectoryPathFile = "";
	m_Driver = driver;
	nameID = 0;
	trajectoryParent = NULL;
}

/**
 * Advanced constructor. Used for pickable items placed in container objects loaded from map files.
 */
CGameObject::CGameObject(stringw _root, s32 _id, IXMLReader* xml, IVideoDriver* driver)
{
	s32 position = _root.findLastChar(L"/",1);
	stringc _name = _root.subString(position+1,_root.size()-position);
	stringc _path = _root.subString(0,position+1);

	animations.clear();
	m_ListOfAbilities_Default.clear();
	m_ListOfSkills_Default.clear();
	m_ListOfTrajectoryPaths.clear();
	isAnimated = false;
	name = _name;
	path = _path;
	root = _name;
	id = _id;
	isContainer = false;
	isMonster = false;
	isAnchored = false;
	isNPC = false;
	isPickable = false;
	isArea = false;
	isTrigger = false;
	isInvisible = false;
	isIllusion = false;
	isStatic = false;
	isTerrain = false;
	isTile = false;
	isWall = false;
	hasTrajectoryPath = false;
	isTrajectoryNode = false;
	trajectoryParent = NULL;
	m_IconTexture = 0;
	description = L"No description specified";
	script = _name + ".script"; //default, but can be different
	icon = _name + ".png"; //default, but can be different
	m_Driver = driver;
	nameID = 0;
	trajectoryPathFile = "";

	if(xml)
	{
		LoadPropertiesFromXMLFile(xml);
		xml->drop();
	}
}

CGameObject::~CGameObject()
{
}

/**
 *
 */
TPath* CGameObject::findNodeTrajectory(s32 id)
{
	for(u32 i=0; i < m_ListOfTrajectoryPaths.size(); i++)
	{
		//usualy there is only one path
		for (u32 j=0; j< m_ListOfTrajectoryPaths[i].nodes.size(); j++)
		{
			if(m_ListOfTrajectoryPaths[i].nodes[j].sceneNode->getID() == id) 
			{
				return &m_ListOfTrajectoryPaths[i];
			}
		}
	}

	return NULL;
}

/**
 *
 *
 */
CGameObject* CGameObject::CreateTrajectoryNodeGO(TPath* trajectoryPath, vector3df position, vector3df rotation, vector3df scale, s32 id, f32 pause, f32 speed, ISceneManager* smgr)
{
	SColor color = SColor(155,10,10,155);
	SColor color2 = SColor(155,10,10,185);
	f32 size = 3;
	TPathNode pathNode;
	pathNode.sceneNode = smgr->addAnimatedMeshSceneNode(smgr->addArrowMesh(TRAJECTORY_NODE_GAME_OBJECT, color, color2, 4, 8, 10.f*size, 6.f*size, 1.f*size, 3.f*size),0,id,position);
	//pathNode.sceneNode->setPosition(position); //q: why is this commented out?? a: i guess its up there in the list of arguments of addArrowMesh
	pathNode.sceneNode->setRotation(rotation);
	pathNode.sceneNode->setScale(scale);
	pathNode.sceneNode->setVisible(false);

	trajectoryPath->nodes.push_back(pathNode);

	CGameObject* pathNodeGO = new CGameObject();
	pathNodeGO->isTrajectoryNode = true;
	pathNodeGO->name = TRAJECTORY_NODE_GAME_OBJECT;
	pathNodeGO->mesh = TRAJECTORY_NODE_GAME_OBJECT;
	//set position, rotation, scale and id
	pathNodeGO->pos = position;
	pathNodeGO->rot = rotation;
	pathNodeGO->scale = scale;
	pathNodeGO->id = pathNode.sceneNode->getID();
	pathNodeGO->trajectoryParent = this;

	return pathNodeGO;
}

void CGameObject::LoadTrajectoryPaths(IXMLReader* xml, ISceneManager* smgr, list <CGameObject*> listOfGameObjects)
{
	bool startStoringPathNodes = false;
	TPath t_path;

	while(xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				if (stringw("Path").equals_ignore_case(xml->getNodeName()))
				{
					startStoringPathNodes = true;
					t_path.name = xml->getAttributeValue(L"name");
					stringw loopStrValue = xml->getAttributeValue(L"loop");
					if(loopStrValue.equals_ignore_case("true")) t_path.loop = true;
					else t_path.loop = false;
					t_path.nodes.clear();
				}
				if(startStoringPathNodes)
				{
					if(stringw("Coord").equals_ignore_case(xml->getNodeName()))
					{
						f32 n_pause = xml->getAttributeValueAsFloat(L"pause");
						f32 n_speed = xml->getAttributeValueAsFloat(L"speed");
						vector3df n_rotation = Util_getVectorFromString(xml->getAttributeValue(L"rotation"));
						vector3df n_position = Util_getVectorFromString(xml->getAttributeValue(L"position"));
						vector3df n_scale = Util_getVectorFromString(xml->getAttributeValue(L"scale"));
						s32 n_id = xml->getAttributeValueAsInt(L"id");
						
						CGameObject* pathNodeGO = CreateTrajectoryNodeGO(&t_path, n_position, n_rotation, n_scale, n_id, n_pause, n_speed, smgr);
						//listOfGameObjects.push_back(pathNodeGO);

					}
				}
			}
			break;

			case io::EXN_ELEMENT_END:
			{
				if (stringw("Path").equals_ignore_case(xml->getNodeName()))
				{
					//Path loaded.
					m_ListOfTrajectoryPaths.push_back(t_path);
					startStoringPathNodes = false;
				}
			}
		}
	}
}

/* 
 * If trajectory path has been edited, it should be saved to path file.
 * 
 */
void CGameObject::SaveTrajectoryPaths(IXMLWriter* writer)
{
	//IXMLWriter* writer = m_EditorManager->getDevice()->getFileSystem()->createXMLWriter(m_MapName.c_str());
	writer->writeXMLHeader(); writer->writeLineBreak();
	writer->writeLineBreak();

	for(u32 i=0; i < m_ListOfTrajectoryPaths.size(); i++)
	{
		//usualy there is only one path
		writer->writeElement(L"Path",false,L"name", m_ListOfTrajectoryPaths[i].name.c_str(), L"loop", m_ListOfTrajectoryPaths[i].loop?L"true":L"false"); writer->writeLineBreak();

		for (u32 j=0; j< m_ListOfTrajectoryPaths[i].nodes.size(); j++)
		{
			array<stringw> names;
			array<stringw> values;
			names.push_back(L"id");
			names.push_back(L"position");
			names.push_back(L"rotation");
			names.push_back(L"scale");
			names.push_back(L"speed");
			names.push_back(L"pause");
			values.push_back(stringw(m_ListOfTrajectoryPaths[i].nodes[j].sceneNode->getID()));
			values.push_back(Util_getStringFromVector(m_ListOfTrajectoryPaths[i].nodes[j].sceneNode->getPosition()));
			values.push_back(Util_getStringFromVector(m_ListOfTrajectoryPaths[i].nodes[j].sceneNode->getRotation()));
			values.push_back(Util_getStringFromVector(m_ListOfTrajectoryPaths[i].nodes[j].sceneNode->getScale()));
			values.push_back(stringw(m_ListOfTrajectoryPaths[i].nodes[j].speed));
			values.push_back(stringw(m_ListOfTrajectoryPaths[i].nodes[j].pause));

			//<Coord id="1030" speed="100" position="1 1 1" rotation="0 0 0" scale="1 1 1" pause="0"/>
			writer->writeElement(L"Coord", true, names, values); writer->writeLineBreak();
			
			/*writer->writeElement(L"Coord", true, L"id", stringw(m_ListOfTrajectoryPaths[i].nodes[j].id).c_str(), L"speed", stringw(m_ListOfTrajectoryPaths[i].nodes[j].speed).c_str(),
								 L"position", Util_getStringFromVector(m_ListOfTrajectoryPaths[i].nodes[j].position).c_str(), L"rotation", Util_getStringFromVector(m_ListOfTrajectoryPaths[i].nodes[j].rotation).c_str(),
								 L"scale", Util_getStringFromVector(m_ListOfTrajectoryPaths[i].nodes[j].scale).c_str(), L"pause", stringw(m_ListOfTrajectoryPaths[i].nodes[j].pause).c_str()); writer->writeLineBreak();*/
		}

		writer->writeClosingTag(L"Path"); writer->writeLineBreak();
		writer->writeLineBreak();
	}
	writer->drop();
}

void CGameObject::LoadPropertiesFromXMLFile(IXMLReader* xml)
{
	bool startStoringAbilities = false;
	bool startStoringSkills = false;
	bool startStoringSounds = false;

	while(xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo;
				if (startStoringAbilities)
				{
					TAbility ability;
					ability.abilityName = xml->getNodeName();
					ability.value = xml->getAttributeValueAsInt(L"value");
					ability.min = xml->getAttributeValueAsInt(L"min");
					ability.max = xml->getAttributeValueAsInt(L"max");
					m_ListOfAbilities.push_back(ability);
					m_ListOfAbilities_Default.push_back(ability);
				}
				else if (startStoringSkills)
				{
					TSkill skill;
					skill.skillName = xml->getNodeName();
					skill.min = xml->getAttributeValueAsInt(L"min");
					skill.max = xml->getAttributeValueAsInt(L"max");
					m_ListOfSkills.push_back(skill);
					m_ListOfSkills_Default.push_back(skill);
				}
				else if (startStoringSounds)
				{
					if(stringw("Sound_Ambient") == xml->getNodeName())
					{
						m_Sound_Ambient = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
					if(stringw("Sound_Attack") == xml->getNodeName())
					{
						m_Sound_Attack = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
					if(stringw("Sound_Hit") == xml->getNodeName())
					{
						m_Sound_Hit = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
					if(stringw("Sound_Hello") == xml->getNodeName())
					{
						m_Sound_Hello = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
					if(stringw("Sound_Wound") == xml->getNodeName())
					{
						m_Sound_Wound = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
					if(stringw("Sound_Die") == xml->getNodeName())
					{
						m_Sound_Die = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
				}
				else if (stringw("Name") == xml->getNodeName())
				{
					name = xml->getAttributeValue(L"value");
					nameID = xml->getAttributeValueAsInt(L"id");
				}
				else if (stringw("Model") == xml->getNodeName())
				{
					mesh = xml->getAttributeValue(L"value");
				}
				else if (stringw("Icon") == xml->getNodeName())
				{
					icon = xml->getAttributeValue(L"value");
				}
				else if (stringw("Description") == xml->getNodeName())
				{
					description = xml->getAttributeValue(L"value");
					descriptionID = xml->getAttributeValueAsInt(L"id");
				}
				else if (stringw("Tile") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isTile = true;
					}
					else
					{
						isTile = false;
					}
				}
				else if (stringw("Wall") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isWall = true;
					}
					else
					{
						isWall = false;
					}
				}
				else if (stringw("Script") == xml->getNodeName())
				{
					script = xml->getAttributeValue(L"value");
				}
				else if (stringw("Radius") == xml->getNodeName())
				{
					m_Radius = xml->getAttributeValueAsFloat(L"value");
					m_Radius_Default = xml->getAttributeValueAsFloat(L"value");
				}
				else if (stringw("Mood") == xml->getNodeName())
				{
					m_Mood = (eMood)xml->getAttributeValueAsInt(L"value");
					m_Mood_Default = (eMood)xml->getAttributeValueAsInt(L"value");
				}
				else if (stringw("Pickable") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isPickable = true;
					}
					else
					{
						isPickable = false;
					}
				}
				else if (stringw("Container") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isContainer = true;
					}
					else
					{
						isContainer = false;
					}
				}
				else if (stringw("Area") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isArea = true;
					}
					else
					{
						isArea = false;
					}
				}
				else if (stringw("Monster") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isMonster = true;
					}
					else
					{
						isMonster = false;
					}
				}
				else if (stringw("NPC") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isNPC = true;
					}
					else
					{
						isNPC = false;
					}
				}
				else if (stringw("Trigger") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isTrigger = true;
					}
					else
					{
						isTrigger = false;
					}
				}
				else if (stringw("Terrain") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isTerrain = true;
					}
					else
					{
						isTerrain = false;
					}
				}
				else if (stringw("Animation") == xml->getNodeName())
				{
					TAnimationRange anim;
					anim.startFrame = xml->getAttributeValueAsInt(L"start");
					anim.endFrame = xml->getAttributeValueAsInt(L"end");
					anim.animName = xml->getAttributeValue(L"name");
					animations.push_back(anim);
					isAnimated = true;
				}
				else if (stringw("Invisible") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isInvisible = true;
					}
					else
					{
						isInvisible = false;
					}
				}
				else if (stringw("Illusion") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if (figo == L"true")
					{
						isIllusion = true;
					}
					else
					{
						isIllusion = false;
					}
				}
				else if (stringw("Ability") == xml->getNodeName())
				{
					//abilities loading
					startStoringAbilities = true;
				}
				else if (core::stringw("Skills") == xml->getNodeName())
				{
					//skills loading.
					startStoringSkills = true;
				}
				else if (core::stringw("Sounds") == xml->getNodeName())
				{
					//sounds loading.
					startStoringSounds = true;
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			{
				if (core::stringw("Ability") == xml->getNodeName())
				{
					//abilities loaded.
					startStoringAbilities = false;
				}
				else if (core::stringw("Skills") == xml->getNodeName())
				{
					//skills loaded.
					startStoringSkills = false;
				}
				else if (core::stringw("Sounds") == xml->getNodeName())
				{
					//sounds loaded.
					startStoringSounds = false;
				}
			}
		}
	}

	if(isPickable) m_IconTexture = m_Driver->getTexture(stringw(ICON_DIR) + icon);
}

void CGameObject::AddPickableItem(CGameObject* pick)
{
	m_ListOfPickableItems.push_back(pick);
}

stringw CGameObject::GetPickableItemRoot(s32 id)
{
	return m_ListOfPickableItems[id]->path + m_ListOfPickableItems[id]->root;
}

/*stringw CGameObject::GetPickableItemPath(s32 id)
{
	return m_ListOfPickableItems[id]->path;
}*/

CGameObject* CGameObject::GetPickableItem(s32 id)
{
	return m_ListOfPickableItems[id];
}

s32 CGameObject::GetPickableItemID(s32 id)
{
	return m_ListOfPickableItems[id]->id;
}

void CGameObject::ClearPickableItems()
{
	m_ListOfPickableItems.clear();
}

void CGameObject::ClearScriptActions()
{
	m_ListOfScriptActions.clear();
}

s32 CGameObject::GetNumberOfPickableItems()
{
	return m_ListOfPickableItems.size();
}

void CGameObject::SetPositionRotationScaleFromNode(ISceneNode* node)
{
	pos = node->getPosition();
	rot = node->getRotation();
	scale = node->getScale();
}