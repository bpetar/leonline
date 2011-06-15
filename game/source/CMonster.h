
/*
 * File CMonster.h 
 ***********************************************
 * Monster control
 * *********************************************
 */

#pragma once
#include <irrlicht.h>
#include <irrKlang.h>
#include "level/GameObject.h"

using namespace irrklang;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CGameManager;
class CGameObject;
class CPlayerCharacter;
class CGUIBar;
//class ISound;

class CMonster : public IAnimationEndCallBack
{
private:
   float fSpeed;

public:
	//Anim
	s32 Anim_Walk_start;
	s32 Anim_Walk_end;
	s32 Anim_Idle_start;
	s32 Anim_Idle_end;
	s32 Anim_Attack_start;
	s32 Anim_Attack_end;

	enum eAttacktype{
		EAttack,
		EMagicAttack,
		EPowerAttack,
		EDownswipeAttack
	};

	enum eState{
	  Run,
	  Walk,
	  Attack,
	  Idle
	};   

	/*enum eMood{
	  Relaxed,
	  Touchy,
	  Mad
	};*/

	s32 m_ID;
	stringw m_Name;
	float m_AttackTimer;
	ISound* m_AmbientSound;
	IAnimatedMeshSceneNode* m_Node;
	f32 m_maxY;
	eState curState;
	float IdleTimer;
	vector3df targetPosition;
	const ISceneNode* targetNode;
	ISceneNodeAnimatorCollisionResponse* cranim;
	ISceneManager *m_smgr;
	CGameManager* m_GameManager;
	CGUIBar* healthBar;
	CGameObject* m_GO;
	IMeshSceneNode* m_RingNode;

	s32 m_MonsterDefenseMin;
	s32 m_MonsterDefenseMax;	
	s32 m_MonsterMinAttack;
	s32 m_MonsterMaxAttack;
	s32 m_MonsterDamageMin;
	s32 m_MonsterDamageMax;
	
	stringw m_CauseOfDeath;

	CMonster();
	~CMonster(void);
	void CleanAfterKilledMonster(void);
	IMeshBuffer* createRingMesh(f32 radius, f32 width, u32 sections);
	bool Init(ISceneManager *smgr, IAnimatedMeshSceneNode* node, CGameObject* go, CGameManager* gm);
	void Nudge(s32 nudgerId, vector3df pos);
	void setState(eState newState);
	void setPosition(vector3df pos);
	void setTargetPosition(vector3df pos);
	void setTargetNode(const ISceneNode* node);
	void setRotation(vector3df rot);
	void OnAnimationEnd(IAnimatedMeshSceneNode* node);
	void run();
	void walk();
	void attack(eAttacktype);
	void idle();
	void move(vector3df pos, f32 elapsed_time);
	void update(IVideoDriver* driver, f32 elapsed_time, CPlayerCharacter* pc, IGUIFont* font, ICameraSceneNode* cam);
	void remove();
	bool ReadXMLAttributes(IFileSystem* fs, stringc xml_filename);
	void notifyMoveEnd();
	void notifyAttackEnd();
	void NotifyOfAttack();
	vector3df getPosition();
	vector3df getRotation();
	eState getState();
	IAnimatedMeshSceneNode* getMesh();
	IAnimatedMeshSceneNode* getNode();
	void playAttackSound(eAttacktype attackType);

	void moveto(ISceneNode *node, vector3df vel, vector3df targetPos);
	vector3df faceTarget(vector3df targetpos, ISceneNode* nod);
};
