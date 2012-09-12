/** 
 * \file EditorManager.cpp
 * \brief CEditorManager class is the boss. The manager. It creates 3D device (irrlicht of course),
 * creates GUI, and 3D environment so called Level Editor. Calls all the initialization functions and 
 * goes to the loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include <irrlicht.h>
using namespace irr;
#include "EditorManager.h"

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CEditorManager::CEditorManager()
{
	m_ID = 0;
	m_FS = NULL;
	m_WorkingDirectory = ".";
	lastFPS = -1;
	m_bFullscreen = false;
	m_SelectedLanguage = "en";
	m_bMaximized = true;
}

/**
 * \brief Initialization function. Calls init of all sub classes (GUI, EdiLevel, etc..)
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorManager::Init()
{
	//load properties
	LoadProperties();
	//create irrlicht device
	CreateDevice();
	//we gonna use filesystem everuwhere so lets store the pointer
	m_FS = m_pDevice->getFileSystem();
	//and useful working dir
	#ifdef _IRR_OSX_PLATFORM_
	m_WorkingDirectory = "/Volumes/NemesisRealm/Programming/Nsis/XProjects/IR Level Editor";
	m_FS->changeWorkingDirectoryTo(m_WorkingDirectory.c_str());
	#else
	m_WorkingDirectory = m_FS->getWorkingDirectory();
	#endif
	//create GUI
	m_pGuiManager = new CEditorGUI();
	m_pGuiManager->Init(this);
	//create "level 3D environment handling class": edilevel
	m_pEdiLevel = new CEditorLevel();
	m_pEdiLevel->Init(this);
	m_ScriptEngine = new CScript();
	m_UndoActionsManager = new CUndoActions(this);
	m_ScriptEngine->Init(m_pDevice, ACTIONS_SCRIPT_FILE, CONDITIONS_SCRIPT_FILE);

	//load languages
	m_pLanguages = new CLanguages(m_FS);
	m_FS->changeWorkingDirectoryTo("media/strings");
	m_pLanguages->Init();
	bool ret = m_pLanguages->setLanguage(m_SelectedLanguage);
	if(!ret)
	{
		//TODO: display warning message that language is not found!
	}
	backToWorkingDirectory();

}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CEditorManager::~CEditorManager()
{
	//save window size, language
	m_Resolution.Width = m_pDriver->getScreenSize().Width;
	m_Resolution.Height = m_pDriver->getScreenSize().Height;

	//because we don't have m_pDevice->isWindowMaximized in irrlicht 1.6, this is how we determine if window is maximized or not...
	if(m_Resolution.Width < m_DesktopResolution.Width)
	{
		m_bMaximized = false;
	}
	else
	{
		m_bMaximized = true;
	}

	StorePropertiesToConfigFile(LEVEL_EDITOR_CONFIG_FILE);
}


/**
 * \brief Save config data to given XML file
 * \author Petar Bajic 
 * \date July, 3 2012.
 */
bool CEditorManager::StorePropertiesToConfigFile(stringc filename)
{
	io::IXMLWriter* xml = m_FS->createXMLWriter(filename.c_str());

	if (!xml)
	{
		//Display error message and exit
		return false;
	}

	xml->writeXMLHeader();

	xml->writeElement(L"Config",false); 
	xml->writeLineBreak();
	
	xml->writeElement(L"Window",true,L"width",stringw(m_Resolution.Width).c_str(),L"height",stringw(m_Resolution.Height).c_str()); 
	xml->writeLineBreak();

	xml->writeElement(L"Language",true,L"value",stringw(m_pLanguages->m_Language->value).c_str()); 
	xml->writeLineBreak();

	xml->writeElement(L"Maximized",true,L"value",stringw(m_bMaximized?"true":"false"/*m_pDevice->isWindowMaximized()*/).c_str()); 
	xml->writeLineBreak();

	xml->writeElement(L"Fullscreen",true,L"value",stringw(m_bFullscreen?"true":"false").c_str()); 
	xml->writeLineBreak();

	xml->writeClosingTag(L"Config");
	xml->writeLineBreak();

	xml->drop(); // don't forget to delete the xml writer

	return true;
}


/**
 * \brief Load config data from given XML file
 * \author Petar Bajic 
 * \date July, 3 2012.
 */
bool CEditorManager::LoadPropertiesFromConfigFile(stringc filename)
{
	io::IXMLReader* xml = m_FS->createXMLReader(filename.c_str());

	if (!xml)
	{
		//Display error message and exit

		return false;
	}

	while(xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo;
				if (core::stringw("Window") == xml->getNodeName())
				{
					m_Resolution.Width = xml->getAttributeValueAsInt(L"width");
					m_Resolution.Height = xml->getAttributeValueAsInt(L"height");
				}
				else if (core::stringw("Language") == xml->getNodeName())
				{
					//Chosen Language.
					m_SelectedLanguage = xml->getAttributeValue(L"value");
				}
				else if (core::stringw("Maximized") == xml->getNodeName())
				{
					// Maximized
					figo = xml->getAttributeValue(L"value");
					if(figo.equals_ignore_case("true"))
					{
						m_bMaximized = true;
					}
					else
					{
						m_bMaximized = false;
					}
				}
				else if (core::stringw("Fullscreen") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if(figo.equals_ignore_case("true"))
					{
						m_bFullscreen = true;
					}
					else
					{
						m_bFullscreen = false;
					}
				}
			}
			break;
		}
	}

	xml->drop(); // don't forget to delete the xml reader

	return true;
}

/**
 * \brief Returns string in chosen language from given id. 
 * \author Petar Bajic 
 * \date July, 3 2012.
 */
stringw CEditorManager::getTranslatedString(u32 id)
{
	return m_pLanguages->getObjectString(id);
}

/**
 * \brief Editor drawing loop. Draws GUI (2D) and current level (3D).
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorManager::Update()
{
	if(!m_pDriver->beginScene(true, true, SColor(255,255,255,255)))
	{
		// reset failed, propably because render target wasn't destroyed.
		if(m_pGuiManager->m_TextureToRenderOn && m_pGuiManager->m_ContainerPickPreview_TextureToRenderOn) 
		{
			// remove rendertarget.
			m_pGuiManager->m_TextureToRenderOn->drop();
			m_pDriver->removeTexture(m_pGuiManager->m_TextureToRenderOn);
			m_pGuiManager->m_TextureToRenderOn = 0;

			m_pGuiManager->m_ContainerPickPreview_TextureToRenderOn->drop();
			m_pDriver->removeTexture(m_pGuiManager->m_ContainerPickPreview_TextureToRenderOn);
			m_pGuiManager->m_ContainerPickPreview_TextureToRenderOn = 0;

			// this should call d3ddevice->reset.. now it should work because rendertarget was destroyed.
			m_pDriver->OnResize(m_pDriver->getScreenSize());
			m_pDriver->beginScene(true, true, 0);

			if(m_pGuiManager->m_wnd_Properties)
				m_pGuiManager->m_wnd_Properties->setRelativePosition(rect<s32>(m_pDriver->getScreenSize().Width-200,14,m_pDriver->getScreenSize().Width,480));

			// and let's create rendertarget again.
			m_pGuiManager->m_TextureToRenderOn = m_pDriver->addRenderTargetTexture(dimension2du(200,200));
			m_pGuiManager->m_ContainerPickPreview_TextureToRenderOn = m_pDriver->addRenderTargetTexture(dimension2du(180,180));
		}
	}

	if (m_pGuiManager->m_wnd_ModelPreview && m_pGuiManager->m_TextureToRenderOn)
	{
		//set render target texture
		m_pDriver->setRenderTarget(m_pGuiManager->m_TextureToRenderOn, true, true, video::SColor(0,0,0,255));     

		//set photo camera as active camera
		m_pSceneManager->setActiveCamera(m_pGuiManager->m_PhotoCamera);

		//draw whole scene into render buffer
		m_pSceneManager->drawAll();                

		//set back old render target
		m_pDriver->setRenderTarget(0);      

		//set the level camera as active one
		m_pSceneManager->setActiveCamera(m_pEdiLevel->m_LevelCamera);
	}
	if (m_pGuiManager->m_wnd_ContainerContent && m_pGuiManager->m_ContainerPickPreview_TextureToRenderOn)
	{
		//set render target texture
		m_pDriver->setRenderTarget(m_pGuiManager->m_ContainerPickPreview_TextureToRenderOn, true, true, video::SColor(0,20,100,20));     

		//set photo camera as active camera
		m_pSceneManager->setActiveCamera(m_pGuiManager->m_PickCamera);

		//draw whole scene into render buffer
		m_pSceneManager->drawAll();                

		//set back old render target
		m_pDriver->setRenderTarget(0);      

		//set the level camera as active one
		m_pSceneManager->setActiveCamera(m_pEdiLevel->m_LevelCamera);
	}

	m_pSceneManager->drawAll();
	m_pEdiLevel->OnRender();
	m_pGUIEnvironment->drawAll();
	//if(m_pGuiManager->m_wnd_ModelPreview)
	//{
	//	m_pDriver->draw2DImage(m_pGuiManager->m_TextureToRenderOn, m_pGuiManager->m_wnd_ModelPreview->getAbsolutePosition().UpperLeftCorner + position2di(5, 25));
	//}
	if (!m_pGuiManager->m_wnd_AddPickScriptAction && m_pGuiManager->m_wnd_ContainerContent)
	{
		m_pDriver->draw2DImage(m_pGuiManager->m_ContainerPickPreview_TextureToRenderOn, m_pGuiManager->m_wnd_ContainerContent->getAbsolutePosition().UpperLeftCorner + position2di(190, 40));
		m_pDriver->draw2DRectangle(SColor(255,50,155,0),rect<s32>(m_pGuiManager->m_wnd_ContainerContent->getAbsolutePosition().UpperLeftCorner + position2di(190, 225),m_pGuiManager->m_wnd_ContainerContent->getAbsolutePosition().UpperLeftCorner + position2di(190, 225) + position2di(64,64)));
		m_pDriver->draw2DImage(m_pGuiManager->m_slotTex, m_pGuiManager->m_wnd_ContainerContent->getAbsolutePosition().UpperLeftCorner + position2di(190, 225));
		m_pDriver->draw2DImage(m_pGuiManager->m_SelectedPickItemIcon_Texture, m_pGuiManager->m_wnd_ContainerContent->getAbsolutePosition().UpperLeftCorner + position2di(190, 225));
	}
	m_pDriver->endScene();

	// display frames per second in window title
	int fps = m_pDriver->getFPS();
	if (lastFPS != fps)
	{
		stringw str = L"Level Editor   -   ";
		str += stringw(m_pEdiLevel->m_MapName.c_str());
		str += "    FPS:";
		str += fps;

		m_pDevice->setWindowCaption(str.c_str());
		lastFPS = fps;
	}

}

/**
 * \brief Loads properties like window resolution, position, size, last used objectID
 * \author Petar Bajic 
 * \date July, 3 2012.
 */
bool CEditorManager::LoadProperties()
{
	IrrlichtDevice *nulldevice = createDevice(video::EDT_NULL);
	m_FS = nulldevice->getFileSystem();

	IVideoModeList* pVideoList = nulldevice->getVideoModeList();
	m_DesktopResolution = pVideoList->getDesktopResolution();

	//Load data from XML config file
	if (!LoadPropertiesFromConfigFile(LEVEL_EDITOR_CONFIG_FILE))
	{
		//initial default settings
	}

	nulldevice->drop();
	return true;
}

/**
 * \brief Creates the Irrlicht device and get pointers to the main subsytems
 * for later use, the Editor manager is the central interface point to the rendering engine
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorManager::CreateDevice()
{
	//create irrlicht device and send "this" as event receiver meaning this class should 
	//implement OnEvent function and handle its own events...
	
	//Daniel start
	//choose platform specific driver type
	video::E_DRIVER_TYPE driverType;
	#ifdef _IRR_OSX_PLATFORM_
	driverType = video::EDT_OPENGL;
	#else
	driverType = video::EDT_DIRECT3D9;	
	#endif
	//Daniel end

	m_pDevice = createDevice(driverType, dimension2d<u32>(m_Resolution.Width, m_Resolution.Height), 16, m_bFullscreen, false, false, this);
	m_pDevice->setResizable(true);
	if (m_bMaximized) m_pDevice->maximizeWindow();
   	m_pDriver = m_pDevice->getVideoDriver();
    m_pSceneManager = m_pDevice->getSceneManager();
	m_pGUIEnvironment = m_pDevice->getGUIEnvironment();
	//change font to garamond
	#ifdef _IRR_OSX_PLATFORM_
	IGUIFont* font = m_pGUIEnvironment->getFont("/Volumes/NemesisRealm/Programming/Nsis/XProjects/IR Level Editor/media/Garamond12.xml");
	#else
	IGUIFont* font = m_pGUIEnvironment->getFont(FONT_FILE);
	#endif
	m_pGUIEnvironment->getSkin()->setFont(font);
}

/**
 * \brief Returns a pointer to the Irrlicht Device subsystem
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
IrrlichtDevice* CEditorManager::getDevice()
{
	return m_pDevice;
}

/**
 * \brief Returns a pointer to the Irrlicht Driver subsystem
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
IVideoDriver* CEditorManager::getDriver()
{
	return m_pDriver;
}

/**
 * \brief Changes working directory to default one.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorManager::backToWorkingDirectory()
{
	m_FS->changeWorkingDirectoryTo(m_WorkingDirectory.c_str());
}

/**
 * \brief Returns a pointer to the Irrlicht SceneManager subsystem
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
ISceneManager* CEditorManager::getSceneMngr()
{
	return m_pSceneManager;
}

/**
 * \brief Returns a pointer to the Irrlicht GUI subsystem
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
IGUIEnvironment* CEditorManager::getGUIEnvironment()
{
	return m_pGUIEnvironment;
}

/**
 * \brief Main event handler derived from IEventHandler, this will be passed down 
 * to the current states keyboard handler. The state controls its own keyboard events
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorManager::OnEvent(const SEvent& event)
{
	if (!m_pDriver)
		return false;

	//Gui Handles its own events here
	bool gui_handles = m_pGuiManager->OnEvent(event);

	//3D level environment handles events here
	if (!gui_handles)
	{
		if (m_pEdiLevel->OnEvent(event))
			return true;
	}

	if (event.EventType == EET_KEY_INPUT_EVENT)
	{
		if(event.KeyInput.PressedDown){
			 if(event.KeyInput.Key==KEY_F2){
				//if(console->isVisible())
				return true;
			 }
			 else{
				//m_bKeys[event.KeyInput.Key] = event.KeyInput.PressedDown;
				// Pass input down to the specific game state keyboard handler
			}
		}

	}

	if (event.EventType == EET_MOUSE_INPUT_EVENT)
    {
        // Pass input down to the specific game state mouse handler
    }
	return false;
}

/**
 * \brief Returns string containing state of object with given id.
 *
 * Empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorManager::GetObjectParameter_State(s32 objectID)
{
	stringw state = m_pEdiLevel->GetObjectParameter_State(objectID);
	return state;
}

/**
 * \brief Returns string containing name of object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorManager::GetObjectParameter_Name(s32 objectID)
{
	stringw name = m_pEdiLevel->GetObjectParameter_Name(objectID);
	return name;
}

/**
 * \brief Returns string containing root name of object with given id.
 *
 * Return empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorManager::GetObjectParameter_RootName(s32 objectID)
{
	stringw name = m_pEdiLevel->GetObjectParameter_RootName(objectID);
	return name;
}

/**
 * \brief Returns string containing Mesh of object with given id.
 *
 * Empty string if object with given ID does not exist.
 *
 * \author Petar Bajic 
 * \date June, 1 2009.
 */
stringw CEditorManager::GetObjectParameter_Mesh(s32 objectID)
{
	stringw mesh = m_pEdiLevel->GetObjectParameter_Mesh(objectID);
	return mesh;
}

stringw CEditorManager::GetObjectParameter_Script(s32 objectID)
{
	return m_pEdiLevel->GetObjectParameter_Script(objectID);
}

s32 CEditorManager::GetSelectedObjectParameter_ID()
{
	return m_pEdiLevel->GetSelectedObjectParameter_ID();
}

s32 CEditorManager::GetObjectNumberOfPickableItems(s32 objectID)
{
	return m_pEdiLevel->GetObjectNumberOfPickableItems(objectID);
}

CGameObject* CEditorManager::GetObjectPickableItem(s32 objectID, s32 index)
{
	return m_pEdiLevel->GetObjectPickableItem(objectID, index);
}

void CEditorManager::ObjectClearPickableItems(s32 objectID)
{
	m_pEdiLevel->ObjectClearPickableItems(objectID);
}

void CEditorManager::ObjectAddPickableItem(s32 objectID, stringc rootname)
{
	m_pEdiLevel->ObjectAddPickableItem(objectID, rootname);
}

void CEditorManager::SetObjectParameter_State(s32 objectID, stringw state)
{
	m_pEdiLevel->SetObjectParameter_State(objectID, state);
}


void CEditorManager::SetSelectedObjectParameter_isContainer(bool isContainer)
{
	m_pEdiLevel->SetSelectedObjectParameter_isContainer(isContainer);
}

void CEditorManager::SetSelectedObjectParameter_isPickable(bool isPickable)
{
	m_pEdiLevel->SetSelectedObjectParameter_isPickable(isPickable);
}

void CEditorManager::SetSelectedObjectParameter_isTrigger(bool isTrigger)
{
	m_pEdiLevel->SetSelectedObjectParameter_isTrigger(isTrigger);
}

void CEditorManager::SetSelectedObjectParameter_isNPC(bool isNPC)
{
	m_pEdiLevel->SetSelectedObjectParameter_isNPC(isNPC);
}

void CEditorManager::SetSelectedObjectParameter_isMonster(bool isMonster)
{
	m_pEdiLevel->SetSelectedObjectParameter_isMonster(isMonster);
}

CGameObject* CEditorManager::GetSelectedGameObject()
{
	return m_pEdiLevel->_getGameObjectFromID(m_pEdiLevel->GetSelectedObjectParameter_ID());
}

bool CEditorManager::SingleObjectSelected()
{
	return m_pEdiLevel->SingleObjectSelected();
}

void CEditorManager::OnNewMap()
{
	m_pEdiLevel->OnNewMap();
	m_pGuiManager->OnNewMap();
}

void CEditorManager::AddUndoAction(TUndoAction undoAction)
{
	m_UndoActionsManager->AddUndoAction(undoAction);
}

void CEditorManager::Undo()
{
	m_UndoActionsManager->Undo();
}

void CEditorManager::AddGameObjectToLevel(CGameObject* go)
{
	m_pEdiLevel->AddGameObjectToLevel(go);
	m_pGuiManager->SetProperties(go);
	m_pGuiManager->AddNodeToSceneTree(go->id,go->name);
}

void CEditorManager::RemoveGameObjectFromMap(CGameObject* go)
{
	getGUIManager()->ClearProperties();
	getGUIManager()->RemoveNodeFromSceneTree(go->id);
	m_pEdiLevel->RemoveGameObjectFromMap(go);
}