/** 
 * \file Game.cpp
 * \brief This file is topmost Game class - CGame. 
 * Practicly doing nothing but creating other classes and running a loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "Game.h"


/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGame::CGame()
{
}

/**
 * \brief Main game loop.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGame::run()
{
	if (m_GameManager.Init())
	{
		//m_GameManager.getDevice()->setWindowCaption(L"Level Game");

		// Keep running game loop if device exists
		while(m_GameManager.getDevice()->run())
		{
			new_time = m_GameManager.getDevice()->getTimer()->getTime();
			elapsed_time = ( new_time - old_time ) / 1000.0f;
			old_time = m_GameManager.getDevice()->getTimer()->getTime(); 

			if (m_GameManager.getDevice()->isWindowActive())
			{
		  		m_GameManager.Update(elapsed_time);
			}
		}
	}
	else
	{
		//Error In Initialization. Exit program.
		while(m_GameManager.getDevice()->run())
		{
	  		m_GameManager.DisplayError();
		}
	}
	return 0;
}



