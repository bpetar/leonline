/** 
 * \file Languages.h
 * \brief This header file defines Languages. 
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date January, 21 2012.
 */

#ifndef __C_LANGUAGES_H_INCLUDED__
#define __C_LANGUAGES_H_INCLUDED__


#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace io;

//language struct
typedef struct 
{
	stringc name;
	stringc value;
	u32 index;
} TLanguage;

//language string struct
typedef struct
{
	stringw value;
	u32 id;
} TLanguageString;

//id constants
enum ELanguageID
{
	E_LANG_STRING_GAME_NAME,
	E_LANG_STRING_NEW,
	E_LANG_STRING_LOAD,
	E_LANG_STRING_EXIT,
	E_LANG_STRING_FULLSCREEN,

	E_LANG_STRING_INTRO_MOVIE_TEXT1 = 10,
	E_LANG_STRING_INTRO_MOVIE_TEXT2,

	E_LANG_STRING_LEVEL_GUI = 20,
	E_LANG_STRING_LEVEL_GUI_SETTINGS,
	E_LANG_STRING_LEVEL_GUI_SAVE,
	E_LANG_STRING_LEVEL_GUI_LOAD,
	E_LANG_STRING_LEVEL_GUI_OPTIONS,
	E_LANG_STRING_LEVEL_GUI_EXIT,
	E_LANG_STRING_LEVEL_GUI_DEATH_FROM_FALL, // "You fell off the edge of the world?"
	E_LANG_STRING_LEVEL_GUI_FAIL_TITLE, // "You failed miserably"
	E_LANG_STRING_LEVEL_GUI_HEALTH_0, // "You successfully reduced your Health below 0!"
	E_LANG_STRING_LEVEL_GUI_KILLED_BY, // "You were killed by "

	E_LANG_STRING_LEVEL_CONSOLE_EXP_GAIN = 30, // " exp gained."
	E_LANG_STRING_LEVEL_CONSOLE_INIT, // "Game Initialized."
	E_LANG_STRING_LEVEL_CONSOLE_LOAD, // "Game Loaded."
	E_LANG_STRING_LEVEL_CONSOLE_SAVE, // "Game Saved."
	E_LANG_STRING_LEVEL_CONSOLE_RESTART, // "Game Restarted"
	E_LANG_STRING_LEVEL_CONSOLE_EMPTY_CLICK, // "Nothing to be had there..."
	E_LANG_STRING_LEVEL_CONSOLE_PLAYER_DIED, // "You died."
	E_LANG_STRING_LEVEL_CONSOLE_ITEM_GAIN, // "Item gained: "
	E_LANG_STRING_LEVEL_CONSOLE_ITEM_MISSUSE, // "This item can't be used like that"
	E_LANG_STRING_LEVEL_CONSOLE_ITEM_OBJECT_ERROR, // "This item can't be used with that object"

	E_LANG_STRING_LEVEL_CONSOLE_MISSED = 40, // "You missed "
	E_LANG_STRING_LEVEL_CONSOLE_CONTAINER_COMBINE_ERROR, // "Can't combine two items in that container."
	E_LANG_STRING_LEVEL_CONSOLE_ITEM_MOVED_TO_INVENTORY, // " moved to inventory."
	E_LANG_STRING_LEVEL_CONSOLE_COMBINE_ERROR, // "Can't combine these two items"
	E_LANG_STRING_LEVEL_CONSOLE_NO_ACTION, // "Unrecognized script action..."
	E_LANG_STRING_LEVEL_CONSOLE_PICKED,// " was picked up."
	E_LANG_STRING_LEVEL_CONSOLE_YOU_HIT,// "You hit "
	E_LANG_STRING_LEVEL_CONSOLE_WITH,// " with "
	E_LANG_STRING_LEVEL_CONSOLE_DAMAGE, // " damage!"
	E_LANG_STRING_LEVEL_CONSOLE_DEAD,// " is dead. "
	
	E_LANG_STRING_LEVEL_CONSOLE_HIT_YOU = 50,//" hit you with "
	E_LANG_STRING_LEVEL_CONSOLE_MISS_YOU, // " missed you!"

	E_LANG_STRING_LEVEL_GUI_MSGBOX = 60,
	E_LANG_STRING_LEVEL_GUI_MSGBOX_INVENTORY_FULL, // "Inventory Full"
	E_LANG_STRING_LEVEL_GUI_MSGBOX_INVENTORY_FULL_MSG, // "Can't add any more items to inventory!"
	E_LANG_STRING_LEVEL_GUI_MSGBOX_CONTAINER_CONTENT, // "Container Content"
	E_LANG_STRING_LEVEL_GUI_MSGBOX_CONTAINER_CONTENT_MSG, // "This is container window. \n\nTo move items to your inventory just click on them. Move mouse over item in your inventory to display info. To use items in your inventory right click on them. Items can be droped on the floor or put in other containers on the map. Some items can be used on other objects on the map."
	E_LANG_STRING_LEVEL_GUI_MSGBOX_CONVERSATION_TITLE, //" says:"

	E_LANG_STRING_LEVEL_MSGBOX = 70,
	E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING, // "Error Loading Game"
	E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING_MSG1, // "There is no saved game!"
	E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING_MSG2, // "Unknown problem reloading game from start!"
	E_LANG_STRING_LEVEL_MSGBOX_SRIPT_ERROR, // "Script Error"
	E_LANG_STRING_LEVEL_MSGBOX_SRIPT_ERROR_MSG1 // "InfoGUI action property missing! Must define both value and attribute fields."

};

/**
 * \brief Game entry point class
 *
 * \author Petar Bajic \date January, 21 2012.
 */
class CLanguages 
{
public:

	CLanguages(IFileSystem* fs);

	bool Init();
	void setFS(IFileSystem* fs);
	bool setLanguage(stringc lang);
	bool setLanguage(s32 index);
	void LoadStringTable();
	stringw getString(u32 id);

	IFileSystem* m_FS;

	TLanguage* m_Language; //chosen language
	array <TLanguage*> m_ListOfAvailableLanguages;
	array <TLanguageString> m_LanguageStringTable;
	array <TLanguageString> m_DefaultLanguageStringTable;

private:
	
};
#endif

