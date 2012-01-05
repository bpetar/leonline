/** 
 * \file GUIButtonHandler.cpp
 * \brief Handles button events.
 * 
 * Who says it all has to be in one cpp file? :) 
 * I moved these functions from EDGui.cpp file, CEditorGUI class, to stop it from growing enormous
 * All funtions that handle menu clicks are neatly placed here.
 * Writting a winning combination of include files was a nigthmare but I did it!
 * Couldn't repeat it if I wanted to, so don't touch anything :)
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date June, 1 2009.
 */

#include "../EditorManager.h"
#include "../Utils.h"
#include "GUIButtonHandler.h"
#include "IGUIContainer.h"

//Called from EDGui.cpp,
//Handles button events in one big switch case.
bool HandleButtonClick(CEditorManager* editorManager, s32 id)
{
	CEditorGUI* edGui = editorManager->getGUIManager();
	IrrlichtDevice* device = editorManager->getDevice();
	IGUIEnvironment* gui = editorManager->getGUIEnvironment();
	ISceneManager* smgr = editorManager->getSceneMngr();

	switch(id)
	{
		// Add container content
		case GUI_ID_BUTTON_ADD_CONTAINER:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				edGui->m_SelectedObjectID = editorManager->GetSelectedObjectParameter_ID();
				edGui->m_wnd_ContainerContent = MakeContainerContentWindow(editorManager,edGui->m_SelectedObjectID);
			}
		}
		break;
		// Container button OK!
		case GUI_ID_BUTTON_CONTAINER_OK:
		{
			//Save Container Content and close the window
			CGameObject* item;
			IGUIContainer* container = (IGUIContainer*) edGui->m_wnd_ContainerContent->getElementFromId(GUI_ID_CONTAINER);
			//TODO: implement smarter object updating, right now we clean all pickables and add them again, 
			//      so IDs get changed on save (button OK) even if no items were added/removed
			editorManager->ObjectClearPickableItems(edGui->m_SelectedObjectID);
			for ( u32 index = 0; index < container->GetNumItems(); index++)
			{
				item = container->GetItem(index);
				if (item != NULL)
				{
					if (edGui->m_SelectedObjectID >= 0)
					{
						editorManager->ObjectAddPickableItem(edGui->m_SelectedObjectID, item->path + item->root);
					}
				}
			}
			edGui->m_wnd_ContainerContent->remove();
			edGui->m_wnd_ContainerContent = NULL;
		}
		break;
		// Container button OK!
		case GUI_ID_BUTTON_CONTAINER_CANCEL:
		{
			edGui->m_wnd_ContainerContent->remove();
			edGui->m_wnd_ContainerContent = NULL;
		}
		break;
		// Add pickable item to container
		case GUI_ID_BUTTON_CONTAINER_ADD_ITEM:
		{
			if(edGui->m_pPreviewPickableItem)
			{
				IGUIContainer* container = (IGUIContainer*) edGui->m_wnd_ContainerContent->getElementFromId(GUI_ID_CONTAINER);
				container->InsertItem(edGui->m_pPreviewPickableItem);
			}
		}
		break;
		// Edit pick script action
		case GUI_ID_BUTTON_CONTAINER_EDIT_PICK_SCRIPT:
		{
			//edit script of preview item
			if(edGui->m_pPreviewPickableItem && !edGui->m_wnd_AddPickScriptAction)
			{
				editorManager->getScriptEngine()->Clear();
				edGui->m_wnd_AddPickScriptAction = MakePickScriptWindow(editorManager, edGui->m_pPreviewPickableItem->script);
			}
		}
		break;

		//////////////////////////////////////
		////////// PICK SCRIPT ///////////////
		//////////////////////////////////////

		// Button clicked: AddPickScriptAction 
		case GUI_ID_BUTTON_PICK_ADD:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getScriptEngine()->Clear();
				edGui->m_SelectedObjectID = editorManager->GetSelectedObjectParameter_ID();
				stringw scriptName = editorManager->GetObjectParameter_Script(edGui->m_SelectedObjectID);
				edGui->m_wnd_AddPickScriptAction = MakePickScriptWindow(editorManager, scriptName);
				edGui->m_bGUIFocused = true;
			}
		}
		break;
		// Add new pick script action
		case GUI_ID_BUTTON_PICK_ADD_ACTION:
		{
			s32 index = edGui->m_PickListBox_ExistingEvents->getSelected();
			TAction action;
			action.name = edGui->m_PickComboBox_Actions->getText();
			if(action.name != stringw(L"none"))
			{
				if(index>-1)
				{
					action.attribute = edGui->m_PickEditBox_ActionAttribute->getText();
					action.value = edGui->m_PickEditBox_ActionValue->getText();
					action.target = edGui->m_PickComboBox_ActionTarget->getText();
					if (action.target == stringw(L"id"))
					{
						if(stringw(edGui->m_PickEditBox_ActionTargetID->getText()) != stringw(L""))
						{
							action.target = edGui->m_PickEditBox_ActionTargetID->getText();

							TCondition condition;
							condition.name = edGui->m_PickComboBox_Conditions->getText();
							if(condition.name.equals_ignore_case("none"))
							{
								//add unconditioned action
								editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].actions.push_back(action);
							}
							else
							{
								//first looking if condition already exists in this event
								bool found = false;
								condition.value = edGui->m_PickEditBox_ConditionValue->getText();
								for(int c=0; c<editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions.size(); c++)
								{
									if(editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions[c].name.equals_ignore_case(condition.name))
									{
										if(editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions[c].value.equals_ignore_case(condition.value))
										{
											//this condition already exists, so we just add action to it
											found = true;
											editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions[c].actions.push_back(action);
										}
									}
								}
								
								if(!found)
								{
									//create new condition
									condition.actions.clear(); //clear actions list if this is first occurance of condition
									condition.actions.push_back(action);
									//add conditioned action
									editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions.push_back(condition);
								}
							}
							stringw scriptText = editorManager->getScriptEngine()->PickScriptActionToString_Index(index);
							edGui->m_PickEditBox_Script->setText(scriptText.c_str());
						}
						else
						{
							//TODO: message: ENTER ID Value for target of this action
						}
					}
					else
					{
						TCondition condition;
						condition.name = edGui->m_PickComboBox_Conditions->getText();
						if(condition.name.equals_ignore_case("none"))
						{
							//add unconditioned action
							editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].actions.push_back(action);
						}
						else
						{
							//first looking if condition already exists in this event
							bool found = false;
							condition.value = edGui->m_PickEditBox_ConditionValue->getText();
							for(int c=0; c<editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions.size(); c++)
							{
								if(editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions[c].name.equals_ignore_case(condition.name))
								{
									if(editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions[c].value.equals_ignore_case(condition.value))
									{
										//this condition already exists, so we just add action to it
										found = true;
										editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions[c].actions.push_back(action);
									}
								}
							}
								
							if(!found)
							{
								//create new condition
								condition.actions.clear(); //clear actions list if this is first occurance of condition
								condition.actions.push_back(action);
								//add conditioned action
								editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].conditions.push_back(condition);
							}
						}
						stringw scriptText = editorManager->getScriptEngine()->PickScriptActionToString_Index(index);
						edGui->m_PickEditBox_Script->setText(scriptText.c_str());
					}
				}
				else
				{
					//TODO message: select event first!
				}
			}
			else
			{
				//TODO: message: "select action first!"
			}
		}
		break;
		// Add new pick script event
		case GUI_ID_BUTTON_PICK_ADD_EVENT:
		{
			stringw event = edGui->m_PickComboBox_Events->getText();
			if(event != stringw(L"none"))
			{
				bool alreadyExists = false;
				for (u32 i=0; i<edGui->m_PickListBox_ExistingEvents->getItemCount(); i++)
				{
					if(stringw(edGui->m_PickListBox_ExistingEvents->getListItem(i)) == event)
					{
						alreadyExists = true;
					}
				}
				if(!alreadyExists)
				{
					edGui->m_PickListBox_ExistingEvents->addItem(event.c_str());
					edGui->m_PickListBox_ExistingEvents->setSelected(edGui->m_PickListBox_ExistingEvents->getItemCount()-1);
					TScriptAction scriptAction;
					scriptAction.event.name = event;
					scriptAction.event.target = edGui->m_PickComboBox_EventTarget->getText();
					editorManager->getScriptEngine()->m_SelectedListOfScriptActions.push_back(scriptAction);
					int scriptIndex = edGui->m_PickListBox_ExistingEvents->getItemCount()-1;
					stringw scriptText = editorManager->getScriptEngine()->PickScriptActionToString_Index(scriptIndex);
					edGui->m_PickEditBox_Script->setText(scriptText.c_str());
					
				}
			}
			else
			{
				//TODO: msg: "Pick event first"
			}
		}
		break;
		// Remove pick script event
		case GUI_ID_BUTTON_PICK_REMOVE_EVENT:
		{
			s32 index = edGui->m_PickListBox_ExistingEvents->getSelected();
			if(index > -1)
			{
				editorManager->getScriptEngine()->m_SelectedListOfScriptActions.erase(index);
				edGui->m_PickListBox_ExistingEvents->removeItem(index);
				u32 b = edGui->m_PickListBox_ExistingEvents->getItemCount();
				if(b > (u32)index)
				{
					edGui->m_PickListBox_ExistingEvents->setSelected(index);
					stringw scriptText = editorManager->getScriptEngine()->PickScriptActionToString_Index(index);
					edGui->m_PickEditBox_Script->setText(scriptText.c_str());
				}
				else
				{
					edGui->m_PickListBox_ExistingEvents->setSelected(index-1);
					stringw scriptText = editorManager->getScriptEngine()->PickScriptActionToString_Index(index-1);
					edGui->m_PickEditBox_Script->setText(scriptText.c_str());
				}
			}
			else
			{
				//TODO: msg: "Pick event first"
			}
		}
		break;
		// Save pick script
		case GUI_ID_BUTTON_PICK_SAVE:
		{
			edGui->SavePickScript(edGui->m_PickScript.c_str());
			edGui->m_wnd_AddPickScriptAction->remove();
			edGui->m_wnd_AddPickScriptAction = NULL;
			edGui->m_bGUIFocused = false;
		}
		break;
		//Button clicked: Cancel Pick Script
		case GUI_ID_BUTTON_PICK_CANCEL:
		{
			edGui->m_wnd_AddPickScriptAction->remove();
			edGui->m_wnd_AddPickScriptAction = NULL;
			edGui->m_bGUIFocused = false;
		}
		break;

		//////////////////////////////////////////////
		//////////////// TRIGGER SCRIPT //////////////
		//////////////////////////////////////////////

		// AddTriggerScriptAction MakeTriggerScriptWindow
		case GUI_ID_BUTTON_TRIGGER_ADD:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getScriptEngine()->Clear();
				edGui->m_SelectedObjectID = editorManager->GetSelectedObjectParameter_ID();
				edGui->m_wnd_AddTriggerScriptAction = MakeTriggerScriptWindow(editorManager,edGui->m_SelectedObjectID, editorManager->GetObjectParameter_Script(edGui->m_SelectedObjectID));
				edGui->m_bGUIFocused = true;
			}
		}
		break;
		// Add new state
		case GUI_ID_BUTTON_TRIGGER_ADD_STATE:
		{
			//add new state
			if(stringw(edGui->m_TriggerEditBox_State->getText()) != stringw(L""))
			{
				bool alreadyExists = false;
				for (u32 i=0; i<edGui->m_TriggerListBox_States->getItemCount(); i++)
				{
					if(stringw(edGui->m_TriggerListBox_States->getListItem(i)) == stringw(edGui->m_TriggerEditBox_State->getText()))
					{
						alreadyExists = true;
					}
				}
				if(!alreadyExists)
				{
					s32 lastIndex = edGui->m_TriggerListBox_States->getItemCount();
					stringw newState = edGui->m_TriggerEditBox_State->getText();
					edGui->m_TriggerListBox_States->addItem(newState.c_str());
					edGui->m_TriggerListBox_States->setSelected(lastIndex);
					editorManager->getScriptEngine()->AddTriggerScriptActionState(newState);
					//Load new Action Script to Editbox 
					stringw scriptText = editorManager->getScriptEngine()->TriggerScriptActionToString_Index(lastIndex);
					edGui->m_TriggerEditBox_Script->setText(scriptText.c_str());
					edGui->m_TriggerEditBox_State->setText(L"");
				}
			}
			else
			{
				//TODO: msg: "First enter state in field above"
			}
		}
		break;
		// Remove state
		case GUI_ID_BUTTON_TRIGGER_REMOVE_STATE:
		{
			s32 selectedIndex = edGui->m_TriggerListBox_States->getSelected();
			if(selectedIndex > -1)
			{
				//if removing default state - erase it from object properties.
				if(stringw(edGui->m_TriggerListBox_States->getListItem(selectedIndex)) == editorManager->GetObjectParameter_State(edGui->m_SelectedObjectID))
				{
					editorManager->SetObjectParameter_State(edGui->m_SelectedObjectID, "");
				}
				//remove state from the list, as well as actions associated with it
				edGui->m_TriggerListBox_States->removeItem(selectedIndex);
				edGui->m_TriggerEditBox_Script->setText(L"");
				editorManager->getScriptEngine()->RemoveTriggerScriptActionState(selectedIndex);
			}
			else
			{
				//TODO: msg: select state from list
			}
		}
		break;
		// Set state
		case GUI_ID_BUTTON_TRIGGER_SET_STATE:
		{
			//add new state
			s32 index = edGui->m_TriggerListBox_States->getSelected();
			if(index > -1)
			{
				editorManager->SetObjectParameter_State(edGui->m_SelectedObjectID, edGui->m_TriggerListBox_States->getListItem(index));
			}
			else
			{
				//TODO: msg: "First select state"
			}
		}
		break;
		// Add action
		case GUI_ID_BUTTON_TRIGGER_ADD_ACTION:
		{
			s32 index = edGui->m_TriggerListBox_States->getSelected();
			if(index>-1)
			{
				TAction action;
				action.name = edGui->m_TriggerComboBox_Actions->getText();
				if(action.name != stringw(L"none"))
				{
					action.attribute = edGui->m_PickEditBox_ActionAttribute->getText();
					action.value = edGui->m_PickEditBox_ActionValue->getText();
					action.target = edGui->m_TriggerComboBox_ActionTarget->getText();
					if (action.target == stringw(L"id"))
					{
						if(stringw(edGui->m_PickEditBox_ActionTargetID->getText()) != stringw(L""))
						{
							action.target = edGui->m_PickEditBox_ActionTargetID->getText();
							editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].actions.push_back(action);
							stringw scriptText = editorManager->getScriptEngine()->TriggerScriptActionToString_Index(index);
							edGui->m_TriggerEditBox_Script->setText(scriptText.c_str());
						}
						else
						{
							//TODO: message: ENTER ID Value for target of this action
						}
					}
					else
					{
						editorManager->getScriptEngine()->m_SelectedListOfScriptActions[index].actions.push_back(action);
						stringw scriptText = editorManager->getScriptEngine()->TriggerScriptActionToString_Index(index);
						edGui->m_TriggerEditBox_Script->setText(scriptText.c_str());
					}
				}
				else
				{
					//TODO: message: "select action first!"
				}
			}
			else
			{
				//TODO message: select event first!
			}
		}
		break;
		// Save trigger script
		case GUI_ID_BUTTON_TRIGGER_SAVE:
		{
			edGui->SaveTriggerScript(editorManager->GetObjectParameter_Script(edGui->m_SelectedObjectID).c_str());
			edGui->m_wnd_AddTriggerScriptAction->remove();
			edGui->m_wnd_AddTriggerScriptAction = NULL;
			edGui->m_bGUIFocused = false;
		}
		break;
		// Cancel trigger script
		case GUI_ID_BUTTON_TRIGGER_CANCEL:
		{
			edGui->m_wnd_AddTriggerScriptAction->remove();
			edGui->m_wnd_AddTriggerScriptAction = NULL;
			edGui->m_bGUIFocused = false;
		}
		break;

		////////////////////////////////////////
		/////////////// ADD NPC ////////////////
		////////////////////////////////////////

		//Button clicked: AddNPCScriptAction MakeNPCScriptWindow
		case GUI_ID_BUTTON_NPC_ADD:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				edGui->m_SelectedObjectID = editorManager->GetSelectedObjectParameter_ID();
				editorManager->getScriptEngine()->Clear();
				//create conversation dialog window
				stringw rootName = editorManager->GetObjectParameter_RootName(edGui->m_SelectedObjectID);
				edGui->m_DialogFilename = rootName.c_str();
				edGui->m_DialogFilename += ".dlg";
				edGui->m_wnd_AddNPCScriptAction = MakeNPCScriptWindow(editorManager,edGui->m_DialogFilename);
				edGui->m_bGUIFocused = true;
			}
		}
		break;
		// Add dialog node
		case GUI_ID_BUTTON_NPC_ADD_NODE:
		{
			s32 windowXcenter = 140;
			s32 windowYtop = 300;

			//show dialog
			edGui->m_SelectedNode = edGui->m_DialogTree->GetSelected();
			if(edGui->m_SelectedNode)
			{
				if(edGui->m_SelectedNode->Player)
				{
					//add what NPC says next
					edGui->m_wnd_AddNPCNewText = gui->addWindow(core::rect<s32>(edGui->mCenterX-windowXcenter,windowYtop,edGui->mCenterX+windowXcenter,windowYtop+130), true, L"Add NPC reply", 0, GUI_ID_WINDOW_NPC_ADD_TEXT);
					gui->addStaticText(L"Add what NPC says next:", core::rect<s32>(0,30,2*windowXcenter,50), false, false, edGui->m_wnd_AddNPCNewText, -1, false)->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);;
					edGui->m_DialogEditBox_Text = gui->addEditBox(L"", core::rect<s32>(windowXcenter-100,55,windowXcenter+100,75), true, edGui->m_wnd_AddNPCNewText, GUI_ID_EDITBOX_NPC_ADD_TEXT);
					edGui->m_DialogButton_AddText = gui->addButton(core::rect<s32>(windowXcenter-30,85,windowXcenter+30,105), edGui->m_wnd_AddNPCNewText, GUI_ID_BUTTON_NPC_ADD_TEXT, L"Ok");
					gui->setFocus(edGui->m_DialogEditBox_Text);
				}
				else
				{
					if (!edGui->m_SelectedNode->isLink)
					{
						//add what Player says next
						edGui->m_wnd_AddNPCNewText = gui->addWindow(core::rect<s32>(edGui->mCenterX-windowXcenter,windowYtop,edGui->mCenterX+windowXcenter,windowYtop+130), true, L"Add Player reply", 0, GUI_ID_WINDOW_NPC_ADD_TEXT);
						gui->addStaticText(L"Add what Player says next:", core::rect<s32>(0,30,2*windowXcenter,50), false, false, edGui->m_wnd_AddNPCNewText, -1, false)->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);;
						edGui->m_DialogEditBox_Text = gui->addEditBox(L"", core::rect<s32>(windowXcenter-100,55,windowXcenter+100,75), true, edGui->m_wnd_AddNPCNewText, GUI_ID_EDITBOX_NPC_ADD_TEXT);
						edGui->m_DialogButton_AddText = gui->addButton(core::rect<s32>(windowXcenter-30,85,windowXcenter+30,105), edGui->m_wnd_AddNPCNewText, GUI_ID_BUTTON_NPC_ADD_TEXT, L"Ok");
						gui->setFocus(edGui->m_DialogEditBox_Text);
					}
					else
					{
						//cant add children to link nodes!
						edGui->m_wnd_AddNPCNewText = gui->addWindow(core::rect<s32>(edGui->mCenterX-windowXcenter,windowYtop,edGui->mCenterX+windowXcenter,windowYtop+100), true, L"Insert Node Error", 0, GUI_ID_WINDOW_NPC_ADD_TEXT);
						gui->addStaticText(L"Can't add child to LINK node!", core::rect<s32>(0,30,2*windowXcenter,60), false, false, edGui->m_wnd_AddNPCNewText, -1, false)->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);;
						edGui->m_DialogButton_Warning = gui->addButton(core::rect<s32>(windowXcenter-30,70,windowXcenter+30,90), edGui->m_wnd_AddNPCNewText, GUI_ID_BUTTON_NPC_WARNING, L"Ok");
						gui->setFocus(edGui->m_DialogButton_Warning);
					}
				}
			}
			else
			{
				//select node first!
				edGui->m_wnd_AddNPCNewText = gui->addWindow(core::rect<s32>(edGui->mCenterX-windowXcenter,windowYtop,edGui->mCenterX+windowXcenter,windowYtop+100), true, L"Select Node", 0, GUI_ID_WINDOW_NPC_ADD_TEXT);
				gui->addStaticText(L"Select Parent Node First!", core::rect<s32>(0,30,windowXcenter*2,60), false, false, edGui->m_wnd_AddNPCNewText, -1, false)->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);;
				edGui->m_DialogButton_Warning = gui->addButton(core::rect<s32>(windowXcenter-30,70,windowXcenter+30,90), edGui->m_wnd_AddNPCNewText, GUI_ID_BUTTON_NPC_WARNING, L"Ok");
				gui->setFocus(edGui->m_DialogButton_Warning);
			}
		}
		break;
		// Remove node
		case GUI_ID_BUTTON_NPC_REMOVE_NODE:
		{
			edGui->m_SelectedNode = edGui->m_DialogTree->GetSelected();
			
			if(edGui->m_SelectedNode)
			{
				u32 Id = edGui->m_SelectedNode->ID;
				if(edGui->m_SelectedNode != edGui->m_DialogTree->GetRootNode()->Child)
				{
					edGui->m_DialogTree->DeleteNode(edGui->m_SelectedNode);
				}
				else
				{
					//can not delete root node!
				}
				//this condition should always be true, but we check anyway because it would crash
				if(edGui->m_DialogTree->GetRootNode()->Child)
				{
					//check if there are actions pointing to deleted node
					edGui->CheckDialogNodeActions(edGui->m_DialogTree->GetRootNode()->Child,Id);
				}
			}
			else
			{
				//nothing selected!
			}
		}
		break;
		// On OK button press
		case GUI_ID_BUTTON_NPC_OK:
		{
			//save and exit
			edGui->SaveDialog(edGui->m_DialogFilename);
			edGui->m_wnd_AddNPCScriptAction->remove();
			edGui->m_wnd_AddNPCScriptAction = 0;
			edGui->m_bGUIFocused = false;
		}
		break;
		// NPC cancel pressed
		case GUI_ID_BUTTON_NPC_CANCEL:
		{
			//exit
			edGui->m_wnd_AddNPCScriptAction->remove();
			edGui->m_wnd_AddNPCScriptAction = 0;
			edGui->m_bGUIFocused = false;
		}
		break;
		//
		case GUI_ID_BUTTON_NPC_LINK_NODE:
		{
			s32 windowXcenter = 140;
			s32 windowYtop = 300;

			edGui->m_SelectedNode = edGui->m_DialogTree->GetSelected();
			if(edGui->m_SelectedNode)
			{
				if(edGui->m_SelectedNode != edGui->m_DialogTree->GetRootNode()->Child)
				{
					if(edGui->m_SelectedNode->Link)
					{
						//can't add more then one link to one node
						edGui->m_wnd_AddNPCNewText = gui->addWindow(core::rect<s32>(edGui->mCenterX-windowXcenter,windowYtop,edGui->mCenterX+windowXcenter,windowYtop+100), true, L"Link Node Error", 0, GUI_ID_WINDOW_NPC_ADD_TEXT);
						gui->addStaticText(L"Can't add more then one link to one node!", core::rect<s32>(0,30,2*windowXcenter,60), false, false, edGui->m_wnd_AddNPCNewText, -1, false)->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);;
						edGui->m_DialogButton_Warning = gui->addButton(core::rect<s32>(windowXcenter-30,70,windowXcenter+30,90), edGui->m_wnd_AddNPCNewText, GUI_ID_BUTTON_NPC_WARNING, L"Ok");
						gui->setFocus(edGui->m_DialogButton_Warning);
						return true;
					}
					edGui->m_bLinkDialogNodes = true;
				}
				else
				{
					//can't link RootNode
				}
			}
			else
			{
				//select node first!
				edGui->m_wnd_AddNPCNewText = gui->addWindow(core::rect<s32>(edGui->mCenterX-windowXcenter,windowYtop,edGui->mCenterX+windowXcenter,windowYtop+100), true, L"Select Node", 0, GUI_ID_WINDOW_NPC_ADD_TEXT);
				gui->addStaticText(L"Select Parent Node First!", core::rect<s32>(0,30,2*windowXcenter,60), false, false, edGui->m_wnd_AddNPCNewText, -1, false)->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);
				edGui->m_DialogButton_Warning = gui->addButton(core::rect<s32>(windowXcenter-30,70,windowXcenter+30,90), edGui->m_wnd_AddNPCNewText, GUI_ID_BUTTON_NPC_WARNING, L"Ok");
				gui->setFocus(edGui->m_DialogButton_Warning);
			}
		}
		break;
		// Toggle enabled/disabled dialog node
		case GUI_ID_BUTTON_NPC_TOGGLE_ENABLED:
		{
			edGui->m_SelectedNode = edGui->m_DialogTree->GetSelected();
			if(edGui->m_SelectedNode)
			{
				PropagateEnabled(edGui->m_SelectedNode,!edGui->m_SelectedNode->isEnabled);
			}
			else
			{
				//Nothing selected!
			}
		}
		break;
		//change node text
		case GUI_ID_BUTTON_NPC_CHANGE_TEXT:
		{
			//get selected node
			edGui->m_SelectedNode = edGui->m_DialogTree->GetSelected();
			stringw newText = edGui->m_DialogEditBox_ChangeText->getText();
			if(edGui->m_SelectedNode)
			{
				if(edGui->m_SelectedNode != edGui->m_DialogTree->GetRootNode()->Child)
				{
					if(newText != stringw(L""))
					{
						edGui->m_SelectedNode->Text = edGui->m_DialogEditBox_ChangeText->getText();
					}
					else
					{
						//Enter new text first!
						//Text Edito field is empty
					}
				}
				else
				{
					//Can't change text of the RootNode
				}
			}
			else
			{
				//Nothing selected!
				//select node first
			}
		}
		break;
		//add npc dialog action
		case GUI_ID_BUTTON_NPC_ADD_ACTION:
		{
			edGui->m_SelectedNode = edGui->m_DialogTree->GetSelected();
			if(edGui->m_SelectedNode)
			{
				if(edGui->m_SelectedNode != edGui->m_DialogTree->GetRootNode()->Child)
				{
					//add action to selected node
					TAction action;
					action.name = edGui->m_DialogComboBox_Actions->getText();
					if(action.name != stringw(L"none"))
					{
						action.attribute = edGui->m_DialogEditBox_ActionAttribute->getText();
						action.value = edGui->m_DialogEditBox_ActionValue->getText();
						action.target = edGui->m_DialogComboBox_ActionTarget->getText();
						if (action.target == stringw(L"id"))
						{
							if(stringw(edGui->m_DialogEditBox_ActionTargetID->getText()) != stringw(L""))
							{
								action.target = edGui->m_DialogEditBox_ActionTargetID->getText();
								edGui->m_SelectedNode->scriptAction.actions.push_back(action);
								edGui->m_SelectedNode->hasScriptAction = true;
								edGui->WriteNPCDialogScriptAction();
							}
							else
							{
								//TODO: message: ENTER ID Value for target of this action
							}
						}
						else
						{
							edGui->m_SelectedNode->scriptAction.actions.push_back(action);
							edGui->m_SelectedNode->hasScriptAction = true;
							edGui->WriteNPCDialogScriptAction();
						}
					}
					else
					{
						//TODO: message: "select action first!"
					}
				}
				else
				{
					//cant add actions to RootNode, its not visible as option in dialog
				}
			}
			else
			{
				//Nothing selected!
			}
		}
		break;
		//clear dialog node action
		case GUI_ID_BUTTON_NPC_CLEAR_ACTION:
		{
			edGui->m_SelectedNode = edGui->m_DialogTree->GetSelected();
			if(edGui->m_SelectedNode)
			{
				edGui->m_SelectedNode->scriptAction.actions.clear();
				edGui->m_SelectedNode->hasScriptAction = false;
				edGui->WriteNPCDialogScriptAction();
			}
		}
		break;
		// Add new text node
		case GUI_ID_BUTTON_NPC_ADD_TEXT:
		{
			//Add new text node to dialog
			TreeNode* node = edGui->m_DialogTree->InsertNode(edGui->m_DialogEditBox_Text->getText(),edGui->m_SelectedNode,edGui->m_NumNodes+1);
			if(node)
			{
				edGui->m_NumNodes++;
				if(edGui->m_SelectedNode != edGui->m_DialogTree->GetRootNode())
				{
					node->Player = !edGui->m_SelectedNode->Player;
				}
				else
				{
					node->Player = false;
				}
				if(!node->Player)
				{
					node->NPCName = editorManager->GetObjectParameter_Name(edGui->m_SelectedObjectID);
				}
				edGui->m_wnd_AddNPCNewText->remove();
				edGui->m_DialogButton_Warning = 0;
				edGui->m_DialogButton_AddText = 0;
				edGui->m_wnd_AddNPCNewText = 0;
				//gui->setFocus(m_DialogButton_AddNode);
			}
			else
			{
				//error adding node!
			}
		}
		break;
		//
		case GUI_ID_BUTTON_NPC_WARNING:
		{
			//remove warning dialog window
			edGui->m_wnd_AddNPCNewText->remove();
			edGui->m_wnd_AddNPCNewText = 0;
			edGui->m_DialogButton_Warning = 0;
			edGui->m_DialogButton_AddText = 0;
			gui->setFocus(edGui->m_DialogButton_AddNode);
		}
		break;

		//////////////////////////////
		////////// ADD MONSTER ///////
		//////////////////////////////

		//Button clicked: AddMonsterScriptAction MakeMonsterScriptWindow
		case GUI_ID_BUTTON_MONSTER_ADD:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				CGameObject* go = editorManager->GetSelectedGameObject();
				edGui->m_wnd_MonsterProperties = MakeMonsterScriptWindow(editorManager,go);
				edGui->m_bGUIFocused = true;
			}
		}
		break;

		case GUI_ID_MONSTER_CANCEL:
		{
			edGui->m_wnd_MonsterProperties->remove();
			edGui->m_wnd_MonsterProperties = 0;
			edGui->m_bGUIFocused = false;
		}
		break;

		case GUI_ID_MONSTER_SAVE:
		{
			//Save values for this monster only (not as xml property for all monsters of that type)
			//Values that are different from default will be saved to map file as this monster object atributes.
			CGameObject* go = editorManager->GetSelectedGameObject();
			
			if(edGui->m_Monsters_EditBox_Health != 0)
			{
				s32 value = go->getAbilityValue("Health");
				swscanf(edGui->m_Monsters_EditBox_Health->getText(),L"%d",&value);
				go->setAbilityValue("Health", value);
			}

			IGUIEditBox* m_MonstersExp = (IGUIEditBox*)edGui->m_wnd_MonsterProperties->getElementFromId(GUI_ID_MONSTER_EXP);
			if(m_MonstersExp != 0)
			{
				s32 value = go->getAbilityValue("Experience");
				swscanf(m_MonstersExp->getText(),L"%d",&value);
				go->setAbilityValue("Experience", value);
			}

			IGUIEditBox* m_MonstersRadius = (IGUIEditBox*)edGui->m_wnd_MonsterProperties->getElementFromId(GUI_ID_MONSTER_RADIUS);
			if(m_MonstersRadius != 0)
			{
				s32 value = go->m_Radius;
				swscanf(m_MonstersRadius->getText(),L"%d",&value);
				go->m_Radius = value;
			}

			//Get in the Mood
			if(edGui->m_Monsters_ComboBox_Mood != 0)
			{
				s32 value = edGui->m_Monsters_ComboBox_Mood->getSelected();
				go->m_Mood = (eMood) value;
			}

			for(u32 i=0; i<go->m_ListOfSkills.size(); i++)
			{
				IGUIEditBox* min = (IGUIEditBox*)edGui->m_wnd_MonsterProperties->getElementFromId(GUI_ID_MONSTER_SKILL+i*10+1);
				IGUIEditBox* max = (IGUIEditBox*)edGui->m_wnd_MonsterProperties->getElementFromId(GUI_ID_MONSTER_SKILL+i*10+2);
				s32 _min = 0;
				s32 _max = 0;
				swscanf(min->getText(),L"%d",&_min);
				swscanf(max->getText(),L"%d",&_max);
				go->m_ListOfSkills[i].min = _min;
				go->m_ListOfSkills[i].max = _max;
			}

			//TODO: Store the sounds!

			edGui->m_wnd_MonsterProperties->remove();
			edGui->m_wnd_MonsterProperties = 0;
		}
		break;

		case GUI_ID_MONSTER_SAVEFORALL:
		{
			//Save values for all monsters of this type.

			CGameObject* go = editorManager->GetSelectedGameObject();

			//But first save for this monster, 
			//and ponder about all monsters of this type that are already on the map!!

			if(edGui->m_Monsters_EditBox_Health != 0)
			{
				s32 value = go->getAbilityValue("Health");
				swscanf(edGui->m_Monsters_EditBox_Health->getText(),L"%d",&value);
				go->setAbilityValue("Health", value);
			}

			IGUIEditBox* m_MonstersExp = (IGUIEditBox*)edGui->m_wnd_MonsterProperties->getElementFromId(GUI_ID_MONSTER_EXP);
			if(m_MonstersExp != 0)
			{
				s32 value = go->getAbilityValue("Experience");
				swscanf(m_MonstersExp->getText(),L"%d",&value);
				go->setAbilityValue("Experience", value);
			}

			IGUIEditBox* m_MonstersRadius = (IGUIEditBox*)edGui->m_wnd_MonsterProperties->getElementFromId(GUI_ID_MONSTER_RADIUS);
			if(m_MonstersRadius != 0)
			{
				s32 value = go->m_Radius;
				swscanf(m_MonstersRadius->getText(),L"%d",&value);
				go->m_Radius = value;
			}

			//Get in the Mood
			if(edGui->m_Monsters_ComboBox_Mood != 0)
			{
				s32 value = edGui->m_Monsters_ComboBox_Mood->getSelected();
				go->m_Mood = (eMood) value;
			}

			for(u32 i=0; i<go->m_ListOfSkills.size(); i++)
			{
				IGUIEditBox* min = (IGUIEditBox*)edGui->m_wnd_MonsterProperties->getElementFromId(GUI_ID_MONSTER_SKILL+i*10+1);
				IGUIEditBox* max = (IGUIEditBox*)edGui->m_wnd_MonsterProperties->getElementFromId(GUI_ID_MONSTER_SKILL+i*10+2);
				s32 _min = 0;
				s32 _max = 0;
				swscanf(min->getText(),L"%d",&_min);
				swscanf(max->getText(),L"%d",&_max);
				go->m_ListOfSkills[i].min = _min;
				go->m_ListOfSkills[i].max = _max;
			}

			//TODO: Store the sounds!

			///Now write to xml
			IXMLWriter* writer = device->getFileSystem()->createXMLWriter(go->path + go->root + stringw(".xml"));
			writer->writeXMLHeader();
			writer->writeLineBreak();writer->writeLineBreak();
			writer->writeComment(L"Game Object Configuration File");
			writer->writeLineBreak();writer->writeLineBreak();

			writer->writeElement(L"Config", false);
			writer->writeLineBreak();

			//WriteAttributes
			writer->writeElement(L"Name", true, L"value", go->name.c_str());
			writer->writeLineBreak();
			writer->writeElement(L"Model", true, L"value", go->mesh.c_str());
			writer->writeLineBreak();
			writer->writeElement(L"Monster", true, L"value", go->isMonster?L"true":L"false");
			writer->writeLineBreak();
			writer->writeElement(L"Description", true, L"value", go->description.c_str());
			writer->writeLineBreak();
			writer->writeElement(L"Radius", true, L"value", stringw(go->m_Radius).c_str());
			writer->writeLineBreak();
			writer->writeElement(L"Mood", true, L"value", stringw((s32)go->m_Mood).c_str());
			writer->writeLineBreak();
			writer->writeLineBreak();

			list<TAnimationRange>::Iterator it = go->animations.begin();
		
			for (; it != go->animations.end(); ++it)
			{
				writer->writeElement(L"Animation", true, L"name", (*it).animName.c_str(), L"start", stringw((*it).startFrame).c_str(), L"end", stringw((*it).endFrame).c_str());
				writer->writeLineBreak();
			}

			writer->writeLineBreak();

			writer->writeElement(L"Ability", false);
			writer->writeLineBreak();
			for (u32 i =0; i<go->m_ListOfAbilities.size(); i++)
			{
				writer->writeElement(go->m_ListOfAbilities[i].abilityName.c_str(), true, L"value", stringw(go->m_ListOfAbilities[i].value).c_str(), L"min", stringw(go->m_ListOfAbilities[i].min).c_str(), L"max", stringw(go->m_ListOfAbilities[i].max).c_str());
				writer->writeLineBreak();
			}
			writer->writeClosingTag(L"Ability");
			writer->writeLineBreak();
			writer->writeLineBreak();

			writer->writeElement(L"Skills", false);
			writer->writeLineBreak();
			for (u32 i =0; i<go->m_ListOfSkills.size(); i++)
			{
				writer->writeElement(go->m_ListOfSkills[i].skillName.c_str(), true, L"min", stringw(go->m_ListOfSkills[i].min).c_str(), L"max", stringw(go->m_ListOfSkills[i].max).c_str());
				writer->writeLineBreak();
			}
			writer->writeClosingTag(L"Skills");
			writer->writeLineBreak();
			writer->writeLineBreak();

			writer->writeElement(L"Sounds", false);
			writer->writeLineBreak();
			writer->writeElement(L"Sound_Ambient",true, L"filename",go->m_Sound_Ambient.c_str(), L"loop", L"true");
			writer->writeLineBreak();
			writer->writeElement(L"Sound_Attack",true, L"filename",go->m_Sound_Attack.c_str());
			writer->writeLineBreak();
			writer->writeElement(L"Sound_Hit",true, L"filename",go->m_Sound_Hit.c_str());
			writer->writeLineBreak();
			writer->writeElement(L"Sound_Hello",true, L"filename",go->m_Sound_Hello.c_str());
			writer->writeLineBreak();
			writer->writeElement(L"Sound_Wound",true, L"filename",go->m_Sound_Wound.c_str());
			writer->writeLineBreak();
			writer->writeElement(L"Sound_Die",true, L"filename",go->m_Sound_Die.c_str());
			writer->writeLineBreak();
			writer->writeClosingTag(L"Sounds");
			writer->writeLineBreak();
			
			writer->writeClosingTag(L"Config");
			writer->drop();

			edGui->m_wnd_MonsterProperties->remove();
			edGui->m_wnd_MonsterProperties = 0;
		}
		break;

		///////////////////////////
		////////  ADD SOUND  //////
		///////////////////////////

		case GUI_ID_BUTTON_LEVELMUSIC_SAVE:
		{
			editorManager->getEdiLevel()->m_LevelMusic_MusicFile = edGui->m_LevelMusic_EditBox_MusicFile->getText();
			editorManager->getEdiLevel()->m_LevelMusic_SoundFile = edGui->m_LevelMusic_EditBox_SoundFile->getText();
			editorManager->getEdiLevel()->m_LevelMusic_NarationFile = edGui->m_LevelMusic_EditBox_NarationFile->getText();
			
			edGui->m_wnd_LevelMusic->remove();
			edGui->m_wnd_LevelMusic = 0;

			edGui->m_bGUIFocused = false;
		}
		break;

		// Add Particles
		case GUI_ID_BUTTON_PARTICLESYSTEM_INSERT:
		{
			//Parse particles parameters and call EDLevel function to insert particles to map.
			TEEmiterType emiterType = E_EMITERTYPE_BOX;
			vector3df direction = vector3df(0.0f,0.01f,0.0f);
			stringc texture = "media/particle1.bmp";
			stringc name = "particles";
			stringc type = "Teleport";

			s32 emitRateMin = 80;
			s32 emitRateMax = 100;
			f32 emiterSizeXmin = -20;
			f32 emiterSizeXmax = 20;
			f32 emiterSizeYmin = -2;
			f32 emiterSizeYmax = 20;
			f32 emiterSizeZmin = -20;
			f32 emiterSizeZmax = 20;
			f32 directionX = 0.0f;
			f32 directionY = 0.01f;
			f32 directionZ = 0.0f;
			f32 angle = 0.0f;

			//name and texture
			name = edGui->m_LevelParticles_EditBox_Name->getText();
			texture = edGui->m_LevelParticles_EditBox_TextureFile->getText();

			//emiter type
			emiterType = (TEEmiterType) edGui->m_LevelParticles_ComboBox_Emiter->getSelected();

			//emiterSize
			swscanf_s(edGui->m_LevelParticles_EditBox_EmiterSizeXMin->getText(),L"%f",&emiterSizeXmin);
			swscanf_s(edGui->m_LevelParticles_EditBox_EmiterSizeYMin->getText(),L"%f",&emiterSizeYmin);
			swscanf_s(edGui->m_LevelParticles_EditBox_EmiterSizeZMin->getText(),L"%f",&emiterSizeZmin);
			swscanf_s(edGui->m_LevelParticles_EditBox_EmiterSizeXMax->getText(),L"%f",&emiterSizeXmax);
			swscanf_s(edGui->m_LevelParticles_EditBox_EmiterSizeYMax->getText(),L"%f",&emiterSizeYmax);
			swscanf_s(edGui->m_LevelParticles_EditBox_EmiterSizeZMax->getText(),L"%f",&emiterSizeZmax);
			aabbox3df emiterSize = aabbox3d<f32>(emiterSizeXmin, emiterSizeYmin, emiterSizeZmin, emiterSizeXmax, emiterSizeYmax, emiterSizeZmax);

			//emit rate min and max
			swscanf(edGui->m_LevelParticles_EditBox_EmitRateMin->getText(),L"%d",&emitRateMin);
			swscanf(edGui->m_LevelParticles_EditBox_EmitRateMax->getText(),L"%d",&emitRateMax);

			//direction
			swscanf_s(edGui->m_LevelParticles_EditBox_DirectionX->getText(), L"%f",&directionX);
			swscanf_s(edGui->m_LevelParticles_EditBox_DirectionY->getText(), L"%f",&directionY);
			swscanf_s(edGui->m_LevelParticles_EditBox_DirectionZ->getText(), L"%f",&directionZ);
			direction = vector3df(directionX,directionY,directionZ);

			//angle
			swscanf_s(edGui->m_LevelParticles_EditBox_Angle->getText(), L"%f",&angle);

			//outline
			bool outline = false;

			//template
			PARTICLE_SYSTEM_TYPE templateType = (PARTICLE_SYSTEM_TYPE)edGui->m_LevelParticles_ComboBox_Template->getSelected();

			//insert particles
			editorManager->getEdiLevel()->InsertParticles(templateType, emiterType, emiterSize, direction, texture, name, emitRateMin, emitRateMax, angle, outline);
			edGui->AddNodeToSceneTree(editorManager->m_ID,name); // ?? isnt m_ID increased before this call - is it valid?
			edGui->m_wnd_ParticleSystem->remove();
			edGui->m_wnd_ParticleSystem = 0;
			edGui->m_bGUIFocused = false;
		}
		break;

		case GUI_ID_BUTTON_LEVELMUSIC_MUSIC_BROWSE:
		{
			device->getFileSystem()->changeWorkingDirectoryTo(MUSIC_DIR);
			editorManager->getGUIEnvironment()->addFileOpenDialog(L"Please select music theme for this level");
			edGui->m_bLevelMusicOpenDialog = true;
		}
		break;

		case GUI_ID_BUTTON_LEVELMUSIC_SOUND_BROWSE:
		{
			device->getFileSystem()->changeWorkingDirectoryTo(SOUND_DIR);
			editorManager->getGUIEnvironment()->addFileOpenDialog(L"Please select ambient sound file for this level");
			edGui->m_bLevelSoundOpenDialog = true;
		}
		break;

		case GUI_ID_BUTTON_LEVELMUSIC_NARATION_BROWSE:
		{
			device->getFileSystem()->changeWorkingDirectoryTo(SOUND_DIR);
			editorManager->getGUIEnvironment()->addFileOpenDialog(L"Please select naration file for this level");
			edGui->m_bLevelNarationOpenDialog = true;
		}
		break;

		///////////////////////////
		//////// ADD TERRAIN //////
		///////////////////////////

		//insert terrain 
		case GUI_ID_BUTTON_TH_INSERT:
		{
			float posX = -400;
			float posY = 0;
			float posZ = -400;
			float rotX = 0;
			float rotY = 0;
			float rotZ = 0;
			float scaleX = 10;
			float scaleY = 1;
			float scaleZ = 10;
			s32 smoothFactor = 3;
			s32 maxLOD = 4;
			E_TERRAIN_PATCH_SIZE patchSize = ETPS_17;

			//collect parameters from dialog window
			stringw filename = edGui->m_TH_EditBox_HeightmapFile->getText();
			stringw texture = edGui->m_TH_EditBox_TexFile->getText();
			stringw detail = edGui->m_TH_EditBox_DetailFile->getText();
			//pos
			swscanf(edGui->m_TH_EditBox_PosX->getText(),L"%f",&posX);
			swscanf(edGui->m_TH_EditBox_PosY->getText(),L"%f",&posY);
			swscanf(edGui->m_TH_EditBox_PosZ->getText(),L"%f",&posZ);
			//rot
			swscanf(edGui->m_TH_EditBox_RotX->getText(),L"%f",&rotX);
			swscanf(edGui->m_TH_EditBox_RotY->getText(),L"%f",&rotY);
			swscanf(edGui->m_TH_EditBox_RotZ->getText(),L"%f",&rotZ);
			//scale
			swscanf(edGui->m_TH_EditBox_ScaleX->getText(),L"%f",&scaleX);
			swscanf(edGui->m_TH_EditBox_ScaleY->getText(),L"%f",&scaleY);
			swscanf(edGui->m_TH_EditBox_ScaleZ->getText(),L"%f",&scaleZ);
			//smoth factor
			swscanf(edGui->m_TH_EditBox_SmoothFactor->getText(),L"%d",&smoothFactor);
			//max LOD
			swscanf(edGui->m_TH_EditBox_MaxLOD->getText(),L"%d",&maxLOD);
			//patchSize
			if(edGui->m_TH_ComboBox_PatchSize->getSelected() == 0)
				patchSize = ETPS_9;
			if(edGui->m_TH_ComboBox_PatchSize->getSelected() == 2)
				patchSize = ETPS_33;
			if(edGui->m_TH_ComboBox_PatchSize->getSelected() == 3)
				patchSize = ETPS_65;
			if(edGui->m_TH_ComboBox_PatchSize->getSelected() == 4)
				patchSize = ETPS_129;

			vector3df position = vector3df(posX,posY,posZ);
			vector3df rotation = vector3df(rotX,rotY,rotZ);
			vector3df scale(scaleX,scaleY,scaleZ);
			//f32 posX = m_TH_EditBox_PosX->getText();

			edGui->AddNodeToSceneTree(editorManager->m_ID, L"Heightmap Terrain");

			editorManager->getEdiLevel()->AddTerrainFromHeightmap(filename.c_str(),
											texture.c_str(), detail.c_str(),position,
											rotation, scale, maxLOD, smoothFactor, patchSize);

			edGui->m_wnd_InsertTerrainHeightmap->remove();
			edGui->m_wnd_InsertTerrainHeightmap = 0;
		}
		break;
		//
		case GUI_ID_BUTTON_TH_HEIGHTMAP_BROWSE:
		{
			device->getFileSystem()->changeWorkingDirectoryTo(TERRAIN_DIR);
			editorManager->getGUIEnvironment()->addFileOpenDialog(L"Please select heightmap to open");
			edGui->m_bHeightmapOpenDialog = true;
		}
		break;
		//
		case GUI_ID_BUTTON_TH_TEXTURE_BROWSE:
		{
			device->getFileSystem()->changeWorkingDirectoryTo(TERRAIN_DIR);
			editorManager->getGUIEnvironment()->addFileOpenDialog(L"Please select terrain texture");
			edGui->m_bTextureOpenDialog = true;
		}
		break;
		//
		case GUI_ID_BUTTON_TH_DETAIL_BROWSE:
		{
			device->getFileSystem()->changeWorkingDirectoryTo(TERRAIN_DIR);
			editorManager->getGUIEnvironment()->addFileOpenDialog(L"Please select terrain texture");
			edGui->m_bDetailOpenDialog = true;
		}
		break;
		
		////////////////////////////////////
		/////////////// POSITION ///////////
		////////////////////////////////////

		//move selected object down the X axis
		case GUI_ID_BUTTON_POSITION_X_DOWN:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->MoveSelectedElement(LE_AXIS_X,-1);
				edGui->m_bMoveModel = false;
			}
		}
		break;
		//move selected object up the X axis
		case GUI_ID_BUTTON_POSITION_X_UP:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->MoveSelectedElement(LE_AXIS_X,1);
				edGui->m_bMoveModel = false;
			}
		}
		break;
		//move selected object down the Y axis
		case GUI_ID_BUTTON_POSITION_Y_DOWN:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->MoveSelectedElement(LE_AXIS_Y,-1);
				edGui->m_bMoveModel = false;
			}
		}
		break;
		//move selected object up the Y axis
		case GUI_ID_BUTTON_POSITION_Y_UP:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->MoveSelectedElement(LE_AXIS_Y,1);
				edGui->m_bMoveModel = false;
			}
		}
		break;
		//move selected object down the Z axis
		case GUI_ID_BUTTON_POSITION_Z_DOWN:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->MoveSelectedElement(LE_AXIS_Z,-1);
				edGui->m_bMoveModel = false;
			}
		}
		break;
		//move selected object up the Z axis
		case GUI_ID_BUTTON_POSITION_Z_UP:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->MoveSelectedElement(LE_AXIS_Z,1);
				edGui->m_bMoveModel = false;
			}
		}
		break;
		//rotate selected object down the X axis
		case GUI_ID_BUTTON_ROTATION_X_DOWN:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->RotateSelectedElement(LE_AXIS_X,-1);
				edGui->m_bRotateModel = false;
			}
		}
		break;
		//rotate selected object up the X axis
		case GUI_ID_BUTTON_ROTATION_X_UP:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->RotateSelectedElement(LE_AXIS_X,1);
				edGui->m_bRotateModel = false;
			}
		}
		break;
		//rotate selected object down the Y axis
		case GUI_ID_BUTTON_ROTATION_Y_DOWN:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->RotateSelectedElement(LE_AXIS_Y,-1);
				edGui->m_bRotateModel = false;
			}
		}
		break;
		//rotate selected object up the Y axis
		case GUI_ID_BUTTON_ROTATION_Y_UP:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->RotateSelectedElement(LE_AXIS_Y,1);
				edGui->m_bRotateModel = false;
			}
		}
		break;
		//rotate selected object down the Z axis
		case GUI_ID_BUTTON_ROTATION_Z_DOWN:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->RotateSelectedElement(LE_AXIS_Z,-1);
				edGui->m_bRotateModel = false;
			}
		}
		break;
		//rotate selected object up the Z axis
		case GUI_ID_BUTTON_ROTATION_Z_UP:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				editorManager->getEdiLevel()->RotateSelectedElement(LE_AXIS_Z,1);
				edGui->m_bRotateModel = false;
			}
		}
		break;
		//set properties
		case GUI_ID_BUTTON_PROPERTY_SET:
		{
			if((!edGui->GUIWindowOpen())&&(editorManager->SingleObjectSelected()))
			{
				//set properties to selected model
				vector3df pos = vector3df(0,0,0);
				vector3df rot = vector3df(0,0,0);
				stringw name = edGui->m_PropEditBox_Name->getText();
				//pos
				swscanf(edGui->m_PropEditBox_Pos_X->getText(),L"%f",&pos.X);
				swscanf(edGui->m_PropEditBox_Pos_Y->getText(),L"%f",&pos.Y);
				swscanf(edGui->m_PropEditBox_Pos_Z->getText(),L"%f",&pos.Z);
				//rot
				swscanf(edGui->m_PropEditBox_Rot_X->getText(),L"%f",&rot.X);
				swscanf(edGui->m_PropEditBox_Rot_Y->getText(),L"%f",&rot.Y);
				swscanf(edGui->m_PropEditBox_Rot_Z->getText(),L"%f",&rot.Z);
				//set pos, rot and name (possible more in the future)
				editorManager->getEdiLevel()->SetModelProperties(pos, rot, name);
			}
		}
		break;
		default: 
			return false;
	}
	return true;
}