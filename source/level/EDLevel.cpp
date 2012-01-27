/** 
 * \file EdLevel.cpp
 * \brief File EdLevel.cpp takes care of level structure - it hold together all game objects on the map.
 *
 * takes care of saving/loading levels 
 * manages game object arrays (adding deleting game objects)
 * allows translation, rotation and scaling of models.
 * implements undo/redo functionality
 * translates events to game objects -> on mouse click, on rotate etc..
 * tightly communicates with GUI environment, for example: 
 *  - selecting different game object causes changes in GUI window Properties
 *  - changed game object properties must be saved to map file
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "EDLevel.h"
#include "../EditorManager.h"
#include "../Utils.h"

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CEditorLevel::CEditorLevel()
{
	m_EditorManager = NULL;
	m_SelectedGameObject = NULL;
	m_PreviewGameObject = 0;
	m_PreviewPickGameObject = 0;
	m_bMoveSelectedNode = false;
	m_bRotateXSelectedNode = false;
	m_bRotateYSelectedNode = false;
	m_bRotateZSelectedNode = false;
	m_bShiftPressed = false;
	m_bCtrlPressed = false;
	m_bKeyXPressed = false;
	m_bKeyZPressed = false;
	m_bNodeBeingMoved = false;
	m_bElementAtHand = false;
	m_bSavedState = true;
	m_MultiSelectStart = false;
	m_MultiSelectPosStart = vector3df(0,0,0);
	m_MultiSelectPosEnd = vector3df(0,0,0);
	m_MoveOldPosition = vector3df(0,0,0);
	m_CurrentZoom = vector3df(1.0f,1.0f,1.0f);
	m_MapName = "untitled";
	m_LevelMetaTriangleSelector = 0;
	m_NodeRotationY = 0;
	m_NodeRotationX = 0;
	m_TileHeight = 0;
	m_WallHeight = 0;
	m_LevelMusic_MusicFile = "";
	m_LevelMusic_SoundFile = "";
	m_LevelMusic_NarationFile = "";
}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CEditorLevel::~CEditorLevel()
{
	m_AspenTreeGenerator->drop();
	m_OakTreeGenerator->drop();
	m_PineTreeGenerator->drop();
	m_WillowTreeGenerator->drop();
}

/**
 * \brief Used to set Map name without directory path and extension.
 * Map name is used to create name of container content xml file.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::SetMapName(stringc mapname)
{
	m_MapName = mapname;
	
	/*if(mapname.size() > 5)
	{
		int position1 = mapname.findLast('\\');
		int position2 = mapname.findLast('/');
		if( position1 < position2)
			position1 = position2;
		m_MapName = mapname.subString(position1+1,mapname.size()-position1 - 5);
	}*/
}

bool CEditorLevel::MapNameExists()
{
	if (m_MapName == "untitled")
		return false;
	else
		return true;
}

void CEditorLevel::SetLoadingDir(stringc mapname)
{
	m_LoadingDir = mapname;
}

void CEditorLevel::OnCreateNode(ISceneNode* node)
{
}

/*
 * createUserData is irrlicht serialization function used for adding user defined attributes to 
 * irr scenes (xml files) for each scene node. In this case, information about nodes like
 * is it a container object or does it responds with dialogue are stored in this function.
 * This function has 'reverse' version for loading user data from irr scene called: OnReadUserData
 */
IAttributes* CEditorLevel::createUserData (irr::scene::ISceneNode *sceneNode)
{
	IAttributes* attr = m_EditorManager->getFS()->createEmptyAttributes(m_EditorManager->getDriver());

	//for each node, add this lovely, useless attribute:
	attr->addString("Homepage",L"www.mystic-peanut.com"); //(valuable testing purpose)
	
	s32 id = sceneNode->getID();
	CGameObject* gameObject = _getGameObjectFromID(id);

	//For each scene node, but camera (although this will be changed)
	if(id >= 0)
	{
		//If node is container, add user data attribute, and create xml file for container content
		if (gameObject->isContainer)
		{
			//adding attribute
			attr->addBool("isContainer",true);

			//Make xml file wich holds container content
			stringw id_text = stringw(id);
			stringc container_filename = "CC_";
			container_filename += m_MapName.c_str();
			container_filename += "_";
			container_filename += id;
			container_filename += ".xml";
			io::IXMLWriter* xml = m_EditorManager->getDevice()->getFileSystem()->createXMLWriter(container_filename.c_str());

			if (xml)
			{
				xml->writeXMLHeader(); xml->writeLineBreak();
				xml->writeElement(L"Container",false,L"id",id_text.c_str()); xml->writeLineBreak();
				for ( s32 index = 0; index < gameObject->GetNumberOfPickableItems(); index++)
				{
					xml->writeElement(L"Pickables",true, L"root", gameObject->GetPickableItemRoot(index).c_str(), L"id", stringw(gameObject->GetPickableItemID(index)).c_str());
					xml->writeLineBreak();
				}
				xml->writeClosingTag(L"Container"); xml->writeLineBreak();
				xml->drop(); // don't forget to delete the xml reader
			}
		}
		if (gameObject->isPickable)
		{
			//adding attribute isPickable
			attr->addBool("isPickable",true);
		}
		if (gameObject->isStatic)
		{
			//adding attribute isStatic
			attr->addBool("isStatic",true);
		}
		if (gameObject->isTerrain)
		{
			//adding attribute isTerrain
			attr->addBool("isTerrain",true);
		}
		if (gameObject->isTrigger)
		{
			//adding atribute isTrigger
			attr->addBool("isTrigger",true);
		}
		if (gameObject->isNPC)
		{
			//adding atribute isNPC
			attr->addBool("isNPC",true);
		}
		if (gameObject->script != stringw(L""))
		{
			attr->addString("Script",gameObject->script.c_str());
		}
		if (gameObject->state != stringw(L""))
		{
			attr->addString("State",gameObject->state.c_str());
		}
	}

	return attr; //returned structure will be saved to irr file by irrlicht automatically
}

/*
 * OnReadUserData is irrlicht serialization function used for loading user defined attributes 
 * from irr scenes (xml files) for each scene node. In this case, information about nodes like
 * is it a container object or does it responds with dialogue are loaded in this function.
 * This function has 'reverse' version for saving user data to irr scene called: createUserData
 */
void CEditorLevel::OnReadUserData (irr::scene::ISceneNode *sceneNode, irr::io::IAttributes *userData)
{
	m_EditorManager->getFS()->changeWorkingDirectoryTo(m_LoadingDir.c_str());
	//ignoring camera, grid, and other special nodes with ID = -1
	s32 id = sceneNode->getID();

	if(id >= 0)
	{
		//adding new game object to the map
		CGameObject* gameObject = new CGameObject();
		if(userData->getAttributeAsBool("isContainer"))
		{
			gameObject->isContainer = true;

			//Load Existing Container Content (if any)
			stringc container_filename = "CC_";
			container_filename += m_MapName.c_str();
			container_filename += "_";
			container_filename += id;
			container_filename += ".xml";
			io::IXMLReader* xml = m_EditorManager->getDevice()->getFileSystem()->createXMLReader(container_filename.c_str());
			while(xml && xml->read())
			{
				//Load container content from xml file to GameObjects list of pickable items
				if (core::stringw("Pickables") == xml->getNodeName())
				{
					//Add this element
					stringc root = xml->getAttributeValue(L"root");
					stringc idStr = xml->getAttributeValue(L"id");
					s32 id = atoi(idStr.c_str());
					stringw xmlProperties = root + L".xml";
					IXMLReader* xmlPropertyReader = m_EditorManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());
					CGameObject* pick = new CGameObject(root,id,xmlPropertyReader,m_EditorManager->getDriver());
					gameObject->AddPickableItem(pick);
					
					//TODO ID handling on map loading to be revisited...
					if(m_EditorManager->m_ID <= id) m_EditorManager->m_ID = id + 1;
				}
			}

			if (xml)
				xml->drop(); // don't forget to delete the xml reader
		}
		if(userData->getAttributeAsBool("isPickable"))
		{
			gameObject->isPickable = true;
		}
		if(userData->getAttributeAsBool("isTerrain"))
		{
			gameObject->isTerrain = true;
		}
		if(userData->getAttributeAsBool("isStatic"))
		{
			gameObject->isStatic = true;
			//if static, add its triangles to Triangle Selector
			ITriangleSelector* selector = 0;
			if(sceneNode->getType() == ESNT_TERRAIN)
			{
				selector = m_EditorManager->getSceneMngr()->createTerrainTriangleSelector((ITerrainSceneNode*)sceneNode, 0);
			}
			else //animated mesh
			{
				selector = m_EditorManager->getSceneMngr()->createTriangleSelector(((IAnimatedMeshSceneNode*)sceneNode)->getMesh(), sceneNode);
			}
			sceneNode->setTriangleSelector(selector);
			m_LevelMetaTriangleSelector->addTriangleSelector(sceneNode->getTriangleSelector());
		}
		if(userData->getAttributeAsBool("isTrigger"))
		{
			gameObject->isTrigger = true;
		}
		if(userData->getAttributeAsBool("isNPC"))
		{
			gameObject->isNPC = true;
		}
		gameObject->script = userData->getAttributeAsStringW("Script");
		gameObject->state = userData->getAttributeAsStringW("State");

		gameObject->id = id;
		m_ListOfGameObjects.push_back(gameObject);

		//add node to gui scene tree
		m_EditorManager->getGUIManager()->AddNodeToSceneTree(id, sceneNode->getName());

		m_EditorManager->m_ID++;
	}
	m_EditorManager->backToWorkingDirectory();
}

/**
 * helper function: draw the object aligned bounding box
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void drawObjectBoundingBox(scene::ISceneNode* node,
                                      video::IVideoDriver* driver,
									  aabbox3df selectedBox,
                                      video::SColor color = video::SColor(255, 0, 255, 0))
{
   if (!node || !driver)
      return;

   video::SMaterial matl;
   matl.Lighting = false;
   driver->setMaterial(matl);
   driver->setTransform(video::ETS_WORLD, node->getAbsoluteTransformation());
   driver->draw3DBox(selectedBox/*node->getBoundingBox()*/, color);
   
   	/*Vitek said:
	Be careful with getTransformedBoundingBox(). It doesn't rotate the box properly so you will occasionally get boxes that don't actually enclose the object. This can be very noticeable when you have an oblong object that is rotated.
	The best is to use matrix4::transformBoxEx() to do something like this...

	core::aabbox3df b1 = cube->getBoundingBox();
	cube->getAbsoluteTransformation().transformBoxEx(b1);
	core::aabbox3df b2 = sphere->getBoundingBox();
	sphere->getAbsoluteTransformation().transformBoxEx(b2);
	*/
}

/**
 * \brief Used for drawing bounding box, rotating preview objects, moving while GUI button is pressed and hold, etc.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::OnRender()
{
	if(m_PreviewGameObject && m_EditorManager)
	{
		m_PreviewGameObject->setRotation(core::vector3df(0,(f32)m_EditorManager->getDevice()->getTimer()->getTime()/10,0));
	}
	if(m_PreviewPickGameObject && m_EditorManager)
	{
		m_PreviewPickGameObject->setRotation(core::vector3df(0,(f32)m_EditorManager->getDevice()->getTimer()->getTime()/15,0));
	}
	if (m_SelectedGameObject && m_EditorManager)
	{
		drawObjectBoundingBox(m_SelectedGameObject,m_EditorManager->getDriver(),m_SelectedBox);
	}
	if (m_SelectedGameObject && m_EditorManager->getGUIManager()->m_bMoveModel)
	{
		if(m_EditorManager->getDevice()->getTimer()->getRealTime() > m_EditorManager->getGUIManager()->m_uClickStartTime + 300)
		{
			if(m_EditorManager->getGUIManager()->m_eMoveDirection == LE_X_UP)
				MoveSelectedElement(LE_AXIS_X,LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eMoveDirection == LE_X_DOWN)
				MoveSelectedElement(LE_AXIS_X,-LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eMoveDirection == LE_Y_UP)
				MoveSelectedElement(LE_AXIS_Y,LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eMoveDirection == LE_Y_DOWN)
				MoveSelectedElement(LE_AXIS_Y,-LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eMoveDirection == LE_Z_UP)
				MoveSelectedElement(LE_AXIS_Z,LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eMoveDirection == LE_Z_DOWN)
				MoveSelectedElement(LE_AXIS_Z,-LE_MOVE_STEP);
		}
	}
	if (m_SelectedGameObject && m_EditorManager->getGUIManager()->m_bRotateModel)
	{
		if(m_EditorManager->getDevice()->getTimer()->getRealTime() > m_EditorManager->getGUIManager()->m_uClickStartTime + 300)
		{
			if(m_EditorManager->getGUIManager()->m_eRotateDirection == LE_X_UP)
				RotateSelectedElement(LE_AXIS_X,LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eRotateDirection == LE_X_DOWN)
				RotateSelectedElement(LE_AXIS_X,-LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eRotateDirection == LE_Y_UP)
				RotateSelectedElement(LE_AXIS_Y,LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eRotateDirection == LE_Y_DOWN)
				RotateSelectedElement(LE_AXIS_Y,-LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eRotateDirection == LE_Z_UP)
				RotateSelectedElement(LE_AXIS_Z,LE_MOVE_STEP);
			if(m_EditorManager->getGUIManager()->m_eRotateDirection == LE_Z_DOWN)
				RotateSelectedElement(LE_AXIS_Z,-LE_MOVE_STEP);
		}
	}
}

/**
 * Sets properties to selected model.
 * Only position, rotation and model name can be set at the moment. 
 * Full properties set could be implemented later.
 */
void CEditorLevel::SetModelProperties(vector3df pos, vector3df rot, stringw name)
{
	if(m_SelectedGameObject)
	{
		m_SelectedGameObject->setPosition(pos);
		m_SelectedGameObject->setRotation(rot);
		m_SelectedGameObject->setName(name.c_str());
	}
}

/**
 * \brief Aahhh... SetElementAtHand. Creates 3D model from given filename and adds it to the scene.
 * \param filename name of the 3d model file
 * \param properties game object properties that will be written to irr scene node.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorLevel::SetElementAtHand(CGameObject* go)
{
	//If there was element under hand and another one was picked from the tree
	if(m_bElementAtHand && m_SelectedGameObject)
	{
		_eraseGameObject(m_SelectedGameObject->getID());
		//delete the bastard
		m_SelectedGameObject->remove();
		m_SelectedGameObject = NULL;
		m_bElementAtHand = false;
	}

	//turn off lights (and make scene bright!)
	DelightAllNodes();

	IAnimatedMesh* m = m_EditorManager->getSceneMngr()->getMesh(stringc(go->path + go->mesh).c_str());
	if (m)
	{
		m_bElementAtHand = true;
		m_SelectedGameObject = m_EditorManager->getSceneMngr()->addAnimatedMeshSceneNode(m);
		m_SelectedBox = m_SelectedGameObject->getBoundingBox();
		m_SelectedGameObject->setMaterialFlag(EMF_LIGHTING, false);
		((IAnimatedMeshSceneNode*)m_SelectedGameObject)->setAnimationSpeed(10);
		if(go->isAnimated)
		{
			//set model to idle animation
			((IAnimatedMeshSceneNode*)m_SelectedGameObject)->setFrameLoop(go->getAnimStart("Idle"),go->getAnimEnd("Idle"));
		}
		if(go->isInvisible)
		{
			/*IMeshBuffer *mb = m->getMeshBuffer(0);
			S3DVertex* v = (S3DVertex*)mb->getVertices();
			for (u32 x = 0; x < mb->getVertexCount(); x++) {
				v[x].Color.setAlpha(12);
			}*/
			//set model semi transparent, to indicate it will be invisible in game
			//IMeshManipulator *manipulator = m_EditorManager->getSceneMngr()->getMeshManipulator();
			//manipulator->setVertexColorAlpha(m, 0);
			//m_SelectedGameObject->setMaterialType(EMT_TRANSPARENT_VERTEX_ALPHA); 
			m_SelectedGameObject->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		}
		if(go->isIllusion)
		{
			//set model semi transparent, to indicate it will be invisible in game
			m_SelectedGameObject->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		}
		//m_SelectedGameObject->setDebugDataVisible(scene::EDS_BBOX); 
		ITriangleSelector *selector = m_EditorManager->getSceneMngr()->createTriangleSelector(((IAnimatedMeshSceneNode*)m_SelectedGameObject)->getMesh(), m_SelectedGameObject);
		m_SelectedGameObject->setTriangleSelector(selector);
		
		//position object under mouse pointer (where mouse ray intersects terrain)
		m_bMoveSelectedNode = true;
		triangle3df instersection_triangle;
		const ISceneNode* hitNode = 0;
		vector3df instersection_point; // the click intersection point
		line3d<f32> picking_line = m_EditorManager->getSceneMngr()->getSceneCollisionManager()->getRayFromScreenCoordinates(m_EditorManager->getDevice()->getCursorControl()->getPosition());

		if(m_EditorManager->getSceneMngr()->getSceneCollisionManager()->getCollisionPoint(picking_line,m_LevelMetaTriangleSelector,instersection_point,instersection_triangle,hitNode))
		{
			m_SelectedGameObject->setPosition(instersection_point);
		}
		else
		{
			//intersect with invisible horizontal plane at Y=0
			plane3df horizontalPlane; // plane horizontal to the character
			horizontalPlane.setPlane(vector3df(0.f, -1.f, 0.f), 0);

			if(horizontalPlane.getIntersectionWithLine(picking_line.start, picking_line.getVector(), instersection_point))
			{
				m_SelectedGameObject->setPosition(instersection_point);
			}
		}

		//save object properties
		m_SelectedGameObject->setName(go->name.c_str());
		m_SelectedGameObject->setID(m_EditorManager->m_ID);
		go->id = m_EditorManager->m_ID;
		m_ListOfGameObjects.push_back(go);
	}
	else
	{
		//allert message: "Model could not be loaded"
		stringw message = "Model ";
		message += go->mesh;
		message += " can not be loaded.";
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"Error loading model!", message.c_str());
		return false;
	}

	return true;
}

void CEditorLevel::AddGameObjectToLevel(CGameObject* go)
{
	IAnimatedMesh* m = m_EditorManager->getSceneMngr()->getMesh(stringc(go->path + go->mesh).c_str());
	if (m)
	{
		m_SelectedGameObject = m_EditorManager->getSceneMngr()->addAnimatedMeshSceneNode(m);
		m_SelectedBox = m_SelectedGameObject->getBoundingBox();
		m_SelectedGameObject->setMaterialFlag(EMF_LIGHTING, false);
		((IAnimatedMeshSceneNode*)m_SelectedGameObject)->setAnimationSpeed(10);
		if(go->isAnimated)
		{
			//set model to idle animation
			((IAnimatedMeshSceneNode*)m_SelectedGameObject)->setFrameLoop(go->getAnimStart("Idle"),go->getAnimEnd("Idle"));
		}
		if(go->isInvisible)
		{
			//set semi-transparent material
			m_SelectedGameObject->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		}
		if(go->isIllusion)
		{
			//set model semi transparent, to indicate it will be invisible in game
			m_SelectedGameObject->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		}

		//set triangle selector for colision
		ITriangleSelector *selector = m_EditorManager->getSceneMngr()->createTriangleSelector(((IAnimatedMeshSceneNode*)m_SelectedGameObject)->getMesh(), m_SelectedGameObject);
		m_SelectedGameObject->setTriangleSelector(selector);
		m_LevelMetaTriangleSelector->addTriangleSelector(m_SelectedGameObject->getTriangleSelector());
		
		//set model position from go property
		m_SelectedGameObject->setPosition(go->pos);

		//save object properties
		m_SelectedGameObject->setName(go->name.c_str());
		m_SelectedGameObject->setID(go->id);
		m_ListOfGameObjects.push_back(go);
	}
	else
	{
		//allert message: "Model could not be loaded"
		printf("Model could not be loaded");
	}
}

void CEditorLevel::RemoveGameObjectFromMap(CGameObject* go)
{
	ISceneNode* node = m_EditorManager->getSceneMngr()->getSceneNodeFromId(go->id);
	if(go->isStatic && node->getTriangleSelector())
	{
		m_LevelMetaTriangleSelector->removeTriangleSelector(node->getTriangleSelector());
	}
	_eraseGameObject(go->id);
	//delete the bastard
	node->remove();
	m_SelectedGameObject = NULL;
}

void CEditorLevel::RemovePhotoSessionModel()
{
	if(m_PreviewGameObject)
	{
		m_PreviewGameObject->remove();
		m_PreviewGameObject = 0;
	}
}

/**
 * \brief Creates 3D model from given filename, adds it to the scene, and sends it to photo session.
 * \param filename name of the 3d model file
 * \param properties game object properties that will be written to irr scene node.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorLevel::SendModelToPhotoSession(CGameObject* go)
{
	IAnimatedMesh* m = m_EditorManager->getSceneMngr()->getMesh(stringc(go->path + go->mesh).c_str());
	if (m)
	{
		if(m_PreviewGameObject)
			m_PreviewGameObject->remove();
		m_PreviewGameObject = m_EditorManager->getSceneMngr()->addAnimatedMeshSceneNode(m);
		if(m_PreviewGameObject)
		{
			m_PreviewGameObject->setMaterialFlag(EMF_LIGHTING, false);
			((IAnimatedMeshSceneNode*)m_PreviewGameObject)->setAnimationSpeed(10);
			vector3df radius = m_PreviewGameObject->getBoundingBox().MaxEdge - m_PreviewGameObject->getBoundingBox().getCenter();
			radius *= 2;
			if ((radius.X < 20) && (radius.Y < 20) && (radius.Z < 20))
			{
				m_PreviewGameObject->setPosition(vector3df(0, -10010, -20));
			}
			else if ((radius.X < 50) && (radius.Y < 50) && (radius.Z < 50))
			{
				m_PreviewGameObject->setPosition(vector3df(0, -10020, -40));
			}
			else if ((radius.X < 110) && (radius.Y < 110) && (radius.Z < 110))
			{
				m_PreviewGameObject->setPosition(vector3df(0, -10040, -90));
			}
			else if ((radius.X < 190) && (radius.Y < 190) && (radius.Z < 190))
			{
				m_PreviewGameObject->setPosition(vector3df(0, -10070, -190));
			}
			else
			{
				m_PreviewGameObject->setPosition(vector3df(0, -10090, -390));
			}
		}
	}
	else
	{
		//allert message: "Model could not be loaded"
		stringw message = "Model ";
		message += go->mesh;
		message += " can not be loaded.";
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"Error loading model!", message.c_str());
		return false;
	}

	return true;
}

void CEditorLevel::RemovePickSessionModel()
{
	if(m_PreviewPickGameObject)
	{
		m_PreviewPickGameObject->remove();
		m_PreviewPickGameObject = 0;
	}
}
/**
 * \brief Sends model to pick session.
 * \param go - game object
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorLevel::SendModelToPickSession(CGameObject* go)
{
	IAnimatedMesh* m = m_EditorManager->getSceneMngr()->getMesh(stringc(go->path + go->mesh).c_str());
	if (m)
	{
		if(m_PreviewPickGameObject)
			m_PreviewPickGameObject->remove();
		m_PreviewPickGameObject = m_EditorManager->getSceneMngr()->addAnimatedMeshSceneNode(m);
		if(m_PreviewPickGameObject)
		{
			m_PreviewPickGameObject->setMaterialFlag(EMF_LIGHTING, false);
			vector3df radius = m_PreviewPickGameObject->getBoundingBox().MaxEdge - m_PreviewPickGameObject->getBoundingBox().getCenter();
			radius *= 2;
			if ((radius.X < 20) && (radius.Y < 20) && (radius.Z < 20))
			{
				m_PreviewPickGameObject->setPosition(vector3df(0, -10010, 20));
			}
			else if ((radius.X < 50) && (radius.Y < 50) && (radius.Z < 50))
			{
				m_PreviewPickGameObject->setPosition(vector3df(0, -10020, 40));
			}
			else if ((radius.X < 110) && (radius.Y < 110) && (radius.Z < 110))
			{
				m_PreviewPickGameObject->setPosition(vector3df(0, -10040, 90));
			}
			else if ((radius.X < 190) && (radius.Y < 190) && (radius.Z < 190))
			{
				m_PreviewPickGameObject->setPosition(vector3df(0, -10070, 190));
			}
			else
			{
				m_PreviewPickGameObject->setPosition(vector3df(0, -10090, -390));
			}
		}
	}
	else
	{
		//allert message: "Model could not be loaded"
		stringw message = "Model ";
		message += go->mesh;
		message += " can not be loaded.";
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"Error loading model!", message.c_str());
		return false;
	}

	return true;
}

/**
 * \brief Creates camera in initial position (used for loading map)
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::CreateCamera(vector3df position)
{
	m_LevelCamera = new RTSCamera(m_EditorManager->getDevice(),m_EditorManager->getSceneMngr()->getRootSceneNode(),m_EditorManager->getSceneMngr(),-1,100.0f,10.0f,100.0f);
	m_LevelCamera->setPosition(position);
	//matrix4 projection;
	//projection.buildProjectionMatrixOrthoLH(300, 300, 3, 30000);
	//camera->setProjectionMatrix(projection);
	//camera->setIsOrthogonal(true);
}

/**
 * \brief Insert start position flag.
 * Flag should be invisible in game and marks the spot where player character appears on loading.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::InsertFlag()
{
}

PROCEDURAL_TREE_TYPE getProceduralTreeTypeFromName(stringc name)
{
	if(name == stringc("Aspen Tree")) return LE_PT_ASPEN;
	if(name == stringc("Oak Tree")) return LE_PT_OAK;
	if(name == stringc("Pine Tree")) return LE_PT_PINE;
	
	return LE_PT_WILLOW;
}

PARTICLE_SYSTEM_TYPE getParticleTypeFromName(stringc name)
{
	if(name.equals_ignore_case("Fire")) return LE_PS_FIRE;
	if(name.equals_ignore_case("Teleport")) return LE_PS_TELEPORT;
	if(name.equals_ignore_case("Particles")) return LE_PS_TELEPORT; //temporary
	if(name.equals_ignore_case("Smoke")) return LE_PS_SMOKE;
	if(name.equals_ignore_case("Dust")) return LE_PS_DUST;
	
	return LE_PS_FIRE;
}

CTreeSceneNode* CEditorLevel::createTree(PROCEDURAL_TREE_TYPE treeType)
{
	CTreeSceneNode* tree = new CTreeSceneNode( m_EditorManager->getSceneMngr()->getRootSceneNode(), m_EditorManager->getSceneMngr(), m_EditorManager->m_ID);
    tree->setMaterialFlag( EMF_LIGHTING, false );

	switch (treeType)
	{
		case LE_PT_ASPEN:
		{
			tree->setup( m_AspenTreeGenerator, rand(), 0 );
			tree->getLeafNode()->setMaterialTexture( 0, m_AspenLeafTexture );
			tree->getLeafNode()->setMaterialType( EMT_TRANSPARENT_ALPHA_CHANNEL );
			tree->setMaterialTexture( 0, m_AspenTreeTexture );
			tree->setName("Aspen Tree");
		}
		break;
		case LE_PT_OAK:
		{
			tree->setup( m_OakTreeGenerator, rand(), 0 );
			tree->getLeafNode()->setMaterialTexture( 0, m_OakLeafTexture );
			tree->getLeafNode()->setMaterialType( EMT_TRANSPARENT_ALPHA_CHANNEL );
			tree->setMaterialTexture( 0, m_OakTreeTexture );
			tree->setName("Oak Tree");
		}
		break;
		case LE_PT_PINE:
		{
			tree->setup( m_PineTreeGenerator, rand(), 0 );
			tree->getLeafNode()->setMaterialTexture( 0, m_PineLeafTexture );
			tree->getLeafNode()->setMaterialType( EMT_TRANSPARENT_ALPHA_CHANNEL );
			tree->setMaterialTexture( 0, m_PineTreeTexture );
			tree->setName("Pine Tree");
		}
		break;
		case LE_PT_WILLOW:
		{
			tree->setup( m_WillowTreeGenerator, rand(), 0 );
			tree->getLeafNode()->setMaterialTexture( 0, m_WillowLeafTexture );
			tree->getLeafNode()->setMaterialType( EMT_TRANSPARENT_ALPHA_CHANNEL );
			tree->setMaterialTexture( 0, m_WillowTreeTexture );
			tree->setName("Willow Tree");
		}
		break;
	}

    tree->drop();

	return tree;
}

ISceneNode* CEditorLevel::CreateLight(f32 radius)
{
	ISceneNode* bilboard = m_EditorManager->getSceneMngr()->addBillboardSceneNode(0, core::dimension2d<f32>(50, 50));
	bilboard->setMaterialFlag(video::EMF_LIGHTING, false);
	bilboard->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bilboard->setMaterialTexture(0,	m_EditorManager->getDriver()->getTexture("media/particle1.bmp"));

	// attach light to bilboard
	ISceneNode* light = m_EditorManager->getSceneMngr()->addLightSceneNode(bilboard, vector3df(0,0,0), 	SColorf(1.0f, 0.6f, 0.6f, 1.0f), radius);

	return bilboard;
}

void CEditorLevel::InsertLight()
{
	stringw name = L"Light GO";
	float radius = 500.0f;

	ISceneNode* bilboard = m_EditorManager->getSceneMngr()->addBillboardSceneNode(0, core::dimension2d<f32>(50, 50));
	bilboard->setMaterialFlag(video::EMF_LIGHTING, false);
	bilboard->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bilboard->setMaterialTexture(0,	m_EditorManager->getDriver()->getTexture("media/particle1.bmp"));

	// attach light to bilboard
	ISceneNode* light = m_EditorManager->getSceneMngr()->addLightSceneNode(bilboard, vector3df(0,0,0), 	SColorf(1.0f, 0.6f, 0.6f, 1.0f), radius);

	m_SelectedGameObject = bilboard;
	m_SelectedBox = m_SelectedGameObject->getBoundingBox();

	CGameObject* gameObject = new CGameObject();
	gameObject->mesh = LIGHT_GAME_OBJECT;
	gameObject->name = name;
	gameObject->m_Radius = radius;
	m_SelectedGameObject->setName(name);
	
	gameObject->id = m_EditorManager->m_ID;
	
	m_SelectedGameObject->setID(m_EditorManager->m_ID);//??
	
	gameObject->description = L"Light Node";
	gameObject->script = L"";
	m_ListOfGameObjects.push_back(gameObject);

	m_EditorManager->getGUIManager()->SetProperties(gameObject);

	EnlightAllNodes();

	m_bMoveSelectedNode = true;
}

void CEditorLevel::InsertDancingLight()
{
	stringw name = L"Dancing Light GO";
	float radius = 500.0f;

	ISceneNode* bilboard = m_EditorManager->getSceneMngr()->addBillboardSceneNode(0, core::dimension2d<f32>(50, 50));
	bilboard->setMaterialFlag(video::EMF_LIGHTING, false);
	bilboard->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bilboard->setMaterialTexture(0,	m_EditorManager->getDriver()->getTexture("media/particle1.bmp"));

	// attach light to bilboard
	ISceneNode* light = m_EditorManager->getSceneMngr()->addLightSceneNode(bilboard, vector3df(0,0,0), 	SColorf(1.0f, 0.6f, 0.6f, 1.0f), radius);

	//Vibrating light for fire dancing
	ISceneNodeAnimator* anim = 0;
	anim = m_EditorManager->getSceneMngr()->createFlyCircleAnimator(vector3df(0,0,0),1.0f,0.04f,vector3df(1,0,0));
	bilboard->addAnimator(anim);
	anim->drop();

	m_SelectedGameObject = bilboard;
	m_SelectedBox = m_SelectedGameObject->getBoundingBox();

	CGameObject* gameObject = new CGameObject();
	gameObject->mesh = LIGHT_GAME_OBJECT;
	gameObject->name = name;
	gameObject->m_Radius = radius;
	m_SelectedGameObject->setName(name);
	
	gameObject->id = m_EditorManager->m_ID;
	
	m_SelectedGameObject->setID(m_EditorManager->m_ID);//??
	
	gameObject->description = L"Animated Light Node";
	gameObject->script = L"";
	m_ListOfGameObjects.push_back(gameObject);

	m_EditorManager->getGUIManager()->SetProperties(gameObject);

	EnlightAllNodes();

	m_bMoveSelectedNode = true;
}

IParticleSystemSceneNode* CEditorLevel::CreateParticles(PARTICLE_SYSTEM_TYPE type)
{
	IParticleSystemSceneNode* ps = NULL;

	switch(type)
	{
	case LE_PS_FIRE:
		{
			aabbox3df emiterSize = aabbox3d<f32>(-1,1,-1,1,-1,1);
			ps = InsertParticlesNode(E_EMITERTYPE_SPHERE,emiterSize,vector3df(0.0f,0.1f,0.0f),L"media/fire.bmp","Fire",280,430,20,false);
			IParticleAffector* paf = ps->createFadeOutParticleAffector(SColor(0,0,0,0),300);
			ps->addAffector(paf);
			paf->drop();
		}
		break;
	case LE_PS_TELEPORT:
		{
			aabbox3df emiterSize = aabbox3d<f32>(-20,-2,-20,20,20,20);
			ps = InsertParticlesNode(E_EMITERTYPE_BOX,emiterSize,vector3df(0.0f,0.01f,0.0f),L"media/particle1.bmp","Teleport",80,100,0,false);
			IParticleAffector* paf = ps->createFadeOutParticleAffector();
			ps->addAffector(paf);
			paf->drop();
		}
		break;
	case LE_PS_SMOKE:
		{
		}
		break;
	default:
		{
		}
		break;
	}

	return ps;
}

IParticleSystemSceneNode* CEditorLevel::InsertParticlesNode(TEEmiterType emiterType, aabbox3df emiterSize, vector3df direction, stringc texture, stringc name, s32 emitRateMin, s32 emitRateMax, s32 angle, bool outlineOnly)
{
	// create a particle system
	IParticleSystemSceneNode* ps = m_EditorManager->getSceneMngr()->addParticleSystemSceneNode(false);
	IParticleEmitter* em = 0;

	switch(emiterType)
	{
	case E_EMITERTYPE_BOX:
		{
			em = ps->createBoxEmitter(emiterSize, direction, emitRateMin, emitRateMax,
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	case E_EMITERTYPE_RING:
		{
			em = ps->createRingEmitter(emiterSize.getCenter(), emiterSize.getExtent().X, emiterSize.getExtent().Y, direction, emitRateMin, emitRateMax,
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	case E_EMITERTYPE_SPHERE:
		{
			em = ps->createSphereEmitter(emiterSize.getCenter(), emiterSize.getExtent().X, direction, emitRateMin, emitRateMax, 
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					100,300,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	case E_EMITERTYPE_CYLINDER:
		{
			vector3df normal = vector3df(10.f,10.f,10.f);
			em = ps->createCylinderEmitter(emiterSize.getCenter(), emiterSize.getExtent().X, normal, emiterSize.getExtent().Y, outlineOnly, direction, emitRateMin, emitRateMax, 
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	case E_EMITERTYPE_POINT:
		{
			em = ps->createPointEmitter(direction, emitRateMin, emitRateMax,
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
		break;
	default:
		{
			em = ps->createBoxEmitter(emiterSize, direction, emitRateMin, emitRateMax,
					SColor(0,55,55,55),       // darkest color
					SColor(0,255,255,255),    // brightest color
					3000,12000,angle,           // min and max age, angle
					dimension2df(5.f,5.f),    // min size
					dimension2df(15.f,15.f)); // max size
		}
	}

	if(em == 0)
	{
		printf("Fatal Error creating emiter!\n");
		return NULL; 
	}

	ps->setEmitter(em); // this grabs the emitter
	em->drop(); // so we can drop it here without deleting it

	ps->setMaterialFlag(video::EMF_LIGHTING, false);
	ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	ps->setMaterialTexture(0, m_EditorManager->getDriver()->getTexture(texture));
	ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
	ps->setName(name);

	return ps;
}

void CEditorLevel::InsertParticles(PARTICLE_SYSTEM_TYPE type, TEEmiterType emiterType, aabbox3df emiterSize, vector3df direction, stringc texture, stringc name, s32 emitRateMin, s32 emitRateMax, s32 angle, bool outlineOnly)
{
	// create a particle system
	IParticleSystemSceneNode* ps = 0;

	ps = InsertParticlesNode(emiterType, emiterSize, direction, texture, name, emitRateMin, emitRateMax, angle, outlineOnly);

	switch(type)
	{
	case LE_PS_FIRE:
		{
			scene::IParticleAffector* paf = ps->createFadeOutParticleAffector(SColor(0,0,0,0),300);
			ps->addAffector(paf); // same goes for the affector
			paf->drop();
		}
		break;
	case LE_PS_TELEPORT:
		{
			IParticleAffector* paf = ps->createFadeOutParticleAffector();
			ps->addAffector(paf);
			paf->drop();
		}
		break;
	default:
		{
		}
	}


	ps->setPosition(core::vector3df(0,-60,0));
	ps->setScale(core::vector3df(2,2,2));
	ps->setID(m_EditorManager->m_ID);

	m_SelectedGameObject = ps;
	m_SelectedBox = m_SelectedGameObject->getBoundingBox();

	CGameObject* gameObject = new CGameObject();
	gameObject->mesh = PARTICLE_GAME_OBJECT;
	gameObject->name = name;
	
	gameObject->isArea = true; //area can detect players presence - used for teleports
	gameObject->id = m_EditorManager->m_ID;
	gameObject->description = L"Particle System";
	gameObject->script = L"";
	m_ListOfGameObjects.push_back(gameObject);

	m_EditorManager->getGUIManager()->SetProperties(gameObject);

	m_EditorManager->m_ID++;

	m_bMoveSelectedNode = true;
}

/**
 * \brief Insert procedural tree
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::InsertTree(PROCEDURAL_TREE_TYPE treeType)
{
	m_SelectedGameObject = m_Tree = createTree(treeType);

	m_SelectedBox = m_SelectedGameObject->getBoundingBox();
	//m_SelectedGameObject->setName(treeName.c_str());

	CGameObject* gameObject = new CGameObject();
	gameObject->mesh = PROCEDURAL_TREE_MESH;
	gameObject->name = m_SelectedGameObject->getName();
	/*gameObject->isContainer = false;
	gameObject->isPickable = false;
	gameObject->isTrigger = false;
	gameObject->isNPC = false;
	gameObject->isMonster = false;
	gameObject->isAnchored = false;
	gameObject->isTerrain = false;
	gameObject->isStatic = false; //true should involve triangle selector thingy*/
	gameObject->id = m_EditorManager->m_ID;
	gameObject->description = L"Procedural tree";
	gameObject->script = L"";
	m_ListOfGameObjects.push_back(gameObject);

	m_EditorManager->m_ID++;

	m_bMoveSelectedNode = true;
}

/**
 * \brief Insert procedural tree
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::InsertTerrainGrass(int param)
{
	//m_EditorManager->getSceneMngr()->getT
    //m_Grass = new scene::CGrassPatchSceneNode(terrain, m_EditorManager->getSceneMngr(), -1, core::vector3d<s32>(x,0,z), "grass", heightMap, textureMap, grassMap, wind);
    //m_Grass->setMaterialFlag(video::EMF_LIGHTING,false);
    //m_Grass->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);        
    //m_Grass->setMaterialTexture(0,tex1);
    //m_Grass->drop();

}

/**
 * \brief Insert terrain from heightmap
 */
void CEditorLevel::AddTerrainFromHeightmap(stringc heightmapFilename,
										   stringc textureFilename,
										   stringc detailsFilename,
										   vector3df position,
										   vector3df rotation,
										   vector3df scale,
										   s32 maxLOD,
										   s32 smoothFactor,
										   E_TERRAIN_PATCH_SIZE patchSize)
{
	/*
	Here comes the terrain renderer scene node: We add it just like any 
	other scene node to the scene using ISceneManager::addTerrainSceneNode(). 
	The only parameter we use is a file name to the heightmap we use. A heightmap
	is simply a gray scale texture. The terrain renderer loads it and creates 
	the 3D terrain from it.
	To make the terrain look more big, we change the scale factor of it to (40, 4.4, 40).
	Because we don't have any dynamic lights in the scene, we switch off the lighting,
	and we set the file terrain-texture.jpg as texture for the terrain and 
	detailmap3.jpg as second texture, called detail map. At last, we set
	the scale values for the texture: The first texture will be repeated only one time over 
	the whole terrain, and the second one (detail map) 20 times. 
	*/

	
	ITerrainSceneNode* terrain = m_EditorManager->getSceneMngr()->addTerrainSceneNode(
		heightmapFilename.c_str(),
		0,										// parent node
		m_EditorManager->m_ID,					// node id
		position,								// position
		rotation,								// rotation
		scale,									// scale
		video::SColor ( 255, 255, 255, 255 ),	// vertexColor,
		maxLOD,									// maxLOD
		patchSize,								// patchSize
		smoothFactor							// smoothFactor
		);

	terrain->setMaterialFlag(EMF_LIGHTING, false);
	terrain->setMaterialTexture(0, m_EditorManager->getDriver()->getTexture(textureFilename.c_str()));
	terrain->setMaterialTexture(1, m_EditorManager->getDriver()->getTexture(detailsFilename.c_str()));
	terrain->setMaterialType(EMT_DETAIL_MAP);
	terrain->scaleTexture(1.0f, 20.0f);

	m_SelectedGameObject = terrain;
	m_SelectedBox = m_SelectedGameObject->getBoundingBox();
	m_SelectedGameObject->setName("terrain 0");

	CGameObject* gameObject = new CGameObject();
	gameObject->mesh = L"none";
	gameObject->name = L"terrain 0";
	/*gameObject->isContainer = false;
	gameObject->isPickable = false;
	gameObject->isTrigger = false;
	gameObject->isNPC = false;
	gameObject->isMonster = false;
	gameObject->isAnchored = false;*/
	gameObject->isStatic = true;
	gameObject->isTerrain = true;
	gameObject->id = m_EditorManager->m_ID;
	gameObject->description = L"Heightmap terrain";
	gameObject->script = L"";
	m_ListOfGameObjects.push_back(gameObject);

	m_EditorManager->m_ID++;

	/*
	To be able to do collision with the terrain, we create a triangle selector.
	If you want to know what triangle selectors do, just take a look into the 
	collision tutorial. The terrain triangle selector works together with the
	terrain. To demonstrate this, we create a collision response animator 
	and attach it to the camera, so that the camera will not be able to fly 
	through the terrain.*/
	

	// create triangle selector for the terrain	
	scene::ITriangleSelector* selector = m_EditorManager->getSceneMngr()->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(selector);

	m_LevelMetaTriangleSelector->addTriangleSelector(selector);
	//terrain->setTriangleSelector(selector);
	//m_EditorManager->getSceneMngr()->
	selector->drop();
}

/**
 * \brief This function will become more complex when multiselect gets functional
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorLevel::SingleObjectSelected()
{
	return (m_SelectedGameObject != NULL);
}

/**
 * \brief Clears scene and reinitialize Level parameters.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::InitLevel()
{
	m_SelectedGameObject = NULL;
	m_PreviewGameObject = 0;
	m_bSavedState = true;

	m_LevelCamera = new RTSCamera(m_EditorManager->getDevice(),m_EditorManager->getSceneMngr()->getRootSceneNode(),m_EditorManager->getSceneMngr(),-1,100.0f,10.0f,100.0f);
	m_LevelCamera->setPosition(vector3df(100,300,100));
	//matrix4 projection;
	//projection.buildProjectionMatrixOrthoLH(300, 300, 3, 30000);
	//camera->setProjectionMatrix(projection);
	//camera->setIsOrthogonal(true);

	//Meta selector holds triangles of all static objects, making them part of the terrain
	if(m_LevelMetaTriangleSelector)
		m_LevelMetaTriangleSelector->removeAllTriangleSelectors();
	else
		m_LevelMetaTriangleSelector = m_EditorManager->getSceneMngr()->createMetaTriangleSelector();

	//m_EditorManager->getDriver()->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	// add irrlicht logo
	m_EditorManager->getGUIEnvironment()->addImage(m_EditorManager->getDriver()->getTexture(IRRLOGO_FILE), core::position2d<s32>(770,580));

	//add grid node
	m_Grid = new CGridSceneNode(m_EditorManager->getSceneMngr()->getRootSceneNode(), m_EditorManager->getSceneMngr());
	m_Grid->SetSpacing(50);
	m_Grid->SetSize(6400);
	m_Grid->drop(); 

}

/**
 * \brief Init function stores pointer to CEditorManager and creates camera and default terrain.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorLevel::Init(CEditorManager* edMngr)
{
	m_EditorManager = edMngr;
	m_ListOfGameObjects.clear();
	m_EditorManager->m_ID = 1000;
	m_MapName = "untitled";
	InitLevel();
	
	// This is to generate random trees.
    srand(m_EditorManager->getDevice()->getTimer()->getTime()); 
	io::IXMLReader* xml = m_EditorManager->getDevice()->getFileSystem()->createXMLReader("media/trees/Pine.xml");
	m_PineTreeGenerator = new CTreeGenerator(edMngr->getSceneMngr());
	m_PineTreeGenerator->loadFromXML( xml );
	xml = m_EditorManager->getDevice()->getFileSystem()->createXMLReader("media/trees/Oak.xml");
	m_OakTreeGenerator = new CTreeGenerator(edMngr->getSceneMngr());
	m_OakTreeGenerator->loadFromXML( xml );
	xml = m_EditorManager->getDevice()->getFileSystem()->createXMLReader("media/trees/Aspen.xml");
	m_AspenTreeGenerator = new CTreeGenerator(edMngr->getSceneMngr());
	m_AspenTreeGenerator->loadFromXML( xml );
	xml = m_EditorManager->getDevice()->getFileSystem()->createXMLReader("media/trees/Willow.xml");
	m_WillowTreeGenerator = new CTreeGenerator(edMngr->getSceneMngr());
	m_WillowTreeGenerator->loadFromXML( xml );
	xml->drop();
	m_AspenTreeTexture = m_EditorManager->getDriver()->getTexture("media/trees/AspenBark.png");
	m_AspenLeafTexture = m_EditorManager->getDriver()->getTexture("media/trees/AspenLeaf.png");
	m_OakTreeTexture = m_EditorManager->getDriver()->getTexture("media/trees/OakBark.png");
	m_OakLeafTexture = m_EditorManager->getDriver()->getTexture("media/trees/OakLeaf.png");
	m_PineTreeTexture = m_EditorManager->getDriver()->getTexture("media/trees/PineBark.png");
    m_PineLeafTexture = m_EditorManager->getDriver()->getTexture("media/trees/PineLeaf.png");
	m_WillowTreeTexture = m_EditorManager->getDriver()->getTexture("media/trees/WillowBark.png");
	m_WillowLeafTexture = m_EditorManager->getDriver()->getTexture("media/trees/WillowLeaf.png");

	return true;
}


/**
 * \brief Saves whole scene to irr file using irrlicht function smgr->saveScene(..);
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::OnSaveMap_IrrScene(stringc mapname)
{
	RemovePhotoSessionModel();
	m_Grid->remove();
	m_EditorManager->getSceneMngr()->saveScene(mapname.c_str(),this);
	//add grid node
	m_Grid = new CGridSceneNode(m_EditorManager->getSceneMngr()->getRootSceneNode(), m_EditorManager->getSceneMngr());
	m_Grid->SetSpacing(50);
	m_Grid->SetSize(6400);
	m_Grid->drop(); 
}

/**
 * \brief Writes a scene node attributes to xml file.
 *
 * And calls itself recursivly.
 * Optimizations can be made to reduce file size:
 * Default attributes (eg. Scale: 1,1,1) can be skipped in saving process, and loaded only if
 * different from default (1,1,1).
 *
 * \author Petar Bajic 
 * \date May, 21 2009.
 */
void CEditorLevel::WriteSceneNode(IXMLWriter* writer, ISceneNode* node)
{
	if (!writer || !node)
		return;

	if (node == m_EditorManager->getSceneMngr()->getRootSceneNode())
	{
		writer->writeElement(L"LevelEditorMap", false);
		writer->writeLineBreak();
		writer->writeLineBreak();

		//Save Map Sound and Music
		writer->writeElement(L"MapAmbientSound", true, L"filename", m_LevelMusic_SoundFile.c_str());
		writer->writeLineBreak();
		writer->writeElement(L"MapMusicTheme", true, L"filename", m_LevelMusic_MusicFile.c_str());
		writer->writeLineBreak();
		writer->writeElement(L"MapNarationFile", true, L"filename", m_LevelMusic_NarationFile.c_str());
		writer->writeLineBreak();
		writer->writeLineBreak();
	}
	else if (node->getID() > -1)
	{
		CGameObject* gameObject = _getGameObjectFromID(node->getID());

		writer->writeElement(L"GameObject", false);
		writer->writeLineBreak();

		// write properties
		io::IAttributes* attr = m_EditorManager->getDevice()->getFileSystem()->createEmptyAttributes(m_EditorManager->getDriver());
		attr->addString("Name",node->getName());
		attr->addInt("NameID",gameObject->nameID);
		attr->addInt("ID",node->getID());
		attr->addString("Mesh",gameObject->mesh.c_str());
		attr->addString("Path",gameObject->path.c_str());
		attr->addVector3d("Position",node->getPosition());
		attr->addVector3d("Rotation",node->getRotation());
		attr->addVector3d("Scale",node->getScale());
		if (gameObject->isInvisible)
			//adding attribute isInvisible
			attr->addBool("isInvisible",true);
		if (gameObject->isIllusion)
			//adding attribute isIllusion
			attr->addBool("isIllusion",true);
		if (gameObject->isTrigger)
			//adding atribute isTrigger
			attr->addBool("isTrigger",true);
		if (gameObject->isStatic)
			//adding atribute isStatic
			attr->addBool("isStatic",true);
		if (gameObject->isTerrain)
			//adding atribute isTerrain
			attr->addBool("isTerrain",true);
		if (gameObject->isAnchored)
			//adding atribute isAnchored
			attr->addBool("isAnchored",true);
		if (gameObject->isPickable)
			//adding atribute isPickable
			attr->addBool("isPickable",true);
		if (gameObject->isMonster)
			//adding atribute isMonster
			attr->addBool("isMonster",true);
		if (gameObject->isNPC)
			//adding atribute isNPC
			attr->addBool("isNPC",true);
		if (gameObject->isContainer)
			//adding attribute isContainer
			attr->addBool("isContainer",true);
		if (gameObject->isArea)
			//adding attribute isArea
			attr->addBool("isArea",true);
		if (gameObject->script != stringw(L""))
			attr->addString("Script",gameObject->script.c_str());
		if (gameObject->state != stringw(L""))
			attr->addString("State",gameObject->state.c_str());

		if (gameObject->isMonster)
		{
			//Monster attributes are written only if differ from default values in xml
			if (gameObject->m_Mood != gameObject->m_Mood_Default)
				attr->addInt("Mood", gameObject->m_Mood);
			if (gameObject->m_Radius != gameObject->m_Radius_Default)
				attr->addFloat("Radius", gameObject->m_Radius);
		}

		if (gameObject->mesh.equals_ignore_case(LIGHT_GAME_OBJECT))
			attr->addFloat("Radius", gameObject->m_Radius);

		attr->write(writer);
		//writer->writeLineBreak();
		attr->drop();

		//Monster attributes are written only if differ from xml default values
		if (gameObject->isMonster)
		{
			for(u32 i=0; i<gameObject->m_ListOfAbilities.size(); i++)
			{
				s32 value = gameObject->m_ListOfAbilities[i].value;
				s32 max = gameObject->m_ListOfAbilities[i].max;
				s32 defaultValue = gameObject->m_ListOfAbilities_Default[i].value;
				if(value != defaultValue)
				{
					writer->writeElement(L"Ability",true,L"name",gameObject->m_ListOfAbilities[i].abilityName.c_str(), L"value",stringw(value).c_str(),L"max",stringw((value>max)?value:max).c_str());
					writer->writeLineBreak();
				}
			}

			for(u32 i=0; i<gameObject->m_ListOfSkills.size(); i++)
			{
				s32 min = gameObject->m_ListOfSkills[i].min;
				s32 defaultMin = gameObject->m_ListOfSkills_Default[i].min;
				s32 max = gameObject->m_ListOfSkills[i].max;
				s32 defaultMax = gameObject->m_ListOfSkills_Default[i].max;
				if((min != defaultMin) || (max != defaultMax))
				{
					writer->writeElement(L"Skill",true,L"name",gameObject->m_ListOfSkills[i].skillName.c_str(), L"min",stringw(min).c_str(),L"max",stringw(max).c_str());
					writer->writeLineBreak();
				}
			}
		}

		//Write container content
		if (gameObject->isContainer)
		{
			writer->writeElement(L"Container",false); 
			writer->writeLineBreak();
			for ( s32 index = 0; index < gameObject->GetNumberOfPickableItems(); index++)
			{
				writer->writeElement(L"Pickables",true,L"root",gameObject->GetPickableItemRoot(index).c_str(), L"id", stringw(gameObject->GetPickableItemID(index)).c_str());
				writer->writeLineBreak();
			}
			writer->writeClosingTag(L"Container"); 
			writer->writeLineBreak();
		}
	}

	// write children

	core::list<ISceneNode*>::ConstIterator it = node->getChildren().begin();
	for (; it != node->getChildren().end(); ++it)
		WriteSceneNode(writer, (*it));

	if ((node == m_EditorManager->getSceneMngr()->getRootSceneNode())||(node->getID() > -1))
	{
		if (node == m_EditorManager->getSceneMngr()->getRootSceneNode())
		{
			writer->writeClosingTag(L"LevelEditorMap");
		}
		else
		{
			writer->writeClosingTag(L"GameObject");
		}
		writer->writeLineBreak();
		writer->writeLineBreak();
	}
}

/**
 * \brief Reads a scene nodes from xml file.
 *
 * \author Petar Bajic 
 * \date May, 21 2009.
 */
void CEditorLevel::ReadSceneNode(IXMLReader* reader)
{
	CGameObject* gameObject;

	//loop through scene nodes in xml
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				//Load attributes
				if (stringw("attributes") == reader->getNodeName())
				{
					//load node attributes
					IAttributes* attr = m_EditorManager->getDevice()->getFileSystem()->createEmptyAttributes(m_EditorManager->getDriver());
					attr->read(reader);
					stringc meshPath = attr->getAttributeAsString("Path");
					meshPath += attr->getAttributeAsString("Mesh");
					
					ISceneNode* node = 0;
					if(meshPath == stringc(PROCEDURAL_TREE_MESH))
					{
						//recreate procedural mesh here
						gameObject = new CGameObject();
						gameObject->mesh = PROCEDURAL_TREE_MESH;
						node = createTree(getProceduralTreeTypeFromName(attr->getAttributeAsString("Name")));
					}
					else if(meshPath == stringc(PARTICLE_GAME_OBJECT))
					{
						//recreate particle system here
						gameObject = new CGameObject();
						gameObject->mesh = PARTICLE_GAME_OBJECT;
						node = CreateParticles(getParticleTypeFromName(attr->getAttributeAsString("Name")));
					}
					else if(meshPath == stringc(LIGHT_GAME_OBJECT))
					{
						//Insert light
						gameObject = new CGameObject();
						gameObject->mesh = LIGHT_GAME_OBJECT;
						f32 radius = attr->getAttributeAsFloat("Radius");
						node = CreateLight(radius);
						if(attr->getAttributeAsString("Name").equals_ignore_case("Dancing Light GO"))
						{
							//Vibrating light for fire dancing
							ISceneNodeAnimator* anim = 0;
							anim = m_EditorManager->getSceneMngr()->createFlyCircleAnimator(attr->getAttributeAsVector3d("Position"),1.0f,0.04f,vector3df(1,0,0));
							node->addAnimator(anim);
							anim->drop();
						}
					}
					else
					{
						//load mesh from file
						IAnimatedMesh* m = m_EditorManager->getSceneMngr()->getMesh(meshPath.c_str());
						if(m)
						{
							node = m_EditorManager->getSceneMngr()->addAnimatedMeshSceneNode(m);
							node->setMaterialFlag(EMF_LIGHTING, false);
							((IAnimatedMeshSceneNode*)node)->setAnimationSpeed(10);
							ITriangleSelector *selector = m_EditorManager->getSceneMngr()->createTriangleSelector(((IAnimatedMeshSceneNode*)node)->getMesh(), node);
							node->setTriangleSelector(selector);
							if(attr->getAttributeAsBool("isStatic"))
							{
								m_LevelMetaTriangleSelector->addTriangleSelector(node->getTriangleSelector());
							}
						}
					}
					if (node)
					{
						//adding new game object to the map
						gameObject = new CGameObject();
						gameObject->SetDriver(m_EditorManager->getDriver());
						//instead of using setDriver maybe use another constructor that takes driver like this:
						//gameObject = new CGameObject(path,name,false,m_EditorManager->getDriver());

						//Load properties from xml file
						stringw rootName = Util_GetRootPathFromPath(meshPath);
						stringw xmlProperties = rootName + L".xml";
						IXMLReader* xml = m_EditorManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());

						if(xml)
						{
							gameObject->LoadPropertiesFromXMLFile(xml);
						}

						gameObject->script = attr->getAttributeAsStringW("Script");
						gameObject->state = attr->getAttributeAsStringW("State");
						gameObject->id = attr->getAttributeAsInt("ID");
						gameObject->path = attr->getAttributeAsString("Path");
						gameObject->mesh = attr->getAttributeAsString("Mesh");
						gameObject->root = Util_GetRootNameFromPath(meshPath);

						s32 nameID = attr->getAttributeAsInt("NameID");
						if(nameID == 0)
						{
							//just for keeping backward compatibility
							gameObject->name = attr->getAttributeAsString("Name");
							node->setName(gameObject->name);
						}
						else
						{
							//get translated name from given ID.
							node->setName(m_EditorManager->getTranslatedString(nameID));
							gameObject->nameID = nameID;
							gameObject->name = node->getName();
						}
						node->setID(gameObject->id);
						node->setPosition(attr->getAttributeAsVector3d("Position"));
						node->setRotation(attr->getAttributeAsVector3d("Rotation"));
						node->setScale(attr->getAttributeAsVector3d("Scale"));

						gameObject->pos = attr->getAttributeAsVector3d("Position");
						gameObject->rot = attr->getAttributeAsVector3d("Rotation");
						gameObject->scale = attr->getAttributeAsVector3d("Scale");
						gameObject->isTerrain = attr->getAttributeAsBool("isTerrain");
						gameObject->isContainer = attr->getAttributeAsBool("isContainer");
						gameObject->isArea = attr->getAttributeAsBool("isArea");
						gameObject->isAnchored = attr->getAttributeAsBool("isAnchored");
						//gameObject->isAnimated = attr->getAttributeAsBool("isAnimated"); //this is not saved to map, but read from xml config file.
						gameObject->isInvisible = attr->getAttributeAsBool("isInvisible");
						gameObject->isIllusion = attr->getAttributeAsBool("isIllusion");
						gameObject->isNPC = attr->getAttributeAsBool("isNPC");
						gameObject->isMonster = attr->getAttributeAsBool("isMonster");
						gameObject->isPickable = attr->getAttributeAsBool("isPickable");
						gameObject->isTrigger = attr->getAttributeAsBool("isTrigger");
						gameObject->isStatic = attr->getAttributeAsBool("isStatic");
						//Monster attributes are written only if differ from default values in xml
						if(attr->existsAttribute("Mood"))
							gameObject->m_Mood = (eMood)attr->getAttributeAsInt("Mood");
						if(attr->existsAttribute("Radius"))
							gameObject->m_Radius = attr->getAttributeAsFloat("Radius");

						if(gameObject->isInvisible)
						{
							//Set model semi transparent. If it is supposed to be invisible in game, we want to see it a little in editor.
							node->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
						}
						if(gameObject->isIllusion)
						{
							//Set model semi transparent. If it is supposed to be visible in game, but we want to indicate it is illusion.
							node->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
						}

						if(gameObject->isAnimated)
						{
							//set model to idle animation
							((IAnimatedMeshSceneNode*)node)->setFrameLoop(gameObject->getAnimStart("Idle"),gameObject->getAnimEnd("Idle"));
						}

						m_ListOfGameObjects.push_back(gameObject);

						//add node to gui scene tree
						m_EditorManager->getGUIManager()->AddNodeToSceneTree(node->getID(), node->getName());

						//TODO ID handling on map loading to be revisited...
						if(m_EditorManager->m_ID <= gameObject->id) m_EditorManager->m_ID = gameObject->id + 1;
					}
				}
				else if (stringw("Ability") == reader->getNodeName())
				{
					stringc name = reader->getAttributeValue(L"name");
					s32 value = reader->getAttributeValueAsInt(L"value");
					s32 max = reader->getAttributeValueAsInt(L"max");
					s32 min = reader->getAttributeValueAsInt(L"min");
					gameObject->setAbilityValues(name,value,min,max);
				}
				else if (stringw("Skill") == reader->getNodeName())
				{
					stringc name = reader->getAttributeValue(L"name");
					s32 min = reader->getAttributeValueAsInt(L"min");
					s32 max = reader->getAttributeValueAsInt(L"max");
					gameObject->setSkillValues(name,min,max);
				}
				//Load container content from xml file to GameObjects list of pickable items
				else if (stringw("Pickables") == reader->getNodeName())
				{
					//Add this element
					stringc root = reader->getAttributeValue(L"root");
					s32 id = reader->getAttributeValueAsInt(L"id");
					stringw xmlProperties = root + L".xml";
					IXMLReader* xml = m_EditorManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());
					CGameObject* pick = new CGameObject(root,id,xml,m_EditorManager->getDriver());
					gameObject->AddPickableItem(pick);
					
					//TODO ID handling on map loading to be revisited...
					if(m_EditorManager->m_ID <= id) m_EditorManager->m_ID = id + 1;
				}

				//Load Map Sound and Music
				else if (stringw("MapAmbientSound") == reader->getNodeName())
				{
					m_LevelMusic_SoundFile = reader->getAttributeValue(L"filename");
					//m_MapAmbientSoundLoop = true; //could be read from map, but no need right now
				}
				else if (stringw("MapMusicTheme") == reader->getNodeName())
				{
					m_LevelMusic_MusicFile = reader->getAttributeValue(L"filename");
					//m_MapMusicThemeLoop = true; //reader->getAttributeAsBool(L"loop");
				}
				else if (stringw("MapNarationFile") == reader->getNodeName())
				{
					m_LevelMusic_NarationFile = reader->getAttributeValue(L"filename");
					//m_MapMusicThemeLoop = true; //reader->getAttributeAsBool(L"loop");
				}

			}
			break;
		}
	}


	//load mesh to scene
	//set scene node attributes

	/*if(userData->getAttributeAsBool("isContainer"))
	{
		gameObject->isContainer = true;

		//Load Existing Container Content (if any)
		stringc container_filename = "CC_";
		container_filename += m_MapName.c_str();
		container_filename += "_";
		container_filename += sceneNode->getID();
		container_filename += ".xml";
		io::IXMLReader* xml = m_EditorManager->getDevice()->getFileSystem()->createXMLReader(container_filename.c_str());
		while(xml && xml->read())
		{
			//Load container content from xml file to GameObjects list of pickable items
			if (core::stringw("Pickables") == xml->getNodeName())
			{
				//Add this element
				stringc item = xml->getAttributeValue(L"iconname");
				gameObject->AddPickableItem(item);
			}
		}

		if (xml)
			xml->drop(); // don't forget to delete the xml reader
	}
	if(userData->getAttributeAsBool("isPickable"))
	{
		gameObject->isPickable = true;
	}
	if(userData->getAttributeAsBool("isStatic"))
	{
		gameObject->isStatic = true;
		//if static, add its triangles to Triangle Selector
		ITriangleSelector* selector = 0;
		if(sceneNode->getType() == ESNT_TERRAIN)
		{
			selector = m_EditorManager->getSceneMngr()->createTerrainTriangleSelector((ITerrainSceneNode*)sceneNode, 0);
		}
		else //animated mesh
		{
			selector = m_EditorManager->getSceneMngr()->createTriangleSelector(((IAnimatedMeshSceneNode*)sceneNode)->getMesh(), sceneNode);
		}
		sceneNode->setTriangleSelector(selector);
		m_LevelMetaTriangleSelector->addTriangleSelector(sceneNode->getTriangleSelector());
	}
	if(userData->getAttributeAsBool("isTrigger"))
	{
		gameObject->isTrigger = true;
	}
	if(userData->getAttributeAsBool("isNPC"))
	{
		gameObject->isNPC = true;
	}
	gameObject->script = userData->getAttributeAsStringW("Script");
	gameObject->state = userData->getAttributeAsStringW("State");

	gameObject->id = m_EditorManager->m_ID;
	m_ListOfGameObjects.push_back(gameObject);
	if (m_EditorManager->m_ID != sceneNode->getID())
	{
		sceneNode->setID(m_EditorManager->m_ID); //This should never happen
		stringw message = "Node IDs are messed up. Map might not work properly.";
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"Error loading map!", message.c_str());
	}

	//add node to gui scene tree
	m_EditorManager->getGUIManager()->InsertElementToTreeofSceneNodes(m_EditorManager->m_ID, sceneNode->getName());

	*/

}

/**
 * \brief Saves level for game use.
 *
 * Static objects are saved in one file.
 * Pickables are saved separately.
 * Monsters are saved separately.
 *
 * On game->save changed objects need to be saved 
 * Even static objects can be moved around. There are situations when script action 
 * can move static object (tree falls down to bridge the chasm, or house dissapears).
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::OnSaveMap()
{
	//write all nodes to one file for now
	IXMLWriter* writer = m_EditorManager->getDevice()->getFileSystem()->createXMLWriter(m_MapName.c_str());
	writer->writeXMLHeader();
	WriteSceneNode(writer, m_EditorManager->getSceneMngr()->getRootSceneNode());
	writer->drop();

}

void CEditorLevel::OnLoadMap(stringc filename)
{
	SetMapName(filename);
	//cleaning old map
	RemovePickSessionModel();
	RemovePhotoSessionModel();
	m_EditorManager->getSceneMngr()->clear();
	m_LevelMetaTriangleSelector->removeAllTriangleSelectors();
	m_ListOfGameObjects.clear();
	m_EditorManager->m_ID = 200;//TODO: !!! check this

	//InitLevel();
	m_SelectedGameObject = NULL;
	m_PreviewGameObject = 0;
	m_bSavedState = true;

	m_LevelCamera = new RTSCamera(m_EditorManager->getDevice(),m_EditorManager->getSceneMngr()->getRootSceneNode(),m_EditorManager->getSceneMngr(),-1,100.0f,10.0f,100.0f);
	m_LevelCamera->setPosition(vector3df(100,300,100));
	
	// add irrlicht logo
	m_EditorManager->getGUIEnvironment()->addImage(m_EditorManager->getDriver()->getTexture(IRRLOGO_FILE), core::position2d<s32>(770,580));
	//add grid node
	m_Grid = new CGridSceneNode(m_EditorManager->getSceneMngr()->getRootSceneNode(), m_EditorManager->getSceneMngr());
	m_Grid->SetSpacing(50);
	m_Grid->SetSize(6400);
	m_Grid->drop();

	IXMLReader* reader = m_EditorManager->getDevice()->getFileSystem()->createXMLReader(filename.c_str());
	if(reader)
	{
		ReadSceneNode(reader);
		reader->drop();
	}
	//Name = in->getAttributeAsString("Name");
	//ID = in->getAttributeAsInt("Id");

	//setPosition(in->getAttributeAsVector3d("Position"));
	//setRotation(in->getAttributeAsVector3d("Rotation"));
	//setScale(in->getAttributeAsVector3d("Scale"));

	//isInvisible = in->getAttributeAsBool("isInvisible");

}

/**
 * \brief Clears all data from scene and initializes level.
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorLevel::OnNewMap()
{
	//cleaning old map
	RemovePickSessionModel();
	RemovePhotoSessionModel();
	m_EditorManager->getSceneMngr()->clear();
	m_ListOfGameObjects.clear();
	m_EditorManager->m_ID = 100;
	m_MapName = "untitled";
	InitLevel();
}

void CEditorLevel::OnLoadMap_IrrScene(stringc filename)
{
	SetMapName(filename);
	//cleaning old map
	RemovePickSessionModel();
	RemovePhotoSessionModel();
	m_EditorManager->getSceneMngr()->clear();
	m_LevelMetaTriangleSelector->removeAllTriangleSelectors();
	m_ListOfGameObjects.clear();
	m_EditorManager->m_ID = 200;

	if (filename.find(".irr") > -1)
	{
		if (m_EditorManager->getSceneMngr()->loadScene(filename.c_str(),this))
		{
			//remove existing camera, if any
			ICameraSceneNode* cam = m_EditorManager->getSceneMngr()->getActiveCamera();
			if(cam) cam->remove();

			//InitLevel();
			m_SelectedGameObject = NULL;
			m_PreviewGameObject = 0;
			m_bSavedState = true;

			m_LevelCamera = new RTSCamera(m_EditorManager->getDevice(),m_EditorManager->getSceneMngr()->getRootSceneNode(),m_EditorManager->getSceneMngr(),-1,100.0f,10.0f,100.0f);
			m_LevelCamera->setPosition(vector3df(100,300,100));
			// add irrlicht logo
			m_EditorManager->getGUIEnvironment()->addImage(m_EditorManager->getDriver()->getTexture(IRRLOGO_FILE), core::position2d<s32>(770,580));
			//add grid node
			m_Grid = new CGridSceneNode(m_EditorManager->getSceneMngr()->getRootSceneNode(), m_EditorManager->getSceneMngr());
			m_Grid->SetSpacing(50);
			m_Grid->SetSize(6400);
			m_Grid->drop(); 
		}
	}
	else
	{
		stringw message  = "Could not load map! Make sure you pick file with .irr extension.";
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"Error Loading", message.c_str());
	}
}


//! Returns the scene node, which is currently visible under the overgiven
//! screencoordinates, viewed from the currently active camera.
ISceneNode* CEditorLevel::getSceneNodeFromScreenCoordinatesBBB(core::position2d<s32> pos, ISceneManager* smgr)
{
	core::line3d<f32> ln = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates(pos, 0);

	if ( ln.start == ln.end )
		return 0;

	return getSceneNodeFromRayBBB(ln, 0, false, smgr);
}



//! Returns the nearest scene node which collides with a 3d ray and
//! which id matches a bitmask.
ISceneNode* CEditorLevel::getSceneNodeFromRayBBB(core::line3d<f32> ray,s32 idBitMask,bool bNoDebugObjects, ISceneManager* smgr)
{
	ISceneNode* best = 0;
	f32 dist = FLT_MAX;

	getPickedNodeBBB(smgr->getRootSceneNode(), ray, 
		idBitMask, bNoDebugObjects, dist, best);

	return best;
}


//! recursive method for going through all scene nodes
void CEditorLevel::getPickedNodeBBB(ISceneNode* root, const core::line3df& ray, s32 bits, bool bNoDebugObjects, f32& outbestdistance, ISceneNode*& outbestnode)
{
   core::vector3df edges[8];

   const core::list<ISceneNode*>& children = root->getChildren();

   core::list<ISceneNode*>::ConstIterator it = children.begin();
   for (; it != children.end(); ++it)
   {
      ISceneNode* current = *it;

      if (current->isVisible() &&
          (bNoDebugObjects ? !current->isDebugObject() : true) &&
          (bits==0 || (bits != 0 && (current->getID() & bits))))
      {
         // get world to object space transform
         core::matrix4 mat;
         if (!current->getAbsoluteTransformation().getInverse(mat))
            continue;

         // transform vector from world space to object space
         core::line3df line(ray);
		 mat.transformVect(line.start);
		 mat.transformVect(line.end);


         const core::aabbox3df& box = current->getBoundingBox();

		 s32 idnj = current->getID();
		 if(idnj >= 0 && !(_getGameObjectFromID(current->getID())->isAnchored))
		 {
			 // do intersection test in object space
			 if (box.intersectsWithLine(line))
			 {
				box.getEdges(edges);
				f32 distance = 0.0f;

				for (s32 e=0; e<8; ++e)
				{
				   f32 t = edges[e].getDistanceFromSQ(line.start); /*What the fuck? I changed this line to line, and it turned back to ray! If we want to pick topmost object, here better be line.start staying!!!*/
				   if (t > distance)
					  distance = t;
				}

				if (distance < outbestdistance)
				{
				   outbestnode = current;
				   outbestdistance = distance;
				}
			 }
		 }
      }

      getPickedNodeBBB(current, ray, bits, bNoDebugObjects, outbestdistance, outbestnode);
   }
} 


vector3df CEditorLevel::GetIntersectionPoint(f32 minHeight)
{
	vector3df instersection_point = vector3df(0,0,0);
	triangle3df instersection_triangle;
	line3d<f32> picking_line = m_EditorManager->getSceneMngr()->getSceneCollisionManager()->getRayFromScreenCoordinates(m_EditorManager->getDevice()->getCursorControl()->getPosition());
	const ISceneNode* hitNode;
	if(m_EditorManager->getSceneMngr()->getSceneCollisionManager()->getCollisionPoint(picking_line,m_LevelMetaTriangleSelector,instersection_point,instersection_triangle,hitNode))
	{
		//if lie intersects with node at Y loser then minimum height, then raise it
		if(instersection_point.Y < minHeight)
		{
			//intersect with invisible horizontal plane at Y=minHeight
			plane3df horizontalPlane; // plane horizontal to the character
			horizontalPlane.setPlane(vector3df(0.f,-1.f,0.f), minHeight);
			if(horizontalPlane.getIntersectionWithLine(picking_line.start, picking_line.getVector(), instersection_point))
			{
				return instersection_point;
			}
		}
		else
		{
			return instersection_point;
		}
	}
	else
	{
		//intersect with invisible horizontal plane at Y=minHeight
		plane3df horizontalPlane; // plane horizontal to the character
		horizontalPlane.setPlane(vector3df(0.f,-1.f,0.f), minHeight);
		if(horizontalPlane.getIntersectionWithLine(picking_line.start, picking_line.getVector(), instersection_point))
		{
			return instersection_point;
		}
	}

	return instersection_point;
}

/**
 * Find element with given id in the scene and mark it selected.
 */
void CEditorLevel::SetElementSelected(int id)
{
	if(id >= 0)
	{
		m_SelectedGameObject = m_EditorManager->getSceneMngr()->getSceneNodeFromId(id);
		if(m_SelectedGameObject)
		{
			CGameObject* gameObject = _getGameObjectFromID(id);
			m_SelectedBox = m_SelectedGameObject->getBoundingBox();
			m_CurrentZoom = m_SelectedGameObject->getScale();
			//tell gui to change properties to this selected item
			m_EditorManager->getGUIManager()->SetProperties(gameObject);
		}
	}
}

/**
 * Set selected element to be Anchored (immobile, not selectable)
 */
void CEditorLevel::SetElementAnchored(bool anchored)
{
	if(m_SelectedGameObject)
	{
		_getGameObjectFromID(m_SelectedGameObject->getID())->isAnchored = anchored;
	}
}

/**
 * Set selected element to be Terrain (player will walk on it)
 */
void CEditorLevel::SetElementTerrain(bool terrain)
{
	if(m_SelectedGameObject)
	{
		_getGameObjectFromID(m_SelectedGameObject->getID())->isTerrain = terrain;
	}
}

/**
 * Set selected element to be Visible or not (semitransparent in editor)
 * Invisible object are invisible in Game, but player can somehow react with them.
 * For example, invisible boundary, clickable area, wrapper for some thin or small object, etc.
 */
void CEditorLevel::SetElementVisible(bool invisible)
{
	if(m_SelectedGameObject)
	{
		_getGameObjectFromID(m_SelectedGameObject->getID())->isInvisible = invisible;
		if(invisible)
		{
			m_SelectedGameObject->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		}
		else
		{
			m_SelectedGameObject->setMaterialType(EMT_SOLID);
		}
	}
}

/**
 * Set selected element to be Illusion or not (semitransparent in editor)
 * Illusion are fully visible in the Game but you can pass right through them.
 */
void CEditorLevel::SetElementIllusion(bool illusion)
{
	if(m_SelectedGameObject)
	{
		_getGameObjectFromID(m_SelectedGameObject->getID())->isIllusion = illusion;
		if(illusion)
		{
			m_SelectedGameObject->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		}
		else
		{
			m_SelectedGameObject->setMaterialType(EMT_SOLID);
		}
	}
}

/**
 * Set selected element name
 */
void CEditorLevel::SetSelectedElement_Name(stringw name)
{
	if(m_SelectedGameObject)
	{
		m_SelectedGameObject->setName(name.c_str());
	}
}

/**
 * Set position of selected element on the given axis
 */
void CEditorLevel::SetPositionSelectedElement(int axis, float value)
{
	if(m_SelectedGameObject)
	{
		switch(axis)
		{
		case LE_AXIS_X:
			{
				//Set value of X coordinate
				vector3df newPos = m_SelectedGameObject->getPosition();
				newPos.X = value;
				m_SelectedGameObject->setPosition(newPos);
			}
			break;
		case LE_AXIS_Y:
			{
				//Set value of Y coordinate
				vector3df newPos = m_SelectedGameObject->getPosition();
				newPos.Y = value;
				m_SelectedGameObject->setPosition(newPos);
			}
			break;
		case LE_AXIS_Z:
			{
				//Set value of Z coordinate
				vector3df newPos = m_SelectedGameObject->getPosition();
				newPos.Z = value;
				m_SelectedGameObject->setPosition(newPos);
			}
			break;
		}
	}
}

/**
 * Set rotation of selected element on the given axis
 */
void CEditorLevel::SetRotationSelectedElement(int axis, float value)
{
	if(m_SelectedGameObject)
	{
		switch(axis)
		{
		case LE_AXIS_X:
			{
				//Set value of X coordinate
				vector3df newRot = m_SelectedGameObject->getRotation();
				newRot.X = value;
				m_SelectedGameObject->setRotation(newRot);
			}
			break;
		case LE_AXIS_Y:
			{
				//Set value of Y coordinate
				vector3df newRot = m_SelectedGameObject->getRotation();
				newRot.Y = value;
				m_SelectedGameObject->setRotation(newRot);
			}
			break;
		case LE_AXIS_Z:
			{
				//Set value of Z coordinate
				vector3df newRot = m_SelectedGameObject->getRotation();
				newRot.Z = value;
				m_SelectedGameObject->setRotation(newRot);
			}
			break;
		}
	}
}

/**
 * Move selected element along the given axis by the given amount
 */
void CEditorLevel::MoveSelectedElement(int axis, int amount)
{
	if(m_SelectedGameObject)
	{
		vector3df newPos;

		CGameObject* go = _getGameObjectFromID(m_SelectedGameObject->getID());
		if(stringc(m_SelectedGameObject->getName()).equals_ignore_case("Dancing Light GO"))
		{
			//For dancing light, we need to remove animator because it won't allow light to be moved
			m_SelectedGameObject->removeAnimators();
			//For dancing light, we need to get position from game object, because scene node is moving about, and GO stores center position.
			newPos = go->pos;
		}
		else
		{
			newPos = m_SelectedGameObject->getPosition();
		}

		switch(axis)
		{
		case LE_AXIS_X:
			{
				//Move model up the X axis
				newPos.X += amount;
				m_SelectedGameObject->setPosition(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
			}
			break;
		case LE_AXIS_Y:
			{
				//Move model up the Y axis
				newPos.Y += amount;
				m_SelectedGameObject->setPosition(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
			}
			break;
		case LE_AXIS_Z:
			{
				//Move model up the Z axis
				newPos.Z += amount;
				m_SelectedGameObject->setPosition(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
			}
			break;
		}

		if(stringc(m_SelectedGameObject->getName()).equals_ignore_case("Dancing Light GO"))
		{
			//set new center position to go
			go->pos = newPos;
			//recreate animator
			ISceneNodeAnimator* anim = 0;
			anim = m_EditorManager->getSceneMngr()->createFlyCircleAnimator(newPos,1.0f,0.04f,vector3df(1,0,0));
			m_SelectedGameObject->addAnimator(anim);
			anim->drop();
		}
	}
}

/**
 * Rotate selected element along the given axis by the given amount
 */
void CEditorLevel::RotateSelectedElement(int axis, int amount)
{
	if(m_SelectedGameObject)
	{
		switch(axis)
		{
		case LE_AXIS_X:
			{
				//Rotate model on the X axis
				vector3df newPos = m_SelectedGameObject->getRotation();
				newPos.X += amount;
				m_SelectedGameObject->setRotation(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesRotation(m_SelectedGameObject->getRotation().X, m_SelectedGameObject->getRotation().Y, m_SelectedGameObject->getRotation().Z);
			}
			break;
		case LE_AXIS_Y:
			{
				//Rotate model on the Y axis
				vector3df newPos = m_SelectedGameObject->getRotation();
				newPos.Y += amount;
				m_SelectedGameObject->setRotation(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesRotation(m_SelectedGameObject->getRotation().X, m_SelectedGameObject->getRotation().Y, m_SelectedGameObject->getRotation().Z);
			}
			break;
		case LE_AXIS_Z:
			{
				//Rotate model on the Z axis
				vector3df newPos = m_SelectedGameObject->getRotation();
				newPos.Z += amount;
				m_SelectedGameObject->setRotation(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesRotation(m_SelectedGameObject->getRotation().X, m_SelectedGameObject->getRotation().Y, m_SelectedGameObject->getRotation().Z);
			}
			break;
		}
	}
}

void CEditorLevel::DrawNormals()
{
	list<CGameObject*>::Iterator it = m_ListOfGameObjects.begin();
	
	for (; it != m_ListOfGameObjects.end(); ++it)
	{
		ISceneNode* node = m_EditorManager->getSceneMngr()->getSceneNodeFromId((*it)->id);
		if(!((*it)->mesh.equals_ignore_case(LIGHT_GAME_OBJECT)||(*it)->mesh.equals_ignore_case(PARTICLE_GAME_OBJECT)))
		{
			node->setDebugDataVisible(node->isDebugDataVisible()^EDS_NORMALS);
		}
	}
}

/**
 * \brief Mouse and Keyboard events are handled here. 
 * Every mouse click on game object is caught, mouse move will move the 
 * models around, del key will delete them from the map... etc.
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorLevel::OnEvent(const SEvent& eventer)
{
	if(eventer.EventType == EET_KEY_INPUT_EVENT)
	{
		int keycode = eventer.KeyInput.Key;
		#ifdef _IRR_OSX_PLATFORM_
		if(eventer.KeyInput.Key == KEY_KEY_C)
		{
			m_bShiftPressed = eventer.KeyInput.PressedDown;
			//printf("SHIFT IS %d ", m_bShiftPressed);
		}
		#else
		if((keycode == KEY_SHIFT)||(keycode == KEY_LSHIFT)||(keycode == KEY_RSHIFT))
		{
			m_bShiftPressed = eventer.KeyInput.PressedDown;
		}
		#endif

		#ifdef _IRR_OSX_PLATFORM_
		if(eventer.KeyInput.Key == KEY_KEY_V)
		{
			m_bShiftPressed = eventer.KeyInput.PressedDown;
			//printf("SHIFT IS %d ", m_bShiftPressed);
		}
		#else
		if((keycode == KEY_CONTROL)||(keycode == KEY_LCONTROL)||(keycode == KEY_RCONTROL))
		{
			m_bCtrlPressed = eventer.KeyInput.PressedDown;
		}
		#endif

		if((eventer.KeyInput.PressedDown) && (eventer.KeyInput.Key == KEY_KEY_R))
		{
			if(m_SelectedGameObject && m_bElementAtHand)
			{
				//Rotate model for 90 degrees
				vector3df newRot = m_SelectedGameObject->getRotation();
				if(m_bShiftPressed)
					newRot.Y += 90;
				else
					newRot.Y -= 90;
				m_SelectedGameObject->setRotation(newRot);
				m_EditorManager->getGUIManager()->SetPropertiesRotation(m_SelectedGameObject->getRotation().X, m_SelectedGameObject->getRotation().Y, m_SelectedGameObject->getRotation().Z);
			}
		}
		if((eventer.KeyInput.PressedDown) && (eventer.KeyInput.Key == KEY_KEY_N))
		{
			DrawNormals();
		}
		if(eventer.KeyInput.Key == KEY_KEY_X)
		{
			m_bKeyXPressed = eventer.KeyInput.PressedDown;
		}
		if(eventer.KeyInput.Key == KEY_KEY_Z)
		{
			if(m_bCtrlPressed)
			{
				if(eventer.KeyInput.PressedDown)
					m_EditorManager->Undo();
			}
			else
			{
				m_bKeyZPressed = eventer.KeyInput.PressedDown;
			}
		}
		if((eventer.KeyInput.Key == KEY_UP)&&m_bShiftPressed)
		{
			if(m_SelectedGameObject && !m_bElementAtHand)
			{
				//Move model up the Y axis
				vector3df newPos = m_SelectedGameObject->getPosition();
				newPos.Y += 1;
				m_SelectedGameObject->setPosition(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
			}
		}
		if((eventer.KeyInput.Key == KEY_DOWN)&&m_bShiftPressed)
		{
			if(m_SelectedGameObject && !m_bElementAtHand)
			{
				//Move model down the Y axis
				vector3df newPos = m_SelectedGameObject->getPosition();
				newPos.Y -= 1;
				m_SelectedGameObject->setPosition(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
			}
		}
		if((eventer.KeyInput.Key == KEY_LEFT)&&m_bShiftPressed)
		{
			if(m_SelectedGameObject && !m_bElementAtHand)
			{
				//Move model down the X axis
				vector3df newPos = m_SelectedGameObject->getPosition();
				newPos.X -= 1;
				m_SelectedGameObject->setPosition(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
			}
		}
		if((eventer.KeyInput.Key == KEY_RIGHT)&&m_bShiftPressed)
		{
			if(m_SelectedGameObject && !m_bElementAtHand)
			{
				//Move model up the X axis
				vector3df newPos = m_SelectedGameObject->getPosition();
				newPos.X += 1;
				m_SelectedGameObject->setPosition(newPos);
				m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
			}
		}
		//Delete selected game object
		if ((eventer.KeyInput.PressedDown) && (eventer.KeyInput.Key == KEY_DELETE) && (m_SelectedGameObject) && (!m_EditorManager->getGUIManager()->m_bGUIFocused))
		{
			int id = m_SelectedGameObject->getID();
			CGameObject* gameObject = _getGameObjectFromID(id);
			
			//first store undo action data, so user can undo delete
			TUndoAction undoAction;
			undoAction.type = E_UNDO_ACTION_DELETED;
			undoAction.go = gameObject;
			m_EditorManager->AddUndoAction(undoAction);

			if(gameObject->isStatic && m_SelectedGameObject->getTriangleSelector())
			{
				m_LevelMetaTriangleSelector->removeTriangleSelector(m_SelectedGameObject->getTriangleSelector());
			}
			_eraseGameObject(id);
			//delete the bastard
			m_SelectedGameObject->remove();
			m_SelectedGameObject = NULL;
			m_EditorManager->getGUIManager()->ClearProperties();
			m_EditorManager->getGUIManager()->RemoveNodeFromSceneTree(id);
		}
		//ESc while object at hand will cancel (delete the object)
		if((eventer.KeyInput.PressedDown) && (eventer.KeyInput.Key == KEY_ESCAPE))
		{
			if(m_bElementAtHand && m_SelectedGameObject)
			{
				_eraseGameObject(m_SelectedGameObject->getID());
				//delete the bastard
				m_SelectedGameObject->remove();
				m_SelectedGameObject = NULL;
				m_bMoveSelectedNode = false;
				m_bElementAtHand = false;
			}
		}
	}

	if(eventer.EventType == EET_MOUSE_INPUT_EVENT)
	{
		CGameObject* go = 0;
		if(m_SelectedGameObject)
			go = _getGameObjectFromID(m_SelectedGameObject->getID());

		switch(eventer.MouseInput.Event)
		{
			case EMIE_LMOUSE_PRESSED_DOWN:
				{
					if (m_SelectedGameObject && m_bShiftPressed && !m_bElementAtHand)
					{
						//we have to save first X because rotation jerks on start, have no clue why
						m_NodeRotationX = eventer.MouseInput.X; 
						//we have to include any already existing rotation of object
						m_CurrentRotationOfSelectedNode = m_SelectedGameObject->getRotation(); 
						//add undo rotate action
						TUndoAction undoAction;
						undoAction.type = E_UNDO_ACTION_ROTATED;
						undoAction.go = go;
						undoAction.oldValue = m_CurrentRotationOfSelectedNode;
						m_EditorManager->AddUndoAction(undoAction);
						//lets do the rotation! (see EMIE_MOUSE_MOVED below)
						m_bRotateYSelectedNode = true;
					}
					else if (m_SelectedGameObject && m_bKeyXPressed && !m_bElementAtHand)
					{
						//we have to save first Y because rotation jerks on start, have no clue why
						m_NodeRotationY = eventer.MouseInput.Y;
						//we have to include any already existing rotation of object
						m_CurrentRotationOfSelectedNode = m_SelectedGameObject->getRotation(); 
						//add undo rotate action
						TUndoAction undoAction;
						undoAction.type = E_UNDO_ACTION_ROTATED;
						undoAction.go = go;
						undoAction.oldValue = m_CurrentRotationOfSelectedNode;
						m_EditorManager->AddUndoAction(undoAction);
						//lets do the rotation! (see EMIE_MOUSE_MOVED below)
						m_bRotateXSelectedNode = true;
					}
					else if (m_SelectedGameObject && m_bKeyZPressed && !m_bElementAtHand)
					{
						//we have to save first Y because rotation jerks on start, have no clue why
						m_NodeRotationY = eventer.MouseInput.Y;
						//we have to include any already existing rotation of object
						m_CurrentRotationOfSelectedNode = m_SelectedGameObject->getRotation(); 
						//add undo rotate action
						TUndoAction undoAction;
						undoAction.type = E_UNDO_ACTION_ROTATED;
						undoAction.go = go;
						undoAction.oldValue = m_CurrentRotationOfSelectedNode;
						m_EditorManager->AddUndoAction(undoAction);
						//lets do the rotation! (see EMIE_MOUSE_MOVED below)
						m_bRotateZSelectedNode = true;
					}
					else if(!m_bMoveSelectedNode)
					{
						m_bRotateYSelectedNode = false;
						m_bRotateXSelectedNode = false;
						m_bRotateZSelectedNode = false;
						m_SelectedGameObject = getSceneNodeFromScreenCoordinatesBBB(m_EditorManager->getDevice()->getCursorControl()->getPosition(),m_EditorManager->getSceneMngr());
						if(m_SelectedGameObject && (m_SelectedGameObject->getType() != ESNT_CAMERA) && (m_SelectedGameObject->getID() > -1))
						{
							//object is selected on click, and its position is memorized
							//object is moved on event mouse move.
							m_bMoveSelectedNode = true;
							m_MoveOldPosition = m_SelectedGameObject->getPosition();
							m_SelectedBox = m_SelectedGameObject->getBoundingBox();
							m_CurrentZoom = m_SelectedGameObject->getScale();
							CGameObject* gameObject = _getGameObjectFromID(m_SelectedGameObject->getID());
							if(gameObject->isStatic && m_SelectedGameObject->getTriangleSelector())
							{
								m_LevelMetaTriangleSelector->removeTriangleSelector(m_SelectedGameObject->getTriangleSelector());
							}
							m_objectMoveOffset = m_SelectedGameObject->getPosition() - GetIntersectionPoint(0);

							//tell gui to change properties to this selected item
							m_EditorManager->getGUIManager()->SetProperties(gameObject);
							m_EditorManager->getGUIManager()->SetSelectedElementInTheTreeofSceneNodes(m_SelectedGameObject->getID());

							//set lights
							if(gameObject->mesh.equals_ignore_case(LIGHT_GAME_OBJECT))
							{
								//enlight scene
								EnlightAllNodes();
							}
							else
							{
								//turn off lights (and make scene bright!)
								DelightAllNodes();
							}
						}
						else 
						{
							m_SelectedGameObject = NULL;
							m_EditorManager->getGUIManager()->ClearProperties();
							m_MultiSelectStart = true;
							//get first rectangle position
							m_MultiSelectPosStart = vector3df(0,0,0);
						}
					}
				}
				break;
			case EMIE_LMOUSE_LEFT_UP:
				{
					bool seedingAnother = false;

					if(m_SelectedGameObject)
					{
						/* I think this is some old stuff? what is this for? im commenting this out!!!*/
						/* Nope, this is used for stacking barrels on top of each other... */
						if(!m_bRotateYSelectedNode && !m_bRotateXSelectedNode)
						{
							//if static, if not in meta selector, add it to meta selector:
							//this static was not consitent, causing crash on delete/add
							//either put static check everywhere when working with m_LevelMetaTriangleSelector
							//or remove it from ewverywhere... cheeez...
							if(go->isStatic)
							{
								//drop and recreate terrain triangle selector
								/*if(m_SelectedGameObject->getType() == ESNT_TERRAIN)
								{
									//((ITerrainSceneNode*)m_SelectedGameObject)->getTriangleSelector()->drop();
									ITriangleSelector* selector = m_EditorManager->getSceneMngr()->createTerrainTriangleSelector((ITerrainSceneNode*)m_SelectedGameObject, 0);
									m_SelectedGameObject->setTriangleSelector(selector);
								}*/
								m_LevelMetaTriangleSelector->addTriangleSelector(m_SelectedGameObject->getTriangleSelector());
							}
						}
						if(go->name.equals_ignore_case("Dancing Light GO"))
						{
							//position is set during dragging, we skip setting it here because Dancing Light is moving about and node position is changed.
							go->rot = m_SelectedGameObject->getRotation();
							go->scale = m_SelectedGameObject->getScale();
						}
						else
						{
							go->SetPositionRotationScaleFromNode(m_SelectedGameObject);
						}

						if (m_bElementAtHand && m_bShiftPressed)
							seedingAnother = true;

						if (m_bElementAtHand)
						{
							TUndoAction undoAction;
							undoAction.type = E_UNDO_ACTION_ADDED;
							undoAction.go = go;
							m_EditorManager->AddUndoAction(undoAction);
						}
						else if (m_bNodeBeingMoved)
						{
							TUndoAction undoAction;
							undoAction.type = E_UNDO_ACTION_MOVED;
							undoAction.oldValue = m_MoveOldPosition;
							undoAction.go = go;
							m_EditorManager->AddUndoAction(undoAction);
						}
					}
				
					m_bNodeBeingMoved = false;
					m_MultiSelectStart = false;
					m_MultiSelectPosStart = vector3df(0,0,0);
					m_MultiSelectPosEnd = vector3df(0,0,0);
					m_MoveOldPosition = vector3df(0,0,0);
					m_bMoveSelectedNode = false;
					m_bRotateYSelectedNode = false;
					m_bRotateXSelectedNode = false;
					m_bRotateZSelectedNode = false;
					m_bElementAtHand = false;
					m_objectMoveOffset = vector3df(0,0,0);

					if (seedingAnother)
					{
						//seed another game object
						//be carefull, m_bElementAtHand is being set to true here again, and it should stay true untill another seed
						m_EditorManager->getGUIManager()->SetElementAtHand(go->path,go->name,go->isStatic);
					}
				}
				break;

			case EMIE_MOUSE_MOVED:
				//Rotate selected node
				if (m_bRotateYSelectedNode)
				{
					if(m_SelectedGameObject)
					{
						//Moving mouse along X axis will cause rotation of model around Y axis (don't let this confuse you)
						f32 RotX = (f32) (m_NodeRotationX - eventer.MouseInput.X);
						m_SelectedGameObject->setRotation(m_CurrentRotationOfSelectedNode + vector3df(0,RotX,0));
						m_EditorManager->getGUIManager()->SetPropertiesRotation(m_SelectedGameObject->getRotation().X, m_SelectedGameObject->getRotation().Y, m_SelectedGameObject->getRotation().Z);
					}
				}
				if(m_bRotateXSelectedNode)
				{
					if(m_SelectedGameObject)
					{
						f32 RotY = (f32) (m_NodeRotationY - eventer.MouseInput.Y);
						m_SelectedGameObject->setRotation(m_CurrentRotationOfSelectedNode + vector3df(RotY,0,0));
						m_EditorManager->getGUIManager()->SetPropertiesRotation(m_SelectedGameObject->getRotation().X, m_SelectedGameObject->getRotation().Y, m_SelectedGameObject->getRotation().Z);
					}
				}
				if(m_bRotateZSelectedNode)
				{
					if(m_SelectedGameObject)
					{
						f32 RotY = (f32) (m_NodeRotationY - eventer.MouseInput.Y);
						m_SelectedGameObject->setRotation(m_CurrentRotationOfSelectedNode + vector3df(0,0,RotY));
						m_EditorManager->getGUIManager()->SetPropertiesRotation(m_SelectedGameObject->getRotation().X, m_SelectedGameObject->getRotation().Y, m_SelectedGameObject->getRotation().Z);
					}
				}
				//Move selected node
				if (m_bMoveSelectedNode)
				{
					if(m_SelectedGameObject)
					{
						m_bNodeBeingMoved = true;
						if(go->isTile)
						{
							//intersect with invisible horizontal plane at Y=0
							vector3df instersection_point = vector3df(0,0,0);
							line3d<f32> picking_line = m_EditorManager->getSceneMngr()->getSceneCollisionManager()->getRayFromScreenCoordinates(m_EditorManager->getDevice()->getCursorControl()->getPosition());
							plane3df horizontalPlane; // plane horizontal to the character
							horizontalPlane.setPlane(vector3df(0.f, -1.f, 0.f), (f32)m_TileHeight);
							if(horizontalPlane.getIntersectionWithLine(picking_line.start, picking_line.getVector(), instersection_point))
							{
								vector3df result = instersection_point + m_objectMoveOffset;
								int x = (int) (instersection_point.X + m_objectMoveOffset.X);
								int z = (int) (instersection_point.Z + m_objectMoveOffset.Z);
								int Xmultiplier = x/100;
								int Zmultiplier = z/100;
								x = x-(Xmultiplier)*100;
								z = z-(Zmultiplier)*100;
								if((abs(x)>30 && abs(x)<70)&&(abs(z)>30 && abs(z)<70))
								{
									//snap
									result.X = (float)((x>0)?50:-50)+Xmultiplier*100;
									result.Z = (float)((z>0)?50:-50)+Zmultiplier*100;
								}
								m_SelectedGameObject->setPosition(result);
								//set properties to reflect position coord change while moving object
								m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
							}
						}
						else if(go->isWall)
						{
							//intersect with invisible horizontal plane at Y=0
							vector3df instersection_point = vector3df(0,0,0);
							line3d<f32> picking_line = m_EditorManager->getSceneMngr()->getSceneCollisionManager()->getRayFromScreenCoordinates(m_EditorManager->getDevice()->getCursorControl()->getPosition());
							plane3df horizontalPlane; // plane horizontal to the character
							horizontalPlane.setPlane(vector3df(0.f, -1.f, 0.f), (f32)m_WallHeight);
							if(horizontalPlane.getIntersectionWithLine(picking_line.start, picking_line.getVector(), instersection_point))
							{
								vector3df result = instersection_point + m_objectMoveOffset;
								int x = (int) (instersection_point.X + m_objectMoveOffset.X);
								int z = (int) (instersection_point.Z + m_objectMoveOffset.Z);
								int Xmultiplier = x/100;
								int Zmultiplier = z/100;
								x = x-(Xmultiplier)*100;
								z = z-(Zmultiplier)*100;
								if((abs(x)>80 || abs(x)<20)&&(abs(z)>80 || abs(z)<20))
								{
									//snap
									result.X = (float)((abs(x)>80)?((x>0)?(1+Xmultiplier)*100:(-1+Xmultiplier)*100):(Xmultiplier*100));
									result.Z = (float)((abs(z)>80)?((z>0)?(1+Zmultiplier)*100:(-1+Zmultiplier)*100):(Zmultiplier*100));
								}
								m_SelectedGameObject->setPosition(result);
								//set properties to reflect position coord change while moving object
								m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
							}
						}
						else if(go->mesh.equals_ignore_case(LIGHT_GAME_OBJECT))
						{
							//TGameObjectProperty properties;
							vector3df instersection_point = GetIntersectionPoint(20);
							if(m_bCtrlPressed)
							{
								//TODO: what is this? why dancing light are not here but just down in else branch?
								m_SelectedGameObject->setPosition(instersection_point + m_objectMoveOffset);
							}
							else
							{
								m_SelectedGameObject->setPosition(instersection_point);
								if(go->name.equals_ignore_case("Dancing Light GO"))
								{
									//For dancing light, we need to remove animator because it won't allow light to be moved
									m_SelectedGameObject->removeAnimators();
									ISceneNodeAnimator* anim = 0;
									anim = m_EditorManager->getSceneMngr()->createFlyCircleAnimator(instersection_point,1.0f,0.04f,vector3df(1,0,0));
									m_SelectedGameObject->addAnimator(anim);
									anim->drop();

									go->pos = instersection_point;
								}
							}
							//set properties to reflect position coord change while moving object
							m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
						}
						else
						{
							//TGameObjectProperty properties;
							vector3df instersection_point = GetIntersectionPoint(0);
							if(m_bCtrlPressed)
								m_SelectedGameObject->setPosition(instersection_point + m_objectMoveOffset);
							else
								m_SelectedGameObject->setPosition(instersection_point);
							//set properties to reflect position coord change while moving object
							m_EditorManager->getGUIManager()->SetPropertiesPosition(m_SelectedGameObject->getPosition().X, m_SelectedGameObject->getPosition().Y, m_SelectedGameObject->getPosition().Z);
						}
					}
				}

				if(m_MultiSelectStart)
				{
					//Multiselect allows user to draw a square and select each object within the square
					//we memorizes starting square point, we calculate current one, and then we loop through game objects to see if their position is in the square.
				}

				break;
			case EMIE_MOUSE_WHEEL:
				{
					//Scale selected object with SHIFT + mouse wheel
					if (m_SelectedGameObject && m_bShiftPressed && !m_bElementAtHand)
					{
						//add undo scale action
						TUndoAction undoAction;
						undoAction.type = E_UNDO_ACTION_SCALED;
						undoAction.go = go;
						undoAction.oldValue = m_SelectedGameObject->getScale();
						m_EditorManager->AddUndoAction(undoAction);

						if (eventer.MouseInput.Wheel > 0)
							m_CurrentZoom += vector3df(0.1f,0.1f,0.1f);
						else
							m_CurrentZoom -= vector3df(0.1f,0.1f,0.1f);
						m_SelectedGameObject->setScale(m_CurrentZoom);
						go->scale = m_CurrentZoom;
					}
				}
				break;
			default:
				break;
		}
	}
	return false;
}

CGameObject* CEditorLevel::_getGameObjectFromID(s32 id)
{
	list<CGameObject*>::Iterator it = m_ListOfGameObjects.begin();
	
	for (; it != m_ListOfGameObjects.end(); ++it)
	{
		if((*it)->id == id)
		{
			return *it;
		}
	}

	return 0;
}

void CEditorLevel::_eraseGameObject(s32 id)
{
	list<CGameObject*>::Iterator it = m_ListOfGameObjects.begin();
	
	for (; it != m_ListOfGameObjects.end(); ++it)
	{
		if((*it)->id == id)
		{
			m_ListOfGameObjects.erase(it);
			return;
		}
	}
}

/**
 * \brief Returns string containing state of object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorLevel::GetObjectParameter_State(s32 objectID)
{
	//get game object with given id
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		return obj->state;
	}

	return L"";
}

/**
 * \brief Returns string containing name of object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorLevel::GetObjectParameter_Name(s32 objectID)
{
	//get game object with given id
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		return obj->name;
	}

	return L"";
}

/**
 * \brief Returns string containing name of object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorLevel::GetObjectParameter_RootName(s32 objectID)
{
	//get game object with given id
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		return obj->root;
	}

	return L"";
}

/**
 * \brief Returns string containing mesh name of object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorLevel::GetObjectParameter_Mesh(s32 objectID)
{
	//get game object with given id
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		return obj->mesh;
	}

	return L"";
}

/**
 * \brief Returns string containing script filename of object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorLevel::GetObjectParameter_Script(s32 objectID)
{
	//get game object with given id
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		return obj->script;
	}

	return L"";
}

/**
 * \brief Returns number of pickable items of (container) object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
s32 CEditorLevel::GetObjectNumberOfPickableItems(s32 objectID)
{
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		return obj->GetNumberOfPickableItems();
	}

	return -1;
}

/**
 * \brief Returns string containing name of pickable item of given index of object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
CGameObject* CEditorLevel::GetObjectPickableItem(s32 objectID, s32 index)
{
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		return obj->GetPickableItem(index);
	}

	return NULL;
}

/**
 * \brief Gets selected objects id.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
s32 CEditorLevel::GetSelectedObjectParameter_ID()
{
	if(m_SelectedGameObject)
	{
		return m_SelectedGameObject->getID();
	}
	return -1;
}

/**
 * \brief Clears pickable items of given object.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
void CEditorLevel::ObjectClearPickableItems(s32 objectID)
{
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		obj->ClearPickableItems();
	}
}

/**
 * \brief Adds pickable item to given object.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
void CEditorLevel::ObjectAddPickableItem(s32 objectID, stringc rootname)
{
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		stringw xmlProperties = rootname + L".xml";
		IXMLReader* xml = m_EditorManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());
		CGameObject* pick = new CGameObject(rootname,m_EditorManager->m_ID,xml,m_EditorManager->getDriver());
		obj->AddPickableItem(pick);
		m_EditorManager->m_ID++;
	}
}

/**
 * \brief Sets objects property 'state' to given value.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
void CEditorLevel::SetObjectParameter_State(s32 objectID, stringw state)
{
	CGameObject* obj = _getGameObjectFromID(objectID);
	if(obj)
	{
		obj->state = state;
	}
}

/**
 * \brief Sets selected object property 'isContainer' to given value.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
void CEditorLevel::SetSelectedObjectParameter_isContainer(bool isContainer)
{
	if(m_SelectedGameObject)
	{
		CGameObject* obj = _getGameObjectFromID(m_SelectedGameObject->getID());
		obj->isContainer = isContainer;
	}
}

/**
 * \brief Sets selected object property 'isPickable' to given value.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
void CEditorLevel::SetSelectedObjectParameter_isPickable(bool isPickable)
{
	if(m_SelectedGameObject)
	{
		CGameObject* obj = _getGameObjectFromID(m_SelectedGameObject->getID());
		obj->isPickable = isPickable;
	}
}

/**
 * \brief Sets selected object property 'isTrigger' to given value.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
void CEditorLevel::SetSelectedObjectParameter_isTrigger(bool isTrigger)
{
	if(m_SelectedGameObject)
	{
		CGameObject* obj = _getGameObjectFromID(m_SelectedGameObject->getID());
		obj->isTrigger = isTrigger;
	}
}

/**
 * \brief Sets selected object property 'isNPC' to given value.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
void CEditorLevel::SetSelectedObjectParameter_isNPC(bool isNPC)
{
	if(m_SelectedGameObject)
	{
		CGameObject* obj = _getGameObjectFromID(m_SelectedGameObject->getID());
		obj->isNPC = isNPC;
	}
}

/**
 * \brief Sets selected object property 'isMonster' to given value.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
void CEditorLevel::SetSelectedObjectParameter_isMonster(bool isMonster)
{
	if(m_SelectedGameObject)
	{
		CGameObject* obj = _getGameObjectFromID(m_SelectedGameObject->getID());
		obj->isMonster = isMonster;
	}
}


void CEditorLevel::EnlightAllNodes()
{
	list<CGameObject*>::Iterator it = m_ListOfGameObjects.begin();
	
	m_EditorManager->getSceneMngr()->setAmbientLight(SColorf(0.2f, 0.2f, 0.1f, 0.8f));

	for (; it != m_ListOfGameObjects.end(); ++it)
	{
		ISceneNode* node = m_EditorManager->getSceneMngr()->getSceneNodeFromId((*it)->id);
		if(!((*it)->mesh.equals_ignore_case(LIGHT_GAME_OBJECT)||(*it)->mesh.equals_ignore_case(PARTICLE_GAME_OBJECT)))
		{
			node->setMaterialFlag(EMF_LIGHTING, true);
		}
	}
}

void CEditorLevel::DelightAllNodes()
{
	list<CGameObject*>::Iterator it = m_ListOfGameObjects.begin();
	
	m_EditorManager->getSceneMngr()->setAmbientLight(SColorf(1.0f, 1.0f, 1.0f, 1.0f)); //not really needed?

	for (; it != m_ListOfGameObjects.end(); ++it)
	{
		ISceneNode* node = m_EditorManager->getSceneMngr()->getSceneNodeFromId((*it)->id);
		node->setMaterialFlag(EMF_LIGHTING, false);
	}
}
