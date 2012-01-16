/** 
 * \file GameGUI.cpp
 * \brief GUI class creates menus and property windows and buttons and handles all the user clicks.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "GameGUI.h"
#include "../level/LevelManager.h"
#include "../GameManager.h"

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGameGUI::CGameGUI()
{
	m_GameManager = NULL;
	m_pHoverOverMenuItem = false;
	m_hoveredSlot = -1;
	m_wnd_containerContent = NULL;
	m_wnd_options = 0;
	m_wnd_skills = 0;
	m_wnd_death = 0;
	m_wnd_Msg = 0;
	m_wnd_charSheet = 0;
	m_wnd_pickInfo = 0;
	m_NumberOfSkills = 0;
	m_ContainerID = -1;
	m_ContainerTakeAllButton = NULL;
	m_bDraggingPickableItem = false;
	m_bFirstTimeContainerClick = true;
	m_DialogWindow = 0;
	m_ActiveDialogIndex = -1;
	m_EditBox_Skills_Min = 0;
	m_Inventory = 0;
}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGameGUI::~CGameGUI()
{
}

/**
 * \brief Event handler - handles user clicks (mouse and keyboard).
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGameGUI::OnEvent(const SEvent& event)
{
	if(!m_GameManager)
		return false;

	IGUIEnvironment* env = m_GameManager->getGUIEnvironment();

	if(event.EventType == EET_KEY_INPUT_EVENT)
	{
		if(event.KeyInput.PressedDown && (event.KeyInput.Key == KEY_ESCAPE))
		{
			if(m_wnd_options)
			{
				//close options window if opened
				m_wnd_options->remove();
				m_wnd_options = 0;
			}
		}
	}

	else if(event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);

		//This is code for silly item moving from inventory to container and back (on click)
		if ((event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) || (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP))
		{
			//if player clicked on settings button return, gui event will follow.. Excluding container window, he has to react on click...
			if (m_wnd_Msg || m_DialogWindow->m_DlgWnd || m_wnd_skills || m_wnd_options || m_wnd_death ||
				m_SettingsButton->getAbsoluteClippingRect().isPointInside(p) ||
				m_LevelUpButton->getAbsoluteClippingRect().isPointInside(p) ||
				m_CSheetButton->getAbsoluteClippingRect().isPointInside(p) ||
				m_Console->getAbsoluteClippingRect().isPointInside(p))
			{

				if(m_wnd_options)
				{
					//click outside of options window closes the window
					if(!m_wnd_options->getAbsoluteClippingRect().isPointInside(p) && !m_SettingsButton->getAbsoluteClippingRect().isPointInside(p))
					{
						m_wnd_options->remove();
						m_wnd_options = 0;
						return true;
					}
				}

				return true; //makes sure others dont handle this click, like player move
			}
			if (m_wnd_containerContent)
			{
				//User has opened chest!
				if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) //you'd think this is an error, but no. thing is, we handle only mouse press down clicks in item transfer between container and inventory, BUT we handle both mouse press up and down if container is open it returns true so mouse up and down events are both ignored in the scene below. :)
				{
					IGUIContainer* container = (IGUIContainer*) m_wnd_containerContent->getElementFromId(GAME_CONST_CONTAINER_ID);
					for (u32 i = 0; i < container->GetNumItems(); i++)
					{
						if (container->isInsideSlot(i,p))
						{
							if(m_bDraggingPickableItem)
							{
								CGameObject* item = container->GetItem(i);
								if(item == NULL)
								{
									container->InsertItem(m_pDraggedPickableItem, i); //add to inventory
									m_bDraggingPickableItem = false;
									m_pDraggedPickableItem = 0;
									return true;
								}
								else
								{
									//msg Cant combine two items in that container, do it in inventory
									AddConsoleText(L"Can't combine two items in that container.");
									return true;
								}
							}
							else
							{
								//Simple mouse click is handled like request to move item.
								//Advanced implementation would involve selection of item
								//Then pressing del key would remove item 
								//Dragging it with mouse away from component, etc
								CGameObject* item = container->GetItem(i);
								if(item != NULL)
								{
									if(!m_Inventory->isFull())
									{
										m_Inventory->InsertItem(item); //add to inventory
										container->RemoveItem(i);
										AddConsoleText(item->name + stringw(L" moved to inventory."));
										return true;
									}
									else
									{
										//shib msg: inventory full!
										stringw message = "Can't add any more items to inventory!";
										AddMsgBox(L"Inventory Full", message.c_str());
										return true;
									}
								}
							}
						}
					}
					//new for loop for inventory
					for (u32 i = 0; i < m_Inventory->GetNumItems(); i++)
					{
						if (m_Inventory->isInsideSlot(i,p))
						{
							if(m_bDraggingPickableItem)
							{
								CGameObject* item = container->GetItem(i);
								if(item == NULL)
								{
									m_Inventory->InsertItem(m_pDraggedPickableItem, i); //add to inventory
									m_bDraggingPickableItem = false;
									m_pDraggedPickableItem = 0;
									return true;
								}
								else
								{
									//TODO: Here combining should take place, no?
									//msg Can't combine these two items
									AddConsoleText(L"Can't combine these two items");
									return true;
								}
							}
							else
							{
								//Simple mouse click is handled like request to move item.
								//Advanced implementation would involve selection of item
								//Then pressing del key would remove item 
								//Dragging it with mouse away from component, etc
								CGameObject* item = m_Inventory->GetItem(i);
								if(item != NULL)
								{
									if(!container->isFull())
									{
										container->InsertItem(item); //add to inventory
										m_Inventory->RemoveItem(i);
										return true;
									}
									else
									{
										//shib msg: inventory full!
										stringw message = "Can't add any more items to inventory!";
										env->addMessageBox(L"Inventory Full", message.c_str());
										return true;
									}
								}
							}
						}
					}
				}
				else if(event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
				{
					if(m_bDraggingPickableItem == false)
					{
						for (u32 i = 0; i < m_Inventory->GetNumItems(); i++)
						{
							if (m_Inventory->isInsideSlot(i,p))
							{
								//Right mouse click activate item under cursor
								m_pDraggedPickableItem = m_Inventory->GetItem(i);
								if(m_pDraggedPickableItem != 0)
								{
									//m_pDraggedPickableItem->index = i;
									//activate item script
									m_GameManager->m_pScriptEngine->OnEvent(SCRIPT_EVENT_ONUSE, m_pDraggedPickableItem->script, 0);
									m_pDraggedPickableItem = 0;
								}
								return true;
							}
						}
					}
				}

				//should return true if (m_wnd_containerContent), even if doing nothing with icons, so that
				//mouse events are ignored (in rest of the program code) while Container is displayed
				return true; 
			}
			//container is not opened, check if clicked in inventory
			else if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
			{
				if(m_bDraggingPickableItem == false)
				{
					for (u32 i = 0; i < m_Inventory->GetNumItems(); i++)
					{
						if (m_Inventory->isInsideSlot(i,p))
						{
							//Simple mouse click moves item under cursor and item can be
							//moved around, dragged with mouse away from component, etc
							m_pDraggedPickableItem = m_Inventory->GetItem(i);
							if(m_pDraggedPickableItem != NULL)
							{
								//item being dragged
								m_bDraggingPickableItem = true;
								m_Inventory->RemoveItem(i);
							}
							return true;
						}
					}
				}
				else
				{
					//Item is being dragged
					for (u32 i = 0; i < m_Inventory->GetNumItems(); i++)
					{
						if (m_Inventory->isInsideSlot(i,p))
						{
							//Simple mouse click moves item under cursor and item can be
							//moved around, dragged with mouse away from component, etc
							CGameObject* item = m_Inventory->GetItem(i);
							if(item != NULL)
							{
								//item being droped on another item
								//if combinable combination
								//combine items
								//m_bItemDragged = false;
								//else
								AddConsoleText(L"Can't combine these two items");
								return true;
							}
							else
							{
								//item dropped in empty space in inventory
								m_Inventory->InsertItem(m_pDraggedPickableItem, i); //add to inventory
								m_bDraggingPickableItem = false;
								m_pDraggedPickableItem = 0;
								return true;
							}
						}
					}
				}
			}
		}
		else if(event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
		{
			//right click is ignored if some dialog is opened
			if (m_wnd_Msg || m_DialogWindow->m_DlgWnd || m_wnd_options || m_wnd_death)
			{
				return true; //makes sure others dont handle this click, like player move
			}
			if(m_bDraggingPickableItem == false)
			{
				for (u32 i = 0; i < m_Inventory->GetNumItems(); i++)
				{
					if (m_Inventory->isInsideSlot(i,p))
					{
						//Right mouse click activate item under cursor
						m_pDraggedPickableItem = m_Inventory->GetItem(i);
						if(m_pDraggedPickableItem != 0)
						{
							//m_pDraggedPickableItem->index = i;
							//activate item script
							m_GameManager->m_pScriptEngine->OnEvent(SCRIPT_EVENT_ONUSE, m_pDraggedPickableItem->script, 0);
							m_pDraggedPickableItem = 0;
						}
						return true;
					}
				}
			}
		}
		else if(event.MouseInput.Event == EMIE_MOUSE_MOVED)
		{
			bool isInInventory = false;
			for (u32 i = 0; i < m_Inventory->GetNumItems(); i++)
			{
				if (m_Inventory->isInsideSlot(i,p))
				{
					//Simple mouse move over item in inventory will popup info window
					CGameObject* go = m_Inventory->GetItem(i);
					if(go != 0)
					{
						isInInventory = true;
						
						if(m_wnd_pickInfo==0)
						{
							//display window
							DisplayPickableInfo(env,m_Inventory->GetItem(i),recti(p.X,p.Y-210,p.X+200,p.Y-10));
						}
						else
						{
							m_wnd_pickInfo->setRelativePosition(position2di(p.X,p.Y-210));
							if(m_hoveredSlot != i)
							{
								m_hoveredSlot = i;
								pickInfoName->setText(go->name.c_str());
								pickInfoDescription->setText(go->description.c_str());
								pickInfoImage->setImage(go->m_IconTexture);
							}
						}
						return true;
					}
				}
			}
			if(m_wnd_pickInfo && !isInInventory)
			{
				m_wnd_pickInfo->remove();
				m_wnd_pickInfo = 0;
				m_hoveredSlot = -1;
			}
		}

		// if gui windows are open, consume event
		if(m_wnd_options || m_wnd_death || m_wnd_charSheet || m_wnd_skills)
		{
			return true;
		}
	}

	//GUI event is special kind of event (not simple mouse or keyboard) generated by irrlicht engine
	//it simplifies event handling by telling us directly if button was clicked or window was moved 
	//or menu item was selected... etc.
	if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();

			switch(event.GUIEvent.EventType)
			{
			case EGET_MESSAGEBOX_OK:
				{
					m_wnd_Msg = 0;
				}
				break;
			case EGET_ELEMENT_HOVERED:
				{
					//if(id==GAME_CONST_INVENTORY_ID)
					//DisplayPickableInfo(env,0,recti(100,100,300,300));
					/*if((id == x))
					{
						m_bElementPicking = true;
					}*/
				}
				break;
			case EGET_ELEMENT_LEFT:
				{
					
					/*if((id > GUI_ID_TREECTRL_GAME_OBJECTS_START) && (id < GUI_ID_TREECTRL_GAME_OBJECTS_END))
					{
						m_bElementPicking = false;
						//bits redundant to load model again, but it worksaround problem with map loading (scene delete)
						CGUITreeCtrl* tree = (CGUITreeCtrl*)event.GUIEvent.Caller;
						tree->m_SavedNode = 0;
					}*/
					break;
				}
			case EGET_ELEMENT_CLOSED:
				{
					if (event.GUIEvent.Caller == m_wnd_options)
					{
						m_wnd_options = 0;
					}
					if (event.GUIEvent.Caller == m_wnd_skills)
					{
						m_wnd_skills = 0;
						m_NumberOfSkills = 0;
					}
					if (event.GUIEvent.Caller == m_wnd_Msg)
					{
						m_wnd_Msg = 0;
					}
					if (event.GUIEvent.Caller == m_wnd_death)
					{
						m_wnd_death = 0;
					}
					if (event.GUIEvent.Caller == m_wnd_charSheet)
					{
						m_wnd_charSheet = 0;
					}
					if (event.GUIEvent.Caller == m_DialogWindow->m_DlgWnd)
					{
						m_DialogWindow->m_DlgWnd = 0;
					}
					else if(event.GUIEvent.Caller == m_wnd_containerContent)
					{
						//Game object should memorize new container content on close.
						IGUIContainer* container = (IGUIContainer*) m_wnd_containerContent->getElementFromId(GAME_CONST_CONTAINER_ID);
						CGameObject* item;
						
						//remove existing container content
						m_GameManager->getLevelManager()->RemoveContainerContent(m_ContainerID);

						//assign new content (if any) to game object
						for ( u32 index = 0; index < container->GetNumItems(); index++)
						{
							item = container->GetItem(index);
							if (item != NULL)
							{
								m_GameManager->getLevelManager()->AddContainerItem(m_ContainerID,item);
							}
						}

						m_wnd_containerContent = NULL;
					}
				}
				break;

			case EGET_BUTTON_CLICKED:
				{
					//Container button Take All.
					if(m_wnd_containerContent && (m_ContainerTakeAllButton->getID() == event.GUIEvent.Caller->getID()))
					{
						//Save Container Content and close the window
						IGUIContainer* container = (IGUIContainer*) m_wnd_containerContent->getElementFromId(GAME_CONST_CONTAINER_ID);

						CGameObject* item;
						
						for ( u32 index = 0; index < container->GetNumItems(); index++)
						{
							item = container->GetItem(index);
							if (item != NULL)
							{
								if (!m_Inventory->isFull())
								{
									m_Inventory->InsertItem(item); //add to inventory
									container->RemoveItem(index); //remove from container
								}
								else
								{
									//shib msg: inventory full!
									stringw message = "Can't add any more items to inventory!";
									env->addMessageBox(L"Inventory Full", message.c_str());
									return true;
								}
							}
						}

						//remove existing container content
						m_GameManager->getLevelManager()->RemoveContainerContent(m_ContainerID);
						m_wnd_containerContent->remove();
						m_wnd_containerContent = 0;

						return true;
					}
					if(!m_wnd_charSheet && !m_wnd_skills && !m_wnd_containerContent && m_SettingsButton->getID() == event.GUIEvent.Caller->getID())
					{
						if(m_wnd_options)
						{
							//close options window if opened
							m_wnd_options->remove();
							m_wnd_options = 0;
						}
						else
						{
							//bring up settings menu!
							DisplayOptionsWindow(env);
						}
						return true;
					}
					if(!m_wnd_charSheet && !m_wnd_skills && !m_wnd_options && !m_wnd_containerContent && m_LevelUpButton->getID() == event.GUIEvent.Caller->getID())
					{
						//bring up settings menu!
						DisplaySkillsWindow(env);
						return true;
					}
					if(!m_wnd_charSheet && !m_wnd_skills && !m_wnd_options && !m_wnd_containerContent && m_CSheetButton->getID() == event.GUIEvent.Caller->getID())
					{
						//bring up char sheet!
						DisplayCharacterSheetWindow(env);
						return true;
					}
					if(m_wnd_death && (m_ExitButton->getID() == event.GUIEvent.Caller->getID()))
					{
						//exit game
						m_GameManager->getDevice()->closeDevice();
						return true;
					}
					if(m_wnd_death && (m_LoadButton->getID() == event.GUIEvent.Caller->getID()))
					{
						//restart game
						m_wnd_death->remove();
						m_wnd_death = 0;
						m_GameManager->Restart();
						return true;
					}
					if(m_wnd_options && (m_LoadButton->getID() == event.GUIEvent.Caller->getID()))
					{
						//Load game
						m_wnd_options->remove();
						m_wnd_options = 0;
						m_GameManager->ReLoadGame(false);
						return true;
					}
					if(m_wnd_options && (m_SaveButton->getID() == event.GUIEvent.Caller->getID()))
					{
						//save game
						m_wnd_options->remove();
						m_wnd_options = 0;
						m_GameManager->SaveGame();
						return true;
					}
					if(m_wnd_options && (m_ExitButton->getID() == event.GUIEvent.Caller->getID()))
					{
						//exit game
						m_GameManager->getDevice()->closeDevice();
						return true;
					}
					if(m_wnd_options && (m_OptionsButton->getID() == event.GUIEvent.Caller->getID()))
					{
						//TODO: open options pannel
						m_wnd_options->remove();
						m_wnd_options = 0;
						return true;
					}
					if(m_wnd_skills && (m_SaveButton->getID() == event.GUIEvent.Caller->getID()))
					{
						//save distributed skills
						m_wnd_skills->remove();
						m_wnd_skills = 0;
						m_NumberOfSkills = 0;
						m_LevelUpButton->setVisible(false);
					}
					if(m_wnd_skills)
					{
						for(u32 i=0; i<m_NumberOfSkills; i++)
						{
							u32 minButtonDownID = 5512+i*10;
							IGUIButton* minDown = (IGUIButton*)m_wnd_skills->getElementFromId(minButtonDownID);
							u32 minEditID = 5513+i*10;
							IGUIEditBox* minEdit = (IGUIEditBox*)m_wnd_skills->getElementFromId(minEditID);
							u32 maxButtonDownID = 5515+i*10;
							IGUIButton* maxDown = (IGUIButton*)m_wnd_skills->getElementFromId(maxButtonDownID);
							u32 maxEditID = 5516+i*10;
							IGUIEditBox* maxEdit = (IGUIEditBox*)m_wnd_skills->getElementFromId(maxEditID);
							u32 maxButtonUpID = 5517+i*10;
							u32 minButtonUpID = 5514+i*10;

							if(event.GUIEvent.Caller->getID() == minButtonDownID)
							{
								//enable all '+' buttons (just in case they are not)
								if(m_GameManager->getPC()->m_AvailableSkillPoints == 0)
								{
									for(u32 i=0; i<m_NumberOfSkills; i++)
									{
										u32 minButtonUpID = 5514+i*10;
										u32 maxButtonUpID = 5517+i*10;

										IGUIButton* min = (IGUIButton*)m_wnd_skills->getElementFromId(minButtonUpID);
										IGUIButton* max = (IGUIButton*)m_wnd_skills->getElementFromId(maxButtonUpID);
										min->setEnabled(true);
										max->setEnabled(true);
									}
								}
								//min skill down
								m_GameManager->getPC()->m_AvailableSkillPoints++;
								m_EditBox_Skills_AvailablePoints->setText(stringw(m_GameManager->getPC()->m_AvailableSkillPoints).c_str());
								m_GameManager->getPC()->m_ListOfSkills[i].min--;
								minEdit->setText(stringw(m_GameManager->getPC()->m_ListOfSkills[i].min).c_str());
								
								if(m_GameManager->getPC()->m_ListOfSkills[i].min==m_OriginalMin[i])
								{
									minDown->setEnabled(false);
								}

								return true;
							}
							if(event.GUIEvent.Caller->getID() == minButtonUpID)
							{
								if(m_GameManager->getPC()->m_ListOfSkills[i].min < m_GameManager->getPC()->m_ListOfSkills[i].max)
								{
									m_GameManager->getPC()->m_ListOfSkills[i].min++;
									minEdit->setText(stringw(m_GameManager->getPC()->m_ListOfSkills[i].min).c_str());
									m_GameManager->getPC()->m_AvailableSkillPoints--;
									m_EditBox_Skills_AvailablePoints->setText(stringw(m_GameManager->getPC()->m_AvailableSkillPoints).c_str());
									
									//corresponding minbutton should be enabled
									minDown->setEnabled(true);

									if(m_GameManager->getPC()->m_AvailableSkillPoints==0)
									{
										for(u32 i=0; i<m_NumberOfSkills; i++)
										{
											u32 minButtonUpID = 5514+i*10;
											u32 maxButtonUpID = 5517+i*10;

											IGUIButton* min = (IGUIButton*)m_wnd_skills->getElementFromId(minButtonUpID);
											IGUIButton* max = (IGUIButton*)m_wnd_skills->getElementFromId(maxButtonUpID);
											min->setEnabled(false);
											max->setEnabled(false);
										}
									}
								}
								return true;
							}

							if(event.GUIEvent.Caller->getID() == maxButtonDownID)
							{
								//enable all '+' buttons (just in case they are not)
								if(m_GameManager->getPC()->m_AvailableSkillPoints == 0)
								{
									for(u32 i=0; i<m_NumberOfSkills; i++)
									{
										u32 minButtonUpID = 5514+i*10;
										u32 maxButtonUpID = 5517+i*10;

										IGUIButton* min = (IGUIButton*)m_wnd_skills->getElementFromId(minButtonUpID);
										IGUIButton* max = (IGUIButton*)m_wnd_skills->getElementFromId(maxButtonUpID);
										min->setEnabled(true);
										max->setEnabled(true);
									}
								}
								//min skill down
								m_GameManager->getPC()->m_AvailableSkillPoints++;
								m_EditBox_Skills_AvailablePoints->setText(stringw(m_GameManager->getPC()->m_AvailableSkillPoints).c_str());
								m_GameManager->getPC()->m_ListOfSkills[i].max--;
								maxEdit->setText(stringw(m_GameManager->getPC()->m_ListOfSkills[i].max).c_str());
								
								if(m_GameManager->getPC()->m_ListOfSkills[i].max==m_OriginalMax[i])
								{
									maxDown->setEnabled(false);
								}
								
								/*if(m_GameManager->getPC()->m_AvailableSkillPoints == SKILLPOINTSPERLEVEL)
								{
									for(u32 i=0; i<m_NumberOfSkills; i++)
									{
										u32 minButtonDownID = 5514+i*10;
										u32 maxButtonDownID = 5517+i*10;

										IGUIButton* min = (IGUIButton*)m_wnd_skills->getElementFromId(minButtonDownID);
										IGUIButton* max = (IGUIButton*)m_wnd_skills->getElementFromId(maxButtonDownID);
										min->setEnabled(false);
										max->setEnabled(false);
									}
								}*/

								return true;
							}
							if(event.GUIEvent.Caller->getID() == maxButtonUpID)
							{
								m_GameManager->getPC()->m_ListOfSkills[i].max++;
								maxEdit->setText(stringw(m_GameManager->getPC()->m_ListOfSkills[i].max).c_str());
								m_GameManager->getPC()->m_AvailableSkillPoints--;
								m_EditBox_Skills_AvailablePoints->setText(stringw(m_GameManager->getPC()->m_AvailableSkillPoints).c_str());
								
								//corresponding minbutton should be enabled
								maxDown->setEnabled(true);

								if(m_GameManager->getPC()->m_AvailableSkillPoints==0)
								{
									for(u32 i=0; i<m_NumberOfSkills; i++)
									{
										u32 minUpID = 5514+i*10;
										u32 maxUpID = 5517+i*10;

										IGUIButton* min = (IGUIButton*)m_wnd_skills->getElementFromId(minUpID);
										IGUIButton* max = (IGUIButton*)m_wnd_skills->getElementFromId(maxUpID);
										min->setEnabled(false);
										max->setEnabled(false);
									}
								}
								return true;
							}
						}
					}
					if(m_DialogWindow && m_DialogWindow->m_DlgWnd)
					{
						//If dialog window is open, button click is probabbly dialogs but
						//to make sure, we check ID of pressed buttons
						s32 buttonID = event.GUIEvent.Caller->getID();
						s32 dlgWndID = m_DialogWindow->m_DlgWnd->getID();
						if((buttonID > dlgWndID) && (buttonID < dlgWndID + 100))
						{
							if(m_ActiveDialogIndex >= 0)
							{
								m_DialogWindow->OnClick(m_GameManager, event.GUIEvent.Caller->getID(), m_ActiveDialogIndex);
							}
							return true;
						}
					}
				}
				break;
			}
		}
	
	return false;
}


/**
 * \brief Event handler - handles user clicks (mouse and keyboard) while in intro movie.
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGameGUI::OnMovieEvent(const SEvent& event)
{
	if(!m_GameManager)
		return false;

	IGUIEnvironment* env = m_GameManager->getGUIEnvironment();

	if(event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			//exit movie on mouse click
			m_GameManager->ExitIntroMovie();
			m_GameManager->NewGame();
		}
	}
	else if(event.EventType == EET_KEY_INPUT_EVENT)
	{
		//any key will exit movie
		m_GameManager->ExitIntroMovie();
		m_GameManager->NewGame();
	}

	return false;
}


/**
 * \brief Event handler - handles user clicks (mouse and keyboard) in main menu state.
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGameGUI::OnMenuEvent(const SEvent& event)
{
	if(!m_GameManager)
		return false;

	IGUIEnvironment* env = m_GameManager->getGUIEnvironment();

	if(event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);

		//This is code checks if menu item is clicked
		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			if(m_MenuNew.rectangle.isPointInside(p))
			{
				//new game clicked
				m_GameManager->ExitMainMenu();
				m_GameManager->PlayIntroMovie();
			}
			else if(m_MenuLoad.rectangle.isPointInside(p))
			{
				//load game clicked
				m_GameManager->ExitMainMenu();
				m_GameManager->LoadGame(false);
			}
			else if(m_MenuExit.rectangle.isPointInside(p))
			{
				//exit game clicked
				m_GameManager->ExitGame();
			}
		}
	}
	else if (event.EventType == EET_GUI_EVENT)
	{
		s32 id = event.GUIEvent.Caller->getID();

			switch(event.GUIEvent.EventType)
			{
			case EGET_CHECKBOX_CHANGED:
				{
					if(m_fullscreenCheck->getID()==id)
					{
						//reset device to change fullscreen
						m_GameManager->m_bFullscreenPreference = m_fullscreenCheck->isChecked();
						m_GameManager->StoreDataToXMLConfig("game/game_config.xml");
						m_GameManager->RestartDevice();
						return true;
					}
				}
				break;
			case EGET_COMBO_BOX_CHANGED:
				{
					if(m_langCombo->getID()==id)
					{
						//change language
						s32 sel = m_langCombo->getSelected();

						m_GameManager->getFS()->changeWorkingDirectoryTo("media/strings");
						m_GameManager->m_pLanguages->setLanguage(sel);
						m_GameManager->backToWorkingDirectory();
						m_GameManager->StoreDataToXMLConfig("game/game_config.xml");
						m_GameName.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_GAME_NAME);
						m_MenuNew.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_NEW);
						m_MenuLoad.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_LOAD);
						m_MenuExit.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_EXIT);
						m_FullscreenText.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_FULLSCREEN);
						return true;
					}
					else if(m_resolutionCombo->getID()==id)
					{
						//change resolution
						s32 sel = m_resolutionCombo->getSelected();
						m_GameManager->m_Resolution.Width = m_GameManager->m_listOfResolutions[sel].width;
						m_GameManager->m_Resolution.Height = m_GameManager->m_listOfResolutions[sel].height;
						m_GameManager->m_Depth = m_GameManager->m_listOfResolutions[sel].depth;

						m_GameManager->StoreDataToXMLConfig("game/game_config.xml");

						if(!m_GameManager->m_bFullscreenPreference)
							m_GameManager->RestartDevice();
						else
							printf("in fullscreen mode, we force desktop resolution");

						return true;
					}
				}
			}
	}

	return false;
}

void CGameGUI::renderFloatingTexts(IGUIFont* font, float elapsedTime)
{
	for (u32 i=0; i< m_FloatingTexts.size(); i++)
	{
		m_FloatingTexts[i]->remainingTime -= elapsedTime;
		if(m_FloatingTexts[i]->remainingTime > 0)
		{
			vector2di screenPos = m_GameManager->getSceneMngr()->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(m_FloatingTexts[i]->position) + vector2di(-50,-70 + (s32)(m_FloatingTexts[i]->remainingTime*10));
			font->draw(m_FloatingTexts[i]->text, recti(screenPos.X,screenPos.Y,screenPos.X+90,screenPos.Y+30),m_FloatingTexts[i]->color);
		}
		else
		{
			//delete from m_FloatingTexts cueue
			m_FloatingTexts.erase(i);
			return; //dont draw others, just quit
		}
	}
}

void CGameGUI::DrawFloatingText(vector3df pos, stringc text, u32 timeout, SColor color)
{
	m_FloatingTexts.push_back(new SFloatingText(pos,text,(float)timeout, color));
}

void CGameGUI::AddMsgBox(stringw title, stringw message)
{
	m_wnd_Msg = m_GameManager->getGUIEnvironment()->addMessageBox(title.c_str(), message.c_str(),true);
}

void CGameGUI::AddGameDialog(stringc dialog)
{
	m_DialogWindow->AddGameDialog(m_GameManager->getFS(), dialog);
}

void CGameGUI::SaveNPCDialogs()
{
	m_DialogWindow->SaveNPCDialogs(m_GameManager->getFS(), m_GameManager->getGUIEnvironment());
}

void CGameGUI::LoadNPCDialogs()
{
	m_DialogWindow->LoadNPCDialogs(m_GameManager->getFS());
}

void CGameGUI::StartNPCDialog(stringw dlgFilename)
{
	m_ActiveDialogIndex = m_DialogWindow->DisplayDialog(m_GameManager->getDevice(),m_GameManager->getGUIEnvironment(),dlgFilename.c_str());
	if(m_ActiveDialogIndex < 0)
	{
		//Display warning: No dialog file found!
		stringw message = stringw("Dialog File: ") + dlgFilename + stringw(" is not found!");
		m_GameManager->getGUIEnvironment()->addMessageBox(L"File missing!", message.c_str());
	}
}

void CGameGUI::RemovePickableFromInventory(s32 id)
{
	for (u32 index = 0; index < m_Inventory->GetNumItems(); index++)
	{
		CGameObject* item = m_Inventory->GetItem(index);
		if((item != NULL)&&(item->id == id))
		{
			m_Inventory->RemoveItem(index);
		}
	}
}

void CGameGUI::ClearInventory()
{
	if(m_Inventory)
	{
		for (u32 index = 0; index < m_Inventory->GetNumItems(); index++)
		{
			m_Inventory->RemoveItem(index);
		}
	}
}

CGameObject* CGameGUI::getItemFromInventory(s32 index)
{
	return m_Inventory->GetItem(index);
}

void CGameGUI::AddPickableToInventory(CGameObject* item)
{
	if (!m_Inventory->isFull())
	{
		//item->m_IconTexture = m_GameManager->getGUIEnvironment()->getVideoDriver()->getTexture(stringc(iconName.c_str()).c_str());
		if (item->m_IconTexture != 0)
		{
			m_Inventory->InsertItem(item); //add to inventory
		}
		else
		{
			//msg: wrong icon texture name!
		}
	}
	else
	{
		//msg: Inventory is full
	}
}

/**
 * \brief Displays GUI Window with player abilities and skills
 * \author Petar Bajic
 * \date July, 21 2008.
 */
void CGameGUI::DisplayCharacterSheetWindow(IGUIEnvironment* env)
{
	m_wnd_charSheet = env->addWindow(core::rect<s32>(100,60,750,550), false, L"Character Sheet", 0, 6500);
	ITexture* texture = m_GameManager->getDriver()->getTexture("media/Icons/bar.png");
	IGUIStaticText* text_abilities = env->addStaticText(L"Abilities:",rect<s32>(20,34,100,60),false,false,m_wnd_charSheet);
	text_abilities->setOverrideFont(env->getBuiltInFont());
	text_abilities->setOverrideColor(SColor(255,0,140,0));
	IGUIStaticText* text_skills = env->addStaticText(L"Skills:",rect<s32>(20,250,100,270),false,false,m_wnd_charSheet);
	text_skills->setOverrideFont(env->getBuiltInFont());
	text_skills->setOverrideColor(SColor(255,0,140,0));

	//char image
	ITexture* charTex = env->getVideoDriver()->getTexture("media/Icons/char.png");
	IGUIImage* img2 = env->addImage(rect<s32>(300,50,600,450),m_wnd_charSheet);
	img2->setImage(charTex);

	u32 i;

	//m_ListOfAbilities
	for(i=0; i<m_GameManager->getPC()->m_ListOfAbilities.size(); i++)
	{
		cs_ability_bars[i] = new CGUIBar(120, 120+i*30, 220, 140+i*30, 
			m_GameManager->getPC()->m_ListOfAbilities[i].max, 
			m_GameManager->getPC()->m_ListOfAbilities[i].value, 
			texture, 
			m_GameManager->getPC()->m_ListOfAbilities[i].name.c_str());
	}

	//m_ListOfSkills
	for(i=0; i<m_GameManager->getPC()->m_ListOfSkills.size(); i++)
	{
		cs_skill_bars[i] = new CGUIBar(120, 330+i*30, 220, 350+i*30,
			m_GameManager->getPC()->m_ListOfSkills[i].max, 
			m_GameManager->getPC()->m_ListOfSkills[i].min, 
			texture, 
			m_GameManager->getPC()->m_ListOfSkills[i].name.c_str());
	}
}

/**
 * \brief Displays GUI Window with options
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGameGUI::DisplayOptionsWindow(IGUIEnvironment* env)
{
	m_wnd_options = env->addWindow(core::rect<s32>(1,m_GameManager->m_WindowHeight-275,200,m_GameManager->m_WindowHeight-85), false, m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_GUI_SETTINGS).c_str(), 0, 5500);
		env->addImage(m_GameManager->getDriver()->getTexture(WINDOW_BKG_FILE), position2d<s32>(0,0), false, m_wnd_options);
		m_SaveButton = env->addButton(core::rect<s32>(5,20,190,50), m_wnd_options, 5501, m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_GUI_SAVE).c_str());
		m_LoadButton  = env->addButton(core::rect<s32>(5,60,190,90), m_wnd_options, 5502, m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_GUI_LOAD).c_str());
		m_OptionsButton = env->addButton(core::rect<s32>(5,100,190,130), m_wnd_options, 5503, m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_GUI_OPTIONS).c_str());
		m_ExitButton = env->addButton(core::rect<s32>(5,140,190,170), m_wnd_options, 5504, m_GameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_GUI_EXIT).c_str());
		m_wnd_options->setDrawTitlebar(false);
		m_wnd_options->setDraggable(false);
}

/**
 * \brief Displays GUI Window with skills when player levels up
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGameGUI::DisplaySkillsWindow(IGUIEnvironment* env)
{
		m_wnd_skills = env->addWindow(core::rect<s32>(250,100,550,400), false, L"Level Up", 0, 5510);
		s32 yoffset = 90;

		env->addStaticText(L"Distribute skill points to your skills.",rect<s32>(20,30,280,50),false,false,m_wnd_skills,5511);
		env->addStaticText(L"Available skill points:",rect<s32>(20,60,220,80),false,false,m_wnd_skills,5511);
		m_EditBox_Skills_AvailablePoints = env->addEditBox(stringw(m_GameManager->getPC()->m_AvailableSkillPoints).c_str(),rect<s32>(220,60,250,80),false,m_wnd_skills,5520);
		env->addStaticText(L"min",rect<s32>(120,90,150,110),false,false,m_wnd_skills,5511);
		env->addStaticText(L"max",rect<s32>(220,90,250,110),false,false,m_wnd_skills,5511);
		
		m_NumberOfSkills = m_GameManager->getPC()->m_ListOfSkills.size();
		for(u32 i=0; i<m_NumberOfSkills; i++)
		{
			m_OriginalMin[i] = m_GameManager->getPC()->m_ListOfSkills[i].min;
			m_OriginalMax[i] = m_GameManager->getPC()->m_ListOfSkills[i].max;

			env->addStaticText(m_GameManager->getPC()->m_ListOfSkills[i].name.c_str(),rect<s32>(20,yoffset+30+i*30,100,yoffset+55+i*30),false,false,m_wnd_skills,5511+i*10);
			m_ButtonSkills_Min_Down = env->addButton(core::rect<s32>(100,yoffset+30+i*30,120,yoffset+50+i*30), m_wnd_skills, 5512+i*10, L"-");
			m_ButtonSkills_Min_Down->setEnabled(false);
			m_EditBox_Skills_Min = env->addEditBox(stringw(m_GameManager->getPC()->m_ListOfSkills[i].min).c_str(),rect<s32>(120,yoffset+30+i*30,150,yoffset+50+i*30),false,m_wnd_skills,5513+i*10);
			m_EditBox_Skills_Min->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);
			m_ButtonSkills_Min_Up = env->addButton(core::rect<s32>(150,yoffset+30+i*30,170,yoffset+50+i*30), m_wnd_skills, 5514+i*10, L"+");
			
			m_ButtonSkills_Max_Down = env->addButton(core::rect<s32>(200,yoffset+30+i*30,220,yoffset+50+i*30), m_wnd_skills, 5515+i*10, L"-");
			m_ButtonSkills_Max_Down->setEnabled(false);
			m_EditBox_Skills_Max = env->addEditBox(stringw(m_GameManager->getPC()->m_ListOfSkills[i].max).c_str(),rect<s32>(220,yoffset+30+i*30,250,yoffset+50+i*30),false,m_wnd_skills,5516+i*10);
			m_EditBox_Skills_Max->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);
			m_ButtonSkills_Max_Up = env->addButton(core::rect<s32>(250,yoffset+30+i*30,270,yoffset+50+i*30), m_wnd_skills, 5517+i*10, L"+");

		}
		m_SaveButton = env->addButton(core::rect<s32>(50,yoffset+180,160,yoffset+200), m_wnd_skills, 5518, L"Save");
}

/**
 * \brief Displays Death Window with options "Quit or Restart"
 * \author Petar Bajic 
 * \date January, 19 2010.
 */
void CGameGUI::DisplayDeathWindow(IGUIEnvironment* env, stringw message)
{
		m_wnd_death = env->addWindow(core::rect<s32>(250,100,650,380), false, L"You failed miserably", 0, 5500);
		env->addStaticText(message.c_str(),core::rect<s32>(70,40,400,100), false, true, m_wnd_death);
		m_LoadButton = env->addButton(core::rect<s32>(150,100,260,120), m_wnd_death, 5502, L"Load Game");
		m_ExitButton = env->addButton(core::rect<s32>(150,160,260,180), m_wnd_death, 5504, L"Exit Game");
}

/**
 * \brief Displays Pickable Tip Info Window
 * \author Petar Bajic 
 * \date June, 1 2010.
 */
void CGameGUI::DisplayPickableInfo(IGUIEnvironment* env, CGameObject* go, recti pos)
{
	m_wnd_pickInfo = env->addWindow(pos, false, L"PickInfo", 0, 5770);
	m_wnd_pickInfo->setDrawBackground(false);
	env->addImage(m_PickInfoBackgroundTexture, vector2di(0,0), true, m_wnd_pickInfo);
	pickInfoName = env->addStaticText(go->name.c_str(), recti(20,100,180,126),false,true,m_wnd_pickInfo);
	pickInfoName->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
	pickInfoDescription = env->addStaticText(go->description.c_str(), recti(20,130,180,190),false,true,m_wnd_pickInfo);
	pickInfoDescription->setOverrideFont(fontGaramond12);
	pickInfoImage = env->addImage(go->m_IconTexture, vector2di(68,30), true, m_wnd_pickInfo);
	//pickInfoDescription->setTextAlignment(EGUIA_CENTER,EGUIA_UPPERLEFT);
}

/**
 * \brief Displays GUI Window with content of container object with given id
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGameGUI::DisplayContainerContent(s32 id,	IVideoDriver* driver, IGUIEnvironment* env, CLevelManager* levelManager)
{
	m_wnd_containerContent = env->addWindow(core::rect<s32>(250,100,470,380), false, L"Container Content", 0, 4500);
	m_ContainerTakeAllButton = env->addButton(core::rect<s32>(70,240,140,260), m_wnd_containerContent, 1355, L"Take All");

	ITexture* slotTex = driver->getTexture("media/Icons/slot.png");
	IGUIContainer* container = AddGUIContainer(
		env,
		core::rect<s32>(10,25,165,180), 
		m_wnd_containerContent, 
		GAME_CONST_CONTAINER_ID,
		core::dimension2d<s32>(3,3),
		core::dimension2d<s32>(1,1),
		slotTex
		);

	s32 numberOfItems = levelManager->GetContainerNumberOfItems(id);
	for (s32 a=0; a< numberOfItems; a++)
	{
		CGameObject* item = levelManager->GetContainerItem(id,a);
		if (item->m_IconTexture == 0)
		{
			//icon file is missing!! critical error
			m_GameManager->getGUIEnvironment()->addMessageBox(L"File missing!", L"Item icon file is missing or relocated! Serious game error. Disengage! Disengage!!");
		}
		else
		{
			container->InsertItem(item);
		}
	}

	m_ContainerID = id;

	if(m_bFirstTimeContainerClick)
	{
		AddMsgBox(L"Container Content",L"This is container window. \n\nTo move items to your inventory just click on them. Move mouse over item in your inventory to display info. To use items in your inventory right click on them. Items can be droped on the floor or put in other containers on the map. Some items can be used on other objects on the map.");
		m_bFirstTimeContainerClick = false;
	}
}


/**
 * \brief Initializes all windows and gui elements...
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGameGUI::AddConsoleText(stringw text)
{
	stringw temp;
	temp = m_Console->getText();
	temp += L"\n";
	temp += text.c_str();
	m_Console->setText(temp.c_str());
}

void CGameGUI::ClearConsole()
{
	m_Console->setText(L"");
}

void CGameGUI::drawIntroMovie(float elapsedTime)
{
	m_MovieTextSlidingY -= elapsedTime*15;
	m_MovieText1.rectangle.UpperLeftCorner.Y = (s32)m_MovieTextSlidingY;
	m_MovieText1.font->draw(m_MovieText1.text,m_MovieText1.rectangle,m_MovieText1.color);

	//m_MovieText2.rectangle.UpperLeftCorner.Y = (s32)m_MovieTextSlidingY + 100;
	m_MovieText3->setRelativePosition(position2di(m_MovieText1.rectangle.UpperLeftCorner.X,(s32)m_MovieTextSlidingY + 100));
	//m_MovieText2.font->draw(m_MovieText2.text,m_MovieText2.rectangle,m_MovieText2.color, false, false, &(m_MovieText2.rectangle));
}

void CGameGUI::drawMenu(float elapsedTime)
{
	position2di markerPosition = position2di(0,0);
	position2di mousePos = m_GameManager->getDevice()->getCursorControl()->getPosition();

	m_GameName.font->draw(m_GameName.text,m_GameName.rectangle,m_GameName.color);

	if(!m_MenuNew.rectangle.isPointInside(mousePos))
	{
		markerPosition.X = m_MenuNew.rectangle.UpperLeftCorner.X-10;
		markerPosition.Y = (m_MenuNew.rectangle.LowerRightCorner.Y-m_MenuNew.rectangle.UpperLeftCorner.Y)/2;
		m_MenuNew.font->draw(m_MenuNew.text,m_MenuNew.rectangle,m_MenuNew.color);
	}
	else
	{
		m_MenuNew.font->draw(m_MenuNew.text,m_MenuNew.rectangle,SColor(255,155,155,0));
	}

	if(m_MenuLoad.rectangle.isPointInside(mousePos)&& m_GameManager->m_SavedGameAvailable)
	{
		m_MenuLoad.font->draw(m_MenuLoad.text,m_MenuLoad.rectangle,SColor(255,155,155,0));
	}
	else
	{
		m_MenuLoad.font->draw(m_MenuLoad.text,m_MenuLoad.rectangle,m_MenuLoad.color);
	}

	if(!m_MenuExit.rectangle.isPointInside(mousePos))
	{
		m_MenuExit.font->draw(m_MenuExit.text,m_MenuExit.rectangle,m_MenuExit.color);
	}
	else
	{
		m_MenuExit.font->draw(m_MenuExit.text,m_MenuExit.rectangle,SColor(255,155,155,0));
	}

	if(m_MenuNew.rectangle.isPointInside(mousePos)||m_MenuLoad.rectangle.isPointInside(mousePos)||m_MenuExit.rectangle.isPointInside(mousePos))
	{
		m_pHoverOverMenuItem = true;
	}
	else
	{
		m_pHoverOverMenuItem = false;
	}

	m_FullscreenText.font->draw(m_FullscreenText.text,m_FullscreenText.rectangle,m_FullscreenText.color);
}

/**
 * \brief Initializes all windows and gui elements...
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGameGUI::Init(CGameManager* gameMngr)
{
	m_GameManager = gameMngr;
	return true;
}

void CGameGUI::ClearIntroMovie()
{
	m_MovieText3->setVisible(false);
}

void CGameGUI::ClearMenu()
{
	//remove
	//m_langCombo->remove();
	//m_fullscreenCheck->remove();

	//set invisible
	m_langCombo->setVisible(false);
	m_fullscreenCheck->setVisible(false);
	m_resolutionCombo->setVisible(false);
}

bool CGameGUI::InitIntroMovie()
{
	u32 text_x = m_GameManager->m_WindowWidth/3;
	u32 text_x_end = m_GameManager->m_WindowWidth-20;

	m_MovieText1.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_INTRO_MOVIE_TEXT1);
	m_MovieText1.rectangle = recti(text_x,260,text_x_end,610);
	m_MovieText1.font = m_GameManager->getGUIEnvironment()->getFont("media/font/brin.xml");
	m_MovieText1.color = SColor(255,255,255,255);
	
	m_MovieText3 = m_GameManager->getGUIEnvironment()->addStaticText(m_GameManager->m_pLanguages->getString(E_LANG_STRING_INTRO_MOVIE_TEXT2).c_str(),recti(text_x,460,text_x_end,910),false,true,0,-1,false);
	m_MovieText3->setOverrideColor(SColor(255,255,255,255));
	m_MovieText3->setOverrideFont(m_GameManager->getGUIEnvironment()->getFont("media/font/brin.xml"));

	m_MovieTextSlidingY = (float)m_MovieText1.rectangle.UpperLeftCorner.Y;

	return true;
}

bool CGameGUI::InitMenu()
{
	// add irrlicht logo
	m_GameManager->getGUIEnvironment()->addImage(m_GameManager->getDriver()->getTexture(IRRLOGO_FILE), position2d<s32>(50,50));

	u32 menu_y_pos = m_GameManager->m_WindowHeight/3;

	u32 pref_y_pos = 2*m_GameManager->m_WindowHeight/3;

	//add game name
	m_GameName.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_GAME_NAME);
	m_GameName.rectangle = recti(60,menu_y_pos,350,menu_y_pos+80);
	m_GameName.font = m_GameManager->getGUIEnvironment()->getFont("media/font/brin56.xml");
	m_GameName.color = SColor(255,255,255,255);

	//add menu options
	m_MenuNew.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_NEW);
	m_MenuNew.rectangle = recti(60,menu_y_pos+80,350,menu_y_pos+130);
	m_MenuNew.font = m_GameManager->getGUIEnvironment()->getFont("media/font/brin.xml");
	m_MenuNew.color = SColor(255,255,255,255);

	m_MenuLoad.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_LOAD);
	m_MenuLoad.rectangle = recti(60,menu_y_pos+130,350,menu_y_pos+180);
	m_MenuLoad.font = m_GameManager->getGUIEnvironment()->getFont("media/font/brin.xml");
	m_MenuLoad.color = SColor(255,255,255,255);

	m_MenuExit.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_EXIT);
	m_MenuExit.rectangle = recti(60,menu_y_pos+180,350,menu_y_pos+230);
	m_MenuExit.font = m_GameManager->getGUIEnvironment()->getFont("media/font/brin.xml");
	m_MenuExit.color = SColor(255,255,255,255);

	//add preferences
	
	m_fullscreenCheck = m_GameManager->getGUIEnvironment()->addCheckBox(false,recti(60,pref_y_pos+50,100,pref_y_pos+70));
	m_fullscreenCheck->setChecked(m_GameManager->m_bFullscreenPreference);
	m_FullscreenText.text = m_GameManager->m_pLanguages->getString(E_LANG_STRING_FULLSCREEN);
	m_FullscreenText.rectangle = recti(100,pref_y_pos+45,350,pref_y_pos+65);
	m_FullscreenText.font = m_GameManager->getGUIEnvironment()->getFont("media/font/brin20.xml");
	m_FullscreenText.color = SColor(255,255,255,255);

	//resolution combo
	m_resolutionCombo = m_GameManager->getGUIEnvironment()->addComboBox(recti(60,pref_y_pos+100,260,pref_y_pos+120),0,GUI_ID_MAIN_MENU_COMBOBOX_RESOLUTION);
	for (u32 i = 0 ; i < m_GameManager->m_listOfResolutions.size() ; i++)
	{
		m_resolutionCombo->addItem(m_GameManager->m_listOfResolutions[i].text.c_str());
	}
	m_resolutionCombo->setSelected(0);
	for(u32 i=0; i<m_GameManager->m_listOfResolutions.size(); i++)
	{
		if((m_GameManager->m_listOfResolutions[i].width == m_GameManager->m_Resolution.Width)&&(m_GameManager->m_listOfResolutions[i].height == m_GameManager->m_Resolution.Height))
		{
			m_resolutionCombo->setSelected(i);
		}
	}

	//language combo
	m_langCombo = m_GameManager->getGUIEnvironment()->addComboBox(recti(60,pref_y_pos+140,260,pref_y_pos+160),0,GUI_ID_MAIN_MENU_COMBOBOX_LANGUAGE);
	for (u32 i=0; i<m_GameManager->m_pLanguages->m_ListOfAvailableLanguages.size(); i++)
	{
		m_langCombo->addItem(stringw(m_GameManager->m_pLanguages->m_ListOfAvailableLanguages[i]->name.c_str()).c_str());
	}
	m_langCombo->setSelected(m_GameManager->m_pLanguages->m_Language->index);

	return true;
}

bool CGameGUI::InitGameGUI()
{
	//init bars
	for (int i=0; i<10; i++)
	{
		cs_skill_bars[i] = 0;
		cs_ability_bars[i] = 0;
	}

	//Setting GUI transparency (alpha)
	for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = m_GameManager->getGUIEnvironment()->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(200);
		m_GameManager->getGUIEnvironment()->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}

	u32 windowBottom = m_GameManager->m_WindowHeight - 10;
	u32 windowRight = m_GameManager->m_WindowWidth - 10;

	// add gui background image
	m_GameManager->getGUIEnvironment()->addImage(m_GameManager->getDriver()->getTexture(GAME_GUI_FILE), position2d<s32>(0,windowBottom-74));
	m_GameManager->getGUIEnvironment()->addImage(m_GameManager->getDriver()->getTexture(GAME_GUI_RIGHT_FILE), position2d<s32>(windowRight-180,windowBottom-74));

	//Add Inventory GUI
	ITexture* slotTex = m_GameManager->getDriver()->getTexture("media/Icons/slot.png");
	m_Inventory = AddGUIContainer(
		m_GameManager->getGUIEnvironment(),
		core::rect<s32>(80,windowBottom-64,600,windowBottom),
		0,
		GAME_CONST_INVENTORY_ID,
		core::dimension2d<s32>(8,1),
		core::dimension2d<s32>(1,1),
		slotTex
		);



	IGUIWindow * pWnd = m_GameManager->getGUIEnvironment()->addWindow(irr::core::rect<irr::s32>(220, 240, 540,580), false, L"Text");
	pWnd->setDraggable(false);
	pWnd->setDrawTitlebar(false);
	//pWnd->setDrawBackground(false);



	//add 'console'
	//m_Console = m_GameManager->getGUIEnvironment()->addEditBox(L"", recti(610,windowBottom-64,windowRight,windowBottom), true, 0, 4519);
	m_Console = new CGUITextBox(m_GameManager->m_Font_Garamond14,L"Test Text", m_GameManager->getGUIEnvironment(), recti(0,0,200,200), pWnd, -1);
	//m_Console->setMultiLine(true);
	//m_Console->setAutoScroll(true);
	//m_Console->setWordWrap(true);
	//m_Console->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT);
		
	// Set the Scrollbar to the left of the Text.
	m_Console->setScrollbarRight(false);
	// Scroll the text per pixel, not per line.
	m_Console->setScrollModeLines(false);

	m_Console->setText(L"Game Initialized.");

	//add settings button:
	m_SettingsButton = m_GameManager->getGUIEnvironment()->addButton(rect<s32>(10,windowBottom-64,74,windowBottom),0,5600,L"");
	m_SettingsButton->setImage(m_GameManager->getDriver()->getTexture("media/Icons/gear.png"));
	m_SettingsButton->setUseAlphaChannel(true);
	m_SettingsButton->setDrawBorder(false);

	//add levelup button:
	m_LevelUpButton = m_GameManager->getGUIEnvironment()->addButton(rect<s32>(0,450,48,498),0,5700,L"");
	m_LevelUpButton->setImage(m_GameManager->getDriver()->getTexture("media/Icons/levelup.png"));
	m_LevelUpButton->setUseAlphaChannel(true);
	m_LevelUpButton->setDrawBorder(false);
	m_LevelUpButton->setVisible(false);

	//add character sheet button:
	m_CSheetButton = m_GameManager->getGUIEnvironment()->addButton(rect<s32>(10,windowBottom-64,74,windowBottom),0,6600,L"");
	m_CSheetButton->setImage(m_GameManager->getDriver()->getTexture("media/Icons/sheet.png"));
	m_CSheetButton->setUseAlphaChannel(true);
	m_CSheetButton->setDrawBorder(false);
	m_CSheetButton->setVisible(false);

	//add health bar
	ITexture* texture = 0;//m_GameManager->getDriver()->getTexture("media/Icons/bar.png");
	healthBar = new CGUIBar(40, 30, 200, 50, 20, 20, texture, L"Health");
	//m_GameManager->getGUIEnvironment()->addStaticText(L"Health",rect<s32>(10,30,40,50),false,false,0,-1,true);

	//dialog window
	IGUIFont* font = m_GameManager->getGUIEnvironment()->getBuiltInFont();
	m_DialogWindow = new CDialogWindow();
	m_DialogWindow->Init(rect<s32>(65,365,585,565),0,6800,font,0,SColor(255,155,155,0));

	m_PickInfoBackgroundTexture = m_GameManager->getDriver()->getTexture("media/Icons/pickBackground.png");

	fontGaramond12 = m_GameManager->getGUIEnvironment()->getFont("media/Garamond12.xml");

	printf("Game GUI Initialized");

	return true;
}