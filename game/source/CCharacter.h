
/*
 * File CCharacter.h is taken from irrlicht forum. Class is created by FOSP Team.
 * link: http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=27882
 * I changed that code little bit to suite my needs.
 * Big thanks to FOSP Team.
 *
 *
 ***********************************************
 * Character control
 * *********************************************
 * file name: CCharacter.h
 * encoding: UTF-8
 * tab size: 8
 * indentation: 4
 * created on: 8:55 PM 3/17/2008
 * init by: Do Quoc Khanh - doqkhanh
 * created by: FOSP Team
 * copyright: FOS Project http://fosp.wordpress.com/ 
 */

#pragma once
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CCharacter : public irr::scene::IAnimationEndCallBack
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
		EByteAttack,
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

	IAnimatedMeshSceneNode* node;
	eAttacktype curAttackType;
	eState curState;
	eState oldState;
	vector3df targetPosition;
	const ISceneNode* targetNode;
	ISceneNodeAnimatorCollisionResponse* cranim;
	ISceneManager *m_smgr;

	CCharacter();
	~CCharacter(void);
	bool Init(ISceneManager *smgr, const c8* meshPath);
	void setState(eState newState);
	void setPosition(vector3df pos);
	void setTargetPosition(vector3df pos);
	void setTargetNode(const ISceneNode* node);
	void attackMonster(const ISceneNode* hitNode);
	void setRotation(vector3df rot);
	void OnAnimationEnd(IAnimatedMeshSceneNode* node);
	void targetRemoved();
	void run();
	void walk();
	void attack(eAttacktype type);
	void idle();
	void move(vector3df pos, f32 elapsed_time);
	void update(f32 elapsed_time);
	void remove();
	void virtual notifyMoveEnd() = 0; 
	void virtual notifyAttackEnd() = 0;
	vector3df getPosition();
	vector3df getRotation();
	eState getState();
	IAnimatedMeshSceneNode* getMesh();
	IAnimatedMeshSceneNode* getNode();
};
