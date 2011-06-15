/** 
 * \file PlayerCharacter.cpp
 * \brief File PlayerCharacter.cpp class manages player state and action.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "GameManager.h"

CPlayerCharacter::CPlayerCharacter(CGameManager* gm)
: CCharacter()
{
	m_bIsAlive = true;
	m_bTranslated = false;
	m_GameManager = gm;
	hp = 20;
	m_ReachedNextLevel = false;
	m_AvailableSkillPoints = 0;
}

CPlayerCharacter::~CPlayerCharacter()
{
}

bool CPlayerCharacter::Load(IrrlichtDevice* device, ISceneManager *smgr, const c8* xmlPath)
{
	if (ReadXMLAttributes(device->getFileSystem(),xmlPath))
	{
		stringc temp = m_MeshFilename.c_str();
		if(Init(smgr, temp.c_str()))
		{
			return true;
		}
	}

	return false;
}

stringc CPlayerCharacter::GetSound(eSoundEvent soundEvent)
{
	switch(soundEvent)
	{
		case EAttack:
			{
				switch (curAttackType)
				{
					case EByteAttack:
					{
						return m_Sound_Hit;
					}
					break;
				}
			}
			break;
	}

	return m_Sound_Boring;
}

void CPlayerCharacter::SetLoadedPosition()
{
	setPosition(m_LoadedPosition);
	setRotation(m_LoadedRotation);
}

bool CPlayerCharacter::ReinitModel(irr::scene::ISceneManager *smgr)
{
	stringc temp = m_MeshFilename.c_str();
	if(Init(smgr, temp.c_str()))
	{
		return true;
	}
	return false;
}

void CPlayerCharacter::AddExperience(s32 experience)
{
	s32 exp = getAbilityValue("Experience");
	exp+=experience;
	setAbilityValue("Experience", exp);

	s32 level = getAbilityValue("Level");

	if(exp> level*1000)
	{
		//Preso nivo
		level++;
		setAbilityValue("Level",level);
		setAbilityMax("Experience",level*1000);
		m_ReachedNextLevel = true;
		m_AvailableSkillPoints += SKILLPOINTSPERLEVEL;
		//increase HP
		s32 maxhp = getAbilityMax("Health");
		s32 hp = getAbilityValue("Health");
		maxhp+=10;
		hp+=10;
		setAbilityMax("Health",maxhp);
		setAbilityValue("Health",hp);
		m_GameManager->getGameGUI()->healthBar->setMaxBarValue(maxhp);
		m_GameManager->getGameGUI()->healthBar->setBarValue(hp);
	}
}

bool CPlayerCharacter::Save()
{
	IXMLWriter* writer = m_GameManager->getFS()->createXMLWriter("save/Player.xml");
	if(writer)
	{
		writer->writeXMLHeader();

		writer->writeElement(L"Character", false);
		writer->writeLineBreak();
		writer->writeLineBreak();

		//pozicija, mapa
		IAttributes* attr = m_GameManager->getFS()->createEmptyAttributes(m_GameManager->getDriver());
		attr->addString("Map",m_GameManager->getCurrentMapName().c_str());
		attr->addString("Model",m_MeshFilename.c_str());
		attr->addString("Name",m_Name.c_str());
		attr->addVector3d("Position",getPosition());
		attr->addVector3d("Rotation",getRotation());
		attr->write(writer);
		attr->drop();

		writer->writeLineBreak();

		writer->writeElement(L"Walk", true, L"animation_start", stringw(Anim_Walk_start).c_str(), L"animation_end", stringw(Anim_Walk_end).c_str());
		writer->writeLineBreak();
		writer->writeElement(L"Idle", true, L"animation_start", stringw(Anim_Idle_start).c_str(), L"animation_end", stringw(Anim_Idle_end).c_str());
		writer->writeLineBreak();
		writer->writeElement(L"Attack", true, L"animation_start", stringw(Anim_Attack_start).c_str(), L"animation_end", stringw(Anim_Attack_end).c_str());
		writer->writeLineBreak();
		writer->writeLineBreak();

		//for petlja za inventory
		for(s32 index=0; index<8; index++)
		{
			CGameObject* go = m_GameManager->getGameGUI()->getItemFromInventory(index);
			if(go)
			{
				writer->writeElement(L"Pickables",true,L"root",go->root.c_str(), L"id", stringw(go->id).c_str());
				writer->writeLineBreak();
			}
		}
		writer->writeLineBreak();

		//for petlja za abilite
		writer->writeElement(L"Ability", false); 
		writer->writeLineBreak();
		for(u32 i=0; i < m_ListOfAbilities.size(); i++)
		{
			writer->writeElement(m_ListOfAbilities[i].name.c_str(), true, L"value", stringw(m_ListOfAbilities[i].value).c_str(), 
				L"min", stringw(m_ListOfAbilities[i].min).c_str(), L"max", stringw(m_ListOfAbilities[i].max).c_str());
			writer->writeLineBreak();
		}
		writer->writeClosingTag(L"Ability");
		writer->writeLineBreak();

		//for petlja za skilove
		writer->writeElement(L"Skills", false);
		writer->writeLineBreak();
		for(u32 i=0; i < m_ListOfSkills.size(); i++)
		{
			writer->writeElement(m_ListOfSkills[i].name.c_str(), true, /*L"value", stringw(m_ListOfSkills[i].value).c_str(),*/ 
				L"min", stringw(m_ListOfSkills[i].min).c_str(), L"max", stringw(m_ListOfSkills[i].max).c_str());
			writer->writeLineBreak();
		}
		writer->writeClosingTag(L"Skills");
		writer->writeLineBreak();

		writer->writeLineBreak();
		writer->writeClosingTag(L"Character");

		writer->drop();

		return true;
	}

	return false;
}

bool CPlayerCharacter::ReadXMLAttributes(IFileSystem* fs, stringc xml_filename)
{
	bool success = false;
	bool startStoringAbilities = false;
	bool startStoringSkills = false;
	bool startStoringSounds = false;

	IXMLReader* xml = fs->createXMLReader(xml_filename.c_str());

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo = xml->getNodeName();
				if (startStoringSounds)
				{
					if(stringw("Sound_Ambient") == xml->getNodeName())
					{
						m_Sound_Walk = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
					if(stringw("Sound_Attack") == xml->getNodeName())
					{
						m_Sound_Attack = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
					if(stringw("Sound_Hit") == xml->getNodeName())
					{
						m_Sound_Hit = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
					}
					if(stringw("Sound_Boring") == xml->getNodeName())
					{
						m_Sound_Boring = stringc(SOUND_DIR) + xml->getAttributeValue(L"filename");
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
				else if (stringw("Character") == xml->getNodeName())
				{
					success = true; //This means it found Player.xml file and it had Character tag
				}
				//Load attributes
				else if (stringw("attributes") == xml->getNodeName())
				{
					//load node attributes
					IAttributes* attr = m_GameManager->getFS()->createEmptyAttributes(m_GameManager->getDriver());
					attr->read(xml);
					m_GameManager->m_LoadedMapName = attr->getAttributeAsString("Map");
					m_MeshFilename = attr->getAttributeAsString("Model");
					m_Name = attr->getAttributeAsString("Name");
					m_LoadedPosition = attr->getAttributeAsVector3d("Position");
					m_LoadedRotation = attr->getAttributeAsVector3d("Rotation");
				}
				//Load inventory
				else if (stringw("Pickables") == xml->getNodeName())
				{
					//Add this element
					stringc root = xml->getAttributeValue(L"root");
					s32 id = xml->getAttributeValueAsInt(L"id");
					stringw xmlProperties = root + L".xml";
					IXMLReader* pickReader = m_GameManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());
					CGameObject* pick = new CGameObject(root,id,pickReader,m_GameManager->getDriver());
					m_GameManager->getGameGUI()->AddPickableToInventory(pick);
				}
				else if (core::stringw("Animation_Walk") == xml->getNodeName())
				{
					Anim_Walk_start = xml->getAttributeValueAsInt(L"animation_start");
					Anim_Walk_end = xml->getAttributeValueAsInt(L"animation_end");
				}
				else if (core::stringw("Animation_Idle") == xml->getNodeName())
				{
					Anim_Idle_start = xml->getAttributeValueAsInt(L"animation_start");
					Anim_Idle_end = xml->getAttributeValueAsInt(L"animation_end");
				}
				else if (core::stringw("Animation_Attack") == xml->getNodeName())
				{
					Anim_Attack_start = xml->getAttributeValueAsInt(L"animation_start");
					Anim_Attack_end = xml->getAttributeValueAsInt(L"animation_end");
				}
				else if (stringw("Ability") == xml->getNodeName())
				{
					startStoringAbilities = true;
				}
				else if (core::stringw("Skills") == xml->getNodeName())
				{
					//abilities loaded.
					startStoringSkills = true;
				}
				else if (core::stringw("Sounds") == xml->getNodeName())
				{
					//sounds loaded.
					startStoringSounds = true;
				}
				else if (startStoringAbilities)
				{
					TAbility ability;
					ability.name = xml->getNodeName();
					ability.value = xml->getAttributeValueAsInt(L"value");
					ability.min = xml->getAttributeValueAsInt(L"min");
					ability.max = xml->getAttributeValueAsInt(L"max");
					m_ListOfAbilities.push_back(ability);
				}
				else if (startStoringSkills)
				{
					TSkill skill;
					skill.name = xml->getNodeName();
					//skill.value = xml->getAttributeValueAsInt(L"value");
					skill.min = xml->getAttributeValueAsInt(L"min");
					skill.max = xml->getAttributeValueAsInt(L"max");
					m_ListOfSkills.push_back(skill);
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
			break;

		}
	}

	if (xml)
		xml->drop(); // don't forget to delete the xml reader
	else
	{
		//TODO: Player xml file not found!
		//xml_filename
	}

	return success;
}

void CPlayerCharacter::notifyMoveEnd()
{
	m_GameManager->PCMoveEnd();
	//Stop playing walking sound??
}

void CPlayerCharacter::notifyAttackEnd()
{
	m_GameManager->PCAttackEnd();
}

void CPlayerCharacter::addAnimator(ITriangleSelector* selector)
{
	ISceneNodeAnimator* anim = m_GameManager->getSceneMngr()->createCollisionResponseAnimator(selector, node, vector3df(6,3,6),vector3df(0,-3,0),vector3df(0,1,0));
	node->addAnimator(anim);
	anim->drop();  // Drop the animator when we're done referring to it.
}

void CPlayerCharacter::removeAnimators()
{
	node->removeAnimators();
}