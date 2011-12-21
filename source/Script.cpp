/** 
 * \file Script.cpp
 * \brief CScript class is the boss. The manager. It creates 3D device (irrlicht of course),
 * creates GUI, and 3D environment so called Level Editor. Calls all the initialization functions and 
 * goes to the loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "Script.h"

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

/**
 * \brief Initialization function. Loads script actions from given XML file.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CScript::Init(IrrlichtDevice* device, stringc actions_filename, stringc contitions_filename)
{
	IFileSystem* fs = device->getFileSystem();
	IXMLReader* xml = fs->createXMLReader(actions_filename.c_str());
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


	xml = fs->createXMLReader(contitions_filename.c_str());
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				if (stringw("ScriptConditions") != xml->getNodeName())
				{
					TCondition* condition = new TCondition;
					condition->name = xml->getNodeName();
					condition->value = xml->getAttributeValue(L"default");
					m_ListOfScriptConditions.push_back(condition);
				}
			}
		}
	}


	return true;
}

void CScript::Clear()
{
	m_SelectedListOfScriptActions.clear();
	isPickable = false;
	isUsable = false;
	isEquipable = false;
	isTrigger = false;
	isMonster = false;
	isNPC = false;
	isConsumable = false;

	mDefaultState = "";
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

stringw CScript::GetConditionLine(TCondition condition)
{
	stringw script = "<Condition name=\"";
	script += condition.name;
	script += "\" value=\"";
	script += condition.value;
	script += "\" />\n";
	return script;
}


/**
 * \brief Add trigger script action to m_SelectedListOfScriptActions
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CScript::AddTriggerScriptActionState(stringw newState)
{
	TScriptAction scriptAction;
	scriptAction.event.name = L"OnClick";
	scriptAction.event.state = newState;
	scriptAction.event.target = L"";
	m_SelectedListOfScriptActions.push_back(scriptAction);
}

stringw CScript::GetTriggerScriptActionState(s32 index)
{
	return m_SelectedListOfScriptActions[index].event.state;
}

stringw CScript::GetPickScriptActionEvent(s32 index)
{
	return m_SelectedListOfScriptActions[index].event.name.c_str();
}

void CScript::RemoveTriggerScriptActionState(s32 selectedIndex)
{
	m_SelectedListOfScriptActions.erase(selectedIndex);
}

stringw CScript::LoadTriggerScriptFromXML(IXMLReader* xml)
{
	TScriptAction eventActions;
	bool startLoadingScriptAction = false;
	bool loadingAction = false;
	stringw default_state = "";

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				if (stringw("State") == xml->getNodeName())
				{
					default_state = xml->getAttributeValue(L"value");
				}
				else if (stringw("Script") == xml->getNodeName())
				{
					startLoadingScriptAction = true;
				}
				else if (startLoadingScriptAction)
				{
					if (!loadingAction)
					{
						eventActions.event.name = xml->getNodeName();
						eventActions.event.state = xml->getAttributeValue(L"state");
						eventActions.actions.clear();
						loadingAction = true;
					}
					else
					{
						TAction action;
						action.name = xml->getNodeName();
						action.target = xml->getAttributeValue(L"target");
						action.attribute = xml->getAttributeValue(L"attribute");
						action.value = xml->getAttributeValue(L"value");
						eventActions.actions.push_back(action);
					}
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			{
				if (stringw("Script") == xml->getNodeName())
				{
					startLoadingScriptAction = false;
				}
				else if (eventActions.event.name == xml->getNodeName())
				{
					m_SelectedListOfScriptActions.push_back(eventActions);
					loadingAction = false;
				}
			}
			break;
		}
	}

	return default_state;
}


void CScript::LoadPickScriptFromXML(IXMLReader* xml)
{
	TScriptAction scriptAction;
	bool startLoadingScriptAction = false;
	bool loadingAction = false;
	bool loadingConditionedActions = false;

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo;
				if (stringw("Usable") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if(figo == L"true")
					{
						isUsable = true;
					}
				}
				else if (stringw("Equipable") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"value");
					if(figo == L"true")
					{
						isEquipable = true;
					}
				}
				else if (stringw("Script") == xml->getNodeName())
				{
					startLoadingScriptAction = true;
				}
				else if (startLoadingScriptAction)
				{
					if (!loadingAction)
					{
						//initialize action loading
						scriptAction.event.name = xml->getNodeName();
						scriptAction.event.target = xml->getAttributeValue(L"target");
						scriptAction.actions.clear();
						scriptAction.conditions.clear();
						loadingAction = true;
					}
					else //loadingAction == true
					{
						if (stringw("Condition").equals_ignore_case(xml->getNodeName()))
						{
							//loading condition
							loadingConditionedActions = true;
							TCondition condition;
							condition.name = xml->getAttributeValue(L"name");
							condition.value = xml->getAttributeValue(L"value");
							condition.actions.clear();
							scriptAction.conditions.push_back(condition);
						}
						else
						{
							TAction action;
							action.name = xml->getNodeName();
							action.target = xml->getAttributeValue(L"target");
							action.attribute = xml->getAttributeValue(L"attribute");
							action.value = xml->getAttributeValue(L"value");
							if(loadingConditionedActions)
							{
								//conditioned action
								scriptAction.conditions.getLast().actions.push_back(action);
							}
							else
							{
								//unconditioned action
								scriptAction.actions.push_back(action);
							}
						}
					}
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			{
				if (stringw("Script").equals_ignore_case(xml->getNodeName()))
				{
					startLoadingScriptAction = false;
				}
				else if (scriptAction.event.name.equals_ignore_case(xml->getNodeName()))
				{
					//one event finished, add it to the list
					m_SelectedListOfScriptActions.push_back(scriptAction);
					loadingAction = false;
				}
				else if (stringw("Condition").equals_ignore_case(xml->getNodeName()))
				{
					loadingConditionedActions = false;
				}
			}
			break;
		}
	}
}


/**
 * \brief Writes script action to string suitable for edit box that is displaying script
 *
 * Takes int index parameter to select event from the list.
 *
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
stringw CScript::PickScriptActionToString_Index(s32 index)
{
	stringw script = "";

	if (index > -1)
	{
		if (index < (s32)m_SelectedListOfScriptActions.size())
		{
			script = "<";
			script += m_SelectedListOfScriptActions[index].event.name.c_str();
			script += " target=\"";
			script += m_SelectedListOfScriptActions[index].event.target.c_str();
			script += "\" >\n";

			//write conditioned actions
			for(u32 c = 0; c < m_SelectedListOfScriptActions[index].conditions.size(); c++)
			{
				script +="\t\t";
				script += GetConditionLine(m_SelectedListOfScriptActions[index].conditions[c]);
				for(u32 a = 0; a < m_SelectedListOfScriptActions[index].conditions[c].actions.size(); a++)
				{
					script +="\t\t\t";
					script += GetActionLine(m_SelectedListOfScriptActions[index].conditions[c].actions[a]);
				}
				script +="\t\t";
				script += "</Condition>\n";
			}

			//write unconditioned actions
			for(u32 a = 0; a < m_SelectedListOfScriptActions[index].actions.size(); a++)
			{
				script +="\t\t";
				script += GetActionLine(m_SelectedListOfScriptActions[index].actions[a]);
			}
			script += "</";
			script += m_SelectedListOfScriptActions[index].event.name.c_str();
			script += ">";
		}
	}
	return script;
}


/**
 * \brief Writes script action to string suitable for edit box that is displaying script
 *
 * Takes int index parameter to select action from the list of actions.
 * 
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
stringw CScript::TriggerScriptActionToString_Index(s32 index)
{
	stringw script = "";

	if (index > -1)
	{
		if (index < (s32)m_SelectedListOfScriptActions.size())
		{
			script = "<";
			script += m_SelectedListOfScriptActions[index].event.name.c_str();
			script += " state=\"";
			script += m_SelectedListOfScriptActions[index].event.state.c_str();
			script += "\" >\n";
			for(u32 a = 0; a < m_SelectedListOfScriptActions[index].actions.size(); a++)
			{
				script +="\t\t";
				script += GetActionLine(m_SelectedListOfScriptActions[index].actions[a]);
			}
			script += "</";
			script += m_SelectedListOfScriptActions[index].event.name.c_str();
			script += ">";
		}
	}
	return script;
}

void CScript::WriteTriggerScriptToXML(IXMLWriter* xml, stringw state)
{
	xml->writeXMLHeader(); xml->writeLineBreak();

	xml->writeElement(L"State",true,L"value",state.c_str());
	xml->writeLineBreak();
	xml->writeLineBreak();

	xml->writeElement(L"Script",false); xml->writeLineBreak();
	for (u32 index = 0; index < m_SelectedListOfScriptActions.size(); index++)
	{
		xml->writeElement(m_SelectedListOfScriptActions[index].event.name.c_str(),false,
			L"state", m_SelectedListOfScriptActions[index].event.state.c_str());
		xml->writeLineBreak();
		for ( u32 a = 0; a < m_SelectedListOfScriptActions[index].actions.size(); a++)
		{
			xml->writeElement(m_SelectedListOfScriptActions[index].actions[a].name.c_str(),true,
				L"target",m_SelectedListOfScriptActions[index].actions[a].target.c_str(),
				L"attribute",m_SelectedListOfScriptActions[index].actions[a].attribute.c_str(),
				L"value", m_SelectedListOfScriptActions[index].actions[a].value.c_str());
			xml->writeLineBreak();
		}
		xml->writeClosingTag(m_SelectedListOfScriptActions[index].event.name.c_str()); xml->writeLineBreak();
	}

	xml->writeClosingTag(L"Script"); xml->writeLineBreak();
}

void CScript::WritePickScriptToXML(IXMLWriter* xml, bool equipable, bool usable)
{
	xml->writeXMLHeader(); xml->writeLineBreak();
	if(equipable)
	{
		xml->writeElement(L"Equipable",true,L"value",L"true");
		xml->writeLineBreak();
	}
	if(usable)
	{
		xml->writeElement(L"Usable",true,L"value",L"true");
		xml->writeLineBreak();
	}
	xml->writeLineBreak();
	xml->writeElement(L"Script",false); xml->writeLineBreak();
	for (u32 index = 0; index < m_SelectedListOfScriptActions.size(); index++)
	{
		xml->writeElement(m_SelectedListOfScriptActions[index].event.name.c_str(),false,
			L"target", m_SelectedListOfScriptActions[index].event.target.c_str());
		xml->writeLineBreak();
		
		//write actions that are under condition
		for ( u32 c = 0; c < m_SelectedListOfScriptActions[index].conditions.size(); c++)
		{
			//Open condition tag
			xml->writeElement(L"Condition", false, L"name", m_SelectedListOfScriptActions[index].conditions[c].name.c_str(), L"value", m_SelectedListOfScriptActions[index].conditions[c].value.c_str());
			xml->writeLineBreak();
			for ( u32 a = 0; a < m_SelectedListOfScriptActions[index].conditions[c].actions.size(); a++)
			{
				xml->writeElement(m_SelectedListOfScriptActions[index].conditions[c].actions[a].name.c_str(),true,
					L"target",m_SelectedListOfScriptActions[index].conditions[c].actions[a].target.c_str(),
					L"attribute",m_SelectedListOfScriptActions[index].conditions[c].actions[a].attribute.c_str(),
					L"value", m_SelectedListOfScriptActions[index].conditions[c].actions[a].value.c_str());
				xml->writeLineBreak();
			}
			xml->writeClosingTag(L"Condition"); 
			xml->writeLineBreak();
		}

		//write conditionless actions
		for ( u32 a = 0; a < m_SelectedListOfScriptActions[index].actions.size(); a++)
		{
			xml->writeElement(m_SelectedListOfScriptActions[index].actions[a].name.c_str(),true,
				L"target",m_SelectedListOfScriptActions[index].actions[a].target.c_str(),
				L"attribute",m_SelectedListOfScriptActions[index].actions[a].attribute.c_str(),
				L"value", m_SelectedListOfScriptActions[index].actions[a].value.c_str());
			xml->writeLineBreak();
		}
		xml->writeClosingTag(m_SelectedListOfScriptActions[index].event.name.c_str()); xml->writeLineBreak();
	}

	xml->writeClosingTag(L"Script"); xml->writeLineBreak();
}
