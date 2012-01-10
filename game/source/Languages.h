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

typedef struct 
{
	stringc name;
	stringc value;
} TLanguage;

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

	TLanguage* m_Language; //chosen language
	array <TLanguage*> m_ListOfAvailableLanguages;

private:
	
};
#endif

