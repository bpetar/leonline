/** 
 * \file Editor.h
 * \brief This header file defines main editor class: CEditor. 
 * This class is topmost entry point practicly doing nothing but creating other classes and 
 * running a loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#ifndef __C_MAIN_MENU_H_INCLUDED__
#define __C_MAIN_MENU_H_INCLUDED__


#include <irrlicht.h>
#include "EditorManager.h"

using namespace irr;

/**
 * \brief Editor entry point class
 *
 * \author Petar Bajic \date July, 21 2008.
 */
class CEditor 
{
public:

	CEditor();

	bool run();

private:
	
	CEditorManager m_EditorManager;
};
#endif

