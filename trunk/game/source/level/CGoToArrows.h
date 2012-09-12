/** 
 * \file CGoToArrows.h
 * \brief Header file containing CGoToArrows class used for drawing mark destination of player movement.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include <irrlicht.h>
#include "../Script.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

class CGoToArrows : public ISceneNode
{
public:
	CGoToArrows(IrrlichtDevice* device, ISceneManager* smgr, ISceneNode* parent, s32 id, SColor color, f32 size);
	~CGoToArrows();
	void Play(vector3df position);
	virtual void render();
	virtual void OnRegisterSceneNode()
	{
		if (isVisible())
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	virtual const aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	aabbox3df Box;
	stringw name;
	f32 m_Size;
	f32 m_Distance;
	SColor m_Color;
	ISceneNode* m_arrowMesh;
	IAnimatedMesh* m_arrow;
	bool m_Play;
	vector3df m_Position;
	IrrlichtDevice* m_pDevice;
	u32 m_StartTime;
};