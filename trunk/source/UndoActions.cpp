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
					m_EditorManager->RemoveGameObjectFromMap(action.go);
				}
				break;
			case E_UNDO_ACTION_DELETED:
				{
					//recreate object
					m_EditorManager->AddGameObjectToLevel(action.go);
				}
				break;
			case E_UNDO_ACTION_MOVED:
				{
					//move back object
					ISceneNode* node = m_EditorManager->getSceneMngr()->getSceneNodeFromId(action.go->id);
					node->setPosition(action.oldValue);
					action.go->pos = action.oldValue;
					m_EditorManager->getEdiLevel()->m_SelectedGameObject = node;
					//update gui
					m_EditorManager->getGUIManager()->SetProperties(action.go);
				}
				break;
			case E_UNDO_ACTION_ROTATED:
				{
					//rotate back object
					ISceneNode* node = m_EditorManager->getSceneMngr()->getSceneNodeFromId(action.go->id);
					node->setRotation(action.oldValue);
					action.go->rot = action.oldValue;
					m_EditorManager->getEdiLevel()->m_SelectedGameObject = node;
					//update gui
					m_EditorManager->getGUIManager()->SetProperties(action.go);
				}
				break;
			case E_UNDO_ACTION_SCALED:
				{
					//scale back object
					ISceneNode* node = m_EditorManager->getSceneMngr()->getSceneNodeFromId(action.go->id);
					node->setScale(action.oldValue);
					action.go->scale = action.oldValue;
					m_EditorManager->getEdiLevel()->m_SelectedGameObject = node;
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
