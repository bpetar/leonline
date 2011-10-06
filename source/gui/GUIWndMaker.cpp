/** 
 * \file GUIWndMaker.cpp
 * \brief Creating floating and various dialog windows for GUI class
 * 
 * Who says it all has to be in one cpp file? :) 
 * I moved these functions from EDGui.cpp file, CEditorGUI class, to stop it from growing enormous
 * All funtions that create some kind of GUI window are neatly placed here.
 * Writting a winning combination of include files was a nigthmare but I did it!
 * Couldn't repeat it if I wanted to, so don't touch anything :)
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */
#include "GUIWndMaker.h"
#include "IGUITreeCtrl.h"
#include "IGUIContainer.h"

#include "../EditorManager.h"
#include "../Utils.h"

typedef struct SDialogNode
{
	int id;
	stringw name;
	stringw text;
	stringw speaker;

}TDialogNode;

//helper function that populates TreeNode with data from dialog XML file
void PopulateDialogTreeNodeFromXML(CEditorManager* editorManager, stringc filename, IGUIElement *parent, rect<s32> position)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	/*Fill Tree*/
	int NUM_NODES = 100;
	edGui->m_DialogTree = AddGUITreeCtrl(env, position, parent, 714, true);
	TreeNode **NodeList = new TreeNode*[NUM_NODES];
	u32 Parenthood[100];
	NodeList[0] = edGui->m_DialogTree->GetRootNode();
	int nodeIndex = 0;
	int nodeParent = 0;
	int lastParentNode = nodeParent;
	bool first = true;
	int ParenthoodDepth = 0;
	Parenthood[ParenthoodDepth] = 0;
	bool loadingAction = false;

	edGui->m_NumNodes = 0;

	//stringc xml_filename = filename + ".xml";
	io::IXMLReader* xml = device->getFileSystem()->createXMLReader(filename.c_str());

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo;
				//Insert New Node
				if (stringw(L"Node") == xml->getNodeName())
				{
					nodeIndex++;
					NodeList[nodeIndex] = edGui->m_DialogTree->InsertNode(L"node",NodeList[Parenthood[ParenthoodDepth]],nodeIndex);
					ParenthoodDepth++;
					Parenthood[ParenthoodDepth] = nodeIndex;
				}
				//InsertAllData
				else if (stringw(L"Id") == xml->getNodeName())
				{
					u32 id = xml->getAttributeValueAsInt(L"value");
					NodeList[Parenthood[ParenthoodDepth]]->ID = id;
					if(id > edGui->m_NumNodes)
					{
						edGui->m_NumNodes = id;
					}
				}
				else if (stringw(L"Enabled") == xml->getNodeName())
				{
					if(stringw(L"true") == xml->getAttributeValue(L"value"))
					{
						NodeList[Parenthood[ParenthoodDepth]]->isEnabled = true;
					}
					else
					{
						NodeList[Parenthood[ParenthoodDepth]]->isEnabled = false;
					}
				}
				else if (stringw(L"Link") == xml->getNodeName())
				{
					NodeList[Parenthood[ParenthoodDepth]]->LinkID = xml->getAttributeValueAsInt(L"value");
				}
				else if (stringw(L"IsLink") == xml->getNodeName())
				{
					if(stringw(L"true") == xml->getAttributeValue(L"value"))
					{
						NodeList[Parenthood[ParenthoodDepth]]->isLink = true;
					}
					else
					{
						NodeList[Parenthood[ParenthoodDepth]]->isLink = false;
					}
				}
				else if (stringw(L"Speaker") == xml->getNodeName())
				{
					NodeList[Parenthood[ParenthoodDepth]]->NPCName = xml->getAttributeValue(L"value");
					if (stringw(L"PLAYER") == NodeList[Parenthood[ParenthoodDepth]]->NPCName)
						NodeList[Parenthood[ParenthoodDepth]]->Player = true;
					else
						NodeList[Parenthood[ParenthoodDepth]]->Player = false;
				}
				else if (stringw(L"Text") == xml->getNodeName())
				{
					NodeList[Parenthood[ParenthoodDepth]]->Text = xml->getAttributeValue(L"value");
				}
				else if (stringw(L"Actions") == xml->getNodeName())
				{
					NodeList[Parenthood[ParenthoodDepth]]->scriptAction.actions.clear();
					loadingAction = true;
				}
				else if (loadingAction)
				{
					TAction action;
					action.name = xml->getNodeName();
					action.target = xml->getAttributeValue(L"target");
					action.attribute = xml->getAttributeValue(L"attribute");
					action.value = xml->getAttributeValue(L"value");
					NodeList[Parenthood[ParenthoodDepth]]->scriptAction.actions.push_back(action);
					NodeList[Parenthood[ParenthoodDepth]]->hasScriptAction = true;
				}

			}
			break;
		case io::EXN_ELEMENT_END:
			{
				if (stringw(L"Node") == xml->getNodeName())
				{
					//Node Closed, stepping out.
					ParenthoodDepth--;
				}
				else if (stringw(L"Actions") == xml->getNodeName())
				{
					loadingAction = false;
				}

			}
			break;
		}
	}

	if (xml)
		xml->drop(); // don't forget to delete the xml reader

	if(nodeIndex == 0)
	{
		NodeList[1] = edGui->m_DialogTree->InsertNode(L"node",NodeList[0],1);
		NodeList[1]->isEnabled = true;
		NodeList[1]->Child = 0;
		NodeList[1]->hasScriptAction = false;
		NodeList[1]->IconClosed = true;
		NodeList[1]->ID = 1;
		NodeList[1]->isLink = false;
		NodeList[1]->Link = 0;
		NodeList[1]->NPCName = L"RootNode";
		NodeList[1]->Player = true;
		NodeList[1]->Text = L"RootNode";
		NodeList[1]->scriptAction.actions.clear();
		edGui->m_NumNodes = 1;
	}
	else
	{
		//Go through the list and Link nodes
		for(u32 a=0; a<(u32)nodeIndex; a++)
		{
			//find link nodes
			if(NodeList[a]->LinkID != 0)
			{
				for(u32 b=0; b<(u32)nodeIndex; b++)
				{
					if(NodeList[b]->ID == NodeList[a]->LinkID)
					{
						NodeList[a]->Link = NodeList[b];
						break;
					}
				}
			}
		}
	}
}



//Initialize game objects window and feed tab control with existing models (search directory structure)
IGUIWindow* MakeGameObjectsWindow(CEditorManager* editorManager)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	/*Game Items window*/
	IGUIWindow* wnd_gameItems = env->addWindow(rect<s32>(0,30,315,530), false, L"Game Objects", 0, GUI_ID_WINDOW_GAME_OBJECTS);
	wnd_gameItems->setToolTipText(L"Select item from tree and then place it on the map.");

	// create tab control and tabs
	IGUITabControl* tabGameObjects = env->addTabControl(rect<s32>(2,20,310,480), wnd_gameItems, true, true, GUI_ID_TAB_GAME_OBJECTS);

	IFileSystem* fs = device->getFileSystem();
	IFileList* filelist = NULL;
	fs->changeWorkingDirectoryTo(OBJECT_DIR_C8);
	filelist = fs->createFileList();
	IGUITab* tabs[10]; 
	int ir = 0;
	
	//first tab containst Scene tree of nodes
	tabs[ir] = tabGameObjects->addTab(L"Scene");
	int NUM_NODES = 100;
	edGui->m_SceneNodesTree = AddGUITreeCtrl(env, rect<s32>(5,5,260,320), tabs[ir], GUI_ID_TREECTRL_GAME_OBJECTS_START+1, false);
	edGui->m_SceneNodesTree->InsertNode(L"Scene root node",edGui->m_SceneNodesTree->GetRootNode(),-2);
	ir++;

	for ( u32 a = 2; a < filelist->getFileCount(); a++)
	{
		if(filelist->isDirectory(a))
		{
			stringw temp = filelist->getFileName(a);
			stringc dir_filename = temp.c_str();
			if(dir_filename.equals_ignore_case(".svn"))
			{
				continue;
			}
			fs->changeWorkingDirectoryTo(dir_filename.c_str());
			tabs[ir] = tabGameObjects->addTab(temp.c_str());
			Util_PopulateTreeNodeGameObjectsFromXML(editorManager, dir_filename,tabs[ir],rect<s32>(5,5,260,320),GUI_ID_TREECTRL_GAME_OBJECTS_START+a);
			fs->changeWorkingDirectoryTo("../");
			ir++;
		}
	}
	fs->changeWorkingDirectoryTo("../../");
	filelist->drop();
	return wnd_gameItems;
}


/*Creates Properties window that displays properties of selected game object and lets you edit them*/
IGUIWindow* MakePropertiesWindow(CEditorManager* editorManager)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	s32 rightX = device->getVideoDriver()->getScreenSize().Width;
	s32 windowWidth = 250;
	s32 X_0 = 5;
	s32 X_1 = 70;
	s32 X_2 = 115;
	
	
	s32 Y_1 = 35;
	s32 Y_2 = 180;
	s32 Y_3 = 470;
	s32 Y_4 = 590;
	s32 Y_5 = 720;
	s32 Y_6 = 740;
	s32 itemHeight = 25;
	s32 marginY = 3;
	s32 marginX = 5;

	IGUIWindow* wnd_properties = env->addWindow(rect<s32>(rightX-windowWidth,18,rightX,800), false, L"Object Properties", 0, GUI_ID_WINDOW_PROPERTIES);
	wnd_properties->setToolTipText(L"Properties of selected game object");
	edGui->m_PropCheckBox_Anchor = env->addCheckBox(false,rect<s32>(X_0,Y_1,windowWidth-marginX,Y_1+itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_ANCOR,L"Anchor");
	edGui->m_PropCheckBox_Invisible = env->addCheckBox(false,rect<s32>(X_0,Y_1+marginY+itemHeight,windowWidth-marginX,Y_1+marginY+2*itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_VISIBLE,L"Invisible");
	edGui->m_PropCheckBox_Illusion = env->addCheckBox(false,rect<s32>(X_2,Y_1+marginY+itemHeight,windowWidth-marginX,Y_1+marginY+2*itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_ILLUSION,L"Illusion");
	edGui->m_PropCheckBox_Terrain = env->addCheckBox(false,rect<s32>(X_2,Y_1,windowWidth-marginX,Y_1+itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_TERRAIN,L"Terrain");
	env->addStaticText(L"Name", rect<s32>(X_0,Y_1+2*marginY+2*itemHeight,X_1,Y_1+2*marginY+3*itemHeight), false, false, wnd_properties, -1, false);
	env->addStaticText(L"Model", rect<s32>(X_0,Y_1+3*marginY+3*itemHeight,X_1,Y_1+3*marginY+4*itemHeight), false, false, wnd_properties, -1, false);
	env->addStaticText(L"ID", rect<s32>(X_0,Y_1+4*marginY+4*itemHeight,X_1,Y_1+4*marginY+5*itemHeight), false, false, wnd_properties, -1, false);
	edGui->m_PropEditBox_Name = env->addEditBox(L"", rect<s32>(X_1,Y_1+2*marginY+2*itemHeight,windowWidth-marginX,Y_1+2*marginY+3*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_NAME);
	edGui->m_PropEditBox_Model = env->addEditBox(L"", rect<s32>(X_1,Y_1+3*marginY+3*itemHeight,windowWidth-marginX,Y_1+3*marginY+4*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_MODEL);
	edGui->m_PropEditBox_ID = env->addEditBox(L"", rect<s32>(X_1,Y_1+4*marginY+4*itemHeight,X_2,Y_1+4*marginY+5*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_ID);
	edGui->m_PropEditBox_ID->setEnabled(false);

	edGui->m_PropCheckBox_Container = env->addCheckBox(false,rect<s32>(X_1-30,Y_2,windowWidth-marginX-30,Y_2+itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_CONTAINER,L"Container");
	edGui->m_ContainerButton = env->addButton(rect<s32>(X_1-30,Y_2+marginY+itemHeight,windowWidth-marginX-30,Y_2+marginY+2*itemHeight), wnd_properties, GUI_ID_BUTTON_ADD_CONTAINER, L"Container Content");
	edGui->m_ContainerButton->setEnabled(false);

	edGui->m_PropCheckBox_Pickable = env->addCheckBox(false,rect<s32>(X_1-30,Y_2+2*marginY+2*itemHeight,windowWidth-marginX-30,Y_2+2*marginY+3*itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_PICKABLE,L"Pickable");
	edGui->m_PickScriptButton = env->addButton(rect<s32>(X_1-30,Y_2+3*marginY+3*itemHeight,windowWidth-marginX-30,Y_2+3*marginY+4*itemHeight), wnd_properties, GUI_ID_BUTTON_PICK_ADD, L"Pick Script");
	edGui->m_PickScriptButton->setEnabled(false);

	edGui->m_PropCheckBox_Trigger = env->addCheckBox(false,rect<s32>(X_1-30,Y_2+4*marginY+4*itemHeight,windowWidth-marginX-30,Y_2+4*marginY+5*itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_TRIGGER,L"Trigger");
	edGui->m_TriggerScriptButton = env->addButton(rect<s32>(X_1-30,Y_2+5*marginY+5*itemHeight,windowWidth-marginX-30,Y_2+5*marginY+6*itemHeight), wnd_properties, GUI_ID_BUTTON_TRIGGER_ADD, L"Trigger Script");
	edGui->m_TriggerScriptButton->setEnabled(false);

	edGui->m_PropCheckBox_NPC = env->addCheckBox(false,rect<s32>(X_1-30,Y_2+6*marginY+6*itemHeight,windowWidth-marginX-30,Y_2+6*marginY+7*itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_NPC,L"NPC");
	edGui->m_NPCScriptButton = env->addButton(rect<s32>(X_1-30,Y_2+7*marginY+7*itemHeight,windowWidth-marginX-30,Y_2+7*marginY+8*itemHeight), wnd_properties, GUI_ID_BUTTON_NPC_ADD, L"NPC Script");
	edGui->m_NPCScriptButton->setEnabled(false);

	edGui->m_PropCheckBox_Monster = env->addCheckBox(false,rect<s32>(X_1-30,Y_2+8*marginY+8*itemHeight,windowWidth-marginX-30,Y_2+8*marginY+9*itemHeight),wnd_properties,GUI_ID_CHECKBOX_PROPERTY_MONSTER,L"Monster");
	edGui->m_MonsterScriptButton = env->addButton(rect<s32>(X_1-30,Y_2+9*marginY+9*itemHeight,windowWidth-marginX-30,Y_2+9*marginY+10*itemHeight), wnd_properties, GUI_ID_BUTTON_MONSTER_ADD, L"Monster Script");
	edGui->m_MonsterScriptButton->setEnabled(false);

	//position
	env->addStaticText(L"Position", rect<s32>(30,Y_3,100,Y_3+itemHeight), false, false, wnd_properties, -1, false);
	edGui->m_PropButton_Pos_X_Down = env->addButton(rect<s32>(5,Y_3+marginY+itemHeight,20,Y_3+marginY+2*itemHeight), wnd_properties, GUI_ID_BUTTON_POSITION_X_DOWN, L"<");
	edGui->m_PropEditBox_Pos_X = env->addEditBox(L"", rect<s32>(20,Y_3+marginY+itemHeight,100,Y_3+marginY+2*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_POS_X);
	edGui->m_PropButton_Pos_X_Up = env->addButton(rect<s32>(101,Y_3+marginY+itemHeight,116,Y_3+marginY+2*itemHeight), wnd_properties, GUI_ID_BUTTON_POSITION_X_UP, L">");
	env->addStaticText(L"X", rect<s32>(120,Y_3+marginY+itemHeight,130,Y_3+marginY+2*itemHeight), false, false, wnd_properties, -1, false);
	edGui->m_PropButton_Pos_Y_Down = env->addButton(rect<s32>(5,Y_3+marginY+2*itemHeight,20,Y_3+marginY+3*itemHeight), wnd_properties, GUI_ID_BUTTON_POSITION_Y_DOWN, L"<");
	edGui->m_PropEditBox_Pos_Y = env->addEditBox(L"", rect<s32>(20,Y_3+marginY+2*itemHeight,100,Y_3+marginY+3*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_POS_Y);
	edGui->m_PropButton_Pos_Y_Up = env->addButton(rect<s32>(101,Y_3+marginY+2*itemHeight,116,Y_3+marginY+3*itemHeight), wnd_properties, GUI_ID_BUTTON_POSITION_Y_UP, L">");
	env->addStaticText(L"Y", rect<s32>(120,Y_3+marginY+2*itemHeight,130,Y_3+marginY+3*itemHeight), false, false, wnd_properties, -1, false);
	edGui->m_PropButton_Pos_Z_Down = env->addButton(rect<s32>(5,Y_3+marginY+3*itemHeight,20,Y_3+marginY+4*itemHeight), wnd_properties, GUI_ID_BUTTON_POSITION_Z_DOWN, L"<");
	edGui->m_PropEditBox_Pos_Z = env->addEditBox(L"", rect<s32>(20,Y_3+marginY+3*itemHeight,100,Y_3+marginY+4*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_POS_Z);
	edGui->m_PropButton_Pos_Z_Up = env->addButton(rect<s32>(101,Y_3+marginY+3*itemHeight,116,Y_3+marginY+4*itemHeight), wnd_properties, GUI_ID_BUTTON_POSITION_Z_UP, L">");
	env->addStaticText(L"Z", rect<s32>(120,Y_3+marginY+3*itemHeight,130,Y_3+marginY+4*itemHeight), false, false, wnd_properties, -1, false);

	//rotation
	env->addStaticText(L"Rotation", rect<s32>(155,Y_3,windowWidth,Y_3+itemHeight), false, false, wnd_properties, -1, false);
	edGui->m_PropButton_Rot_X_Down = env->addButton(rect<s32>(135,Y_3+marginY+itemHeight,150,Y_3+marginY+2*itemHeight), wnd_properties, GUI_ID_BUTTON_ROTATION_X_DOWN, L"<");
	edGui->m_PropEditBox_Rot_X = env->addEditBox(L"", rect<s32>(150,Y_3+marginY+itemHeight,windowWidth-marginX-16,Y_3+marginY+2*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_ROT_X);
	edGui->m_PropButton_Rot_X_Up = env->addButton(rect<s32>(windowWidth-marginX-15,Y_3+marginY+itemHeight,windowWidth-marginX,Y_3+marginY+2*itemHeight), wnd_properties, GUI_ID_BUTTON_ROTATION_X_UP, L">");
	edGui->m_PropButton_Rot_Y_Down = env->addButton(rect<s32>(135,Y_3+marginY+2*itemHeight,150,Y_3+marginY+3*itemHeight), wnd_properties, GUI_ID_BUTTON_ROTATION_Y_DOWN, L"<");
	edGui->m_PropEditBox_Rot_Y = env->addEditBox(L"", rect<s32>(150,Y_3+marginY+2*itemHeight,windowWidth-marginX-16,Y_3+marginY+3*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_ROT_Y);
	edGui->m_PropButton_Rot_Y_Up = env->addButton(rect<s32>(windowWidth-marginX-15,Y_3+marginY+2*itemHeight,windowWidth-marginX,Y_3+marginY+3*itemHeight), wnd_properties, GUI_ID_BUTTON_ROTATION_Y_UP, L">");
	edGui->m_PropButton_Rot_Z_Down = env->addButton(rect<s32>(135,Y_3+marginY+3*itemHeight,150,Y_3+marginY+4*itemHeight), wnd_properties, GUI_ID_BUTTON_ROTATION_Z_DOWN, L"<");
	edGui->m_PropEditBox_Rot_Z = env->addEditBox(L"", rect<s32>(150,Y_3+marginY+3*itemHeight,windowWidth-marginX-16,Y_3+marginY+4*itemHeight), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_ROT_Z);
	edGui->m_PropButton_Rot_Z_Up = env->addButton(rect<s32>(windowWidth-marginX-15,Y_3+marginY+3*itemHeight,windowWidth-marginX,Y_3+marginY+4*itemHeight), wnd_properties, GUI_ID_BUTTON_ROTATION_Z_UP, L">");

	//description
	env->addStaticText(L"Description:", rect<s32>(X_0,Y_4,150,Y_4+itemHeight), false, false, wnd_properties, -1, false);
	edGui->m_PropEditBox_Description = env->addEditBox(L"", rect<s32>(X_0,Y_4+itemHeight,windowWidth-marginX,Y_5), true, wnd_properties, GUI_ID_EDITBOX_PROPERTY_DESCRIPTION);
	edGui->m_PropEditBox_Description->setMultiLine(true);
	edGui->m_PropEditBox_Description->setWordWrap(true);

	edGui->m_PropButton_Set = env->addButton(rect<s32>(80,Y_6,160,Y_6+itemHeight), wnd_properties, GUI_ID_BUTTON_PROPERTY_SET, L"Set");

	return wnd_properties;
}


//creates window that allows user to add content to container
IGUIWindow* MakeContainerContentWindow(CEditorManager* editorManager, s32 id)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	s32 centerX = device->getVideoDriver()->getScreenSize().Width/2;
	s32 centerY = device->getVideoDriver()->getScreenSize().Height/2;

	stringw objName = editorManager->GetObjectParameter_Name(id);
	stringw caption = stringw(L"Container Content: ") + objName;

	IGUIWindow* wnd_containerContent = env->addWindow(rect<s32>(centerX-330,centerY-200,centerX+330,centerY+250), false, caption.c_str(), 0, GUI_ID_WINDOW_CONTAINER);

	IFileSystem* fs = device->getFileSystem();
	fs->changeWorkingDirectoryTo(PICKABLES_DIR_C8);
	IGUITabControl* tabPickables = env->addTabControl(rect<s32>(2,20,180,430), wnd_containerContent, true, true);
	IGUITab* tab = tabPickables->addTab(L"Pickables");
	Util_PopulateTreeNodeGameObjectsFromXML(editorManager, stringc("Pickables"),tab,rect<s32>(5,2,175,370),GUI_ID_TREECTRL_PICKABLE_OBJECTS);

	edGui->m_PickCheckBox_DirectPick = env->addCheckBox(false, rect<int>(260,260,440,280), wnd_containerContent, GUI_ID_CHECKBOX_CONTAINER_DIRECT_PICK, L"Add item on select");
	edGui->m_pContainer_AddPickItemToContainer = env->addButton(rect<s32>(260,225,370,255), wnd_containerContent, GUI_ID_BUTTON_CONTAINER_ADD_ITEM, L"Add Item >>");
	env->addStaticText(L"Preview:", rect<s32>(190,20,350,40), false, false, wnd_containerContent, -1, false);
	//env->addStaticText(L"Description:", rect<s32>(190,290,350,310), false, false, wnd_containerContent, -1, false);
	edGui->m_pPickPreviewDescriptionText = env->addStaticText(L"No description.", rect<s32>(190,300,410,390), true, true, wnd_containerContent, GUI_ID_TEXT_CONTAINER_PICKABLE_DESC, false);
	edGui->m_pContainer_EditPickItemScript = env->addButton(rect<s32>(190,400,370,430), wnd_containerContent, GUI_ID_BUTTON_CONTAINER_EDIT_PICK_SCRIPT, L"Edit Pick Script");
	edGui->m_pContainer_Ok = env->addButton(rect<s32>(460,380,530,430), wnd_containerContent, GUI_ID_BUTTON_CONTAINER_OK, L"Ok");
	edGui->m_pContainer_Cancel = env->addButton(rect<s32>(550,380,620,430), wnd_containerContent, GUI_ID_BUTTON_CONTAINER_CANCEL, L"Cancel");
	fs->changeWorkingDirectoryTo("../../../");

	//init GUI state
	edGui->m_pPreviewPickableItem = 0;
	edGui->m_PickCheckBox_DirectPick->setChecked(edGui->m_bAddPickDirectly);
	edGui->m_pContainer_AddPickItemToContainer->setEnabled(false);
	edGui->m_pContainer_EditPickItemScript->setEnabled(false);

	//remove preview object
	editorManager->getEdiLevel()->RemovePickSessionModel();
	edGui->m_SelectedPickItemIcon_Texture = 0;

	//Get empty Slot texture
	edGui->m_slotTex = env->getVideoDriver()->getTexture("media/icons/slot.png");
	//Load Container
	IGUIContainer* container = AddGUIContainer(
		env,
		rect<s32>(440,40,635,330), 
		wnd_containerContent, 
		GUI_ID_CONTAINER,
		dimension2d<s32>(3,3),
		dimension2d<s32>(1,1),
		edGui->m_slotTex
		);

	//Load Existing Container Content (if any)
	s32 numberOfItems = editorManager->GetObjectNumberOfPickableItems(id);
	for(s32 i = 0; i<numberOfItems; i++)
	{
		//Add this element
		CGameObject* pick = editorManager->GetObjectPickableItem(id,i);
		container->InsertItem(pick);
	}

	return wnd_containerContent;
}


//creates window for defining pick script action
IGUIWindow* MakePickScriptWindow(CEditorManager* editorManager, stringw script)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	s32 centerX = device->getVideoDriver()->getScreenSize().Width/2;
	s32 centerY = device->getVideoDriver()->getScreenSize().Height/2;

	stringw caption = stringw(L"Pickable Script: ") + script;

	IGUIWindow* wnd_pickScript = env->addWindow(rect<s32>(200,190,1000,640), false, caption.c_str(), 0, GUI_ID_WINDOW_PICKABLE);

	//Layout GUI elements around
	edGui->m_PickCheckBox_Equipable = env->addCheckBox(false, rect<int>(20,30,190,50), wnd_pickScript, GUI_ID_CHECKBOX_PICK_EQUIPABLE, L"Equipable");
	edGui->m_PickCheckBox_Usable = env->addCheckBox(false, rect<int>(20,55,190,75), wnd_pickScript, GUI_ID_CHECKBOX_PICK_USABLE, L"Usable");
	env->addStaticText(L"EVENTS:", rect<s32>(20,100,190,120), false, false, wnd_pickScript, -1, false);
	edGui->m_PickListBox_ExistingEvents = env->addListBox(rect<int>(20,120,170,220), wnd_pickScript, GUI_ID_LISTBOX_PICK_EVENTS, true);
	env->addStaticText(L"Event Script Action:", rect<s32>(200,30,520,50), false, false, wnd_pickScript, -1, false);
	edGui->m_PickEditBox_Script = env->addEditBox(L"", rect<s32>(200,50,790,220), true, wnd_pickScript, GUI_ID_EDITBOX_PICK_SCRIPT);
	edGui->m_PickEditBox_Script->setMultiLine(true);
	edGui->m_PickEditBox_Script->setWordWrap(true);
	env->addStaticText(L"Add event:", rect<s32>(20,227,170,250), false, false, wnd_pickScript, -1, false);
	env->addStaticText(L"Event target:", rect<s32>(20,277,170,300), false, false, wnd_pickScript, -1, false);
	env->addStaticText(L"Add new action:", rect<s32>(200,227,400,250), false, false, wnd_pickScript, -1, false);
	env->addStaticText(L"Action target:", rect<s32>(200,277,350,300), false, false, wnd_pickScript, -1, false);
	env->addStaticText(L"Action attribute:", rect<s32>(500,277,650,300), false, false, wnd_pickScript, -1, false);
	env->addStaticText(L"Action value:", rect<s32>(650,277,800,300), false, false, wnd_pickScript, -1, false);
	env->addStaticText(L"Action target id:", rect<s32>(350,277,500,300), false, false, wnd_pickScript, -1, false);
	env->addStaticText(L"Event target id:", rect<s32>(20,327,170,350), false, false, wnd_pickScript, -1, false);
	edGui->m_PickComboBox_Events = env->addComboBox(rect<s32>(20,250,170,270), wnd_pickScript, GUI_ID_COMBOBOX_PICK_EVENTS);
	edGui->m_PickComboBox_Actions = env->addComboBox(rect<s32>(200,250,400,270), wnd_pickScript, GUI_ID_COMBOBOX_PICK_ACTIONS);
	edGui->m_PickComboBox_EventTarget = env->addComboBox(rect<s32>(20,300,170,320), wnd_pickScript, GUI_ID_COMBOBOX_PICK_EVENT_TARGET);
	edGui->m_PickComboBox_ActionTarget = env->addComboBox(rect<s32>(200,300,340,320), wnd_pickScript, GUI_ID_COMBOBOX_PICK_ACTION_TARGET);
	edGui->m_PickEditBox_ActionAttribute = env->addEditBox(L"", rect<s32>(500,300,640,320), true, wnd_pickScript, GUI_ID_EDITBOX_PICK_ACTION_ATTR);
	edGui->m_PickEditBox_ActionValue = env->addEditBox(L"", rect<s32>(650,300,790,320), true, wnd_pickScript, GUI_ID_EDITBOX_PICK_ACTION_VALUE);
	edGui->m_PickEditBox_ActionTargetID = env->addEditBox(L"", rect<s32>(350,300,490,320), true, wnd_pickScript, GUI_ID_EDITBOX_PICK_ACTION_TARGET_ID);
	edGui->m_PickEditBox_EventTargetID = env->addEditBox(L"", rect<s32>(20,350,170,370), true, wnd_pickScript, GUI_ID_EDITBOX_PICK_EVENT_TARGET_ID);
	edGui->m_PickButton_AddEvent = env->addButton(rect<s32>(20,380,170,400), wnd_pickScript, GUI_ID_BUTTON_PICK_ADD_EVENT, L"Add Event");
	edGui->m_PickButton_RemoveEvent = env->addButton(rect<s32>(20,410,170,430), wnd_pickScript, GUI_ID_BUTTON_PICK_REMOVE_EVENT, L"Remove Event");
	edGui->m_PickButton_AddAction = env->addButton(rect<s32>(420,250,550,270), wnd_pickScript, GUI_ID_BUTTON_PICK_ADD_ACTION, L"Add Action");
	edGui->m_PickButton_Save = env->addButton(rect<s32>(300,400,400,430), wnd_pickScript, GUI_ID_BUTTON_PICK_SAVE, L"Save");
	edGui->m_PickButton_Cancel = env->addButton(rect<s32>(500,400,600,430), wnd_pickScript, GUI_ID_BUTTON_PICK_CANCEL, L"Cancel");
	edGui->m_PickComboBox_Actions->addItem(L"none");
	edGui->m_PickComboBox_Actions->setSelected(0);
	edGui->m_PickComboBox_Events->addItem(L"none");
	edGui->m_PickComboBox_Events->setSelected(0);
	edGui->m_PickComboBox_Events->addItem(L"OnApproach");
	edGui->m_PickComboBox_Events->addItem(L"OnPick");
	edGui->m_PickComboBox_Events->addItem(L"OnDrop");
	edGui->m_PickComboBox_Events->addItem(L"OnGive");
	edGui->m_PickComboBox_Events->addItem(L"OnCombine");
	edGui->m_PickComboBox_EventTarget->addItem(L"none");
	edGui->m_PickComboBox_EventTarget->setSelected(0);
	edGui->m_PickComboBox_EventTarget->addItem(L"player");
	edGui->m_PickComboBox_EventTarget->addItem(L"self");
	edGui->m_PickComboBox_EventTarget->addItem(L"id");
	edGui->m_PickComboBox_ActionTarget->addItem(L"none");
	edGui->m_PickComboBox_ActionTarget->setSelected(0);
	edGui->m_PickComboBox_ActionTarget->addItem(L"player");
	edGui->m_PickComboBox_ActionTarget->addItem(L"self");
	edGui->m_PickComboBox_ActionTarget->addItem(L"id");

	//add actions
	for(u32 i=  0; i < editorManager->getScriptEngine()->m_ListOfScriptActions.size(); i++)
	{
		edGui->m_PickComboBox_Actions->addItem(editorManager->getScriptEngine()->m_ListOfScriptActions[i].c_str());
	}

	//Save active script file name
	edGui->m_PickScript = script.c_str();

	//change to script dir
	IFileSystem* fs = device->getFileSystem();
	fs->changeWorkingDirectoryTo(PICKABLES_SCRIPTS_DIR);

	//Load existing script file if exists
	stringc xml_filename = script.c_str();
	io::IXMLReader* xml = fs->createXMLReader(xml_filename.c_str());
	if (xml)
	{
		editorManager->getScriptEngine()->LoadPickScriptFromXML(xml);
		xml->drop();
	}

	for(u32 i=  0; i < editorManager->getScriptEngine()->m_SelectedListOfScriptActions.size(); i++)
	{
		stringw scriptEvent = editorManager->getScriptEngine()->GetPickScriptActionEvent(i);
		edGui->m_PickListBox_ExistingEvents->addItem(scriptEvent.c_str());
	}
	

	//additional events
	if(editorManager->getScriptEngine()->IsUsable())
	{
		edGui->m_PickCheckBox_Usable->setChecked(true);
		edGui->m_PickComboBox_Events->addItem(L"OnUse");
		edGui->m_PickComboBox_Events->addItem(L"OnUseAgainst");
	}
	if(editorManager->getScriptEngine()->IsEquipable())
	{
		edGui->m_PickCheckBox_Equipable->setChecked(true);
		edGui->m_PickComboBox_Events->addItem(L"OnEquip");
		edGui->m_PickComboBox_Events->addItem(L"OnUnequip");
	}

	if(edGui->m_PickListBox_ExistingEvents->getItemCount()>0)
	{
		edGui->m_PickListBox_ExistingEvents->setSelected(0);
		stringw scriptText = editorManager->getScriptEngine()->PickScriptActionToString_Index(0);
		edGui->m_PickEditBox_Script->setText(scriptText.c_str());
	}
	
	editorManager->backToWorkingDirectory();
	return wnd_pickScript;
}


//creates window for defining trigger action
IGUIWindow* MakeTriggerScriptWindow(CEditorManager* editorManager, s32 id, stringw script)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	s32 centerX = device->getVideoDriver()->getScreenSize().Width/2;
	s32 centerY = device->getVideoDriver()->getScreenSize().Height/2;
	s32 windowWidth = 850;
	s32 windowHeight = 440;

	//margins
	s32 X_center_GUI = windowWidth/2;
	s32 itemWidth = 140;
	s32 itemHeight = 23;
	s32 margin = 10;
	s32 Y_marg = 5;
	s32 X_marg = 10;
	s32 X_1 = 20;
	s32 X_2 = 200;
	s32 X_3 = 400;
	s32 Y_1 = 30;
	s32 Y_2 = 60;
	s32 Y_3 = 180;
	s32 Y_4 = Y_3+2*Y_marg+2*itemHeight;
	s32 Y_5 = 380;


	stringw objName = editorManager->GetObjectParameter_Name(id);
	stringw fileName = stringw(" (") + script + stringw(")");
	stringw caption = stringw(L"Trigger Script: ") + objName + fileName;

	IGUIWindow* wnd_triggerScript = env->addWindow(rect<s32>(centerX - windowWidth/2, centerY - windowHeight/2, centerX + windowWidth/2, centerY + windowHeight/2), false, caption.c_str(), 0, GUI_ID_WINDOW_TRIGGER);

	//Layout GUI elements around
	env->addStaticText(L"Chose State:", rect<s32>(X_1, Y_1, X_2, Y_1+itemHeight), false, false, wnd_triggerScript, -1, false);
	env->addStaticText(L"Trigger Script Action:", rect<s32>(X_2, Y_1, windowWidth-margin, Y_1+itemHeight), false, false, wnd_triggerScript, -1, false);
	env->addStaticText(L"Add new state:", rect<s32>(X_1,Y_3+2*Y_marg,X_2,Y_3+2*Y_marg+itemHeight), false, false, wnd_triggerScript, -1, false);
	//env->addStaticText(L"Event target:", rect<s32>(20,275,90,295), false, false, wnd_triggerScript, -1, false);
	env->addStaticText(L"Add new action:", rect<s32>(X_2,Y_4+2*Y_marg,X_3,Y_4+2*Y_marg+itemHeight), false, false, wnd_triggerScript, -1, false);

	env->addStaticText(L"Action target:", rect<s32>(X_2,Y_4+4*Y_marg+2*itemHeight,X_2+X_marg+itemWidth,Y_4+4*Y_marg+3*itemHeight), false, false, wnd_triggerScript, -1, false);
	env->addStaticText(L"Target ID:", rect<s32>(X_2+2*X_marg+itemWidth,Y_4+4*Y_marg+2*itemHeight,X_2+2*X_marg+2*itemWidth,Y_4+4*Y_marg+3*itemHeight), false, false, wnd_triggerScript, -1, false);
	env->addStaticText(L"Action attribute:", rect<s32>(X_2+3*X_marg+2*itemWidth,Y_4+4*Y_marg+2*itemHeight,X_2+3*X_marg+3*itemWidth,Y_4+4*Y_marg+3*itemHeight), false, false, wnd_triggerScript, -1, false);
	env->addStaticText(L"Action value:", rect<s32>(X_2+4*X_marg+3*itemWidth,Y_4+4*Y_marg+2*itemHeight,X_2+4*X_marg+4*itemWidth,Y_4+4*Y_marg+3*itemHeight), false, false, wnd_triggerScript, -1, false);

	edGui->m_TriggerListBox_States = env->addListBox(rect<int>(X_1,Y_1+itemHeight,X_1+itemWidth,Y_3-margin), wnd_triggerScript, GUI_ID_LISTBOX_TRIGGER_STATES, true);
	edGui->m_TriggerEditBox_Script = env->addEditBox(L"", rect<s32>(X_2,Y_1+itemHeight,windowWidth-margin,Y_4), true, wnd_triggerScript, GUI_ID_EDITBOX_TRIGGER_SCRIPT);
	edGui->m_TriggerEditBox_Script->setMultiLine(true);
	edGui->m_TriggerEditBox_Script->setWordWrap(true);
	edGui->m_TriggerEditBox_State = env->addEditBox(L"", rect<s32>(X_1,Y_3+2*Y_marg+itemHeight,X_1+itemWidth,Y_3+2*Y_marg+2*itemHeight), true, wnd_triggerScript, GUI_ID_EDITBOX_TRIGGER_STATE);
	//edGui->m_PickComboBox_Events = env->addComboBox(rect<s32>(20,250,100,270), wnd_triggerScript, GUI_ID_COMBOBOX_TRIGGER_EVENTS);
	edGui->m_TriggerComboBox_Actions = env->addComboBox(rect<s32>(X_2,Y_4+2*Y_marg+itemHeight,X_3,Y_4+2*Y_marg+2*itemHeight), wnd_triggerScript, GUI_ID_COMBOBOX_TRIGGER_ACTIONS);
	//edGui->m_PickComboBox_EventTarget = env->addComboBox(rect<s32>(20,290,100,310), wnd_triggerScript, GUI_ID_COMBOBOX_TRIGGER_EVENT_TARGET);

	edGui->m_TriggerComboBox_ActionTarget = env->addComboBox(rect<s32>(X_2,Y_4+4*Y_marg+3*itemHeight,X_2+X_marg+itemWidth,Y_4+4*Y_marg+4*itemHeight), wnd_triggerScript, GUI_ID_COMBOBOX_TRIGGER_ACTION_TARGET);
	edGui->m_PickEditBox_ActionTargetID = env->addEditBox(L"", rect<s32>(X_2+2*X_marg+itemWidth,Y_4+4*Y_marg+3*itemHeight,X_2+2*X_marg+2*itemWidth,Y_4+4*Y_marg+4*itemHeight), true, wnd_triggerScript, GUI_ID_EDITBOX_TRIGGER_ACTION_TARGET_ID);
	edGui->m_PickEditBox_ActionAttribute = env->addEditBox(L"", rect<s32>(X_2+3*X_marg+2*itemWidth,Y_4+4*Y_marg+3*itemHeight,X_2+3*X_marg+3*itemWidth,Y_4+4*Y_marg+4*itemHeight), true, wnd_triggerScript, GUI_ID_EDITBOX_TRIGGER_ACTION_ATTRIBUTE);
	edGui->m_PickEditBox_ActionValue = env->addEditBox(L"", rect<s32>(X_2+4*X_marg+3*itemWidth,Y_4+4*Y_marg+3*itemHeight,X_2+4*X_marg+4*itemWidth,Y_4+4*Y_marg+4*itemHeight), true, wnd_triggerScript, GUI_ID_EDITBOX_TRIGGER_ACTION_VALUE);

	edGui->m_TriggerButton_SetState = env->addButton(rect<s32>(X_1,Y_3+3*Y_marg+2*itemHeight,X_1+itemWidth,Y_3+3*Y_marg+3*itemHeight), wnd_triggerScript, GUI_ID_BUTTON_TRIGGER_SET_STATE, L"Set Default State");
	edGui->m_TriggerButton_AddState = env->addButton(rect<s32>(X_1,Y_3+4*Y_marg+3*itemHeight,X_1+itemWidth,Y_3+4*Y_marg+4*itemHeight), wnd_triggerScript, GUI_ID_BUTTON_TRIGGER_ADD_STATE, L"Add State");
	edGui->m_TriggerButton_RemoveState = env->addButton(rect<s32>(X_1,Y_3+5*Y_marg+4*itemHeight,X_1+itemWidth,Y_3+5*Y_marg+5*itemHeight), wnd_triggerScript, GUI_ID_BUTTON_TRIGGER_REMOVE_STATE, L"Remove State");
	edGui->m_TriggerButton_AddAction = env->addButton(rect<s32>(X_3+X_marg,Y_4+2*Y_marg+itemHeight,X_3+X_marg+itemWidth,Y_4+2*Y_marg+2*itemHeight), wnd_triggerScript, GUI_ID_BUTTON_TRIGGER_ADD_ACTION, L"Add Action");

	edGui->m_PickButton_Save = env->addButton(rect<s32>((s32)(X_center_GUI-1.5*itemWidth),Y_5,(s32)(X_center_GUI-0.5*itemWidth),Y_5+30), wnd_triggerScript, GUI_ID_BUTTON_TRIGGER_SAVE, L"Save");
	edGui->m_PickButton_Cancel = env->addButton(rect<s32>((s32)(X_center_GUI+0.5*itemWidth),Y_5,(s32)(X_center_GUI+1.5*itemWidth),Y_5+30), wnd_triggerScript, GUI_ID_BUTTON_TRIGGER_CANCEL, L"Cancel");
	edGui->m_TriggerComboBox_Actions->addItem(L"none");
	edGui->m_TriggerComboBox_Actions->setSelected(0);
	edGui->m_TriggerComboBox_ActionTarget->addItem(L"none");
	edGui->m_TriggerComboBox_ActionTarget->setSelected(0);
	edGui->m_TriggerComboBox_ActionTarget->addItem(L"player");
	edGui->m_TriggerComboBox_ActionTarget->addItem(L"self");
	edGui->m_TriggerComboBox_ActionTarget->addItem(L"id");

	for(u32 i=  0; i < editorManager->getScriptEngine()->m_ListOfScriptActions.size(); i++)
	{
		edGui->m_TriggerComboBox_Actions->addItem(editorManager->getScriptEngine()->m_ListOfScriptActions[i].c_str());
	}

	//Load existing script file if exists
	IFileSystem* fs = device->getFileSystem();
	fs->changeWorkingDirectoryTo(STATIC_SCRIPTS_DIR);
	stringc xml_filename = script.c_str();
	IXMLReader* xml = fs->createXMLReader(xml_filename.c_str());
	if(xml)
	{
		stringw default_state = editorManager->getScriptEngine()->LoadTriggerScriptFromXML(xml);
		editorManager->SetObjectParameter_State(id, default_state);
		xml->drop();
	}

	//Add existing states (that were loaded from script xml file)
	for(u32 i=  0; i < editorManager->getScriptEngine()->m_SelectedListOfScriptActions.size(); i++)
	{
		stringw state = editorManager->getScriptEngine()->GetTriggerScriptActionState(i);
		edGui->m_TriggerListBox_States->addItem(state.c_str());
	}
	
	//Set first state selected and load its actions in edit window.
	if(edGui->m_TriggerListBox_States->getItemCount() > 0)
	{
		edGui->m_TriggerListBox_States->setSelected(0);
		stringw scriptText = editorManager->getScriptEngine()->TriggerScriptActionToString_Index(0);
		edGui->m_TriggerEditBox_Script->setText(scriptText.c_str());
	}

	editorManager->backToWorkingDirectory();
	return wnd_triggerScript;
}


//creates window that allows user to write NPC dialogs
IGUIWindow* MakeNPCScriptWindow(CEditorManager* editorManager, stringc dialogFilename)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	s32 centerX = device->getVideoDriver()->getScreenSize().Width/2;
	s32 centerY = device->getVideoDriver()->getScreenSize().Height/2;
	s32 windowWidth = 950;
	s32 windowHeight = 700;
	s32 itemHeight = 20;
	s32 middleX = windowWidth/2;
	s32 leftMarginX = 5;
	s32 rightMarginX = 5;
	s32 topMarginY = 24;
	s32 topMarginY_2 = 24;
	s32 spaceX = 10;
	s32 spaceY = 5;
	s32 rightX = windowWidth - rightMarginX;
	s32 Y_2 = 55;
	s32 Y_10 = 300;
	s32 Y_11 = Y_10+2*spaceY+5*itemHeight;
	s32 Y_12 = Y_11+4*spaceY+8*itemHeight;
	s32 X_10 = 700;
	s32 X_11 = X_10 + spaceX;
	s32 X_12 = X_11 + ((rightX-X_11)/2 - 5);

	IGUIWindow* wnd_NPC = env->addWindow(rect<s32>(centerX-windowWidth/2,centerY-windowHeight/2,centerX+windowWidth/2,centerY+windowHeight/2), false, L"NPC Script and Dialog", 0, GUI_ID_WINDOW_NPC);

	//Layout GUI elements around

	//tab containing the tree element (tree element is not working correctly if not placed on tab!)
	IGUITabControl* tabPickables = env->addTabControl(rect<s32>(leftMarginX,topMarginY,X_10,Y_10), wnd_NPC, true, true);
	IGUITab* tab = tabPickables->addTab(L"Dialog Tree");

	//here are add/remove/link/enable dialog tree node buttons
	edGui->m_DialogButton_AddNode = env->addButton(rect<s32>(X_11,Y_2,rightX,Y_2+itemHeight), wnd_NPC, GUI_ID_BUTTON_NPC_ADD_NODE, L"Add Node");
	edGui->m_DialogButton_RemoveNode = env->addButton(rect<s32>(X_11,Y_2+itemHeight+spaceY,rightX,Y_2+2*itemHeight+spaceY), wnd_NPC, GUI_ID_BUTTON_NPC_REMOVE_NODE, L"Remove Node");
	edGui->m_DialogButton_LinkNodes = env->addButton(rect<s32>(X_11,Y_2+2*itemHeight+2*spaceY,rightX,Y_2+3*itemHeight+2*spaceY), wnd_NPC, GUI_ID_BUTTON_NPC_LINK_NODE, L"Link Nodes");
	edGui->m_DialogButton_ToggleEnabled = env->addButton(rect<s32>(X_11,Y_2+3*itemHeight+3*spaceY,rightX,Y_2+4*itemHeight+3*spaceY), wnd_NPC, GUI_ID_BUTTON_NPC_TOGGLE_ENABLED, L"Toggle Enabled");
	env->addStaticText(L"Selected Node ID:", rect<s32>(X_11,Y_2+4*itemHeight+4*spaceY,rightX,Y_2+5*itemHeight+4*spaceY), false, false, wnd_NPC, -1, false);
	edGui->m_DialogEditBox_SelectedNodeID = env->addEditBox(L"", rect<s32>(X_11,Y_2+5*itemHeight+5*spaceY,rightX,Y_2+6*itemHeight+5*spaceY), true, wnd_NPC, GUI_ID_EDITBOX_NPC_SELECTED_NODE_ID);
	edGui->m_DialogEditBox_SelectedNodeID->setEnabled(false);

	//dialog text edit window
	env->addStaticText(L"Dialog Text:", rect<s32>(leftMarginX,Y_10+spaceY,X_10,Y_10+spaceY+itemHeight), false, false, wnd_NPC, -1, false);
	edGui->m_DialogEditBox_ChangeText = env->addEditBox(L"", rect<s32>(leftMarginX,Y_10+spaceY+itemHeight,X_10,Y_10+spaceY+5*itemHeight), true, wnd_NPC, GUI_ID_EDITBOX_NPC_CHANGE_TEXT);
	edGui->m_DialogButton_ChangeText = env->addButton(rect<s32>(X_11,Y_10+spaceY+itemHeight,rightX,Y_10+spaceY+2*itemHeight), wnd_NPC, GUI_ID_BUTTON_NPC_CHANGE_TEXT, L"Change Text");
	edGui->m_DialogEditBox_ChangeText->setMultiLine(true);
	edGui->m_DialogEditBox_ChangeText->setWordWrap(true);
	edGui->m_DialogEditBox_ChangeText->setAutoScroll(true);

	//dialog actions gui elements
	env->addStaticText(L"Add Action to selected dialog node:", rect<s32>(leftMarginX,Y_11,X_10,Y_11+itemHeight), false, false, wnd_NPC, -1, false);
	edGui->m_DialogEditBox_ActionsText = env->addEditBox(L"", rect<s32>(leftMarginX,Y_11+itemHeight,X_10,Y_11+8*itemHeight), true, wnd_NPC, GUI_ID_EDITBOX_NPC_ACTIONS);
	edGui->m_DialogEditBox_ActionsText->setMultiLine(true);
	edGui->m_DialogEditBox_ActionsText->setWordWrap(true);
	edGui->m_DialogComboBox_Actions = env->addComboBox(rect<s32>(X_11,Y_11+itemHeight,rightX,Y_11+2*itemHeight), wnd_NPC, GUI_ID_COMBOBOX_NPC_ACTIONS);
	env->addStaticText(L"Target:", rect<s32>(X_11,Y_11+spaceY+2*itemHeight,X_12,Y_11+spaceY+3*itemHeight), false, false, wnd_NPC, -1, false);
	env->addStaticText(L"ID:", rect<s32>(X_12+10,Y_11+spaceY+2*itemHeight,rightX,Y_11+spaceY+3*itemHeight), false, false, wnd_NPC, -1, false);
	edGui->m_DialogComboBox_ActionTarget = env->addComboBox(rect<s32>(X_11,Y_11+spaceY+3*itemHeight,X_12,Y_11+spaceY+4*itemHeight), wnd_NPC, GUI_ID_COMBOBOX_NPC_ACTION_TARGET);
	edGui->m_DialogEditBox_ActionTargetID = env->addEditBox(L"", rect<s32>(X_12+10,Y_11+spaceY+3*itemHeight,rightX,Y_11+spaceY+4*itemHeight), true, wnd_NPC, GUI_ID_EDITBOX_NPC_ACTION_TARGET_ID);
	env->addStaticText(L"Value:", rect<s32>(X_11,Y_11+2*spaceY+4*itemHeight,X_12,Y_11+2*spaceY+5*itemHeight), false, false, wnd_NPC, -1, false);
	env->addStaticText(L"Attribute:", rect<s32>(X_12+10,Y_11+2*spaceY+4*itemHeight,rightX,Y_11+2*spaceY+5*itemHeight), false, false, wnd_NPC, -1, false);
	edGui->m_DialogEditBox_ActionAttribute = env->addEditBox(L"", rect<s32>(X_12+10,Y_11+2*spaceY+5*itemHeight,rightX,Y_11+2*spaceY+6*itemHeight), true, wnd_NPC, GUI_ID_EDITBOX_NPC_ACTION_ATTR);
	edGui->m_DialogEditBox_ActionValue = env->addEditBox(L"", rect<s32>(X_11,Y_11+2*spaceY+5*itemHeight,X_12,Y_11+2*spaceY+6*itemHeight), true, wnd_NPC, GUI_ID_EDITBOX_NPC_ACTION_VALUE);
	edGui->m_DialogButton_AddAction = env->addButton(rect<s32>(X_12+10,Y_11+4*spaceY+6*itemHeight,rightX,Y_11+4*spaceY+7*itemHeight), wnd_NPC, GUI_ID_BUTTON_NPC_ADD_ACTION, L"Add Action");
	edGui->m_DialogButton_ClearAction = env->addButton(rect<s32>(X_11,Y_11+4*spaceY+6*itemHeight,X_12,Y_11+4*spaceY+7*itemHeight), wnd_NPC, GUI_ID_BUTTON_NPC_CLEAR_ACTION, L"Clear Actions");
	for(u32 i=  0; i < editorManager->getScriptEngine()->m_ListOfScriptActions.size(); i++)
	{
		edGui->m_DialogComboBox_Actions->addItem(editorManager->getScriptEngine()->m_ListOfScriptActions[i].c_str());
	}
	edGui->m_DialogComboBox_ActionTarget->addItem(L"none");
	edGui->m_DialogComboBox_ActionTarget->setSelected(0);
	edGui->m_DialogComboBox_ActionTarget->addItem(L"player");
	edGui->m_DialogComboBox_ActionTarget->addItem(L"self");
	edGui->m_DialogComboBox_ActionTarget->addItem(L"id");

	//dialog conditions
	env->addStaticText(L"Add Condition to selected dialog node:", rect<s32>(leftMarginX,Y_12,X_10,Y_12+itemHeight), false, false, wnd_NPC, -1, false);
	edGui->m_DialogComboBox_Conditions = env->addComboBox(rect<s32>(leftMarginX,Y_12+itemHeight,X_10,Y_12+2*itemHeight), wnd_NPC, GUI_ID_COMBOBOX_NPC_CONDITIONS);
	edGui->m_DialogButton_AddCondition = env->addButton(rect<s32>(X_11,Y_12+itemHeight,rightX,Y_12+2*itemHeight), wnd_NPC, GUI_ID_BUTTON_NPC_ADD_CONDITION, L"Add Condition");

	//ok and cancel buttons
	edGui->m_DialogButton_Ok = env->addButton(rect<s32>(middleX-100-20,Y_12+3*itemHeight,middleX-20,Y_12+4*itemHeight+10), wnd_NPC, GUI_ID_BUTTON_NPC_OK, L"Ok");
	edGui->m_DialogButton_Cancel = env->addButton(rect<s32>(middleX+20,Y_12+3*itemHeight,middleX+20+100,Y_12+4*itemHeight+10), wnd_NPC, GUI_ID_BUTTON_NPC_CANCEL, L"Cancel");
	env->setFocus(edGui->m_DialogButton_AddNode);

	//File system
	IFileSystem* fs = device->getFileSystem();
	fs->changeWorkingDirectoryTo(DIALOG_DIR);

	//Load Dialog Tree
	PopulateDialogTreeNodeFromXML(editorManager, dialogFilename.c_str(), tab, rect<s32>(leftMarginX,spaceY,X_10-3*leftMarginX,Y_10-spaceY-Y_2));

	//Back to root dir
	editorManager->backToWorkingDirectory();

	return wnd_NPC;
}


//creates window that allows user to define monster behavior and attributes
IGUIWindow* MakeMonsterScriptWindow(CEditorManager* editorManager, CGameObject* go)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	s32 centerX = device->getVideoDriver()->getScreenSize().Width/2;
	s32 centerY = device->getVideoDriver()->getScreenSize().Height/2;
	s32 windowWidth = 950;
	s32 windowHeight = 500;
	s32 itemHeight = 25;
	s32 leftMarginX = 5;
	s32 rightMarginX = 5;
	s32 topMarginY = 24;
	s32 topMarginY_2 = 24;
	s32 spaceX = 10;
	s32 spaceY = 5;
	s32 rightX = windowWidth - rightMarginX;
	s32 Y_SKILLS = 55;
	s32 column2leftX = 300;
	s32 column2midX = 400;
	s32 column2rightX = 470;
	
	s32 column3leftX = 500;
	s32 column3midX = 600;
	s32 column3rightX = 850;

	s32 saveX1 = 200;
	s32 saveX2 = 400;
	s32 saveX3 = 600;
	s32 saveY = 350;

	IGUIWindow* wnd_monster = env->addWindow(rect<s32>(centerX-windowWidth/2,centerY-windowHeight/2,centerX+windowWidth/2,centerY+windowHeight/2), false, L"Monster Properties", 0, GUI_ID_WINDOW_MONSTER);
	u32 i=0;
	//Layout GUI elements around
	env->addStaticText(L"Ability:", rect<s32>(leftMarginX,topMarginY+20,200,topMarginY+40), false, false, wnd_monster, 0, false);
	env->addStaticText(L"Min:", rect<s32>(110,topMarginY+20,200,topMarginY+40), false, false, wnd_monster, 0, false);
	env->addStaticText(L"Max:", rect<s32>(190,topMarginY+20,300,topMarginY+40), false, false, wnd_monster, 0, false);
	for(i=0; i<go->m_ListOfSkills.size(); i++)
	{
		env->addStaticText(go->m_ListOfSkills[i].skillName.c_str(), rect<s32>(leftMarginX,Y_SKILLS+topMarginY+itemHeight*i+spaceY*i,100,Y_SKILLS+topMarginY+itemHeight*(i+1)+spaceY*i), false, false, wnd_monster, GUI_ID_MONSTER_SKILL+i*10, false);
		env->addEditBox(stringw(go->m_ListOfSkills[i].min).c_str(), rect<s32>(110,Y_SKILLS+topMarginY+itemHeight*i+spaceY*i,180,Y_SKILLS+topMarginY+itemHeight*(i+1)+spaceY*i), true, wnd_monster, GUI_ID_MONSTER_SKILL+i*10+1);
		env->addEditBox(stringw(go->m_ListOfSkills[i].max).c_str(), rect<s32>(190,Y_SKILLS+topMarginY+itemHeight*i+spaceY*i,260,Y_SKILLS+topMarginY+itemHeight*(i+1)+spaceY*i), true, wnd_monster, GUI_ID_MONSTER_SKILL+i*10+2);
		//go->m_ListOfSkills[i].skillName;
	}
	env->addButton(rect<s32>(leftMarginX,Y_SKILLS+topMarginY+itemHeight*i+spaceY*i,100,Y_SKILLS+topMarginY+itemHeight*(i+1)+spaceY*i),wnd_monster,GUI_ID_MONSTER_SKILL+i*10+3,L"Add Skill");

	env->addStaticText(L"Preferences:", rect<s32>(column2leftX,topMarginY+20,column2leftX+100,topMarginY+40), false, false, wnd_monster, 0, false);
	env->addStaticText(L"Radius:", rect<s32>(column2leftX,Y_SKILLS+topMarginY,column2midX,Y_SKILLS+topMarginY+itemHeight), false, false, wnd_monster, 0, false);
	env->addEditBox(stringw(go->m_Radius).c_str(), rect<s32>(column2midX,Y_SKILLS+topMarginY,column2rightX,Y_SKILLS+topMarginY+itemHeight), true, wnd_monster, GUI_ID_MONSTER_RADIUS);
	env->addStaticText(L"Mood:", rect<s32>(column2leftX,Y_SKILLS+topMarginY+itemHeight+spaceY,column2midX,Y_SKILLS+topMarginY+2*itemHeight+spaceY), false, false, wnd_monster, 0, false);
	edGui->m_Monsters_ComboBox_Mood = env->addComboBox(rect<s32>(column2midX,Y_SKILLS+topMarginY+itemHeight+spaceY,column2rightX,Y_SKILLS+topMarginY+2*itemHeight+spaceY), wnd_monster, GUI_ID_MONSTER_MOOD);
	edGui->m_Monsters_ComboBox_Mood->addItem(L"Relaxed");
	edGui->m_Monsters_ComboBox_Mood->addItem(L"Touchy");
	edGui->m_Monsters_ComboBox_Mood->addItem(L"Rabid");
	edGui->m_Monsters_ComboBox_Mood->setSelected(go->m_Mood);
	env->addStaticText(L"Health:", rect<s32>(column2leftX,Y_SKILLS+topMarginY+2*itemHeight+2*spaceY,column2midX,Y_SKILLS+topMarginY+3*itemHeight+2*spaceY), false, false, wnd_monster, 0, false);
	edGui->m_Monsters_EditBox_Health = env->addEditBox(stringw(go->getAbilityValue("Health")).c_str(), rect<s32>(column2midX,Y_SKILLS+topMarginY+2*itemHeight+2*spaceY,column2rightX,Y_SKILLS+topMarginY+3*itemHeight+2*spaceY), true, wnd_monster, GUI_ID_MONSTER_HP);
	env->addStaticText(L"Exp:", rect<s32>(column2leftX,Y_SKILLS+topMarginY+3*itemHeight+3*spaceY,column2midX,Y_SKILLS+topMarginY+4*itemHeight+3*spaceY), false, false, wnd_monster, 0, false);
	env->addEditBox(stringw(go->getAbilityValue("Experience")).c_str(), rect<s32>(column2midX,Y_SKILLS+topMarginY+3*itemHeight+3*spaceY,column2rightX,Y_SKILLS+topMarginY+4*itemHeight+3*spaceY), true, wnd_monster, GUI_ID_MONSTER_EXP);

	env->addStaticText(L"Sounds:", rect<s32>(column3leftX,topMarginY+20,column3leftX+100,topMarginY+40), false, false, wnd_monster, 0, false);
	env->addStaticText(L"Ambient:", rect<s32>(column3leftX,Y_SKILLS+topMarginY,column3midX,Y_SKILLS+topMarginY+itemHeight), false, false, wnd_monster, 0, false);
	env->addEditBox(go->m_Sound_Ambient.c_str(), rect<s32>(column3midX,Y_SKILLS+topMarginY,column3rightX,Y_SKILLS+topMarginY+itemHeight), true, wnd_monster, GUI_ID_MONSTER_SOUND_AMBIENT);
	env->addStaticText(L"Attack:", rect<s32>(column3leftX,Y_SKILLS+topMarginY+itemHeight+spaceY,column3midX,Y_SKILLS+topMarginY+2*itemHeight+spaceY), false, false, wnd_monster, 0, false);
	env->addEditBox(go->m_Sound_Attack.c_str(), rect<s32>(column3midX,Y_SKILLS+topMarginY+itemHeight+spaceY,column3rightX,Y_SKILLS+topMarginY+2*itemHeight+spaceY), true, wnd_monster, GUI_ID_MONSTER_SOUND_ATTACK);
	env->addStaticText(L"Hit:", rect<s32>(column3leftX,Y_SKILLS+topMarginY+2*itemHeight+2*spaceY,column3midX,Y_SKILLS+topMarginY+3*itemHeight+2*spaceY), false, false, wnd_monster, 0, false);
	env->addEditBox(go->m_Sound_Hit.c_str(), rect<s32>(column3midX,Y_SKILLS+topMarginY+2*itemHeight+2*spaceY,column3rightX,Y_SKILLS+topMarginY+3*itemHeight+2*spaceY), true, wnd_monster, GUI_ID_MONSTER_SOUND_HIT);
	env->addStaticText(L"Wounded:", rect<s32>(column3leftX,Y_SKILLS+topMarginY+3*itemHeight+3*spaceY,column3midX,Y_SKILLS+topMarginY+4*itemHeight+3*spaceY), false, false, wnd_monster, 0, false);
	env->addEditBox(go->m_Sound_Wound.c_str(), rect<s32>(column3midX,Y_SKILLS+topMarginY+3*itemHeight+3*spaceY,column3rightX,Y_SKILLS+topMarginY+4*itemHeight+3*spaceY), true, wnd_monster, GUI_ID_MONSTER_SOUND_WOUND);
	env->addStaticText(L"Hello:", rect<s32>(column3leftX,Y_SKILLS+topMarginY+4*itemHeight+4*spaceY,column3midX,Y_SKILLS+topMarginY+5*itemHeight+4*spaceY), false, false, wnd_monster, 0, false);
	env->addEditBox(go->m_Sound_Hello.c_str(), rect<s32>(column3midX,Y_SKILLS+topMarginY+4*itemHeight+4*spaceY,column3rightX,Y_SKILLS+topMarginY+5*itemHeight+4*spaceY), true, wnd_monster, GUI_ID_MONSTER_SOUND_HELLO);
	env->addStaticText(L"Die:", rect<s32>(column3leftX,Y_SKILLS+topMarginY+5*itemHeight+5*spaceY,column3midX,Y_SKILLS+topMarginY+6*itemHeight+5*spaceY), false, false, wnd_monster, 0, false);
	env->addEditBox(go->m_Sound_Die.c_str(), rect<s32>(column3midX,Y_SKILLS+topMarginY+5*itemHeight+5*spaceY,column3rightX,Y_SKILLS+topMarginY+6*itemHeight+5*spaceY), true, wnd_monster, GUI_ID_MONSTER_SOUND_DIE);

	env->addButton(rect<s32>(saveX1,saveY,saveX1+150,saveY+25),wnd_monster,GUI_ID_MONSTER_SAVE,L"Save", L"Save different preferences for just this monster (Saved to map file).");
	env->addButton(rect<s32>(saveX2,saveY,saveX2+150,saveY+25),wnd_monster,GUI_ID_MONSTER_SAVEFORALL,L"Save For All", L"Save preferences for all monsters of this type (Saved to monster personal xml file).");
	env->addButton(rect<s32>(saveX3,saveY,saveX3+150,saveY+25),wnd_monster,GUI_ID_MONSTER_CANCEL,L"Cancel");

	return wnd_monster;
}



//creates window for importing terain heightmap
IGUIWindow* MakeTerrainHeightmapWindow(CEditorManager* editorManager)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	IGUIWindow* wnd_TH = env->addWindow(rect<s32>(270,90,700,450), false, L"Terrain from Hightmap", 0, GUI_ID_WINDOW_TH);

	//Layout GUI elements around
	
	env->addStaticText(L"Heightmap file:", rect<s32>(15,40,160,55), false, false, wnd_TH, -1, false);
	edGui->m_TH_EditBox_HeightmapFile = env->addEditBox(L"media/terrain/terrain-heightmap.bmp", rect<s32>(15,55,290,75), true, wnd_TH, GUI_ID_EDITBOX_TH_HEIGHT_FILE);
	edGui->m_TH_Button_HeightmapBrowse = env->addButton(rect<s32>(300,55,390,75), wnd_TH, GUI_ID_BUTTON_TH_HEIGHTMAP_BROWSE, L"Browse");
	env->addStaticText(L"Texture file:", rect<s32>(15,80,160,95), false, false, wnd_TH, -1, false);
	edGui->m_TH_EditBox_TexFile = env->addEditBox(L"media/terrain/terrain-texture.jpg", rect<s32>(15,95,290,115), true, wnd_TH, GUI_ID_EDITBOX_TH_TEX_FILE);
	edGui->m_TH_Button_TexBrowse = env->addButton(rect<s32>(300,95,390,115), wnd_TH, GUI_ID_BUTTON_TH_TEXTURE_BROWSE, L"Browse");
	env->addStaticText(L"Detail map file:", rect<s32>(15,120,160,135), false, false, wnd_TH, -1, false);
	edGui->m_TH_EditBox_DetailFile = env->addEditBox(L"media/terrain/detailmap2.jpg", rect<s32>(15,135,290,155), true, wnd_TH, GUI_ID_EDITBOX_TH_DETAIL_FILE);
	edGui->m_TH_Button_DetailBrowse = env->addButton(rect<s32>(300,135,390,155), wnd_TH, GUI_ID_BUTTON_TH_DETAIL_BROWSE, L"Browse");

	env->addStaticText(L"Position:", rect<s32>(15,170,160,185), false, false, wnd_TH, -1, false);
	edGui->m_TH_EditBox_PosX = env->addEditBox(L"0.0", rect<s32>(15,185,90,205), true, wnd_TH, GUI_ID_EDITBOX_TH_POS_X);
	edGui->m_TH_EditBox_PosY = env->addEditBox(L"0.0", rect<s32>(95,185,170,205), true, wnd_TH, GUI_ID_EDITBOX_TH_POS_Y);
	edGui->m_TH_EditBox_PosZ = env->addEditBox(L"0.0", rect<s32>(175,185,250,205), true, wnd_TH, GUI_ID_EDITBOX_TH_POS_Z);

	env->addStaticText(L"Rotation:", rect<s32>(15,210,160,225), false, false, wnd_TH, -1, false);
	edGui->m_TH_EditBox_RotX = env->addEditBox(L"0.0", rect<s32>(15,225,90,245), true, wnd_TH, GUI_ID_EDITBOX_TH_ROT_X);
	edGui->m_TH_EditBox_RotY = env->addEditBox(L"0.0", rect<s32>(95,225,170,245), true, wnd_TH, GUI_ID_EDITBOX_TH_ROT_Y);
	edGui->m_TH_EditBox_RotZ = env->addEditBox(L"0.0", rect<s32>(175,225,250,245), true, wnd_TH, GUI_ID_EDITBOX_TH_ROT_Z);

	env->addStaticText(L"Scale:", rect<s32>(15,250,160,265), false, false, wnd_TH, -1, false);
	edGui->m_TH_EditBox_ScaleX = env->addEditBox(L"1.0", rect<s32>(15,265,90,285), true, wnd_TH, GUI_ID_EDITBOX_TH_SCALE_X);
	edGui->m_TH_EditBox_ScaleY = env->addEditBox(L"1.0", rect<s32>(95,265,170,285), true, wnd_TH, GUI_ID_EDITBOX_TH_SCALE_Y);
	edGui->m_TH_EditBox_ScaleZ = env->addEditBox(L"1.0", rect<s32>(175,265,250,285), true, wnd_TH, GUI_ID_EDITBOX_TH_SCALE_Z);

	env->addStaticText(L"MaxLOD:", rect<s32>(300,170,380,185), false, false, wnd_TH, -1, false);
	edGui->m_TH_EditBox_MaxLOD = env->addEditBox(L"5", rect<s32>(300,185,350,205), true, wnd_TH, GUI_ID_EDITBOX_TH_MAXLOD);

	env->addStaticText(L"Smooth Factor:", rect<s32>(300,210,380,225), false, false, wnd_TH, -1, false);
	edGui->m_TH_EditBox_SmoothFactor = env->addEditBox(L"4", rect<s32>(300,225,350,245), true, wnd_TH, GUI_ID_EDITBOX_TH_SMOOTH);

	env->addStaticText(L"Patch Size:", rect<s32>(300,250,380,265), false, false, wnd_TH, -1, false);
	edGui->m_TH_ComboBox_PatchSize = env->addComboBox(rect<s32>(300,265,390,285), wnd_TH, GUI_ID_EDITBOX_TH_PATCH);
	edGui->m_TH_ComboBox_PatchSize->addItem(L"ETPS_9");
	edGui->m_TH_ComboBox_PatchSize->addItem(L"ETPS_17");
	edGui->m_TH_ComboBox_PatchSize->setSelected(1);
	edGui->m_TH_ComboBox_PatchSize->addItem(L"ETPS_33");
	edGui->m_TH_ComboBox_PatchSize->addItem(L"ETPS_65");
	edGui->m_TH_ComboBox_PatchSize->addItem(L"ETPS_129");

	edGui->m_TH_Button_Insert = env->addButton(rect<s32>(150,320,280,340), wnd_TH, GUI_ID_BUTTON_TH_INSERT, L"Insert Terrain");
	env->setFocus(edGui->m_TH_Button_Insert);

	return wnd_TH;
}

//creates window for setting particle properties before adding them to map
IGUIWindow* MakeParticleWindow(CEditorManager* editorManager)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	IGUIWindow* wnd_Particles = env->addWindow(rect<s32>(370,90,800,470), false, L"Add Particles", 0, GUI_ID_WINDOW_LEVEL_MUSIC);

	s32 itemHeight = 25;
	s32 leftMarginX = 10;
	s32 topMarginY = 40;
	s32 spaceX = 10;
	s32 spaceY = 5;
	s32 widthX = 100;

	//Layout GUI elements around
	
	env->addStaticText(L"Name:", rect<s32>(leftMarginX,topMarginY,leftMarginX+widthX,topMarginY+itemHeight), false, false, wnd_Particles, -1, false);
	edGui->m_LevelParticles_EditBox_Name = env->addEditBox(L"Particles", rect<s32>(leftMarginX+widthX+spaceX,topMarginY,leftMarginX+3*widthX+spaceX,topMarginY+itemHeight), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_NAME);
	
	env->addStaticText(L"Texture:", rect<s32>(leftMarginX,topMarginY+itemHeight+spaceY,leftMarginX+widthX,topMarginY+2*itemHeight+spaceY), false, false, wnd_Particles, -1, false);
	edGui->m_LevelParticles_EditBox_TextureFile = env->addEditBox(L"media/particle1.bmp", rect<s32>(leftMarginX+widthX+spaceX,topMarginY+itemHeight+spaceY,leftMarginX+3*widthX+spaceX,topMarginY+2*itemHeight+spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_TEXTURE_FILE);
	edGui->m_LevelParticles_Button_TextureBrowse = env->addButton(rect<s32>(leftMarginX+3*widthX+2*spaceX,topMarginY+itemHeight+spaceY,(s32)(leftMarginX+3.4*widthX+2*spaceX),topMarginY+2*itemHeight+spaceY), wnd_Particles, GUI_ID_BUTTON_LEVELPARTICLES_TEXTURE_BROWSE, L"...");
	
	env->addStaticText(L"Emiter:", rect<s32>(leftMarginX,topMarginY+2*itemHeight+2*spaceY,leftMarginX+widthX,topMarginY+3*itemHeight+2*spaceY), false, false, wnd_Particles, -1, false);
	edGui->m_LevelParticles_ComboBox_Emiter = env->addComboBox(rect<s32>(leftMarginX+widthX+spaceX,topMarginY+2*itemHeight+2*spaceY,leftMarginX+2*widthX+spaceX,topMarginY+3*itemHeight+2*spaceY), wnd_Particles, GUI_ID_COMBOBOX_LEVELPARTICLES_EMITER);
	edGui->m_LevelParticles_ComboBox_Emiter->addItem(L"BOX");
	edGui->m_LevelParticles_ComboBox_Emiter->addItem(L"RING");
	edGui->m_LevelParticles_ComboBox_Emiter->addItem(L"SPHERE");
	edGui->m_LevelParticles_ComboBox_Emiter->addItem(L"CYLINDER");
	edGui->m_LevelParticles_ComboBox_Emiter->addItem(L"POINT");
	edGui->m_LevelParticles_ComboBox_Emiter->setSelected(0);
	
	env->addStaticText(L"Direction:", rect<s32>(leftMarginX,topMarginY+3*itemHeight+3*spaceY,leftMarginX+widthX,topMarginY+4*itemHeight+3*spaceY), false, false, wnd_Particles, -1, false);
	edGui->m_LevelParticles_EditBox_DirectionX = env->addEditBox(L"0.0", rect<s32>(leftMarginX+widthX+spaceX,topMarginY+3*itemHeight+3*spaceY,(s32)(leftMarginX+1.4*widthX+spaceX),topMarginY+4*itemHeight+3*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_DIRECTION_X);
	edGui->m_LevelParticles_EditBox_DirectionY = env->addEditBox(L"0.01", rect<s32>((s32)(leftMarginX+1.4*widthX+2*spaceX),topMarginY+3*itemHeight+3*spaceY,(s32)(leftMarginX+1.8*widthX+2*spaceX),topMarginY+4*itemHeight+3*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_DIRECTION_Y);
	edGui->m_LevelParticles_EditBox_DirectionZ = env->addEditBox(L"0.0", rect<s32>((s32)(leftMarginX+1.8*widthX+3*spaceX),topMarginY+3*itemHeight+3*spaceY,(s32)(leftMarginX+2.2*widthX+3*spaceX),topMarginY+4*itemHeight+3*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_DIRECTION_Z);

	env->addStaticText(L"Min", rect<s32>(leftMarginX+widthX+spaceX,topMarginY+4*itemHeight+5*spaceY,(s32)(leftMarginX+1.5*widthX+spaceX),topMarginY+5*itemHeight+5*spaceY), false, false, wnd_Particles, -1, false);
	env->addStaticText(L"Max", rect<s32>((s32)(leftMarginX+1.5*widthX+2*spaceX),topMarginY+4*itemHeight+5*spaceY,leftMarginX+2*widthX+2*spaceX,topMarginY+5*itemHeight+5*spaceY), false, false, wnd_Particles, -1, false);

	env->addStaticText(L"Emit Rate:", rect<s32>(leftMarginX,topMarginY+5*itemHeight+4*spaceY,leftMarginX+widthX,topMarginY+6*itemHeight+4*spaceY), false, false, wnd_Particles, -1, false);
	edGui->m_LevelParticles_EditBox_EmitRateMin = env->addEditBox(L"80", rect<s32>(leftMarginX+widthX+spaceX,topMarginY+5*itemHeight+4*spaceY,(s32)(leftMarginX+1.5*widthX+spaceX),topMarginY+6*itemHeight+4*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_EMITRATE_MIN);
	edGui->m_LevelParticles_EditBox_EmitRateMax = env->addEditBox(L"100", rect<s32>((s32)(leftMarginX+1.5*widthX+2*spaceX),topMarginY+5*itemHeight+4*spaceY,leftMarginX+2*widthX+2*spaceX,topMarginY+6*itemHeight+4*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_EMITRATE_MAX);
	edGui->m_LevelParticles_EditBox_EmitRateMin->setToolTipText(L"Minimum number of particles per second");
	edGui->m_LevelParticles_EditBox_EmitRateMax->setToolTipText(L"Maximum number of particles per second");

	env->addStaticText(L"Emiter Size:", rect<s32>(leftMarginX,topMarginY+6*itemHeight+5*spaceY,leftMarginX+widthX,topMarginY+7*itemHeight+5*spaceY), false, false, wnd_Particles, -1, false);
	edGui->m_LevelParticles_EditBox_EmiterSizeXMin = env->addEditBox(L"-20", rect<s32>(leftMarginX+widthX+spaceX,topMarginY+6*itemHeight+5*spaceY,(s32)(leftMarginX+1.5*widthX+spaceX),topMarginY+7*itemHeight+5*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_EMITERSIZE_XMIN);
	edGui->m_LevelParticles_EditBox_EmiterSizeXMax = env->addEditBox(L"20", rect<s32>((s32)(leftMarginX+1.5*widthX+2*spaceX),topMarginY+6*itemHeight+5*spaceY,leftMarginX+2*widthX+2*spaceX,topMarginY+7*itemHeight+5*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_EMITERSIZE_XMAX);
	edGui->m_LevelParticles_EditBox_EmiterSizeYMin = env->addEditBox(L"-2", rect<s32>(leftMarginX+widthX+spaceX,topMarginY+7*itemHeight+6*spaceY,(s32)(leftMarginX+1.5*widthX+spaceX),topMarginY+8*itemHeight+6*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_EMITERSIZE_YMIN);
	edGui->m_LevelParticles_EditBox_EmiterSizeYMax = env->addEditBox(L"20", rect<s32>((s32)(leftMarginX+1.5*widthX+2*spaceX),topMarginY+7*itemHeight+6*spaceY,leftMarginX+2*widthX+2*spaceX,topMarginY+8*itemHeight+6*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_EMITERSIZE_YMAX);
	edGui->m_LevelParticles_EditBox_EmiterSizeZMin = env->addEditBox(L"-20", rect<s32>(leftMarginX+widthX+spaceX,topMarginY+8*itemHeight+7*spaceY,(s32)(leftMarginX+1.5*widthX+spaceX),topMarginY+9*itemHeight+7*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_EMITERSIZE_ZMIN);
	edGui->m_LevelParticles_EditBox_EmiterSizeZMax = env->addEditBox(L"20", rect<s32>((s32)(leftMarginX+1.5*widthX+2*spaceX),topMarginY+8*itemHeight+7*spaceY,leftMarginX+2*widthX+2*spaceX,topMarginY+9*itemHeight+7*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_EMITERSIZE_ZMAX);

	env->addStaticText(L"Angle:", rect<s32>(leftMarginX,topMarginY+9*itemHeight+8*spaceY,leftMarginX+widthX,topMarginY+10*itemHeight+8*spaceY), false, false, wnd_Particles, -1, false);
	edGui->m_LevelParticles_EditBox_Angle = env->addEditBox(L"0", rect<s32>(leftMarginX+widthX+spaceX,topMarginY+9*itemHeight+8*spaceY,(s32)(leftMarginX+1.5*widthX+spaceX),topMarginY+10*itemHeight+8*spaceY), true, wnd_Particles, GUI_ID_EDITBOX_LEVELPARTICLES_ANGLE);

	edGui->m_LevelParticle_Button_Save = env->addButton(rect<s32>(150,340,280,360), wnd_Particles, GUI_ID_BUTTON_PARTICLESYSTEM_INSERT, L"Insert");
	env->setFocus(edGui->m_LevelParticle_Button_Save);

	return wnd_Particles;

}

//creates window for selecting music and sounds for this level
IGUIWindow* MakeMusicWindow(CEditorManager* editorManager)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();
	CEditorGUI* edGui = editorManager->getGUIManager();

	IGUIWindow* wnd_Music = env->addWindow(rect<s32>(270,90,700,450), false, L"Level Music", 0, GUI_ID_WINDOW_LEVEL_MUSIC);

	//Layout GUI elements around
	
	env->addStaticText(L"Music file:", rect<s32>(15,40,190,60), false, false, wnd_Music, -1, false);
	edGui->m_LevelMusic_EditBox_MusicFile = env->addEditBox(editorManager->getEdiLevel()->m_LevelMusic_MusicFile.c_str(), rect<s32>(15,60,290,80), true, wnd_Music, GUI_ID_EDITBOX_LEVELMUSIC_MUSIC_FILE);
	edGui->m_LevelMusic_Button_MusicBrowse = env->addButton(rect<s32>(300,60,390,80), wnd_Music, GUI_ID_BUTTON_LEVELMUSIC_MUSIC_BROWSE, L"Browse");
	env->addStaticText(L"Ambient sounds file:", rect<s32>(15,90,190,110), false, false, wnd_Music, -1, false);
	edGui->m_LevelMusic_EditBox_SoundFile = env->addEditBox(editorManager->getEdiLevel()->m_LevelMusic_SoundFile.c_str(), rect<s32>(15,110,290,130), true, wnd_Music, GUI_ID_EDITBOX_LEVELMUSIC_SOUND_FILE);
	edGui->m_LevelMusic_Button_SoundBrowse = env->addButton(rect<s32>(300,110,390,130), wnd_Music, GUI_ID_BUTTON_LEVELMUSIC_SOUND_BROWSE, L"Browse");
	env->addStaticText(L"Naration file:", rect<s32>(15,130,190,150), false, false, wnd_Music, -1, false);
	edGui->m_LevelMusic_EditBox_NarationFile = env->addEditBox(editorManager->getEdiLevel()->m_LevelMusic_NarationFile.c_str(), rect<s32>(15,150,290,170), true, wnd_Music, GUI_ID_EDITBOX_LEVELMUSIC_NARATION_FILE);
	edGui->m_LevelMusic_Button_NarationBrowse = env->addButton(rect<s32>(300,150,390,170), wnd_Music, GUI_ID_BUTTON_LEVELMUSIC_NARATION_BROWSE, L"Browse");

	edGui->m_LevelMusic_Button_Save = env->addButton(rect<s32>(150,220,280,240), wnd_Music, GUI_ID_BUTTON_LEVELMUSIC_SAVE, L"Save");
	//env->setFocus(edGui->m_Music_Button_Save);

	return wnd_Music;
}
