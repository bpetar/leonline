/** 
 * \file Editor.cpp
 * \brief This file is about main editor class CEditor. 
 * This class is topmost entry point practicly doing nothing but creating other classes and 
 * running a loop.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "Editor.h"


/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CEditor::CEditor()
{
}

/**
 * \brief Main editor loop.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CEditor::run()
{
	m_EditorManager.Init();	
	//m_EditorManager.getDevice()->setWindowCaption(L"Level Editor");

	// Keep running game loop if device exists
	while(m_EditorManager.getDevice()->run())
	{	 
		if (m_EditorManager.getDevice()->isWindowActive())
		{
			if(m_EditorManager.isLoaded())
			{
		  		m_EditorManager.Update();
			}
		}
	}

	m_EditorManager.getDevice()->drop();
	return 0;
}



