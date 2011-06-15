/** 
 * \file PlayerCharacter.h
 * \brief Header file containing CPlayerCharacter class.
 * CPlayerCharacter class manages player state and action.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include <irrlicht.h>
#include "CCharacter.h"

class CGameManager;

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

enum eSoundEvent{
		EAttack,
		EDeath,
		EWound,
		ELaugh
	};

class CPlayerCharacter : public CCharacter
{
public:

	CPlayerCharacter(CGameManager* gm);
	~CPlayerCharacter();
	bool Load(IrrlichtDevice* device, ISceneManager *smgr, const c8* xmlPath);
	bool ReinitModel(irr::scene::ISceneManager *smgr);
	bool Save();
	void SetLoadedPosition();
	bool ReadXMLAttributes(IFileSystem* fs, stringc xml_filename);
	bool isAlive() {return m_bIsAlive;}
	void Dies() {m_bIsAlive = false;}
	void virtual notifyMoveEnd();
	void virtual notifyAttackEnd();
	void addAnimator(ITriangleSelector* selector);
	void removeAnimators();
	void AddExperience(s32 experience);
	bool ReachedNextLevel() {return m_ReachedNextLevel;}
	stringc GetSound(eSoundEvent soundEvent);

	int getSkillMin(stringw name)
	{
		for (u32 i =0; i<m_ListOfSkills.size(); i++)
		{
			if(m_ListOfSkills[i].name.equals_ignore_case(name))
			{
				return m_ListOfSkills[i].min;
			}
		}

		return -1;
	}

	int getSkillMax(stringw name)
	{
		for (u32 i =0; i<m_ListOfSkills.size(); i++)
		{
			if(m_ListOfSkills[i].name.equals_ignore_case(name))
			{
				return m_ListOfSkills[i].max;
			}
		}

		return -1;
	}

	int getAbilityValue(stringw name)
	{
		for (u32 i =0; i<m_ListOfAbilities.size(); i++)
		{
			if(m_ListOfAbilities[i].name.equals_ignore_case(name))
			{
				return m_ListOfAbilities[i].value;
			}
		}

		return -1;
	}

	void setAbilityValue(stringw name, s32 value)
	{
		for (u32 i =0; i<m_ListOfAbilities.size(); i++)
		{
			if(m_ListOfAbilities[i].name.equals_ignore_case(name))
			{
				m_ListOfAbilities[i].value = value;
				return;
			}
		}
	}

	int getAbilityMin(stringw name)
	{
		for (u32 i =0; i<m_ListOfAbilities.size(); i++)
		{
			if(m_ListOfAbilities[i].name.equals_ignore_case(name))
			{
				return m_ListOfAbilities[i].min;
			}
		}

		return -1;
	}

	int getAbilityMax(stringw name)
	{
		for (u32 i =0; i<m_ListOfAbilities.size(); i++)
		{
			if(m_ListOfAbilities[i].name.equals_ignore_case(name))
			{
				return m_ListOfAbilities[i].max;
			}
		}

		return -1;
	}

	void setAbilityMax(stringw name, s32 value)
	{
		for (u32 i =0; i<m_ListOfAbilities.size(); i++)
		{
			if(m_ListOfAbilities[i].name.equals_ignore_case(name))
			{
				m_ListOfAbilities[i].max = value;
				return;
			}
		}
	}

	array<TAbility> m_ListOfAbilities;
	array<TSkill> m_ListOfSkills;
	CGameManager* m_GameManager;
	stringw m_MeshFilename;
	stringw m_Name;
	vector3df m_LoadedPosition;
	vector3df m_LoadedRotation;
	bool isInvisible;
	bool m_bIsAlive;
	bool m_bTranslated;
	u32 id;
	s32 hp;
	bool m_ReachedNextLevel;
	u32 m_AvailableSkillPoints;

	//Sounds
	stringc m_Sound_Hit;
	stringc m_Sound_Walk;
	stringc m_Sound_Attack;
	stringc m_Sound_Boring;
	stringc m_Sound_Wound;
	stringc m_Sound_Die;
	stringc m_Sound_Die2;

	//Inventory!!
};