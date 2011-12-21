/** 
 * \file Script.h
 * \brief CScript class is containing script actions defined in XML file.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#if !defined(AFX_SCRIPTMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_SCRIPTMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace io;

typedef struct S_Action 
{
   stringw name;
   stringw target;
   stringw attribute;
   stringw value;
}TAction;

typedef struct S_Condition
{
   stringw name;
   stringw value;
   array<TAction> actions; //actions that are executed under this condition
}TCondition;

typedef struct S_Event 
{
   stringw name;
   stringw state;
   stringw target;
}TEvent;

typedef struct S_ScriptAction
{
    TEvent event;
	array<TCondition> conditions; //more actions are available on single event
    array<TAction> actions; //more actions are available on single event
    //TCondition cond;
} TScriptAction;

class CScript
{
public:

	CScript();
	~CScript();
	//bool Init(IrrlichtDevice* device, stringc filename);
	bool Init(IrrlichtDevice* device, stringc actions_filename, stringc contitions_filename);
	void Clear();
	stringw GetActionLine(TAction action);
	stringw GetConditionLine(TCondition condition);
	stringw PickScriptActionToString_Index(s32 index);
	stringw TriggerScriptActionToString_Index(s32 index);
	void WriteTriggerScriptToXML(IXMLWriter* xml, stringw state);
	void WritePickScriptToXML(IXMLWriter* xml, bool equipable, bool usable);
	void AddTriggerScriptActionState(stringw newState);
	stringw GetTriggerScriptActionState(s32 index);
	void RemoveTriggerScriptActionState(s32 selectedIndex);
	stringw LoadTriggerScriptFromXML(IXMLReader* xml);
	void LoadPickScriptFromXML(IXMLReader* xml);
	stringw GetPickScriptActionEvent(s32 index);
	bool IsEquipable() {return isEquipable;}
	bool IsUsable() {return isUsable;}

	//types of objects
	bool isPickable;
	bool isUsable;
	bool isEquipable;
	bool isTrigger;
	bool isMonster;
	bool isNPC;
	bool isConsumable;

	stringw mDefaultState;

	stringw script;
	//list of available actions
	array <stringw> m_ListOfScriptActions;
	//list of available conditions
	array<TCondition*> m_ListOfScriptConditions;
	//list of available events
	array <stringw> m_ListOfScriptEvents;
	//list of script actions for selected object
	array<TScriptAction> m_SelectedListOfScriptActions;

};

#endif;