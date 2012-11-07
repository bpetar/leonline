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
	m_Driver = driver;
	nameID = 0;
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
	m_IconTexture = 0;
	description = L"No description specified";
	script = _name + ".script"; //default, but can be different
	icon = _name + ".png"; //default, but can be different
	m_Driver = driver;
	nameID = 0;

	if(xml)
	{
		LoadPropertiesFromXMLFile(xml);
		xml->drop();
	}
}

CGameObject::~CGameObject()
{
}

void CGameObject::LoadTrajectoryPaths(IXMLReader* xml, ISceneManager* smgr)
{
	bool startStoringPathNodes = false;
	TPath path;

	while(xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				if (stringw("Path").equals_ignore_case(xml->getNodeName()))
				{
					startStoringPathNodes = true;
					path.name = xml->getAttributeValue(L"name");
					stringw loopStrValue = xml->getAttributeValue(L"loop");
					if(loopStrValue.equals_ignore_case("true")) path.loop = true;
					else path.loop = false;
					path.nodes.clear();
				}
				if(startStoringPathNodes)
				{
					if(stringw("Coord").equals_ignore_case(xml->getNodeName()))
					{
						TPathNode pathNode;
						pathNode.speed = xml->getAttributeValueAsFloat(L"speed");
						pathNode.position = Util_getVectorFromString(xml->getAttributeValue(L"position"));
						pathNode.rotation = Util_getVectorFromString(xml->getAttributeValue(L"rotation"));
						pathNode.scale = Util_getVectorFromString(xml->getAttributeValue(L"scale"));
						pathNode.pause = xml->getAttributeValueAsFloat(L"pause");
						pathNode.id = xml->getAttributeValueAsInt(L"id");
						SColor color = SColor(255,10,10,155);
						SColor color2 = SColor(255,10,10,185);
						f32 size = 10;
						pathNode.sceneNode = smgr->addAnimatedMeshSceneNode(smgr->addArrowMesh(TRAJECTORY_NODE_GAME_OBJECT, color, color2, 4, 8, 10.f*size, 6.f*size, 1.f*size, 3.f*size),0,pathNode.id,pathNode.position,vector3df(180,0,0));
						//pathNode.sceneNode->setPosition(pathNode.position); //q: why is this commented out?? a: i guess its up there in the list of arguments of addArrowMesh
						pathNode.sceneNode->setVisible(false);
						path.nodes.push_back(pathNode);
					}
				}
			}
			break;

			case io::EXN_ELEMENT_END:
			{
				if (stringw("Path").equals_ignore_case(xml->getNodeName()))
				{
					//Path loaded.
					m_ListOfTrajectoryPaths.push_back(path);
					startStoringPathNodes = false;
				}
			}
		}
	}
}

/* 
 * If trajectory path has been edited, it should be saved to script file.
 * Script file should remain the same and only changed path nodes should be updated.
 * This function reads existing script file and writes 
 */
void CGameObject::SaveTrajectoryPaths()
{
	
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