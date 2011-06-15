/** 
 * \file CGoToArrows.cpp
 * \brief CGoToArrows class is used for drawing mark destination of player movement.
 *
 * When player clicks on the map to move his character, these arrows are drawn forming X position.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "CGoToArrows.h"

CGoToArrows::CGoToArrows(IrrlichtDevice* device, ISceneManager* smgr, ISceneNode* parent, s32 id, SColor color, f32 size)
: ISceneNode(parent, smgr, id)
{
	m_Play = false;
	m_Size = size;
	m_Color = color;
	m_pDevice = device;

	if (m_Size > 0)
	{
		//m_arrowMesh = smgr->addAnimatedMeshSceneNode(
		m_arrow = smgr->addArrowMesh("arrows", color, color, 4, 8, 10.f*m_Size, 6.f*m_Size, 1.f*m_Size, 3.f*m_Size);
	}
	else
	{
		//m_arrowMesh = smgr->addAnimatedMeshSceneNode(
		m_arrow = smgr->addArrowMesh("arrows", color, color, 10, 8, 10.f, 6.f, 1.f, 3.f);
	}

	if(!m_arrow)
	{
		u32 a = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBufferCount();
		IMeshBuffer* mb = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBuffer(a-1);
		Box = mb->getBoundingBox();
	}

}

CGoToArrows::~CGoToArrows()
{
}

void CGoToArrows::Play(vector3df position)
{
	f32 size = 1.f;
	m_Position = position;
	m_Play = true;
	if (m_Size > 0)
	{
		size=m_Size;
	}
	m_Distance = 10*size;
	m_StartTime = m_pDevice->getTimer()->getTime();
}

void CGoToArrows::render()
{
	f32 size = 1.f;
	IVideoDriver* driver = SceneManager->getVideoDriver();
	u32 newTime = m_pDevice->getTimer()->getTime();
	if (m_Size > 0)
	{
		size=m_Size;
	}
	m_Distance -= (f32)(newTime - m_StartTime)/20; //speed of closing arrows
	m_StartTime = newTime;
	if(m_Distance < 1.f)
	{
		m_Play = false;
		return;
	}
	if(m_Play)
	{
		this->setRotation(vector3df(90,0,0));
		this->setPosition(m_Position + vector3df(0,0,(-10.f - m_Distance)*size));
		this->updateAbsolutePosition();
		matrix4 wrldtrnsfrm = this->getAbsoluteTransformation();
		driver->setTransform(ETS_WORLD, wrldtrnsfrm);
		
		
		if (m_pDevice->getSceneManager()->getMeshCache()->isMeshLoaded("arrows"))
		{
			u32 maxBufCnt = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBufferCount();
			for(u32 buffIndex = 0; buffIndex < maxBufCnt; buffIndex++)
			{
				IMeshBuffer* mb = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBuffer(buffIndex);
				driver->drawMeshBuffer(mb);
			}
			this->setRotation(vector3df(-90,0,0));
			this->setPosition(m_Position + vector3df(0,0,(10.f + m_Distance)*size));
			this->updateAbsolutePosition();
			wrldtrnsfrm = this->getAbsoluteTransformation();
			driver->setTransform(ETS_WORLD, wrldtrnsfrm); 
			maxBufCnt = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBufferCount();
			for(u32 buffIndex = 0; buffIndex < maxBufCnt; buffIndex++)
			{
				IMeshBuffer* mb = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBuffer(buffIndex);
				driver->drawMeshBuffer(mb);
			}
			this->setRotation(vector3df(0,0,90));
			this->setPosition(m_Position + vector3df((10.f + m_Distance)*size,0,0));
			this->updateAbsolutePosition();
			wrldtrnsfrm = this->getAbsoluteTransformation();
			driver->setTransform(ETS_WORLD, wrldtrnsfrm); 
			maxBufCnt = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBufferCount();
			for(u32 buffIndex = 0; buffIndex < maxBufCnt; buffIndex++)
			{
				IMeshBuffer* mb = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBuffer(buffIndex);
				driver->drawMeshBuffer(mb);
			}
			this->setRotation(vector3df(0,0,-90));
			this->setPosition(m_Position + vector3df((-10.f - m_Distance)*size,0,0));
			this->updateAbsolutePosition();
			wrldtrnsfrm = this->getAbsoluteTransformation();
			driver->setTransform(ETS_WORLD, wrldtrnsfrm); 
			maxBufCnt = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBufferCount();
			for(u32 buffIndex = 0; buffIndex < maxBufCnt; buffIndex++)
			{
				IMeshBuffer* mb = m_pDevice->getSceneManager()->getMeshCache()->getMeshByFilename("arrows")->getMesh(0)->getMeshBuffer(buffIndex);
				driver->drawMeshBuffer(mb);
			}
		}
	}
}
