/** 
 * \file UndoActions.h
 * \brief CUndoActions class is handling Undo actions.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date September, 1 2011.
 */

#if !defined(AFX_SCRIPTMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_3)
#define AFX_SCRIPTMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_3

#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace scene;
//using namespace io;
#include "level/GameObject.h"
//#include "EditorManager.h"

class CEditorManager;

enum EUndoActionType
{
	E_UNDO_ACTION_ADDED,
	E_UNDO_ACTION_DELETED,
	E_UNDO_ACTION_MOVED
};

typedef struct S_UndoAction 
{
	EUndoActionType type;
	vector3df oldPos;
	ISceneNode* node;
	CGameObject* go;
}TUndoAction;

class CUndoActions
{
public:

	CUndoActions(CEditorManager* em);
	~CUndoActions();
	void ClearAllUndoActions();
	void AddUndoAction(TUndoAction action);
	void Undo();
	void Redo();
	//list of available undo actions
	array <TUndoAction> m_ListOfUndoActions;

	CEditorManager* m_EditorManager;
};

#endif;