/** 
 * \file UndoActions.cpp
 * \brief CUndoActions class is handling Undo actions.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date September, 1 2011.
 */


#include "EditorManager.h"
#include "UndoActions.h"

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date September, 1 2011.
 */
CUndoActions::CUndoActions(CEditorManager* em)
{
	m_EditorManager = em;	
}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date September, 1 2011.
 */
CUndoActions::~CUndoActions()
{
	
}

/**
 * \brief Clears all undo actions.
 * \author Petar Bajic 
 * \date September, 1 2011.
 */
 void CUndoActions::ClearAllUndoActions()
 {
 }

/**
 * \brief Adds new undo action.
 * \author Petar Bajic 
 * \date September, 1 2011.
 */
void CUndoActions::AddUndoAction(TUndoAction action)
{
	m_ListOfUndoActions.push_back(action);
}

/**
 * \brief Undo function.
 * \author Petar Bajic 
 * \date September, 1 2011.
 */
void CUndoActions::Undo()
{
	if(m_ListOfUndoActions.size()>0)
	{
		TUndoAction action = m_ListOfUndoActions.getLast();

		switch(action.type)
		{
			case E_UNDO_ACTION_ADDED:
				{
					//remove object
				}
				break;
			case E_UNDO_ACTION_DELETED:
				{
					//recreate object
					action.go->name;
					m_EditorManager->AddGameObjectToLevel(action.go);
				}
				break;
			case E_UNDO_ACTION_MOVED:
				{
					//move back object
					action.node->setPosition(action.oldPos);
				}
				break;
		}

		m_ListOfUndoActions.erase(m_ListOfUndoActions.size()-1);
	}
}

/**
 * \brief Redo function.
 * \author Petar Bajic 
 * \date September, 1 2011.
 */
void CUndoActions::Redo()
{
}
