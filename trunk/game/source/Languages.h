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
	E_LANG_STRING_LEVEL_GUI_26,

	E_LANG_STRING_LEVEL_CONSOLE_MSG = 30,
	E_LANG_STRING_LEVEL_CONSOLE_GAME_INIT,
	E_LANG_STRING_LEVEL_CONSOLE_GAME_LOAD,
	E_LANG_STRING_LEVEL_CONSOLE_GAME_SAVE,
	E_LANG_STRING_LEVEL_CONSOLE_GAME_RESTART,
	E_LANG_STRING_LEVEL_CONSOLE_GAME_EMPTY_CLICK,
	E_LANG_STRING_LEVEL_CONSOLE_GAME_PLAYER_DIED,
	E_LANG_STRING_LEVEL_CONSOLE_GAME_ITEM_GAIN,

	E_LANG_STRING_LEVEL_CONSOLE_GUI_MSG = 40,
	E_LANG_STRING_LEVEL_CONSOLE_GUI_CONTAINER_COMBINE_ERROR, // "Can't combine two items in that container."
	E_LANG_STRING_LEVEL_CONSOLE_GUI_ITEM_MOVED_TO_INVENTORY, // " moved to inventory."
	E_LANG_STRING_LEVEL_CONSOLE_GUI_COMBINE_ERROR, // "Can't combine these two items"
	
	E_LANG_STRING_LEVEL_GUI_MSGBOX = 50,
	E_LANG_STRING_LEVEL_GUI_MSGBOX_INVENTORY_FULL, // "Inventory Full"
	E_LANG_STRING_LEVEL_GUI_MSGBOX_INVENTORY_FULL_MSG, // "Can't add any more items to inventory!"
	E_LANG_STRING_LEVEL_GUI_MSGBOX_CONTAINER_CONTENT, // "Container Content"
	E_LANG_STRING_LEVEL_GUI_MSGBOX_CONTAINER_CONTENT_MSG, // "This is container window. \n\nTo move items to your inventory just click on them. Move mouse over item in your inventory to display info. To use items in your inventory right click on them. Items can be droped on the floor or put in other containers on the map. Some items can be used on other objects on the map."

	E_LANG_STRING_LEVEL_MSGBOX = 60,
	E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING, // "Error Loading Game"
	E_LANG_STRING_LEVEL_MSGBOX_ERROR_LOADING_MSG1 // "There is no saved game!"

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

