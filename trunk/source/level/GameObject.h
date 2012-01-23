/** 
 * \file GameObject.h
 * \brief Header file containing CGameObject class used for storing Game object attributes and data.
 * CGameObject class handles mesh name and id, list of pickable items (if the object is container), etc...
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */


#if !defined(AFX_GAMEOBJECTCA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_GAMEOBJECTCA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
#include "../Script.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

/**
 * \brief TAnimationRanges is struct describing animated object animation frames.
 * Animated character usually has standard animations like Walk, Idle, Attack, Run, Sleep...
 * \author Petar Bajic \date July, 21 2008.
 */
typedef struct S_AnimationRanges
{
	u32 startFrame;
	u32 endFrame;
	stringw animName;

	//structs can have functions in C++! Yeey!
	void init() {
		startFrame = 0;
		endFrame = 0;
		animName = "";
	}

} TAnimationRange;

typedef struct S_Skill
{
	u32 min;
	u32 max;
	stringw skillName;

	//structs can have functions in C++! Yeey!
	void init() {
		min = 0;
		max = 0;
		skillName = "";
	}

} TSkill;

typedef struct S_Ability
{
	u32 min;
	u32 max;
	u32 value;
	stringw abilityName;

	//structs can have functions in C++! Yeey!
	void init() {
		min = 0;
		max = 0;
		value = 0;
		abilityName = "";
	}

} TAbility;

//Monster mood
enum eMood{
	  Relaxed,
	  Touchy,
	  Mad
	}; 

/**
 * \brief CGameObject class is describing game object properties.
 * Standard irrlicht scene node attributes are expanded with few properties like: isContainer, isPickable...
 * \author Petar Bajic \date July, 21 2008.
 */
class CGameObject
{
public:

	CGameObject();
	CGameObject(stringw path, stringw name, bool isStatic, IVideoDriver* driver);
	CGameObject(stringw _root, s32 _id, IXMLReader* xml, IVideoDriver* driver);
	~CGameObject();
	void LoadPropertiesFromXMLFile(IXMLReader* xml);
	void AddPickableItem(CGameObject* pick);
	CGameObject* GetPickableItem(s32 id);
	stringw GetPickableItemRoot(s32 id);
	s32 GetPickableItemID(s32 id);
	void ClearPickableItems();
	s32 GetNumberOfPickableItems();
	void ClearScriptActions();
	void SetDriver(IVideoDriver* _driver) {m_Driver = _driver;}
	void SetPositionRotationScaleFromNode(ISceneNode* node);

	//properties
	stringw name;
	u32 nameID;
	stringw mesh;
	stringw path;
	stringw position;
	vector3d<f32> pos;
	vector3d<f32> rot;
	vector3d<f32> scale;
	f32 m_Radius;
	f32 m_Radius_Default;
	eMood m_Mood;
	eMood m_Mood_Default;
	s32 m_HP;
	s32 m_Exp;
	bool isInvisible;
	bool isIllusion;
	bool isPickable;
	bool isTrigger;
	bool isStatic;
	bool isTerrain;
	bool isTile;
	bool isWall;
	bool isNPC;
	bool isMonster;
	bool isContainer;
	bool isArea;
	bool isAnimated;
	bool isAnchored;
	stringw description;
	u32 descriptionID;
	stringw script;
	stringw state;
	stringw icon;
	stringw root;
	list <TAnimationRange> animations;
	stringw m_Sound_Ambient;
	stringw m_Sound_Attack;
	stringw m_Sound_Hit;
	stringw m_Sound_Hello;
	stringw m_Sound_Wound;
	stringw m_Sound_Die;
	array <TAbility> m_ListOfAbilities;
	array <TSkill> m_ListOfSkills;
	array <TAbility> m_ListOfAbilities_Default;
	array <TSkill> m_ListOfSkills_Default;

	//only container objects have this list to asociate them to pickable items they contain
	ITexture* m_IconTexture;
	array <CGameObject*> m_ListOfPickableItems;
	array <TScriptAction> m_ListOfScriptActions;
	s32 id;
		
	IVideoDriver* m_Driver;

	//helper function to get ability value
	int getAbilityValue(stringw name)
	{
		for (u32 i =0; i<m_ListOfAbilities.size(); i++)
		{
			if(m_ListOfAbilities[i].abilityName.equals_ignore_case(name))
			{
				return m_ListOfAbilities[i].value;
			}
		}

		return -1;
	}

	int getAbilityValue_Default(stringw name)
	{
		for (u32 i =0; i<m_ListOfAbilities_Default.size(); i++)
		{
			if(m_ListOfAbilities_Default[i].abilityName.equals_ignore_case(name))
			{
				return m_ListOfAbilities_Default[i].value;
			}
		}

		return -1;
	}

	//helper function to get ability value
	void setAbilityValue(stringw name, s32 _value)
	{
		for (u32 i=0; i<m_ListOfAbilities.size(); i++)
		{
			if(m_ListOfAbilities[i].abilityName.equals_ignore_case(name))
			{
				m_ListOfAbilities[i].value = _value;
			}
		}
	}

	//helper function to set skill values
	void setSkillValues(stringw name, s32 _min, s32 _max)
	{
		for (u32 i=0; i<m_ListOfSkills.size(); i++)
		{
			if(m_ListOfSkills[i].skillName.equals_ignore_case(name))
			{
				m_ListOfSkills[i].min = _min;
				m_ListOfSkills[i].max = _max;
			}
		}
	}

	//helper function to set ability values
	void setAbilityValues(stringw name, s32 _value, s32 _min, s32 _max)
	{
		for (u32 i=0; i<m_ListOfAbilities.size(); i++)
		{
			if(m_ListOfAbilities[i].abilityName.equals_ignore_case(name))
			{
				m_ListOfAbilities[i].value = _value;
				m_ListOfAbilities[i].min = _min;
				m_ListOfAbilities[i].max = _max;
			}
		}
	}

	//Animation helper functions
	int getAnimStart(stringw name)
	{
		list<TAnimationRange>::Iterator it = animations.begin();
		
		for (; it != animations.end(); ++it)
		{
			if((*it).animName == name)
			{
				return (*it).startFrame;
			}
		}

		return 0;
	}

	int getAnimEnd(stringw name)
	{
		list<TAnimationRange>::Iterator it = animations.begin();
		
		for (; it != animations.end(); ++it)
		{
			if((*it).animName == name)
			{
				return (*it).endFrame;
			}
		}

		return 0;
	}
};

#endif

