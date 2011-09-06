/** 
 * \file Script.cpp
 * \brief CScript class is handling script actions.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include <irrlicht.h>
using namespace irr;
using namespace io;
#include "Script.h"
#include "GameManager.h"

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CScript::CScript()
{
	
}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CScript::~CScript()
{
}

vector3df getVectorFromString(stringw str) 
{ 
   vector3df vector; 
   swscanf(str.c_str(), L"%f %f %f", &vector.X,&vector.Y,&vector.Z ); 
   return vector; 
}

/**
 * \brief Initialization function. Loads script actions from given XML file.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CScript::Init(CGameManager* gameMngr, IrrlichtDevice* device, stringc filename)
{
	m_GameManager = gameMngr;
	IFileSystem* fs = device->getFileSystem();
	IXMLReader* xml = fs->createXMLReader(filename.c_str());
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				if (stringw("ScriptActions") != xml->getNodeName())
				{
					stringw figo = xml->getNodeName();
					m_ListOfScriptActions.push_back(figo.c_str());
				}
			}
		}
	}
	return true;
}

stringw CScript::GetActionLine(TAction action)
{
	stringw script = "<";
	script += action.name;
	script += " target=\"";
	script += action.target;
	script += "\" attribute=\"";
	script += action.attribute;
	script += "\" value=\"";
	script += action.value;
	script += "\" />\n";
	return script;
}

/*
 *
 */
void CScript::ParseScriptForActions(io::IXMLReader*xml, TScriptAction* scriptAction, stringw eventName, s32 id)
{
	bool startLoadingScriptAction = false;
	bool loadingAction = false;
	bool loadingConditions = false;
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				if (eventName.equals_ignore_case(xml->getNodeName()))
				{
					stringw name = xml->getNodeName();
					stringw target = xml->getAttributeValue(L"target");
					
					if(name.equals_ignore_case("OnUseAgainst"))
					{
						//If event is "OnUseAgainst" additional condition must be met: target id must equal the id of clicked object.
						//There can be multiple 'OnUseAgainst' events for different targets, so we check for target to return the right set of actions.
						if(target.equals_ignore_case(stringw(id)))
						{
							scriptAction->event.name = name;
							scriptAction->event.target = target;
							scriptAction->actions.clear();
							loadingAction = true;
						}
					}
					else
					{
						scriptAction->event.name = name;
						scriptAction->event.target = target;
						scriptAction->actions.clear();
						loadingAction = true;
					}
				}
				else if (loadingAction)
				{
					if (stringw("Condition").equals_ignore_case(xml->getNodeName()))
					{
						loadingConditions = true;
						TCondition condition;
						condition.name = xml->getAttributeValue(L"attribute");
						condition.value = xml->getAttributeValue(L"value");
						scriptAction->actions.getLast()->conditions.push_back(condition);
					}
					else
					{
						TAction* action = new TAction;
						action->name = xml->getNodeName();
						action->target = xml->getAttributeValue(L"target");
						action->attribute = xml->getAttributeValue(L"attribute");
						action->value = xml->getAttributeValue(L"value");
						action->conditions.clear();
						scriptAction->actions.push_back(action);
					}
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			{
				if (scriptAction->event.name == xml->getNodeName())
				{
					loadingAction = false;
				}
				if ((scriptAction->actions.size() != 0) && (scriptAction->actions.getLast()->name.equals_ignore_case(xml->getNodeName())))
				{
					loadingConditions = false;
				}
			}
			break;
		}
	}
}

/* Giant Script Switch statement. 
 * All possible game events are handled here (scripts are called if applicable).
 */
void CScript::OnEvent(SCRIPT_EVENT_TYPE event, stringw script_name, s32 id)
{
	//CGameObject* go = m_GameManager->getLevelManager()->getGameObjectFromID(id);
	CGameObject* go = m_GameManager->getLevelManager()->getGameObjectFromID(id);

	switch(event)
	{
	case SCRIPT_EVENT_NONE:
		{
		}
		break;
	case SCRIPT_EVENT_ONCONTAINERCLICK:
		{
			//player clicked on container
			m_GameManager->getGameGUI()->DisplayContainerContent(id,
				m_GameManager->getDriver(), m_GameManager->getGUIEnvironment(), m_GameManager->getLevelManager());
		}
		break;
	case SCRIPT_EVENT_AREA_ONAPPROACH:
		{
			//area approached
			//if(m_GameManager->getLevelManager()->isObjectArea(id))
			if(go->isArea)
			{
				//parse XML file for script?
				m_GameManager->getFS()->changeWorkingDirectoryTo("media/Scripts");
				stringc xml_filename = script_name.c_str();
				TScriptAction scriptAction;
				io::IXMLReader* xml = m_GameManager->getFS()->createXMLReader(xml_filename.c_str());
				ParseScriptForActions(xml,&scriptAction,"OnApproach", id);
				if (xml)
					xml->drop(); // don't forget to delete the xml reader
				
				//If script exists, execute it:
				for(u32 i=0; i<scriptAction.actions.size(); i++)
				{
					ExecuteScriptAction(scriptAction.actions[i], false, id);
				}

				//back to working dir
				m_GameManager->backToWorkingDirectory();
			}
		}
		break;
	case SCRIPT_EVENT_AREA_ONLEAVE:
		{
			//area approached
			//if(m_GameManager->getLevelManager()->isObjectArea(id))
			if(go->isArea)
			{
				//parse XML file for script?
				m_GameManager->getFS()->changeWorkingDirectoryTo("media/Scripts"); //TODO: Fix this path - it won't work if area script is in some subdirectory
				stringc xml_filename = script_name.c_str();
				TScriptAction scriptAction;
				io::IXMLReader* xml = m_GameManager->getFS()->createXMLReader(xml_filename.c_str());
				ParseScriptForActions(xml,&scriptAction,"OnLeave", id);
				if (xml)
					xml->drop(); // don't forget to delete the xml reader
				
				//If script exists, execute it:
				for(u32 i=0; i<scriptAction.actions.size(); i++)
				{
					ExecuteScriptAction(scriptAction.actions[i], false, id);
				}

				//back to working dir
				m_GameManager->backToWorkingDirectory();
			}
		}
		break;
	case SCRIPT_EVENT_ONPICK:
		{
			//FindScript
			//double check if object is pickable
			if(m_GameManager->getLevelManager()->isObjectPickable(id))
			{
				//parse XML file for script?
				m_GameManager->getFS()->changeWorkingDirectoryTo("media/Scripts/Pickables");
				stringc xml_filename = script_name.c_str();
				TScriptAction scriptAction;
				io::IXMLReader* xml = m_GameManager->getFS()->createXMLReader(xml_filename.c_str());
				ParseScriptForActions(xml,&scriptAction,"OnPick", id);
				if (xml)
					xml->drop(); // don't forget to delete the xml reader

				//or load script actions on load level?

				//hmmmm

				//well, on pick definitly is not that often called, so we can do xml
				// but 
				//on approach, on leave, on move and others caled every other frame... I think not

				//can we make dualism? in loading scripts?

				//is it going to speed up loading of level?

				//scheck in the next episode of

				//script squad chronicles

				//If script exists, execute it:
				for(u32 i=0; i<scriptAction.actions.size(); i++)
				{
					ExecuteScriptAction(scriptAction.actions[i], false, id);
				}

				//back to working dir
				m_GameManager->backToWorkingDirectory();
			}
		}
		break;
	case SCRIPT_EVENT_ONDROP:
		{
		}
		break;
	case SCRIPT_EVENT_ONUSE:
		{
			//FindScript
			//double check if object is pickable
			//if(m_GameManager->getLevelManager()->m_pLevels[m_LevelIndex]->m_ListOfGameObjects[id]->isPickable)
			{
				//parse XML file for script?
				m_GameManager->getFS()->changeWorkingDirectoryTo("media/Scripts/Pickables");
				stringc xml_filename = script_name.c_str();
				bool startLoadingScriptAction = false;
				bool loadingAction = false;
				TScriptAction scriptAction;
				io::IXMLReader* xml = m_GameManager->getFS()->createXMLReader(xml_filename.c_str());

				ParseScriptForActions(xml, &scriptAction, "OnUse", id);
				/*while(xml && xml->read())
				{
					switch(xml->getNodeType())
					{
					case io::EXN_ELEMENT:
						{
							if (stringw("OnUse") == xml->getNodeName())
							{
								scriptAction.event.name = xml->getNodeName();
								scriptAction.event.target = xml->getAttributeValue(L"target");
								scriptAction.actions.clear();
								loadingAction = true;
							}
							else if (loadingAction)
							{
								TAction action;
								action.name = xml->getNodeName();
								action.target = xml->getAttributeValue(L"target");
								action.attribute = xml->getAttributeValue(L"attribute");
								action.value = xml->getAttributeValue(L"value");
								scriptAction.actions.push_back(action);
							}
						}
						break;
					case io::EXN_ELEMENT_END:
						{
							if (scriptAction.event.name == xml->getNodeName())
							{
								loadingAction = false;
							}
						}
						break;
					}
				}*/
				if (xml)
					xml->drop(); // don't forget to delete the xml reader

				//back to working dir
				m_GameManager->backToWorkingDirectory();

				//If script exists, execute it:
				if((s32)(scriptAction.actions.size()) > 0)
				{
					for(u32 i=0; i<scriptAction.actions.size(); i++)
					{
						ExecuteScriptAction(scriptAction.actions[i], true, id);
					}
				}
				else //this item doesn't have script action for event OnUse
				{
					//msg: This item can't be used like that
					m_GameManager->getGameGUI()->AddConsoleText(L"This item can't be used like that");
				}
			}
		}
		break;
	case SCRIPT_EVENT_ONUSEAGAINST:
		{
			//parse XML file for script?
			m_GameManager->getFS()->changeWorkingDirectoryTo("media/Scripts/Pickables");
			stringc xml_filename = script_name.c_str();
			bool startLoadingScriptAction = false;
			bool loadingAction = false;
			TScriptAction scriptAction;
			io::IXMLReader* xml = m_GameManager->getFS()->createXMLReader(xml_filename.c_str());

			ParseScriptForActions(xml,&scriptAction,"OnUseAgainst", id);
			/*while(xml && xml->read())
			{
				switch(xml->getNodeType())
				{
				case io::EXN_ELEMENT:
					{
						if (stringw("OnUseAgainst") == xml->getNodeName())
						{
							scriptAction.event.name = xml->getNodeName();
							scriptAction.event.target = xml->getAttributeValue(L"target");
							scriptAction.actions.clear();
							loadingAction = true;
						}
						else if (loadingAction)
						{
							TAction action;
							action.name = xml->getNodeName();
							action.target = xml->getAttributeValue(L"target");
							action.attribute = xml->getAttributeValue(L"attribute");
							action.value = xml->getAttributeValue(L"value");
							scriptAction.actions.push_back(action);
						}
					}
					break;
				case io::EXN_ELEMENT_END:
					{
						if (scriptAction.event.name == xml->getNodeName())
						{
							loadingAction = false;
						}
					}
					break;
				}
			}*/
			if (xml)
				xml->drop(); // don't forget to delete the xml reader

			//back to working dir
			m_GameManager->backToWorkingDirectory();

			//if clicked on the specified object
			if(scriptAction.event.target == stringw(id))
			{
				//If script exists, execute it:
				for(u32 i=0; i<scriptAction.actions.size(); i++)
				{
					ExecuteScriptAction(scriptAction.actions[i], true, id);
				}
			}
			else
			{
				//msg: This item can't be used with that object
				m_GameManager->getGameGUI()->AddConsoleText(L"This item can't be used with that object");
			}
		}
		break;
	case SCRIPT_EVENT_TRIGGER_ONCLICK:
		{
			//FindScript
			//double check if object is trigger
			if(go->isTrigger)
			{
				//parse XML file for script?
				m_GameManager->getFS()->changeWorkingDirectoryTo("media/Scripts/Static");
				stringc xml_filename = m_GameManager->getLevelManager()->GetObjectScript(id).c_str();
				bool loadingAction = false;
				bool loadingConditions = false;
				TScriptAction scriptAction;
				io::IXMLReader* xml = m_GameManager->getFS()->createXMLReader(xml_filename.c_str());

				//ParseScriptForActions(xml,&scriptAction,"OnClick");
				while(xml && xml->read())
				{
					switch(xml->getNodeType())
					{
					case io::EXN_ELEMENT:
						{
							if (stringw("OnClick") == xml->getNodeName())
							{
								//loading onclick event for currnet state of the game object
								if (m_GameManager->getLevelManager()->GetObjectState(id) == stringw(xml->getAttributeValue(L"state")))
								{
									stringw state = xml->getAttributeValue(L"state");
									if(go->state.equals_ignore_case(state))
									{
										scriptAction.event.name = xml->getNodeName();
										scriptAction.event.state = state;
										scriptAction.actions.clear();
										loadingAction = true;
									}
								}
							}
							else if (loadingAction)
							{
								if (stringw("Condition").equals_ignore_case(xml->getNodeName()))
								{
									loadingConditions = true;
									TCondition condition;
									condition.name = xml->getAttributeValue(L"attribute");
									condition.value = xml->getAttributeValue(L"value");
									scriptAction.actions.getLast()->conditions.push_back(condition);
								}
								else
								{
									TAction* action = new TAction;
									action->name = xml->getNodeName();
									action->target = xml->getAttributeValue(L"target");
									action->attribute = xml->getAttributeValue(L"attribute");
									action->value = xml->getAttributeValue(L"value");
									action->conditions.clear();
									scriptAction.actions.push_back(action);
								}
							}
						}
						break;
					case io::EXN_ELEMENT_END:
						{
							if (scriptAction.event.name == xml->getNodeName())
							{
								loadingAction = false;
							}
							if ((scriptAction.actions.size() != 0) && (scriptAction.actions.getLast()->name.equals_ignore_case(xml->getNodeName())))
							{
								loadingConditions = false;
							}
						}
						break;
					}
				}
				if (xml)
					xml->drop(); // don't forget to delete the xml reader

				//back to working dir
				m_GameManager->backToWorkingDirectory();

				//If script exists, execute it:
				for(u32 i=0; i<scriptAction.actions.size(); i++)
				{
					ExecuteScriptAction(scriptAction.actions[i], false, id);
				}
			}
		}
		break;
	}
}

/* 
 * Checking if this condition is set to given value.
 */
bool CScript::CheckCondition(stringw condition, stringw value)
{
	for(u32 i=0; i < m_GameManager->m_ListOfConditions.size(); i++)
	{
		if(condition.equals_ignore_case(m_GameManager->m_ListOfConditions[i]->name))
		{
			if(value.equals_ignore_case(m_GameManager->m_ListOfConditions[i]->value))
			{
				//Condition satisified
				return true;
			}
			else
			{
				//Condition not satisfied
				return false;
			}
		}
	}

	//Condition does not exists (so its ok)
	debugPrint("Condition does not exists!");

	return true;
}

/* Another Giant Script Switch statement. 
 * All script actions are decoded and executed here.
 */
void CScript::ExecuteScriptAction(TAction* action, bool consumePickable, s32 id)
{
	//If this action has conditions that needs to be met, we check them first.
	if(action->conditions.size()>0)
	{
		for(u32 i=0; i<action->conditions.size(); i++)
		{
			if(!CheckCondition(action->conditions[i].name, action->conditions[i].value))
			{
				//Action condition not fullfiled, we skip performing action.
				debugPrint("Action is not executed because conditions are not met.");
				return;
			}
		}
	}

	//Now this is giant action switch:
	if(action->name == stringw(L"InfluenceAbility"))
	{
		if(action->target == stringw("player"))
		{
			stringc value = action->value.c_str();
			f32 fValue = 0;
			s32 intValue = 0;
			s32 hasdot = value.findFirst('.');
			bool isInteger = (hasdot == -1);
			if(isInteger)
			{
				sscanf(value.c_str(),"%d",&intValue);
			}
			else
			{
				sscanf(value.c_str(),"%f",&fValue);
			}

			m_GameManager->PCChangeAbility(action->attribute, intValue + (s32)fValue /*one of two is 0, so we can add them*/);

		}
		else if (action->target == stringw("self"))
		{
		}
		else //target is id
		{
			stringc value = action->target.c_str();
		}
	}
	else if(action->name == stringw("Info"))
	{
		if(action->value != stringw(L""))
		{
			m_GameManager->getGameGUI()->DrawFloatingText(m_GameManager->m_pPC->node->getPosition()+vector3df(0,15,0), action->value, 5, SColor(255,40,240,40));
			m_GameManager->getGameGUI()->AddConsoleText(action->value);
		}
	}
	else if(action->name == stringw("InfoGUI"))
	{
		if(action->value != stringw(L""))
		{
			m_GameManager->getGameGUI()->AddMsgBox(action->attribute, action->value);
		}
	}
	else if(action->name == stringw("InfluenceState"))
	{
		if(action->target == stringw("player"))
		{
		}
		else if (action->target == stringw("self"))
		{
			m_GameManager->getLevelManager()->SetObjectState(id,action->value);
		}
		else //target is id
		{
			u32 targetID;
			swscanf(action->target.c_str(), L"%d", &targetID);
			m_GameManager->getLevelManager()->SetObjectState(targetID,action->value);
		}
	}
	else if(action->name == stringw("Disintegrate"))
	{
		if(action->target == stringw("player"))
		{
		}
		else if (action->target == stringw("self"))
		{
			if (consumePickable)
			{
				//TODO: check this logic here
				if(m_GameManager->getGameGUI()->m_bDraggingPickableItem)
				{
					m_GameManager->getGameGUI()->m_bDraggingPickableItem = false;
					m_GameManager->getGameGUI()->m_pDraggedPickableItem = 0;
				}
				else
				{
					if(m_GameManager->getGameGUI()->m_pDraggedPickableItem != 0)
					{
						m_GameManager->getGameGUI()->RemovePickableFromInventory(m_GameManager->getGameGUI()->m_pDraggedPickableItem->id);
					}
				}
			}
		}
		else //target is id
		{
			stringc value = action->target.c_str();
		}
	}
	else if(action->name == stringw("LeadToMap"))
	{
		s32 targetID = -1;
		stringc targetIDstring = action->target;
		if(targetIDstring != stringc("none"))
		{
			targetID = atoi(targetIDstring.c_str());
		}
		m_GameManager->ChangeLevel(action->value, targetID);
	}
	else if(action->name == stringw("SetDialogNodeEnabled"))
	{
		bool enabled = false;
		if(action->value == stringw(L"true"))
		{
			enabled = true;
		}
		stringc nodeIDstr = action->target.c_str();
		m_GameManager->getGameGUI()->getDialogManager()->SetDialogNodeEnabled(action->attribute,atoi(nodeIDstr.c_str()),enabled);
	}
	else if(action->name == stringw("GiveItem"))
	{
		if(action->target == stringw("player"))
		{
			//Someone is giving item to PC, get his id, and id of item!
			stringc giverIDstr = action->attribute;
			stringc itemIDstr = action->value;
			s32 giverID = atoi(giverIDstr.c_str());
			s32 itemID = atoi(itemIDstr.c_str());

			m_GameManager->TransferPickableFromNPCToPlayer(giverID, itemID);
		}
	}
	else if(action->name == stringw("AnimateTrigger"))
	{
		//m_GameManager->getGameGUI()->AddConsoleText(L"Entering AnimateTrigger...");
		
		u32 TargetID = 0;

		if(action->target == stringw("self"))
		{
			//This action is usually called when player clicks on trigger
			//Object should be animated from keyframes given as attributes
			TargetID = id;
		}
		else
		{
			TargetID = atoi(stringc(action->target).c_str());
		}
		stringc keyframeStartStr = action->attribute;
		stringc keyframeEndStr = action->value;
		s32 keyframeStart = atoi(keyframeStartStr.c_str());
		s32 keyframeEnd = atoi(keyframeEndStr.c_str());
		s32 animationSpeed = 15;
		if(keyframeStart<0)
		{
			keyframeStart = -keyframeStart;
			animationSpeed = -animationSpeed;
		}
		m_GameManager->AnimateTrigger(TargetID, keyframeStart, keyframeEnd, animationSpeed);
	}
	else if(action->name == stringw("TranslateGameObject"))
	{
		//m_GameManager->getGameGUI()->AddConsoleText(L"Entering TranslateGameObject...");
		
		u32 TargetID = 0;

		stringc translationVectorEndPositionStr = action->attribute;
		stringc translationTimeStr = action->value;
		vector3df translationVectorEndPosition = getVectorFromString(translationVectorEndPositionStr.c_str());
		u32 translationTime = atoi(translationTimeStr.c_str());

		if(action->target.equals_ignore_case("self"))
		{
			//This action is usually called when player clicks on trigger
			//Object should be moved to coordinates given as attributes
			TargetID = id;
		}
		else if(action->target.equals_ignore_case("player"))
		{
			m_GameManager->TranslatePlayer(translationVectorEndPosition, translationTime);
		}
		else
		{
			TargetID = atoi(stringc(action->target).c_str());
		}

		m_GameManager->TranslateGameObject(TargetID, translationVectorEndPosition, translationTime);
	}
	else if(action->name == stringw("ChangeCondition"))
	{
		stringw ConditionName = action->attribute;
		stringw ConditionValue = action->value;
		m_GameManager->ChangeCondition(ConditionName, ConditionValue);
	}
	else if(action->name == stringw("PlacePickable"))
	{
		vector3df vectorPosition = getVectorFromString(action->value.c_str());
		m_GameManager->getLevelManager()->DropPickableToMap(m_GameManager->getGameGUI()->m_pDraggedPickableItem, vectorPosition);
		m_GameManager->getGameGUI()->m_pDraggedPickableItem = 0;
		m_GameManager->getGameGUI()->m_bDraggingPickableItem = false;
	}
	else
	{
		m_GameManager->getGameGUI()->AddConsoleText(L"Unrecognized script action...");
	}
}