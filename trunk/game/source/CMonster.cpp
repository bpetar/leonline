
/*
 * File CMonster.cpp 
 *
 * ***********************************************
 * Monster control
 * *********************************************
 */
#include "CMonster.h"
#include "GameManager.h"
#include "gui/CGUIBar.h"
#include <irrKlang.h>
#include <irrlicht.h>
#define ANIMATION_SPEED 15


CMonster::CMonster()
{
	targetNode = 0;
	cranim = 0;
	IdleTimer = 0.0f;
	IdleTimer = 100;
	m_ID = 0;
	m_AttackTimer = 3.0f;
	m_GameManager = 0;

	m_MonsterDefenseMin = 0;
	m_MonsterDefenseMax = 0;	
	m_MonsterMinAttack = 0;
	m_MonsterMaxAttack = 0;
	m_MonsterDamageMin = 0;
	m_MonsterDamageMax = 0;

	m_CauseOfDeath = "You were killed by the monster";

	m_AmbientSound = 0;
	
	m_RingNode = 0;

	healthBar = 0;
}

CMonster::~CMonster(void)
{
	if(m_AmbientSound)
	{
		m_AmbientSound->stop();
		m_AmbientSound->drop();
	}

	//we have to set this to prevent callback crash if node is removed
	if(m_Node) 
	{
		((IAnimatedMeshSceneNode*)m_Node)->setAnimationEndCallback(0);
		m_Node->removeAnimators();
		m_Node->remove();
		m_Node = 0;
	}
}

void CMonster::CleanAfterKilledMonster(void)
{
	if(m_RingNode) m_RingNode->remove();

	if(cranim)
	{
		m_Node->removeAnimator(cranim);
		cranim->drop();
		cranim = 0;
	}
	//we have to set this to prevent callback crash if node is removed
	((IAnimatedMeshSceneNode*)m_Node)->setAnimationEndCallback(0);
	m_Node->removeAnimators();
	m_Node->remove();
	m_Node = 0;
}

IMeshBuffer* CMonster::createRingMesh(f32 radius, f32 width, u32 sections)
{
    CMeshBuffer<S3DVertex>* mb = new CMeshBuffer<S3DVertex>();
    mb->Vertices.set_used(sections*2 + 2); // 2 new verts per section, plus dupe at the start/end
    mb->Indices.set_used(sections*6); // two triangles per section

    // radians per segment
    f32 segRadians = (core::PI*2) / f32(sections);
    f32 segTCoord = 1.0f / f32(sections);

    // position vertices
    for (u32 i=0; i < sections; ++i)
    {
        const vector2df outer = vector2df(sin(segRadians*f32(i)), cos(segRadians*f32(i))) * radius;
        const vector2df inner = vector2df(sin(segRadians*f32(i)), cos(segRadians*f32(i))) * (radius-width);

        S3DVertex &v1 = mb->Vertices[i*2 +0];
        S3DVertex &v2 = mb->Vertices[i*2 +1];
        v1.Color.set(255, 255, 255, 255);
        v1.Normal.set(0, 1.0, 0);
        v1.Pos.set(outer.X, 0, outer.Y);
        v1.TCoords.set(1.0, segTCoord * f32(i));

        v2.Color.set(255,255,255,255);
        v2.Normal.set(0, 1.0, 0);
        v2.Pos.set(inner.X, 0,inner.Y);
        v2.TCoords.set(0.0, segTCoord * f32(i));
    }

    // dupe vertices at the end
    mb->Vertices[sections*2 +0] = mb->Vertices[0];
    mb->Vertices[sections*2 +0].TCoords.set(1.0, 1.0);
    mb->Vertices[sections*2 +1] = mb->Vertices[1];
    mb->Vertices[sections*2 +1].TCoords.set(0.0, 1.0);

    // stitch indices
    u32 v=0;
    for (u32 i=0; i < sections*6; i+=6, v+=2)
    {
        mb->Indices[i+0] = (v+0);
        mb->Indices[i+1] = (v+2);
        mb->Indices[i+2] = (v+3);
        mb->Indices[i+3] = (v+0);
        mb->Indices[i+4] = (v+3);
        mb->Indices[i+5] = (v+1);
    }

    mb->recalculateBoundingBox();
    return mb;
} 

bool CMonster::Init(ISceneManager *smgr, IAnimatedMeshSceneNode* node, CGameObject* go, CGameManager* gm)
{
	m_Node = node;
	m_smgr = smgr;
	m_GameManager = gm;
	m_ID = go->id;
	m_Name = go->name;

	if(go)
	{

		m_GO = go;
		
		// create ring
		IMeshBuffer* mb = createRingMesh(go->m_Radius, 5.f, 15);
		// add it to a mesh and recalculate the bounding box
		SMesh *mesh = new SMesh();
		mesh->addMeshBuffer(mb);
		mesh->recalculateBoundingBox();
		mb->drop();
		m_RingNode = smgr->addMeshSceneNode(mesh);
		if (m_RingNode)
		{
			m_RingNode->setMaterialFlag(EMF_LIGHTING, false);
			// johnny cash
			if(m_GO->m_Mood != Relaxed)
				m_RingNode->setMaterialTexture( 0, gm->getDriver()->getTexture("media/fire.bmp") );
			else
				m_RingNode->setMaterialTexture( 0, gm->getDriver()->getTexture("media/grass.bmp") );
			m_RingNode->setPosition(m_Node->getPosition()+vector3df(0,3.0f,0));
		}

		m_CauseOfDeath = stringw("You were killed by ") + go->name;

		m_MonsterDefenseMin = go->getSkillMin("Defense");
		m_MonsterDefenseMax = go->getSkillMax("Defense");	
		m_MonsterMinAttack = go->getSkillMin("Attack");
		m_MonsterMaxAttack = go->getSkillMax("Attack");
		m_MonsterDamageMin = go->getSkillMin("Damage");
		m_MonsterDamageMax = go->getSkillMax("Damage");

		Anim_Walk_start = go->getAnimStart("Walk");
		Anim_Walk_end = go->getAnimEnd("Walk");
		Anim_Idle_start = go->getAnimStart("Idle");
		Anim_Idle_end = go->getAnimEnd("Idle");
		Anim_Attack_start = go->getAnimStart("Attack");
		Anim_Attack_end = go->getAnimEnd("Attack");
		
		m_AmbientSound = m_GameManager->m_SoundEngine->play3D(stringc(m_GO->m_Sound_Ambient).c_str(), node->getPosition(), true, false, true);

		if(m_AmbientSound)
			m_AmbientSound->setMinDistance(10.0f);
	}
	else
	{
		//go not existing?
		return false;
	}

   if (m_Node)
   {
		m_Node->setMaterialFlag(video::EMF_LIGHTING, false);

		fSpeed = 84.f;

		curState = Idle;

		m_Node->setJointMode(irr::scene::EJUOR_CONTROL);
		m_Node->setTransitionTime(0.1f);
			
		idle();

		return true;
   }

   return false;
}

void CMonster::setState(eState newState)
{
   //oldState = curState;
   curState = newState;
}

void CMonster::setPosition(core::vector3df pos)
{
   m_Node->setPosition(pos);
}

void CMonster::setTargetPosition(core::vector3df pos)
{
   targetPosition = pos;
   walk();
}

void CMonster::setTargetNode(const ISceneNode* tnode)
{
	if(cranim)
	{
		m_Node->removeAnimator(cranim);
		cranim->drop();
		cranim = 0;
	}

	if(targetNode)
	{
		targetNode = 0;
	}

	//approach target node
	cranim = m_smgr->createCollisionResponseAnimator(tnode->getTriangleSelector(), m_Node, vector3df(29,19,29),vector3df(0,0,0),vector3df(0,0,0));
	m_Node->addAnimator(cranim);
	targetNode = tnode;
	targetPosition = tnode->getPosition();
	walk();
}

void CMonster::setRotation(core::vector3df rot)
{
   m_Node->setRotation(rot);
}

   
core::vector3df CMonster::getPosition()
{
   return m_Node->getPosition();
}

core::vector3df CMonster::getRotation()
{
   return m_Node->getRotation();
}

CMonster::eState CMonster::getState()
{
   return curState;
}

IAnimatedMeshSceneNode* CMonster::getMesh()
{
   return m_Node;
}

void CMonster::run()
{
   setState(Run);
   
   m_Node->setAnimationSpeed(ANIMATION_SPEED);
   m_Node->setLoopMode(true);
   m_Node->setFrameLoop(1,14);
}

void CMonster::walk()
{
   setState(Walk);
   
   m_Node->setAnimationSpeed(ANIMATION_SPEED);
   m_Node->setLoopMode(true);
   m_Node->setFrameLoop(Anim_Walk_start,Anim_Walk_end);
}

void CMonster::idle()
{
   setState(Idle);
   
   m_Node->setAnimationSpeed(ANIMATION_SPEED);
   m_Node->setLoopMode(true);
   m_Node->setFrameLoop(Anim_Idle_start,Anim_Idle_end);

   IdleTimer = (float)(rand()%10+3);
}

//When PC Attacks Relaxed monster (unsuspecting) we have to inform it that it is being attacked!
//How else can he notice?
void CMonster::NotifyOfAttack()
{
	//Going to touchy mode...
	m_GO->m_Mood = Touchy;
	m_RingNode->setMaterialTexture( 0, m_GameManager->getDriver()->getTexture("media/fire.bmp") );
	//Display Health Bar above monster
	vector3df Edges[8];
	m_maxY = -10000.0;
	m_Node->getTransformedBoundingBox().getEdges(Edges);
	for(int i=0;i<8;i++)
	{
		//Get Highest point of monster to place Health bar above his head
		if(m_maxY<Edges[i].Y) m_maxY = Edges[i].Y;
	}
	vector3df pos = m_Node->getPosition() + vector3df(0,m_maxY+10.0f,0);
	vector2di screenPos = m_smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(pos) + vector2di(-50,-20);
	if(healthBar == 0) 
	{
		healthBar = new CGUIBar(screenPos.X, screenPos.Y, screenPos.X+100, screenPos.Y+16, m_GO->getAbilityValue("Health"), m_GO->getAbilityMax("Health"), 0, m_Name);
	}
}

void CMonster::attack(eAttacktype attackType)
{
	setState(Attack);
	m_AttackTimer = 3.0f;
	playAttackSound(attackType);

	switch (attackType)
	{
		case EAttack:
		{
			// If animation ranges are not loaded corectly we have to check, otherwise it does not behave well
			if(Anim_Attack_start != Anim_Attack_end)
			{
				m_Node->setAnimationSpeed(ANIMATION_SPEED);
				m_Node->setLoopMode(false);
				m_Node->setFrameLoop(Anim_Attack_start,Anim_Attack_end);
				m_Node->setAnimationEndCallback(this);
			}
			else
			{
				//No animation interval, we callback imediately
				OnAnimationEnd(m_Node);
			}
			return;
		}

      case EPowerAttack:
      {
         m_Node->setAnimationSpeed(ANIMATION_SPEED);
         m_Node->setLoopMode(false);
         m_Node->setFrameLoop(45,59);
         m_Node->setAnimationEndCallback(this);
         return;
      }

      case EDownswipeAttack:
      {   
         m_Node->setAnimationSpeed(ANIMATION_SPEED);
         m_Node->setLoopMode(false);
         m_Node->setFrameLoop(60,68);
         m_Node->setAnimationEndCallback(this);
         return;
      }

      case EMagicAttack:
      {   
         m_Node->setAnimationSpeed(ANIMATION_SPEED);
         m_Node->setLoopMode(false);
         m_Node->setFrameLoop(69,72);
         m_Node->setAnimationEndCallback(this);
         return;
      }

   }

}

vector3df CMonster::faceTarget(vector3df targetpos, ISceneNode* nod) {

	vector3df posDiff = targetpos - nod->getPosition();
	f32 degree = nod->getRotation().Y + 180; //keep current rotation if nothing to do
	posDiff.normalize();

  if (posDiff.X != 0.0f || posDiff.Z != 0.0f)
    degree = atan2(posDiff.X,posDiff.Z) * core::RADTODEG;
  //else
	//degree = nod->getRotation().Y; //breakpoint to check axis problems

  return vector3df(0,degree -180, 0);
}

void CMonster::Nudge(s32 nudgerId, vector3df nudgerPosition)
{
	irr::core::matrix4 m;
	vector3df vel;

	vector3df meTargetLine = m_Node->getPosition() - targetPosition;
	meTargetLine.Y = 0;
	meTargetLine.normalize();

	vector3df meNudgerLine = m_Node->getPosition() - nudgerPosition;
	meNudgerLine.Y = 0;
	meNudgerLine.normalize();

	vel = meTargetLine + meNudgerLine;

	//check colision with other monsters
	if(m_GameManager->getLevelManager()->isNudgedMonsterColliding(nudgerId, m_GO->m_Radius, m_Node->getID(), m_Node->getPosition() + vel, targetPosition))
	{
		//someone in the path -> don't budge
		vel = vector3df(0,0,0);
	}

	m_Node->setPosition(m_Node->getPosition() + vel);

}

void CMonster::moveto(ISceneNode *node, //node to move
            			vector3df vel,    //velocity vector
						vector3df targetPos) //target position
{
    irr::core::matrix4 m;
	m.setRotationDegrees(node->getRotation());
    m.transformVect(vel);
	vector3df obstaclePos;
	targetPos.Y = 0;
	//check colision with other monsters
	if(m_GameManager->getLevelManager()->isMonsterColliding(m_Node->getID(), m_GO->m_Radius, node->getPosition() + vel, targetPos, &obstaclePos))
	{
		//change direction because another monster is in the way
		//we slide along the circle of obstacle monster by moving in the direction of normal
		vector3df colisionNormal = node->getPosition() - obstaclePos;
		colisionNormal.Y = 0;
		colisionNormal.normalize();
		vel.Y = 0;
		vel.normalize();
		vel = vel + colisionNormal; //this simple vector add results in colision tangent that we need!
	}

    node->setPosition(node->getPosition() - vel);
    node->updateAbsolutePosition();

	if(m_AmbientSound)
		m_AmbientSound->setPosition(node->getPosition());
}

void CMonster::move(core::vector3df pos, f32 elapsed_time)
{
	//Rotate even if its close enough
	m_Node->setRotation(  faceTarget(pos, m_Node) );

	// Avoid complex operation in next step
	if(getState() != Walk && getState() != Run)
		return;

	if(cranim && targetNode)
	{
		if(cranim->collisionOccurred())
		{
			const ISceneNode* cnode = cranim->getCollisionNode();
			if(cnode)
			{
				s32 cid = cnode->getID();
				if(cid == targetNode->getID())
				{
					attack(EAttack);
					return;
				}
			}
		}

		moveto(m_Node, core::vector3df(0,0,fSpeed*elapsed_time), pos);
	}
	else
	{
		if (m_Node->getPosition().getDistanceFrom(pos) < 9)
		{
			idle();
			//notify that Monster reached the goal
			notifyMoveEnd();
		}
		else
		{
			moveto(m_Node, core::vector3df(0,0,fSpeed*elapsed_time), pos);
		}
	}
}

void CMonster::update(IVideoDriver* driver, f32 elapsed_time, CPlayerCharacter* pc, IGUIFont* font, ICameraSceneNode* cam)
{
	vector3df monsterPos = m_Node->getPosition();
	monsterPos.Y = 0;
	vector3df playerPos = pc->getPosition();
	playerPos.Y = 0;

	if(healthBar != 0)
	{
		vector3df pos = m_Node->getPosition() + vector3df(0,m_maxY+10.0f,0);
		vector2di screenPos = m_smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(pos) + vector2di(-50,-10);
		healthBar->setPos(screenPos.X,screenPos.Y,screenPos.X+100,screenPos.Y+16);
		healthBar->renderGUIBars(driver,font);
	}

	if(monsterPos.getDistanceFrom(playerPos) < 119)
	{
		//If Target is trying to escape, follow it!
		if (targetNode && (monsterPos.getDistanceFrom(playerPos) > 50))
		{
			if(getState() != Walk)
				walk();
			//m_StandBack = false;
		}

		if(m_GO->m_Mood == Touchy)
		{
			m_GO->m_Mood = Mad;
			setTargetNode(pc->node);
			//PlayAttackStartSound
			m_GameManager->Play3DSound(m_GO->m_Sound_Attack,getPosition(),false);
			//Display Health Bar above monster
			vector3df Edges[8];
			m_maxY = -10000.0;
			m_Node->getTransformedBoundingBox().getEdges(Edges);
			for(int i=0;i<8;i++)
			{
				//Get Highest point of monster to place Health bar above his head
				if(m_maxY<Edges[i].Y) m_maxY = Edges[i].Y;
			}
			vector3df pos = m_Node->getPosition() + vector3df(0,m_maxY+10.0f,0);
			vector2di screenPos = m_smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(pos) + vector2di(-50,-20);
			if(healthBar == 0) 
			{
				healthBar = new CGUIBar(screenPos.X, screenPos.Y, screenPos.X+100, screenPos.Y+16, m_GO->getAbilityValue("Health"), m_GO->getAbilityMax("Health"), 0, m_Name);
			}
		}
	}

	if(m_GO->m_Mood != Mad)
	{
		if(getState() == Idle)
		{
			IdleTimer -= elapsed_time;
			if(IdleTimer <= 0)
			{
				//get random direction to walk to
				vector3df randomVector = vector3df((float)(rand()%60 - 30),0,(float)(rand()%60 - 30));
				setTargetPosition(m_Node->getPosition() + randomVector);
			}
		}
	}
	else
	{
		if(getState() == Attack)
		{
			m_AttackTimer -= elapsed_time;
			
			if(m_AttackTimer <= 0)
			{
				printf("%s attacks!\n", stringc(m_Name.c_str()).c_str());
				attack(EAttack);
			}
		}
	}

	if(targetNode) targetPosition = targetNode->getPosition();

	move(targetPosition, elapsed_time);
	
	if(m_Node) m_Node->animateJoints();

	m_RingNode->setPosition(m_Node->getPosition()+vector3df(0,3.0f,0));
}

void CMonster::OnAnimationEnd(IAnimatedMeshSceneNode* node)
{
	//set idle animation between attacks, but don't go to idle state!
	m_Node->setLoopMode(true);
	m_Node->setFrameLoop(Anim_Idle_start,Anim_Idle_end);
    //idle();

	if(getState() == Attack)
	{
		notifyAttackEnd();
	}
}

void CMonster::remove()
{
   m_Node->remove();
}


void CMonster::notifyMoveEnd()
{
}

void CMonster::notifyAttackEnd()
{
	//Calculate Hit and Damage
	if(m_GameManager && m_GameManager->getPC() && m_GameManager->getPC()->isAlive())
	{
		int PCMinDefence = m_GameManager->getPC()->getSkillMin("Defense");
		int PCMaxDefence = m_GameManager->getPC()->getSkillMax("Defense");

		int randomMonsterAttack = 0;
		if(m_MonsterMinAttack != m_MonsterMaxAttack)
		{
			randomMonsterAttack = rand()%(m_MonsterMaxAttack-m_MonsterMinAttack+1) + m_MonsterMinAttack;
		}
		else
		{
			randomMonsterAttack = m_MonsterMinAttack;
		}
		int randomPCDefence = 0;
		if(PCMinDefence != PCMaxDefence)
		{
			randomPCDefence = rand()%(PCMaxDefence-PCMinDefence+1) + PCMinDefence;
		}
		else
		{
			randomPCDefence = PCMinDefence;
		}

		if(randomMonsterAttack>=randomPCDefence)
		{
			//Hit! Calculate Damage.
			int damage = 0;
			if(m_MonsterDamageMin != m_MonsterDamageMax)
			{
				damage = rand()%(m_MonsterDamageMax-m_MonsterDamageMin+1) + m_MonsterDamageMin;
			}
			else
			{
				damage = m_MonsterDamageMin;
			}

			m_GameManager->getGameGUI()->AddConsoleText(m_Name + stringw(" hit you with ") + stringw(damage) + stringw(" damage! ") + stringw("(") + stringw(randomMonsterAttack) + stringw(">=") + stringw(randomPCDefence) + stringw(")"));

			//PlayHitSound();

			m_GameManager->PCChangeHealth(-damage, m_CauseOfDeath);
		}
		else
		{
			//Miss! 
			m_GameManager->getGameGUI()->AddConsoleText(m_Name + stringw(" missed you!"));
			//Play miss sound?
		}
	}
}

bool CMonster::ReadXMLAttributes(IFileSystem* fs, stringc xml_filename)
{
	bool success = false;
	bool startStoringAbilities = false;
	bool startStoringSkills = false;

	IXMLReader* xml = fs->createXMLReader(xml_filename.c_str());

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo = xml->getNodeName();
				if (stringw("Character") == xml->getNodeName())
				{
					success = true; //This means it found Player.xml file and it had Character tag
				}
				//Load attributes
				/*else if (stringw("attributes") == xml->getNodeName())
				{
					//load node attributes
					IAttributes* attr = m_GameManager->getFS()->createEmptyAttributes(m_GameManager->getDriver());
					attr->read(xml);
					//m_MeshFilename = attr->getAttributeAsString("Model");
					//m_Name = attr->getAttributeAsString("Name");
					//m_LoadedPosition = attr->getAttributeAsVector3d("Position");
					//m_LoadedRotation = attr->getAttributeAsVector3d("Rotation");
				}*/
				//Load inventory
				/*else if (stringw("Pickables") == xml->getNodeName())
				{
					//Add this element
					stringc root = xml->getAttributeValue(L"root");
					s32 id = xml->getAttributeValueAsInt(L"id");
					stringw xmlProperties = root + L".xml";
					IXMLReader* pickReader = m_GameManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());
					CGameObject* pick = new CGameObject(root,id,pickReader,m_GameManager->getDriver());
					m_GameManager->getGameGUI()->AddPickableToInventory(pick);
				}*/
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
				/*else if (stringw("Ability") == xml->getNodeName())
				{
					startStoringAbilities = true;
				}
				else if (core::stringw("Skills") == xml->getNodeName())
				{
					//abilities loaded.
					startStoringSkills = true;
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
					skill.value = xml->getAttributeValueAsInt(L"value");
					skill.min = xml->getAttributeValueAsInt(L"min");
					skill.max = xml->getAttributeValueAsInt(L"max");
					m_ListOfSkills.push_back(skill);
				}*/
			}
			break;
		/*case io::EXN_ELEMENT_END:
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
			}
			break;*/

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


irr::scene::IAnimatedMeshSceneNode* CMonster::getNode()
{
   return m_Node;   
} 

void CMonster::playAttackSound(eAttacktype attackType)
{
	switch (attackType)
	{
		case EAttack:
			{
				//PlayAttackSound
				m_GameManager->Play3DSound(m_GO->m_Sound_Hit,getPosition(),false);
			}
			break;
		case EPowerAttack:
			{
			}
			break;
	}
}
