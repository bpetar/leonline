/** 
 * \file EdGui.cpp
 * \brief GUI class creates menus and property windows and buttons and handles all the user clicks.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "IGUITreeCtrl.h"
#include "CGUITreeCtrl.h"
#include "EDGui.h"
#include "../EditorManager.h"
#include "../Utils.h"
#include "IGUIContainer.h"

// June 27 2011 - current irrlicht version 1.7
// Reson: To use setlocale() function.
// We are including this header file because some locales use the comma character for floating point values
// and this caused a problem with reading the XML file within the game and the level editor.
// TODO: If Irrlicht fixes this error in the future, will remove it.
#include <locale>

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CEditorGUI::CEditorGUI()
:
	m_EditorManager(NULL),

	m_PropCheckBox_Invisible(NULL),
	m_PropCheckBox_Illusion(NULL),
	m_PropCheckBox_Container(NULL),
	m_PropCheckBox_Pickable(NULL),
	m_PropCheckBox_Trigger(NULL),
	m_PropCheckBox_NPC(NULL),
	m_PropCheckBox_Monster(NULL),
	m_PropEditBox_Name(NULL),
	m_PropEditBox_Pos_X(NULL),
	m_PropEditBox_Pos_Y(NULL),
	m_PropEditBox_Pos_Z(NULL),
	m_PropEditBox_Rot_X(NULL),
	m_PropEditBox_Rot_Y(NULL),
	m_PropEditBox_Rot_Z(NULL),

	m_wnd_ContainerContent(0),
	m_wnd_AddPickScriptAction(0),
	m_wnd_AddTriggerScriptAction(0),
	m_wnd_InsertTerrainHeightmap(0),
	m_wnd_AddNPCScriptAction(0),
	m_wnd_AddNPCNewText(0),
	m_wnd_MonsterProperties(0),
	m_wnd_ModelPreview(0),
	m_wnd_Properties(0),
	m_wnd_GameItems(0),
	m_wnd_LevelMusic(0),
	m_wnd_ParticleSystem(0),  

	m_DialogButton_Warning(0),
	m_DialogButton_AddText(0),
	m_bLinkDialogNodes(false),
	m_pPreviewPickableItem(0),
	m_bIrrFileSaveDialog(false),
	m_bIrrFileOpenDialog(false),
	m_bMapSaveDialog(false),
	m_bMapOpenDialog(false),
	m_bHeightmapOpenDialog(false),
	m_bLevelMusicOpenDialog(false),
	m_bLevelSoundOpenDialog(false),
	m_bLevelNarationOpenDialog(false),
	m_bTextureOpenDialog(false),
	m_bDetailOpenDialog(false),
	m_bElementPicking(false),
	m_bAddPickDirectly(false),
	m_TreeSubmenuWasVisible(false),
	m_TextureToRenderOn(0),
	m_ContainerPickPreview_TextureToRenderOn(0),
	m_SelectedPickItemIcon_Texture(0),
	m_slotTex(0),
	m_PhotoCamera(0),
	m_PickCamera(0),
	m_bGUIFocused(false),
	m_bMoveModel(false),
	m_bRotateModel(false),
	m_eMoveDirection(LE_LAST),
	m_eRotateDirection(LE_LAST),

	m_LevelMusic_EditBox_MusicFile(0),
	m_LevelMusic_EditBox_SoundFile(0),
	m_LevelMusic_EditBox_NarationFile(0),
	m_LevelMusic_Button_MusicBrowse(0),
	m_LevelMusic_Button_SoundBrowse(0),
	m_LevelMusic_Button_NarationBrowse(0),
	m_LevelMusic_Button_Save(0),

	m_LevelParticle_Button_Save(0),
	m_LevelParticles_EditBox_Name(0),
	m_LevelParticles_EditBox_TextureFile(0),
	m_LevelParticles_Button_TextureBrowse(0),
	m_LevelParticles_ComboBox_Emiter(0),
	m_LevelParticles_EditBox_DirectionX(0),
	m_LevelParticles_EditBox_DirectionY(0),
	m_LevelParticles_EditBox_DirectionZ(0),
	m_LevelParticles_EditBox_EmitRateMin(0),
	m_LevelParticles_EditBox_EmitRateMax(0),
	m_LevelParticles_EditBox_EmiterSizeXMin(0),
	m_LevelParticles_EditBox_EmiterSizeXMax(0),
	m_LevelParticles_EditBox_EmiterSizeYMin(0),
	m_LevelParticles_EditBox_EmiterSizeYMax(0),
	m_LevelParticles_EditBox_EmiterSizeZMin(0),
	m_LevelParticles_EditBox_EmiterSizeZMax(0),
	m_Monsters_EditBox_Health(0)
{
}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CEditorGUI::~CEditorGUI()
{
}

void CEditorGUI::OnNewMap()
{
	//recreate cameras
	m_PhotoCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, vector3df(0,-10000,0), vector3df(0,-10000,-100));
	m_PickCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, vector3df(0,-10000,0), vector3df(0,-10000,100));
	
	//clear scene tree and GUI properties
	ClearTreeOfSceneNodes();
}

/**
 * \brief Writes dialog script to edit box that is displaying script actions
 *
 * Note: Each dialog node has its own list of actions.
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorGUI::WriteNPCDialogScriptAction()
{
	m_SelectedNode = m_DialogTree->GetSelected();
	if(m_SelectedNode)
	{
		if(m_SelectedNode != m_DialogTree->GetRootNode()->Child)
		{
			stringw script = "<Actions>\n";
			for(u32 a=0; a < m_SelectedNode->scriptAction.actions.size(); a++)
			{
				script +="\t";
				script += m_EditorManager->getScriptEngine()->GetActionLine(m_SelectedNode->scriptAction.actions[a]);
			}
			script += "</Actions>";
			m_DialogEditBox_ActionsText->setText(script.c_str());
		}
		else
		{
			m_DialogEditBox_ActionsText->setText(L"");
		}
	}
	else
	{
		m_DialogEditBox_ActionsText->setText(L"");
	}
}

/**
 * \brief Save script to given file
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorGUI::SavePickScript(const stringc &file)
{
	if( file != stringc("") )
	{
		m_EditorManager->getDevice()->getFileSystem()->changeWorkingDirectoryTo(PICKABLES_SCRIPTS_DIR);

		IXMLWriter* xml = m_EditorManager->getDevice()->getFileSystem()->createXMLWriter(file.c_str());

		if (xml)
		{
			m_EditorManager->getScriptEngine()->WritePickScriptToXML(xml, m_PickCheckBox_Equipable->isChecked(), m_PickCheckBox_Usable->isChecked());
			xml->drop(); // don't forget to delete the xml reader
		}
		m_EditorManager->backToWorkingDirectory();
	}
	else
	{
		//script file does not exist for this element, this should never happen
		stringw message  = "Script filename is missing!";
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"Abort!", message.c_str());
	}
}

/**
 * \brief Save script to given file
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorGUI::SaveTriggerScript(const stringc &file)
{
	if (file != stringc(""))
	{
		m_EditorManager->getDevice()->getFileSystem()->changeWorkingDirectoryTo(STATIC_SCRIPTS_DIR);

		IXMLWriter* xml = m_EditorManager->getDevice()->getFileSystem()->createXMLWriter(file.c_str());

		if (xml)
		{
			m_EditorManager->getScriptEngine()->WriteTriggerScriptToXML(xml, m_EditorManager->GetObjectParameter_State(m_SelectedObjectID));
			xml->drop(); // don't forget to delete the xml reader
		}
		m_EditorManager->backToWorkingDirectory();
	}
	else
	{
		//script file does not exist for this element, this should never happen
		stringw message  = "Script filename is missing!";
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"Abort!", message.c_str());
	}
}

//check consistency of dialog node actions, if action is pointing to deleted node - remove it
void CEditorGUI::CheckDialogNodeActions(TreeNode* node, u32 deletedNodeID)
{
	if(node->hasScriptAction)
	{
		for (u32 index = 0; index < node->scriptAction.actions.size(); index++)
		{
			if(node->scriptAction.actions[index].name == stringw(L"SetDialogNodeEnabled"))
			{
				if(node->scriptAction.actions[index].target == stringw(deletedNodeID))
				{
					node->scriptAction.actions.erase(index);
					if(node->scriptAction.actions.size() == 0)
					{
						//if node has no more actions
						node->hasScriptAction = false;
					}
					break; //here we assume there are no two actions pointing at the same deleted node!!!
				}
			}
		}
	}
	if (node->Child)
	{
		CheckDialogNodeActions(node->Child, deletedNodeID);
	}
	if (node->Sibling)
	{
		CheckDialogNodeActions(node->Sibling, deletedNodeID);
	}
}

//Write dialog node attributes to XML like file (recursive)
void CEditorGUI::WriteElement(IXMLWriter* xml, TreeNode* node)
{
	xml->writeElement(L"Node",false); xml->writeLineBreak();
	stringw text_id = stringw(node->ID);
	if(node->isEnabled)
	{
		xml->writeElement(L"Enabled",true,L"value",L"true"); xml->writeLineBreak();
	}
	else
	{
		xml->writeElement(L"Enabled",true,L"value",L"false"); xml->writeLineBreak();
	}
	xml->writeElement(L"Id",true,L"value",text_id.c_str()); xml->writeLineBreak();
	if(node->Link)
	{
		stringw text_link = stringw(node->Link->ID);
		xml->writeElement(L"Link",true,L"value",text_link.c_str()); xml->writeLineBreak();
	}
	if(node->isLink)
	{
		xml->writeElement(L"IsLink",true,L"value",L"true"); xml->writeLineBreak();
	}
	if(node->Player)
	{
		xml->writeElement(L"Speaker",true,L"value",L"PLAYER"); xml->writeLineBreak();
	}
	else
	{
		stringw text_name = node->NPCName;
		xml->writeElement(L"Speaker",true,L"value",text_name.c_str()); xml->writeLineBreak();
	}
	xml->writeElement(L"Text",true,L"value",node->Text.c_str()); xml->writeLineBreak();
	//write actions
	if(node->scriptAction.actions.size() > 0)
	{
		xml->writeElement(L"Actions",false); xml->writeLineBreak();
		for ( u32 a = 0; a < node->scriptAction.actions.size(); a++)
		{
			xml->writeElement(node->scriptAction.actions[a].name.c_str(),true,
				L"target",node->scriptAction.actions[a].target.c_str(),
				L"attribute",node->scriptAction.actions[a].attribute.c_str(),
				L"value",node->scriptAction.actions[a].value.c_str());
			xml->writeLineBreak();
		}
		xml->writeClosingTag(L"Actions"); xml->writeLineBreak();
	}

	if (node->Child)
	{
		WriteElement(xml, node->Child); //continue populating child
	}

	xml->writeClosingTag(L"Node"); xml->writeLineBreak();

	if (node->Sibling)
	{
		WriteElement(xml, node->Sibling); //continue populating sibilings.
	}
}

//Saves NPC conversation and actions related to dialog nodes in one XML like file
void CEditorGUI::SaveDialog(const stringc &file)
{
	if (file != stringc(""))
	{
		m_EditorManager->getDevice()->getFileSystem()->changeWorkingDirectoryTo(DIALOG_DIR);

		IXMLWriter* xml = m_EditorManager->getDevice()->getFileSystem()->createXMLWriter(file.c_str());
		
		if (xml)
		{
			xml->writeXMLHeader(); xml->writeLineBreak();

			TreeNode* node = m_DialogTree->GetRootNode()->Child;

			//recursive function
			WriteElement(xml, node);
			
			xml->drop(); // don't forget to delete the xml reader
		}
		m_EditorManager->backToWorkingDirectory();
	}
	else
	{
		//script file does not exist for this element, this should never happen
		stringw message  = "Unexpected error: Dialog filename is missing!";
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"Abort!", message.c_str());
	}
}

/**
 * \brief Event handler - handles user clicks (mouse and keyboard).
 *
 * Handles menu items, if menu item was selected - appropriate function is called (save, open, new...)
 * Handles button clicks, tree node selection, keyboard entries etc.
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorGUI::OnEvent(const SEvent& event)
{
	if(event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if ((event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) || (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP))
		{
			position2di mouse_pos(event.MouseInput.X, event.MouseInput.Y);
			m_bMoveModel = false;
			m_bRotateModel = false;
			//if mouse click is within GUI windows, we declare this event handled, and do nothing
			//because GUI event will be fired moment later and we handle it below
			if ((m_wnd_GameItems && m_wnd_GameItems->getAbsolutePosition().isPointInside(mouse_pos)) || 
				(m_wnd_Properties && m_wnd_Properties->getAbsolutePosition().isPointInside(mouse_pos)))
			{
				if(m_PropButton_Pos_X_Up->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bMoveModel = true;
					m_eMoveDirection = LE_X_UP;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Pos_X_Down->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bMoveModel = true;
					m_eMoveDirection = LE_X_DOWN;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Pos_Y_Up->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bMoveModel = true;
					m_eMoveDirection = LE_Y_UP;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Pos_Y_Down->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bMoveModel = true;
					m_eMoveDirection = LE_Y_DOWN;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Pos_Z_Up->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bMoveModel = true;
					m_eMoveDirection = LE_Z_UP;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Pos_Z_Down->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bMoveModel = true;
					m_eMoveDirection = LE_Z_DOWN;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				//rotation
				else if(m_PropButton_Rot_X_Up->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bRotateModel = true;
					m_eRotateDirection = LE_X_UP;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Rot_X_Down->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bRotateModel = true;
					m_eRotateDirection = LE_X_DOWN;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Rot_Y_Up->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bRotateModel = true;
					m_eRotateDirection = LE_Y_UP;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Rot_Y_Down->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bRotateModel = true;
					m_eRotateDirection = LE_Y_DOWN;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Rot_Z_Up->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bRotateModel = true;
					m_eRotateDirection = LE_Z_UP;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				else if(m_PropButton_Rot_Z_Down->getAbsolutePosition().isPointInside(mouse_pos) && (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN))
				{
					m_bRotateModel = true;
					m_eRotateDirection = LE_Z_DOWN;
					m_uClickStartTime = m_EditorManager->getDevice()->getTimer()->getRealTime();
				}
				return true;
			}
			//When one of these dialogs windows is open, mouse click is ignored on terrain!
			if (GUIWindowOpen())
			{
				return true;
			}
			//When tree is selected from the menu, we return true so mouse click will be ignored on terrain.
			if(m_TreeSubmenu->isVisible())
			{
				//menu dissappears before mouse up click
				m_TreeSubmenuWasVisible = true;
				return true;
			}
			if(m_TreeSubmenuWasVisible)
			{
				m_TreeSubmenuWasVisible = false;
				return true;
			}
		}
	}
	if(event.EventType == EET_KEY_INPUT_EVENT)
	{
		if (m_wnd_AddNPCNewText && m_DialogButton_AddText)
		{
			//add dialog node on enter!
			if ((!event.KeyInput.PressedDown) && (event.KeyInput.Key == KEY_RETURN))
			{
				//insert new node
				TreeNode* node = m_DialogTree->InsertNode(m_DialogEditBox_Text->getText(),m_SelectedNode,m_NumNodes+1);
				if(node)
				{
					m_NumNodes++;
					if(m_SelectedNode != m_DialogTree->GetRootNode())
					{
						node->Player = !m_SelectedNode->Player;
					}
					else
					{
						node->Player = false;
					}
					if(!node->Player)
					{
						node->NPCName = m_EditorManager->GetObjectParameter_Name(m_SelectedObjectID);
					}
				}
				else
				{
					//error adding new node!
				}
				m_wnd_AddNPCNewText->remove();
				m_DialogButton_Warning = 0;
				m_DialogButton_AddText = 0;
				m_wnd_AddNPCNewText = 0;
				m_EditorManager->getGUIEnvironment()->setFocus(m_DialogButton_AddNode);

				/*SEvent NewEvent;
				NewEvent.EventType = irr::EET_GUI_EVENT;
				NewEvent.GUIEvent.Caller = m_DialogButton_AddText;
				NewEvent.GUIEvent.EventType = (irr::gui::EGUI_EVENT_TYPE)(EGET_BUTTON_CLICKED);
				m_EditorManager->getGUIEnvironment()->getRootGUIElement()->OnEvent(NewEvent);*/
				
				return true;
			}
		}
	}
	//GUI event is special kind of event (not simple mouse or keyboard) generated by irrlicht engine
	//it simplifies event handling by telling us directly if button was clicked or window was moved 
	//or menu item was selected... etc.
	if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = m_EditorManager->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{
				case EGET_ELEMENT_FOCUSED:
				{
					if(m_wnd_Properties && ((id == m_PropEditBox_Pos_X->getID())||(id == m_PropEditBox_Pos_Y->getID())||(id == m_PropEditBox_Pos_Z->getID())||
					   (id == m_PropEditBox_Rot_X->getID())||(id == m_PropEditBox_Rot_Y->getID())||(id == m_PropEditBox_Rot_Z->getID())||(id == m_PropEditBox_Name->getID())))
					{
						m_bGUIFocused = true;
						return true;
					}
				}
				break;
				case EGET_ELEMENT_FOCUS_LOST:
				{
					if (m_wnd_Properties && ((id == m_PropEditBox_Pos_X->getID())||(id == m_PropEditBox_Pos_Y->getID())||(id == m_PropEditBox_Pos_Z->getID())||
					   (id == m_PropEditBox_Rot_X->getID())||(id == m_PropEditBox_Rot_Y->getID())||(id == m_PropEditBox_Rot_Z->getID())||(id == m_PropEditBox_Name->getID())))
					{
						m_bGUIFocused = false;
						return true;
					}
				}
				break;
				case EGET_ELEMENT_HOVERED:
				{
					if((id > GUI_ID_TREECTRL_GAME_OBJECTS_START) && (id < GUI_ID_TREECTRL_GAME_OBJECTS_END))
					{
						m_bElementPicking = true;
					}
				}
				break;
				case EGET_ELEMENT_LEFT:
				{
					if((id > GUI_ID_TREECTRL_GAME_OBJECTS_START) && (id < GUI_ID_TREECTRL_GAME_OBJECTS_END))
					{
						m_bElementPicking = false;
						//bits redundant to load model again, but it worksaround problem with map loading (scene delete)
						CGUITreeCtrl* tree = (CGUITreeCtrl*)event.GUIEvent.Caller;
						tree->m_SavedNode = 0;
					}
				}
				break;
				case EGET_TREEVIEW_MOUSEHOVERNODE:
				{
					//ISSUE ID 3
					//SaveDialog is not doing modal thingi well so we check if save dialog is open and skip this hovering
					//because while save dialog is open, working direcotory is changed and SendModelToPhotoSession can not find models
					if (m_wnd_ModelPreview && m_bElementPicking && (id > GUI_ID_TREECTRL_GAME_OBJECTS_START) && (id < GUI_ID_TREECTRL_GAME_OBJECTS_END) && !m_bIrrFileSaveDialog && !m_bMapSaveDialog)
					{
						CGUITreeCtrl* tree = (CGUITreeCtrl*)event.GUIEvent.Caller;
						SendModelToPhotoSession(tree->m_SavedNode->Parent->Text, tree->m_SavedNode->Text);
						return true;
					}
				}
				break;
				case EGET_TREEVIEW_MOUSECLICK_SELECTION:
				//this is tree event, node selection with mouse click.
				{
					//Its better to add icons by clicking button Add, instead on mouse click
					if (id == GUI_ID_TREECTRL_PICKABLE_OBJECTS) //Container Content Tree of Pickables
					{
						IGUITreeCtrl* tree = (IGUITreeCtrl*)event.GUIEvent.Caller;
						SetGameObjectToContainer(tree->GetSelected()->Parent->Text, tree->GetSelected()->Text);
						return true;
					}
					//Model picked from the tree, add model to level, set model under mouse pointer
					else if ((id > GUI_ID_TREECTRL_GAME_OBJECTS_SCENE) && (id < GUI_ID_TREECTRL_GAME_OBJECTS_END))
					{
						IGUITreeCtrl* tree = (IGUITreeCtrl*)event.GUIEvent.Caller;

						IGUITabControl* tabGameObjects = (IGUITabControl*) m_wnd_GameItems->getElementFromId(GUI_ID_TAB_GAME_OBJECTS,true);
						int a = tabGameObjects->getActiveTab();
						IGUITab* tabs = tabGameObjects->getTab(a);
						stringw Folder = OBJECT_DIR;
						Folder += tabs->getText();
						Folder += "/";
						if (!tree->GetSelected()->Parent->Text.equals_ignore_case("Root Node"))
						{
							Folder += tree->GetSelected()->Parent->Text;
							Folder += "/";
						}

						//check if Static model
						bool staticModel = false;
						stringw modelType = tabs->getText();
						if(modelType.equals_ignore_case(L"Static"))
							staticModel = true;

						SetElementAtHand(Folder, tree->GetSelected()->Text, staticModel);
						return true;
					}
					//Model picked from the scene tree, select model on level
					else if (id == GUI_ID_TREECTRL_GAME_OBJECTS_SCENE)
					{
						//Get wich model is picked
						IGUITreeCtrl* tree = (IGUITreeCtrl*)event.GUIEvent.Caller;
						int elementID = tree->GetSelected()->ID;
						//Select model on the scene
						m_EditorManager->getEdiLevel()->SetElementSelected(elementID);
						return true;
					}
					else if (id == 714)
					{
						IGUITreeCtrl* tree = (IGUITreeCtrl*)event.GUIEvent.Caller;
						TreeNode* link = tree->GetSelected();
						m_DialogEditBox_SelectedNodeID->setText(stringw(link->ID).c_str());
						if (m_bLinkDialogNodes)
						{
							//This code will create link to selected node (from previously selected node)
							//new tree node is created to represent this link in green color
							if (link->Player)
							{
								//TODO: must select NPC text node! Try Again.
								return false;
							}
							if (link == tree->GetRootNode()->Child)
							{
								//TODO: cant link to RootNode! Try Again.
								return false;
							}
							m_SelectedNode->Link = link;
							link = new TreeNode;
							link->isLink = true;
							link->hasScriptAction = false;
							link->Child = 0;
							link->Parent = m_SelectedNode;
							link->Sibling = 0;
							link->Link = 0;
							link->ID = ++m_NumNodes;
							link->Player = false;
							link->NPCName = tree->GetSelected()->NPCName;
							link->Text = tree->GetSelected()->Text;

							if(!m_SelectedNode->Child)	// Childless parent is simple
							{
								m_SelectedNode->Child = link;
							}
							else
							{
								TreeNode *curnode = m_SelectedNode->Child;
								while(curnode->Sibling)
									curnode = curnode->Sibling;
								curnode->Sibling = link;
								// Find last sibling of parent and include new sibling
							}
							m_bLinkDialogNodes = false;
						}
						//This line sets m_SelectedNode to new one, and writes its actions to the window
						WriteNPCDialogScriptAction();
						m_DialogEditBox_ChangeText->setText(stringw(link->Text).c_str());
					}
				}
				break;
				case EGET_ELEMENT_CLOSED:
				{
					if(event.GUIEvent.Caller == m_wnd_ContainerContent)
						m_wnd_ContainerContent = 0;
					if(event.GUIEvent.Caller == m_wnd_AddPickScriptAction)
						m_wnd_AddPickScriptAction = NULL;
					if(event.GUIEvent.Caller == m_wnd_AddTriggerScriptAction)
						m_wnd_AddTriggerScriptAction = NULL;
					if(event.GUIEvent.Caller == m_wnd_AddNPCScriptAction)
						m_wnd_AddNPCScriptAction = 0;
					if(event.GUIEvent.Caller == m_wnd_AddNPCNewText)
						m_wnd_AddNPCNewText = 0;
					if(event.GUIEvent.Caller == m_wnd_InsertTerrainHeightmap)
						m_wnd_InsertTerrainHeightmap = 0;
					if(event.GUIEvent.Caller == m_wnd_MonsterProperties)
						m_wnd_MonsterProperties = 0;
					if(event.GUIEvent.Caller == m_wnd_LevelMusic)
						m_wnd_LevelMusic = 0;
					if(event.GUIEvent.Caller == m_wnd_ParticleSystem)
						m_wnd_ParticleSystem = 0;

					//these windows are asociated with menu option, so we update checked state
					if(event.GUIEvent.Caller == m_wnd_Properties)
					{
						m_wnd_Properties = 0;
						m_ViewMenu->setItemChecked(m_ViewMenuItem_Properties,false);
					}
					if(event.GUIEvent.Caller == m_wnd_GameItems)
					{
						m_wnd_GameItems = 0;
						m_ViewMenu->setItemChecked(m_ViewMenuItem_GameObjects,false);
					}
					if(event.GUIEvent.Caller == m_wnd_ModelPreview)
					{
						m_wnd_ModelPreview = 0;
						m_ViewMenu->setItemChecked(m_ViewMenuItem_ObjectPreview,false);
					}

					m_bGUIFocused = GUIWindowOpen();
				}
				break;
				case EGET_FILE_CHOOSE_DIALOG_CANCELLED:
				{
					m_bIrrFileOpenDialog = false;
					m_bIrrFileSaveDialog = false;
					m_bMapSaveDialog = false;
					m_bMapOpenDialog = false;
					m_bLevelMusicOpenDialog = false;
					m_bLevelSoundOpenDialog = false;
					m_bLevelNarationOpenDialog = false;
					m_bHeightmapOpenDialog = false;
					m_bTextureOpenDialog = false;
					m_bDetailOpenDialog = false;
					m_EditorManager->backToWorkingDirectory();
				}
				break;
				case EGET_EDITBOX_ENTER:
				{
					//Entering value in edit box will trigger this event after enter is pressed
					//Position, rotation, model name and other properties are handled here.
					float value = 0.0f;
					if(event.GUIEvent.Caller == m_PropEditBox_Pos_X)
					{
						swscanf(m_PropEditBox_Pos_X->getText(),L"%f",&value);
						m_EditorManager->getEdiLevel()->SetPositionSelectedElement(LE_AXIS_X, value);
					}
					if(event.GUIEvent.Caller == m_PropEditBox_Pos_Y)
					{
						swscanf(m_PropEditBox_Pos_Y->getText(),L"%f",&value);
						m_EditorManager->getEdiLevel()->SetPositionSelectedElement(LE_AXIS_Y, value);
					}
					if(event.GUIEvent.Caller == m_PropEditBox_Pos_Z)
					{
						swscanf(m_PropEditBox_Pos_Z->getText(),L"%f",&value);
						m_EditorManager->getEdiLevel()->SetPositionSelectedElement(LE_AXIS_Z, value);
					}
					if(event.GUIEvent.Caller == m_PropEditBox_Rot_X)
					{
						swscanf(m_PropEditBox_Rot_X->getText(),L"%f",&value);
						m_EditorManager->getEdiLevel()->SetRotationSelectedElement(LE_AXIS_X, value);
					}
					if(event.GUIEvent.Caller == m_PropEditBox_Rot_Y)
					{
						swscanf(m_PropEditBox_Rot_Y->getText(),L"%f",&value);
						m_EditorManager->getEdiLevel()->SetRotationSelectedElement(LE_AXIS_Y, value);
					}
					if(event.GUIEvent.Caller == m_PropEditBox_Rot_Z)
					{
						swscanf(m_PropEditBox_Rot_Z->getText(),L"%f",&value);
						m_EditorManager->getEdiLevel()->SetRotationSelectedElement(LE_AXIS_Z, value);
					}
					if(event.GUIEvent.Caller == m_PropEditBox_Name)
					{
						m_EditorManager->getEdiLevel()->SetSelectedElement_Name(m_PropEditBox_Name->getText());
					}
				}
				break;
				case EGET_FILE_SELECTED:
				{
					stringc mapname;
					if(m_bMapOpenDialog)
					{
						m_bMapOpenDialog = false;
						ClearTreeOfSceneNodes();
						m_EditorManager->getEdiLevel()->SetLoadingDir(m_EditorManager->getFS()->getWorkingDirectory());
						m_EditorManager->backToWorkingDirectory();
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						mapname = dialog->getFileName();
						setlocale(LC_ALL,"C"); // locale fix

						m_EditorManager->getEdiLevel()->OnLoadMap(mapname);
						//recreate cameras
						m_PhotoCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, 
							core::vector3df(0,-10000,0),
							core::vector3df(0,-10000,-100));
						m_PickCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, 
							core::vector3df(0,-10000,0),
							core::vector3df(0,-10000,100));
					}
					else if(m_bMapSaveDialog)
					{
						m_bMapSaveDialog = false;
						//save the scene to selected map file
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						mapname = dialog->getFileName();
						setlocale(LC_ALL,"C");

						m_EditorManager->getEdiLevel()->SetMapName(mapname);
						m_EditorManager->getEdiLevel()->OnSaveMap();
						m_EditorManager->backToWorkingDirectory();
					}
					else if (m_bIrrFileOpenDialog) // import irr scene
					{
						// load the map file, selected in the file open dialog
						ClearTreeOfSceneNodes();
						m_EditorManager->getEdiLevel()->SetLoadingDir(m_EditorManager->getFS()->getWorkingDirectory());
						m_EditorManager->backToWorkingDirectory();
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						mapname = dialog->getFileName();
						setlocale(LC_ALL,"C");
						m_EditorManager->getEdiLevel()->OnLoadMap_IrrScene(mapname);
						//recreate cameras
						m_PhotoCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, 
							core::vector3df(0,-10000,0),
							core::vector3df(0,-10000,-100));
						m_PickCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, 
							core::vector3df(0,-10000,0),
							core::vector3df(0,-10000,100));
						m_bIrrFileOpenDialog = false;
					}
                    else if (m_bIrrFileSaveDialog) // save irr scene
					{
						//save the scene to selected map file
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						mapname = dialog->getFileName();
						setlocale(LC_ALL,"C");
						//we don't save our static preview cameras to the scene, so it is necessary to remove them
						m_PickCamera->remove();
						m_PhotoCamera->remove();
						//save map
						m_EditorManager->getEdiLevel()->SetMapName(mapname);
						m_EditorManager->getEdiLevel()->OnSaveMap_IrrScene(mapname);
						//recreate cameras
						m_PhotoCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, 
							core::vector3df(0,-10000,0),
							core::vector3df(0,-10000,-100));
						m_PickCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, 
							core::vector3df(0,-10000,0),
							core::vector3df(0,-10000,100));
						m_EditorManager->backToWorkingDirectory();
						m_bIrrFileSaveDialog = false;
					}
					else if (m_bLevelMusicOpenDialog)
					{
						//load music filename to edit box
						m_EditorManager->backToWorkingDirectory();
						m_bLevelMusicOpenDialog = false;
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						stringc file = dialog->getFileName();
						setlocale(LC_ALL,"C");
						u32 start = m_EditorManager->m_WorkingDirectory.size();
						stringw local = file.subString(start+1,file.size()-m_EditorManager->m_WorkingDirectory.size());
						m_LevelMusic_EditBox_MusicFile->setText(local.c_str());
					}
					else if (m_bLevelSoundOpenDialog)
					{
						//load music filename to edit box
						m_EditorManager->backToWorkingDirectory();
						m_bLevelSoundOpenDialog = false;
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						setlocale(LC_ALL,"C");
						stringc file = dialog->getFileName();
						u32 start = m_EditorManager->m_WorkingDirectory.size();
						stringw local = file.subString(start+1,file.size()-m_EditorManager->m_WorkingDirectory.size());
						m_LevelMusic_EditBox_SoundFile->setText(local.c_str());
					}
					else if (m_bLevelNarationOpenDialog)
					{
						//load naration filename to edit box
						m_EditorManager->backToWorkingDirectory();
						m_bLevelNarationOpenDialog = false;
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						stringc file = dialog->getFileName();
						setlocale(LC_ALL,"C");
						u32 start = m_EditorManager->m_WorkingDirectory.size();
						stringw local = file.subString(start+1,file.size()-m_EditorManager->m_WorkingDirectory.size());
						m_LevelMusic_EditBox_NarationFile->setText(local.c_str());
					}
					else if(m_bHeightmapOpenDialog)
					{
						//open heightmap
						m_EditorManager->backToWorkingDirectory();
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						mapname = dialog->getFileName();
						setlocale(LC_ALL,"C");
						m_TH_EditBox_HeightmapFile->setText(stringw(mapname.c_str()).c_str());
						m_bHeightmapOpenDialog = false;
					}
					else if(m_bTextureOpenDialog)
					{
						//open terrain texture
						m_EditorManager->backToWorkingDirectory();
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						mapname = dialog->getFileName();
						setlocale(LC_ALL,"C");
						m_TH_EditBox_TexFile->setText(stringw(mapname.c_str()).c_str());
						m_bTextureOpenDialog = false;
					}
					else if(m_bDetailOpenDialog)
					{
						//open detail terrain texture
						m_EditorManager->backToWorkingDirectory();
						IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
						mapname = dialog->getFileName();
						setlocale(LC_ALL,"C");
						m_TH_EditBox_DetailFile->setText(stringw(mapname.c_str()).c_str());
						m_bDetailOpenDialog = false;
					}
				}
				break;
				case EGET_MENU_ITEM_SELECTED:
				{
					//a menu item was clicked
					IGUIContextMenu* menu = (IGUIContextMenu*)event.GUIEvent.Caller;
					s32 id = menu->getItemCommandId(menu->getSelectedItem());

					HandleMenuClick(m_EditorManager, id);

					return true;
				}
				break;
				case EGET_LISTBOX_CHANGED:
				{
					//PickScript Event ListBox changed
					if(m_wnd_AddPickScriptAction && (m_PickListBox_ExistingEvents->getID() == event.GUIEvent.Caller->getID()))
					{
						s32 index = m_PickListBox_ExistingEvents->getSelected();
						stringw scriptText = m_EditorManager->getScriptEngine()->PickScriptActionToString_Index(index);
						m_PickEditBox_Script->setText(scriptText.c_str());
					}
					else if (m_wnd_AddTriggerScriptAction && (m_TriggerListBox_States->getID() == event.GUIEvent.Caller->getID()))
					{
						s32 index = m_TriggerListBox_States->getSelected();
						stringw scriptText = m_EditorManager->getScriptEngine()->TriggerScriptActionToString_Index(index);
						m_TriggerEditBox_Script->setText(scriptText.c_str());
					}
				}
				break;
				case EGET_CHECKBOX_CHANGED:
				{
					if(m_wnd_Properties && (m_PropCheckBox_Anchor->getID() == event.GUIEvent.Caller->getID()))
					{
						m_EditorManager->getEdiLevel()->SetElementAnchored(m_PropCheckBox_Anchor->isChecked());
					}
					else if(m_wnd_Properties && (m_PropCheckBox_Terrain->getID() == event.GUIEvent.Caller->getID()))
					{
						m_EditorManager->getEdiLevel()->SetElementTerrain(m_PropCheckBox_Terrain->isChecked());
					}
					else if(m_wnd_Properties && (m_PropCheckBox_Invisible->getID() == event.GUIEvent.Caller->getID()))
					{
						m_EditorManager->getEdiLevel()->SetElementVisible(m_PropCheckBox_Invisible->isChecked());
					}
					else if(m_wnd_Properties && (m_PropCheckBox_Illusion->getID() == event.GUIEvent.Caller->getID()))
					{
						m_EditorManager->getEdiLevel()->SetElementIllusion(m_PropCheckBox_Illusion->isChecked());
					}
					//container checkbox changed - enable/disable button and store new object setting
					else if(m_wnd_Properties && (m_PropCheckBox_Container->getID() == event.GUIEvent.Caller->getID()))
					{
						m_ContainerButton->setEnabled(m_PropCheckBox_Container->isChecked());
						m_EditorManager->SetSelectedObjectParameter_isContainer(m_PropCheckBox_Container->isChecked());
					}
					//pickable checkbox changed - enable/disable button and store new object setting
					else if(m_wnd_Properties && (m_PropCheckBox_Pickable->getID() == event.GUIEvent.Caller->getID()))
					{
						m_PickScriptButton->setEnabled(m_PropCheckBox_Pickable->isChecked());
						m_EditorManager->SetSelectedObjectParameter_isPickable(m_PropCheckBox_Pickable->isChecked());
					}
					//trigger checkbox changed - enable/disable button and store new object setting
					else if(m_wnd_Properties && (m_PropCheckBox_Trigger->getID() == event.GUIEvent.Caller->getID()))
					{
						m_TriggerScriptButton->setEnabled(m_PropCheckBox_Trigger->isChecked());
						m_EditorManager->SetSelectedObjectParameter_isTrigger(m_PropCheckBox_Trigger->isChecked());
					}
					//npc checkbox changed - enable/disable button and store new object setting
					else if(m_wnd_Properties && (m_PropCheckBox_NPC->getID() == event.GUIEvent.Caller->getID()))
					{
						m_NPCScriptButton->setEnabled(m_PropCheckBox_NPC->isChecked());
						m_EditorManager->SetSelectedObjectParameter_isNPC(m_PropCheckBox_NPC->isChecked());
					}
					//monster checkbox changed - enable/disable button and store new object setting
					else if(m_wnd_Properties && (m_PropCheckBox_Monster->getID() == event.GUIEvent.Caller->getID()))
					{
						m_MonsterScriptButton->setEnabled(m_PropCheckBox_Monster->isChecked());
						m_EditorManager->SetSelectedObjectParameter_isMonster(m_PropCheckBox_Monster->isChecked());
					}
					//container window check box (direct picking)
					else if(m_wnd_ContainerContent && (m_PickCheckBox_DirectPick->getID() == event.GUIEvent.Caller->getID()))
					{
						if (m_PickCheckBox_DirectPick->isChecked())
						{
							m_bAddPickDirectly = true;
							m_pContainer_AddPickItemToContainer->setEnabled(false);
						}
						else
						{
							m_bAddPickDirectly = false;
							m_pContainer_AddPickItemToContainer->setEnabled(true);
						}
					}
					else if(m_wnd_AddPickScriptAction && (m_PickCheckBox_Usable->getID() == event.GUIEvent.Caller->getID()))
					{
						if (m_PickCheckBox_Usable->isChecked())
						{
							m_PickComboBox_Events->addItem(L"OnUse");
							m_PickComboBox_Events->addItem(L"OnUseAgainst");
						}
						else
						{
							//removal is painful, but necessary (?)
							for (u32 index = 0; index < m_PickComboBox_Events->getItemCount(); index++)
							{
								if ((stringw(m_PickComboBox_Events->getItem(index)) == stringw("OnUse"))||
									(stringw(m_PickComboBox_Events->getItem(index)) == stringw("OnUseAgainst")))
								{
									m_PickComboBox_Events->removeItem(index);
									index--;//item removed, next one takes his place at his index
								}
							}
						}
					}
					else if(m_wnd_AddPickScriptAction && (m_PickCheckBox_Equipable->getID() == event.GUIEvent.Caller->getID()))
					{
						if (m_PickCheckBox_Equipable->isChecked())
						{
							m_PickComboBox_Events->addItem(L"OnEquip");
							m_PickComboBox_Events->addItem(L"OnUnequip");
						}
						else
						{
							//removal is painful, but necessary (?)
							for (u32 index = 0; index < m_PickComboBox_Events->getItemCount(); index++)
							{
								if ((stringw(m_PickComboBox_Events->getItem(index)) == stringw("OnEquip"))||
									(stringw(m_PickComboBox_Events->getItem(index)) == stringw("OnUnequip")))
								{
									m_PickComboBox_Events->removeItem(index);
									index--;//item removed size of list is reduced
								}
							}
						}
					}
				}
				break;
			
				//button clicked
				case EGET_BUTTON_CLICKED:
				{
					//Button clicked
					HandleButtonClick(m_EditorManager, event.GUIEvent.Caller->getID());
				}
				break;
			}
		}
	
	return false;
}

void CEditorGUI::SetPropertiesPosition(f32 x, f32 y, f32 z)
{
	m_PropEditBox_Pos_X->setText(stringw(x).c_str());
	m_PropEditBox_Pos_Y->setText(stringw(y).c_str());
	m_PropEditBox_Pos_Z->setText(stringw(z).c_str());
}

void CEditorGUI::SetPropertiesRotation(f32 x, f32 y, f32 z)
{
	m_PropEditBox_Rot_X->setText(stringw(x).c_str());
	m_PropEditBox_Rot_Y->setText(stringw(y).c_str());
	m_PropEditBox_Rot_Z->setText(stringw(z).c_str());
}

/**
 * \brief Sets properties of selected object in GUI window.
 * When user selects game object (3D model) this function is called to fill out properties fields in 
 * Properties floating window.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorGUI::SetProperties(CGameObject* go)
{
	m_PropEditBox_Name->setText(go->name.c_str());
	m_PropEditBox_Pos_X->setText(stringw(go->pos.X).c_str());
	m_PropEditBox_Pos_Y->setText(stringw(go->pos.Y).c_str());
	m_PropEditBox_Pos_Z->setText(stringw(go->pos.Z).c_str());
	m_PropEditBox_Rot_X->setText(stringw(go->rot.X).c_str());
	m_PropEditBox_Rot_Y->setText(stringw(go->rot.Y).c_str());
	m_PropEditBox_Rot_Z->setText(stringw(go->rot.Z).c_str());
	m_PropEditBox_Model->setText(go->mesh.c_str());
	m_PropEditBox_ID->setText(stringw(go->id).c_str());
	m_PropEditBox_Description->setText(go->description.c_str());

	m_PropCheckBox_Anchor->setEnabled(true);
	m_PropCheckBox_Anchor->setChecked(go->isAnchored);
	m_PropCheckBox_Invisible->setEnabled(true);
	m_PropCheckBox_Invisible->setChecked(go->isInvisible);
	m_PropCheckBox_Illusion->setEnabled(true);
	m_PropCheckBox_Illusion->setChecked(go->isIllusion);
	m_PropCheckBox_Terrain->setEnabled(true);
	m_PropCheckBox_Terrain->setChecked(go->isTerrain);

	m_PropCheckBox_Container->setEnabled(true);
	m_PropCheckBox_Pickable->setEnabled(true);
	m_PropCheckBox_Trigger->setEnabled(true);
	m_PropCheckBox_NPC->setEnabled(true);
	m_PropCheckBox_Monster->setEnabled(true);
	m_PropButton_Set->setEnabled(true);
	
	if (go->isContainer)
	{
		m_PropCheckBox_Container->setChecked(true);
		m_ContainerButton->setEnabled(true);
	}
	else
	{
		m_PropCheckBox_Container->setChecked(false);
		m_ContainerButton->setEnabled(false);
	}
	if (go->isPickable)
	{
		m_PropCheckBox_Pickable->setChecked(true);
		m_PickScriptButton->setEnabled(true);
	}
	else
	{
		m_PropCheckBox_Pickable->setChecked(false);
		m_PickScriptButton->setEnabled(false);
	}
	if (go->isTrigger)
	{
		m_PropCheckBox_Trigger->setChecked(true);
		m_TriggerScriptButton->setEnabled(true);
	}
	else
	{
		m_PropCheckBox_Trigger->setChecked(false);
		m_TriggerScriptButton->setEnabled(false);
	}
	if (go->isNPC)
	{
		m_PropCheckBox_NPC->setChecked(true);
		m_NPCScriptButton->setEnabled(true);
	}
	else
	{
		m_PropCheckBox_NPC->setChecked(false);
		m_NPCScriptButton->setEnabled(false);
	}
	if (go->isMonster)
	{
		m_PropCheckBox_Monster->setChecked(true);
		m_MonsterScriptButton->setEnabled(true);
	}
	else
	{
		m_PropCheckBox_Monster->setChecked(false);
		m_MonsterScriptButton->setEnabled(false);
	}
}

/**
 * \brief Sets all properties to empty (or default) values.
 * When user deselects all game object (when no object is selected) this function is called to clear properties fields in 
 * Properties floating window.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorGUI::ClearProperties()
{
	if(m_wnd_Properties)
	{
		m_PropEditBox_Name->setText(L"");
		m_PropEditBox_Pos_X->setText(L"");
		m_PropEditBox_Pos_Y->setText(L"");
		m_PropEditBox_Pos_Z->setText(L"");
		m_PropEditBox_Rot_X->setText(L"");
		m_PropEditBox_Rot_Y->setText(L"");
		m_PropEditBox_Rot_Z->setText(L"");
		m_PropEditBox_Model->setText(L"");
		m_PropEditBox_ID->setText(L"");
		m_PropEditBox_Description->setText(L"");

		m_PropCheckBox_Invisible->setChecked(false); //checkbox unchecked
		m_PropCheckBox_Invisible->setEnabled(false);
		m_PropCheckBox_Illusion->setChecked(false); //checkbox unchecked
		m_PropCheckBox_Illusion->setEnabled(false);
		m_PropCheckBox_Container->setChecked(false); //checkbox unchecked
		m_PropCheckBox_Container->setEnabled(false);
		m_PropCheckBox_Pickable->setChecked(false); //checkbox unchecked
		m_PropCheckBox_Pickable->setEnabled(false);
		m_PropCheckBox_Trigger->setChecked(false); //checkbox unchecked
		m_PropCheckBox_Trigger->setEnabled(false);
		m_PropCheckBox_NPC->setChecked(false); //checkbox unchecked
		m_PropCheckBox_NPC->setEnabled(false);
		m_PropCheckBox_Monster->setChecked(false); //checkbox unchecked
		m_PropCheckBox_Monster->setEnabled(false);
		m_ContainerButton->setEnabled(false);
		m_PickScriptButton->setEnabled(false);
		m_TriggerScriptButton->setEnabled(false);
		m_NPCScriptButton->setEnabled(false);
		m_MonsterScriptButton->setEnabled(false);
		m_PropCheckBox_Anchor->setEnabled(false);
		m_PropCheckBox_Anchor->setChecked(false);
		m_PropCheckBox_Terrain->setEnabled(false);
		m_PropCheckBox_Terrain->setChecked(false);

		m_PropButton_Set->setEnabled(false);
	}
}

bool CEditorGUI::GUIWindowOpen()
{
	return (m_wnd_AddNPCScriptAction || m_wnd_ContainerContent || m_wnd_AddPickScriptAction || m_wnd_AddTriggerScriptAction || m_wnd_InsertTerrainHeightmap || m_wnd_MonsterProperties || m_wnd_LevelMusic || m_wnd_ParticleSystem);
}

/**
 * \brief Loads 3D model and its properties and place it under the mouse pointer.
 * This lovely function is called when user clicks on element in the tree of game objects.
 * It makes sure GO exists in the file system, reads GO's parameters from XML, 
 * and opens GO's 3D model in EdLevel view. And sets GO's parameters in parameter window.
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorGUI::SetElementAtHand(const stringw &path, const stringw &name, bool isStatic)
{
	//Init game object
	CGameObject* go = new CGameObject(path,name,isStatic,m_EditorManager->getDriver());

	//Load properties from xml file
	stringw xmlProperties = path + name + L".xml";
	IXMLReader* xml = m_EditorManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());

	if(xml)
	{
		go->LoadPropertiesFromXMLFile(xml);

		//Load Model to Scene Tree
		AddNodeToSceneTree(m_EditorManager->m_ID, go->name);
		
		//Tell editor wich model to load 
		bool success = m_EditorManager->getEdiLevel()->SetElementAtHand(go);
		if (success)
		{
			go->id = m_EditorManager->m_ID;
			m_EditorManager->m_ID++;
		}

		//finally, set collected properties to GUI window
		SetProperties(go);

		xml->drop();
	}
	else
	{
		//shibMessage("Unfortunately, properties xml file is not found.");
		stringw message = "File ";
		message += name.c_str();
		message += ".xml is missing from folder ";
		message += path.c_str();
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"File is missing!", message.c_str());

	}
}


/**
 * \brief Finds Icon file and loads 2d image to container object.
 * This function is called when user wants to add content to container.
 * When he clicks on Item, icon of that Item is moved to the container.
 * This can be used later for draging icons around to other containers.
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorGUI::SetGameObjectToContainer(const stringw &parent, const stringw &name)
{
	stringw path = PICKABLES_DIR;
	if(parent != "Root Node") path += parent + L"/"; //add slash to parent string, and add it to the path

	//Init game object
	m_pPreviewPickableItem = new CGameObject(path,name,false,m_EditorManager->getDriver());

	//Load properties from xml file
	stringw xmlProperties = path + name + L".xml";
	IXMLReader* xml = m_EditorManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());
	if(xml)
	{
		m_pPreviewPickableItem->LoadPropertiesFromXMLFile(xml);
		xml->drop();

		m_SelectedPickItemIcon_Texture = m_pPreviewPickableItem->m_IconTexture;
		m_pContainer_EditPickItemScript->setEnabled(true); //??

		//Tell editor wich model to load 
		m_EditorManager->getEdiLevel()->SendModelToPickSession(m_pPreviewPickableItem);
		//finally, set collected properties
		m_pPickPreviewDescriptionText->setText(m_pPreviewPickableItem->description.c_str());

		if(m_bAddPickDirectly)
		{
			IGUIContainer* container = (IGUIContainer*) m_wnd_ContainerContent->getElementFromId(GUI_ID_CONTAINER);
			container->InsertItem(m_pPreviewPickableItem);
		}
		else
		{
			m_pContainer_AddPickItemToContainer->setEnabled(true);
		}
	}
	else
	{
		//warn user that this file is not found
		stringw message = "File ";
		message += name.c_str();
		message += stringw(".xml is missing from folder ") + path;
		m_EditorManager->getGUIEnvironment()->addMessageBox(L"File is missing!", message.c_str());
	}
}


/**
 * \brief Loads 3D model and its properties and place it under the mouse pointer.
 * This lovely function is called when user clicks on element in the tree of game objects.
 * It makes sure GO exists in the file system, reads GO's parameters from XML, 
 * and opens GO's 3D model in EdLevel view. And sets GO's parameters in parameter window.
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CEditorGUI::SendModelToPhotoSession(const stringw &parent, const stringw &name)
{
	stringw path = OBJECT_DIR;
	IGUITabControl* tabGameObjects = (IGUITabControl*) m_wnd_GameItems->getElementFromId(GUI_ID_TAB_GAME_OBJECTS,true);
	int a = tabGameObjects->getActiveTab();
	if(a>0 && !m_EditorManager->getEdiLevel()->m_bElementAtHand) 
	{
		IGUITab* tabs = tabGameObjects->getTab(a);
		path += tabs->getText();
		path += "/";
		if (parent != "Root Node") path += parent + L"/";

		//Init game object
		CGameObject* go = new CGameObject(path,name,false,m_EditorManager->getDriver());

		//Load properties from xml file
		stringw xmlProperties = path + name + L".xml";
		IXMLReader* xml = m_EditorManager->getFS()->createXMLReader(stringc(xmlProperties.c_str()).c_str());
		if(xml)
		{
			go->LoadPropertiesFromXMLFile(xml);
			xml->drop();

			//Tell editor wich model to load 
			bool success = m_EditorManager->getEdiLevel()->SendModelToPhotoSession(go);

			//finally, set collected properties
			m_pPreviewDescriptionText->setText(go->description.c_str());
		}
		else
		{
			//warn user that this model is not found
			stringw message = "File ";
			message += name;
			message += ".xml is missing from folder ";
			message += path;
			m_pPreviewDescriptionText->setText(message.c_str());
		}
	}
}

/**
 * \brief Remove deleted item from tree of scene nodes.
 */
void CEditorGUI::RemoveNodeFromSceneTree(s32 id)
{
	TreeNode* node = m_SceneNodesTree->FindNode(id);
	m_SceneNodesTree->DeleteNode(node);
}

/**
 * \brief Add item to tree of scene nodes.
 */
void CEditorGUI::AddNodeToSceneTree(s32 id, const stringw &name)
{
	TreeNode* parentNode = m_SceneNodesTree->FindNode(-2);
	m_SceneNodesTree->InsertNode(name, parentNode, id);
}

/**
 * \brief Set selected element in the tree of scene nodes.
 */
void CEditorGUI::SetSelectedElementInTheTreeofSceneNodes(int id)
{
	TreeNode* node = m_SceneNodesTree->FindNode(id);
	m_SceneNodesTree->SetSelected(node);
}

/**
 * \brief Clear tree of scene nodes.
 */
void CEditorGUI::ClearTreeOfSceneNodes()
{
	m_SceneNodesTree->Clear();
	m_SceneNodesTree->InsertNode(L"Scene root node",m_SceneNodesTree->GetRootNode(),-2);
}

/**
 * \brief Initializes all windows and gui elements...
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditorGUI::Init(CEditorManager* edMngr)
{
	m_EditorManager = edMngr;

	//Setting GUI transparency (alpha)
	for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = m_EditorManager->getGUIEnvironment()->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(240);
		m_EditorManager->getGUIEnvironment()->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}

	/*Create Menu*/
	CreateMenu(m_EditorManager);

	/*Properties window*/
	m_wnd_Properties = MakePropertiesWindow(m_EditorManager);
	ClearProperties();

	/*Game Objects window*/
	m_wnd_GameItems = MakeGameObjectsWindow(m_EditorManager);

	//Preview models with render on texture
	if (m_EditorManager->getDriver()->queryFeature(video::EVDF_RENDER_TO_TARGET))
	{
		// add fixed cameras
		m_PhotoCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, 
			core::vector3df(0,-10000,0),
			core::vector3df(0,-10000,-100));
		m_PickCamera = m_EditorManager->getSceneMngr()->addCameraSceneNode(0, 
			core::vector3df(0,-10000,0),
			core::vector3df(0,-10000,100));

		// add textures to render on
		m_TextureToRenderOn = m_EditorManager->getDriver()->addRenderTargetTexture(core::dimension2du(200,200));
		m_ContainerPickPreview_TextureToRenderOn = m_EditorManager->getDriver()->addRenderTargetTexture(core::dimension2du(180,180));
		
		//model preview window
		m_wnd_ModelPreview = m_EditorManager->getGUIEnvironment()->addWindow(core::rect<s32>(0,610,230,970), false, L"Object Preview", 0, 4800);
		IGUIImage* previewImage = m_EditorManager->getGUIEnvironment()->addImage(rect<s32>(10,20,210,220),m_wnd_ModelPreview);
		previewImage->setImage(m_TextureToRenderOn);
		m_EditorManager->getGUIEnvironment()->addStaticText(L"Description:", core::rect<s32>(10,230,210,255), false, false, m_wnd_ModelPreview, 4560, false);
		m_pPreviewDescriptionText = m_EditorManager->getGUIEnvironment()->addStaticText(L"Description:", core::rect<s32>(10,255,210,355), true, true, m_wnd_ModelPreview, 4561, false);
	}

	mCenterX = m_EditorManager->getDevice()->getVideoDriver()->getScreenSize().Width/2;
	mCenterY = m_EditorManager->getDevice()->getVideoDriver()->getScreenSize().Height/2;

	printf("GUI Initialized");

	return true;
}
