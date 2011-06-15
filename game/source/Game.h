/** 
 * \file Game.h
 * \brief This header file defines main game class: CGame. 
 * This class is topmost entry point practicly doing nothing but creating other classes and 
 * running a loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#ifndef __C_MAIN_MENU_H_INCLUDED__
#define __C_MAIN_MENU_H_INCLUDED__


#include <irrlicht.h>
#include "GameManager.h"

using namespace irr;

/**
 * \brief Game entry point class
 *
 * \author Petar Bajic \date July, 21 2008.
 */
class CGame 
{
public:

	CGame();

	bool run();

private:
	
	CGameManager m_GameManager;
	u32 new_time;
	u32 old_time;
	f32 elapsed_time;
};
#endif

