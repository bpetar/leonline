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
#include "../GameManager.h"

CGameObject::CGameObject()
{
	isContainer = false;
	isArea = false;
	isActuator = false;
	isAnimated = false;
	isAnchored = false;
	isPickable = false;
	isTrigger = false;
	isNPC = false;
	isMonster = false;
	isIllusion = false;

	isInvisible = false;
	isStatic = false;
	isTerrain = false;

	m_IconTexture = 0;
	//hp = 10;
	m_Radius = 15;
	m_Mood = Touchy;

	m_ListOfAbilities_Default.clear();
	m_ListOfSkills_Default.clear();
}

/**
 * Advanced constructor.
 */
CGameObject::CGameObject(IXMLReader* xml, IVideoDriver* driver)
{
	m_Sound_Ambient = "";
	m_Sound_Attack = "";
	m_Sound_Hit = "";
	m_Sound_Hello = "";
	m_Sound_Wound = "";
	m_Sound_Die = "";
	animations.clear();
	isAnimated = false;
	isArea = false;
	isActuator = false;
	isContainer = false;
	isMonster = false;
	isAnchored = false;
	isNPC = false;
	isPickable = false;
	isTrigger = false;
	isInvisible = false;
	isIllusion = false;
	isStatic = false;
	state = "";
	description = L"No description specified";
	m_Driver = driver;
	//hp = 10;
	m_Radius = 15;
	m_Mood = Touchy;

	m_ListOfAbilities_Default.clear();
	m_ListOfSkills_Default.clear();

	if(xml)
	{
		LoadPropertiesFromXMLFile(xml);
		xml->drop();
	}
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
	isAnimated = false;
	name = _name;
	mesh = _path;
	root = _root;
	id = _id;
	isContainer = false;
	isArea = false;
	isActuator = false;
	isMonster = false;
	isAnchored = false;
	isNPC = false;
	isPickable = false;
	isTrigger = false;
	isInvisible = false;
	isIllusion = false;
	isStatic = false;
	isTerrain = false;
	m_IconTexture = NULL;
	m_Driver = driver;

	m_ListOfAbilities_Default.clear();
	m_ListOfSkills_Default.clear();

	description = L"No description specified";
	script = _name + ".script"; //default, but can be different
	icon = _name + ".png"; //default, but can be different

	pos = vector3df(0,0,0);
	rot = vector3df(0,0,0);
	scale = vector3df(1,1,1);

	m_Radius = 15;
	m_Mood = Touchy;

	if(xml)
	{
		LoadPropertiesFromXMLFile(xml);
		xml->drop();
	}
}

CGameObject::~CGameObject()
{
}

stringw CGameObject::GetPickableItemRoot(s32 id)
{
	return m_ListOfPickableItems[id]->root;
}

s32 CGameObject::GetPickableItemID(s32 index)
{
	return m_ListOfPickableItems[index]->id;
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
					ability.name = xml->getNodeName();
					ability.value = xml->getAttributeValueAsInt(L"value");
					ability.min = xml->getAttributeValueAsInt(L"min");
					ability.max = xml->getAttributeValueAsInt(L"max");
					m_ListOfAbilities.push_back(ability);
					m_ListOfAbilities_Default.push_back(ability);
				}
				else if (startStoringSkills)
				{
					TSkill skill;
					skill.name = xml->getNodeName();
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
				}
				else if (stringw("Model") == xml->getNodeName())
				{
					//Why += ? because path is already stored in mesh, so we just add model filename
					mesh += xml->getAttributeValue(L"value");
				}
				else if (stringw("Icon") == xml->getNodeName())
				{
					icon = xml->getAttributeValue(L"value");
				}
				else if (stringw("Description") == xml->getNodeName())
				{
					description = xml->getAttributeValue(L"value");
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
				else if (stringw("Script") == xml->getNodeName())
				{
					script = xml->getAttributeValue(L"value");
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
					startStoringAbilities = true;
				}
				else if (core::stringw("Skills") == xml->getNodeName())
				{
					//skills loaded.
					startStoringSkills = true;
				}
				else if (core::stringw("Sounds") == xml->getNodeName())
				{
					//sounds loaded.
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
					//sounds loaded
					startStoringSounds = false;
				}
			}
		}
	}

	if(isPickable) m_IconTexture = m_Driver->getTexture(stringw(ICON_DIR) + icon);
}
/*
void CGameObject::AddPickableItem(stringc _root, s32 _id, IXMLReader* xml)
{
	s32 position = _root.findLastChar("/",1);
	stringc name = _root.subString(position+1,_root.size()-position);
	stringc path = _root.subString(0,position+1);
	CGameObject* pickObject = new CGameObject(path, name, false);
	pickObject->root = _root;
	pickObject->id = _id;

	if(xml)
	{
		pickObject->LoadPropertiesFromXMLFile(xml);
		xml->drop();
	}

	m_ListOfPickableItems.push_back(pickObject);
}*/

void CGameObject::AddPickableItem(CGameObject* pick)
{
	m_ListOfPickableItems.push_back(pick);
}

void CGameObject::RemovePickableItem(s32 itemID)
{
	for(u32 i=0; i<m_ListOfPickableItems.size(); i++)
	{
		if(m_ListOfPickableItems[i]->id == itemID)
		{
			m_ListOfPickableItems.erase(i);
			return;
		}
	}
}

CGameObject* CGameObject::GetPickableItem(s32 index)
{
	return m_ListOfPickableItems[index];
}

CGameObject* CGameObject::GetPickableItemFromID(s32 id)
{
	for(u32 index=0; index < m_ListOfPickableItems.size(); index++)
	{
		if(m_ListOfPickableItems[index]->id == id)
		{
			return m_ListOfPickableItems[index];
		}
	}

	return 0;
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