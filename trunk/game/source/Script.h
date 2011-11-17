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

enum SCRIPT_EVENT_TYPE
{
	//! An event of the graphical user interface.
	/** GUI events are created by the GUI environment or the GUI elements in response
	to mouse or keyboard events. When a GUI element receives an event it will either 
	process it and return true, or pass the event to its parent. If an event is not absorbed
	before it reaches the root element then it will then be passed to the user receiver. */
	SCRIPT_EVENT_NONE = 0,

	//!On container open (trapsies! :)
	SCRIPT_EVENT_ONCONTAINERCLICK,

	//!On picking pickable object
	SCRIPT_EVENT_ONPICK,

	//!On droping pickable object
	SCRIPT_EVENT_ONDROP,

	//!On activating usable object
	SCRIPT_EVENT_ONUSE,

	//!When you click usable object against another object
	SCRIPT_EVENT_ONUSEAGAINST,

	//!Trigger object is clicked
	SCRIPT_EVENT_TRIGGER_ONCLICK,

	//!Area approached
	SCRIPT_EVENT_AREA_ONAPPROACH,

	//!Area left
	SCRIPT_EVENT_AREA_ONLEAVE
};


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
	array<TAction*> actions; //actions that are executed under this condition
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
    array<TAction*> actions; //unconditional actions
	array<TCondition> conditions; //contitions with their actions
} TScriptAction;

class CGameManager;

class CScript
{
public:

	CScript();
	~CScript();
	bool Init(CGameManager* gameMngr, IrrlichtDevice* device, stringc filename);
	stringw GetActionLine(TAction action);
	void OnEvent(SCRIPT_EVENT_TYPE event, stringw script_name, s32 id);
	void ParseScriptForActions(io::IXMLReader*xml, TScriptAction* scriptAction, stringw eventName, s32 id);
	void ExecuteScriptAction(TAction* action, bool consumePickable, s32 id);
	bool CheckCondition(stringw condition, stringw value);

	//types of objects
	bool isPickable;
	bool isTrigger;
	bool isMonster;
	bool isNPC;
	bool isConsumable;

	stringw script;
	//list of available actions
	array <stringw> m_ListOfScriptActions;
	//list of available events
	array <stringw> m_ListOfScriptEvents;
	//list of script actions for selected object
	array<TScriptAction> SelectedListOfScriptActions;

	CGameManager* m_GameManager;

};

#endif;