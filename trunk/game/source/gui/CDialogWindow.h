/** 
 * \file CDialogWindow.h
 * \brief Header file containing CDialogWindow class used for managing GUI Dialog Window.
 * CDialogWindow class creates window which displays communication with NPCs.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date September, 15 2008.
 */


#if !defined(AFX_CDialogWindowsCA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_CDialogWindowsCA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
#include "../Script.h"

using namespace irr;
using namespace core;
using namespace gui;
using namespace video;
using namespace io;

class CGameManager;

struct TreeNode
{
	stringw Text;
	stringw NPCName;
	TreeNode *Parent;
	TreeNode *Child;
	TreeNode *Sibling;
	u32 ID;
	// Display
	bool Selected;
	bool Player;
	u32 Link;
	bool isLink;
	bool isRead;
	bool Enabled;
	TScriptAction scriptAction;
};

struct SDialog
{
	stringw m_Filename;
	TreeNode* m_RootNode;
	TreeNode** m_NodeList;
	bool m_DialogRead;
};

class CDialogWindow {

private:
	SDialog** m_LevelDialogs;
	u32 m_NumDialogs;
	ITexture* m_Texture;
	IGUIElement* m_Parent;
	rect<s32> m_Rect;
	IGUIFont* m_Font;
	ITexture* m_ImageBtnRead;
	/** 
	 * m_ID is GUI ID constant, for example 10000. It means all dialog buttons, as GUI elements, will have id starting from 10000.
	 * When player clicks the button, GUI button ID is sent to dialog function, for example 10023. 
	 * We can get text node id (23) by subracting from m_ID (10000). Game Manager assigns number to m_ID at Init.
	 */
	s32 m_ID;
	//IGUIStaticText* 
	IGUIEditBox* m_Answer;
	IGUIButton** m_ListOfButtons;
	u32 m_NumberOfCurrentDialogChoices;

public:

	CDialogWindow(); //constructor
	~CDialogWindow(); //destructor
	s32 DisplayDialog(CGameManager* pGameManager, stringw filename, stringw NPCName); //show dialog window
	TreeNode* FindLink(TreeNode* node, u32 linkNodeID);
	void OnClick(CGameManager* pGameManager, u32 nodeIndex, u32 dialogIndex);
	void HideDialog();
	void OnEvent(s32 Value);
	void Init(rect<s32> Rect, IGUIElement* parent, s32 id, IGUIFont* font, ITexture* texture, SColor color);//font colors, font, position, dimension
	void AddGameDialog(IFileSystem* fs, stringw language, stringw filename);
	void SetDialogNodeEnabled(stringw filename, s32 nodeID, bool enable);
	bool SetNodeEnable(TreeNode* node, u32 nodeID, bool enabled);
	bool FindDialog(stringw dialogName, s32* dialogIndex);
	void SaveNPCDialogs(IFileSystem* fs, IGUIEnvironment* env);
	bool SaveNPCDialog(IFileSystem* fs, stringc file, TreeNode* node);
	void WriteNPCDialogNode(IXMLWriter* xml, TreeNode* node);
	void LoadNPCDialogs(IFileSystem* fs);
	bool LoadDialogFromXML(IXMLReader* xml, u32 index);
	void InitNPCDialog(stringc dialog);

	IGUIWindow* m_DlgWnd;
}; 

#endif

