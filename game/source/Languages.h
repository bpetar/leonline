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
	E_LANG_STRING_FULLSCREEN
};

/**
 * \brief Game entry point class
 *
 * \author Petar Bajic \date January, 21 2012.
 */
class CLanguages 
{
public:

	CLanguages();

	bool Init(IFileSystem* fs, stringc filename);
	bool setLanguage(stringc lang);
	stringw getString(u32 id);

	IFileSystem* m_FS;

	TLanguage* m_Language; //chosen language
	array <TLanguage*> m_ListOfAvailableLanguages;
	array <TLanguageString> m_LanguageStringTable;
	array <TLanguageString> m_DefaultLanguageStringTable;

private:
	
};
#endif

