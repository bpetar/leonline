
/*
 * File CCharacter.cpp is taken from irrlicht forum. Class is created by FOSP Team.
 * link: http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=27882
 * I changed that code little bit to suite my needs.
 * Big thanks to FOSP Team.
 *
 * ***********************************************
 * Character control
 * *********************************************
 * file name: CCharacter.cpp
 * encoding: UTF-8
 * tab size: 8
 * indentation: 4
 * created on: 9:03 PM 3/17/2008
 * init by: Do Quoc Khanh - doqkhanh
 * created by: FOSP Team
 * copyright: FOS Project http://fosp.wordpress.com/ 
 */
#include "CCharacter.h"

#include <irrlicht.h>
#define ANIMATION_SPEED 25
irr::scene::IAnimatedMeshSceneNode* CCharacter::getMesh()
{
   return node;
}

CCharacter::CCharacter()
{
	targetNode = 0;
	cranim = 0;
}

CCharacter::~CCharacter(void)
{
	//node is removed by smgr->clear()
   //node->remove();
	if(cranim)
	{
		node->removeAnimator(cranim);
		cranim->drop();
		cranim = 0;
	}
}

bool CCharacter::Init(ISceneManager *smgr, const c8* meshPath)
{
	targetNode = 0;

	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh(meshPath));
	m_smgr = smgr;
	ITriangleSelector *selector = m_smgr->createTriangleSelector(((IAnimatedMeshSceneNode*)node));
	node->setTriangleSelector(selector);

   if (node)
   {
		node->setMaterialFlag(video::EMF_LIGHTING, false);

		fSpeed = 849.f;

		curState = Idle;
		oldState = Idle;

		curAttackType = EByteAttack;

		node->setJointMode(irr::scene::EJUOR_CONTROL);
		node->setTransitionTime(0.5);
			
		idle();

		node->setID(-1);

		return true;
   }

   return false;
}

void CCharacter::setState(eState newState)
{
   oldState = curState;
   curState = newState;
}

void CCharacter::setPosition(core::vector3df pos)
{
   node->setPosition(pos);
}

void CCharacter::setTargetPosition(core::vector3df pos)
{
   targetPosition = pos;
   targetNode = 0;
   walk();
}

void CCharacter::attackMonster(const ISceneNode* hitNode)
{
	targetNode = hitNode;
	targetPosition = hitNode->getPosition();
	attack(EByteAttack);
	//play attack sound
	//m_GameManager->m_SoundEngine->play2D("media/sounds/explosion.wav");
}

void CCharacter::setTargetNode(const ISceneNode* tnode)
{
	if(cranim)
	{
		node->removeAnimator(cranim);
		cranim->drop();
		cranim = 0;
	}

	if(targetNode)
	{
		targetNode = 0;
	}

	//approach target node
	cranim = m_smgr->createCollisionResponseAnimator(tnode->getTriangleSelector(), node, vector3df(49,79,49),vector3df(0,0,0),vector3df(0,0,0));
	node->addAnimator(cranim);
	targetNode = tnode;
	targetPosition = tnode->getPosition();
	walk();
}

void CCharacter::setRotation(core::vector3df rot)
{
   node->setRotation(rot);
}

   
core::vector3df CCharacter::getPosition()
{
   return node->getPosition();
}

core::vector3df CCharacter::getRotation()
{
   return node->getRotation();
}

CCharacter::eState CCharacter::getState()
{
   return curState;
}


void CCharacter::run()
{
   setState(Run);
   
   node->setAnimationSpeed(ANIMATION_SPEED);
   node->setLoopMode(true);
   node->setFrameLoop(1,14);
}

void CCharacter::walk()
{
   setState(Walk);
   
   node->setAnimationSpeed(ANIMATION_SPEED);
   node->setLoopMode(true);
   node->setFrameLoop(Anim_Walk_start,Anim_Walk_end);
}

void CCharacter::idle()
{
   setState(Idle);
   
   node->setAnimationSpeed(ANIMATION_SPEED);
   node->setLoopMode(true);
   node->setFrameLoop(Anim_Idle_start,Anim_Idle_end);
}


void CCharacter::attack(eAttacktype attackType)
{
   setState(Attack);

   curAttackType = attackType;

   switch (attackType)
   {
		case EByteAttack:
		{
			// If animation ranges are not loaded corectly we have to check, otherwise it does not behave well
			if(Anim_Attack_start != Anim_Attack_end)
			{
				node->setAnimationSpeed(ANIMATION_SPEED);
				node->setLoopMode(false);
				node->setFrameLoop(Anim_Attack_start,Anim_Attack_end);
				node->setAnimationEndCallback(this);
			}
			else
			{
				//No animation interval, we callback imediately
				OnAnimationEnd(node);
			}
			return;
		}

      case EPowerAttack:
      {   
         node->setAnimationSpeed(ANIMATION_SPEED);
         node->setLoopMode(false);
         node->setFrameLoop(45,59);
         node->setAnimationEndCallback(this);
         return;
      }    

      case EDownswipeAttack:
      {   
         node->setAnimationSpeed(ANIMATION_SPEED);
         node->setLoopMode(false);
         node->setFrameLoop(60,68);
         node->setAnimationEndCallback(this);
         return;
      }

      case EMagicAttack:
      {   
         node->setAnimationSpeed(ANIMATION_SPEED);
         node->setLoopMode(false);
         node->setFrameLoop(69,72);
         node->setAnimationEndCallback(this);
         return;
      }

   }

}


core::vector3df faceTarget(irr::core::vector3df targetpos, ISceneNode* nod) {

	core::vector3df posDiff = targetpos - nod->getPosition();
	f32 degree = nod->getRotation().Y; //keep current rotation if nothing to do
	posDiff.normalize();

  if (posDiff.X != 0.0f || posDiff.Z != 0.0f)
    degree = atan2(posDiff.X,posDiff.Z) * core::RADTODEG;
  else
	degree = nod->getRotation().Y; //breakpoint to check axis problems

  return core::vector3df(0,degree,0);
}


void moveto(irr::scene::ISceneNode *node, //node to move
            irr::core::vector3df vel) //velocity vector
{
    irr::core::matrix4 m;
    m.setRotationDegrees(node->getRotation());
    m.transformVect(vel);
    node->setPosition(node->getPosition() + vel);
    node->updateAbsolutePosition();
}

void CCharacter::move(core::vector3df pos, f32 elapsed_time)
{
	//Rotate even if its close enough
	node->setRotation(  faceTarget(pos, node) );

	// Avoid complex operation in next step
	if(getState() != Walk && getState() != Run)
		return;

	//list<ISceneNodeAnimator*>::ConstIterator animator = node->getAnimators().begin();
	//ISceneNodeAnimatorCollisionResponse* cr = (*animator);
	if(cranim && targetNode && cranim->collisionOccurred())
	{
		const ISceneNode* cnode = cranim->getCollisionNode();
		if(cnode)
		{
			s32 cid = cnode->getID();
			if(cid == targetNode->getID())
			{
				idle();
				targetNode = 0;
				node->removeAnimator(cranim);
				cranim->drop();
				cranim = 0;
				//notify that character reached the goal
				notifyMoveEnd();
			}
			else
			{
				moveto(node, core::vector3df(0,0,fSpeed*elapsed_time));
			}
		}
	}
	else
	{
		if (node->getPosition().getDistanceFrom(pos) < 9)
		{
			idle();
			//notify that character reached the goal
			notifyMoveEnd();
		}
		else
		{
			moveto(node, core::vector3df(0,0,fSpeed*elapsed_time));
		}
	}
}

void CCharacter::targetRemoved()
{
	targetNode = 0;
	if(cranim)
	{
		node->removeAnimator(cranim);
		cranim->drop();
		cranim = 0;
	}
}

void CCharacter::update(f32 elapsed_time)
{
	if(targetNode) targetPosition = targetNode->getPosition();

	move(targetPosition, elapsed_time);

	if(node)
		node->animateJoints();
}

void CCharacter::OnAnimationEnd(IAnimatedMeshSceneNode* node)
{
    idle();
	notifyAttackEnd();
}

void CCharacter::remove()
{
   node->remove();
}

irr::scene::IAnimatedMeshSceneNode* CCharacter::getNode()
{
   return node;   
} 