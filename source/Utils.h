/** 
 * \file Utils.f
 * \brief Utility functions
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date June, 19 2009.
 */

#if !defined(AFX_UTILFUNCTIONS_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_UTILFUNCTIONS_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
using namespace irr;
using namespace io;
using namespace gui;
using namespace core;

#include "gui/IGUITreeCtrl.h"

class CEditorManager;

void Util_PopulateTreeNodeGameObjectsFromXML(CEditorManager* editorManager, stringc dir_filename, IGUIElement *parent, core::rect<s32> position, s32 id);
stringw Util_GetRootNameFromPath(stringw path);
stringw Util_GetRootPathFromPath(stringw path);
void PropagateEnabled(TreeNode* node, bool enabled);
vector3df Util_getVectorFromString(stringw str);
stringw GetPathFromRootName(stringw root);

#endif;