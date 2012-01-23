/** 
 * \file CDialogWindow.cpp
 * \brief GUI class creates menus and property windows and buttons and handles all the user clicks.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "CDialogWindow.h"
#include "../GameManager.h"

CDialogWindow::CDialogWindow()
{
	m_Texture = 0;
	m_Parent = 0;
	m_Font = 0;
	m_ID = -1;
	m_Rect = rect<s32>(100,400,600,600);
	m_Answer = 0;
	m_NumberOfCurrentDialogChoices = 0;
	m_DlgWnd = 0;
}

CDialogWindow::~CDialogWindow()
{
}

//check consistency of dialog node actions, if action is pointing to deleted node - remove it
TreeNode* CDialogWindow::FindLink(TreeNode* node, u32 linkNodeID)
{
	TreeNode* temp = 0;

	if(node->ID == linkNodeID)
		return node;

	if (node->Child)
	{
		temp = FindLink(node->Child, linkNodeID);
	}

	//if temp is found in child node, do not step into 
	//sibilings cause it might set temp to 0 again!!
	if (!temp && node->Sibling)
	{
		temp = FindLink(node->Sibling, linkNodeID);
	}

	//fuck?
	return temp;
}

//check consistency of dialog node actions, if action is pointing to deleted node - remove it
bool CDialogWindow::SetNodeEnable(TreeNode* node, u32 nodeID, bool enabled)
{
	bool temp = false;

	if(node->ID == nodeID)
	{
		node->Enabled = enabled;
		return true;
	}

	if (node->Child)
	{
		temp = SetNodeEnable(node->Child, nodeID, enabled);
	}

	//if node is found in child node, do not step into sibilings
	if (!temp && node->Sibling)
	{
		temp = SetNodeEnable(node->Sibling, nodeID, enabled);
	}

	return temp;
}

void CDialogWindow::OnClick(CGameManager* pGameManager, u32 id, u32 dialogIndex)
{
	//id is GUI button ID, eg. 6803, to get nodeID, we subtract from m_ID wich is 6800.
	u32 nodeID = id - m_ID;
	TreeNode* node = FindLink(m_LevelDialogs[dialogIndex]->m_RootNode,nodeID);
	IGUIEnvironment* env = pGameManager->getGUIEnvironment();

	for (u32 i=0; i < m_NumberOfCurrentDialogChoices; i++)
	{
		m_ListOfButtons[i]->remove();
		m_ListOfButtons[i] = 0;
	}
	m_NumberOfCurrentDialogChoices = 0;

	//run player dialog node action
	//if(node->scriptAction.actions.size() > 0)
	//{
	for (u32 i=0; i < node->scriptAction.actions.size(); i++)
	{
		pGameManager->m_pScriptEngine->ExecuteScriptAction(node->scriptAction.actions[i],false,-1);
	}

	node->isRead = true;

	if(node->Child)
	{
		//if selected node has children, dialog continues, new text options are loaded
		TreeNode* child = node->Child;

		if(child->isLink)
		{
			TreeNode* temp = m_LevelDialogs[dialogIndex]->m_RootNode->Child;
			child = FindLink(temp, node->Link);

			//no link found??
			if(child == 0)
				return;
		}

		s32 btnHeight = m_Font->getDimension(L"Pera").Height + 4; //4 pixels offset
		s32 middle = (m_Rect.LowerRightCorner.Y - m_Rect.UpperLeftCorner.Y)/2;
		rect<s32> btnRect;
		btnRect = rect<s32>(10, 30, m_Rect.LowerRightCorner.X - m_Rect.UpperLeftCorner.X - 10, middle);
		//Ok I don't want to do this but im forced to. I create new edit box each time, cause I have problem with 
		//setting new line of text. If previous text was scroled down, new line of text is not visible or scrolled as well.
		//All the created edit boxes will hopefully all be destroyed when dialog window is closed. I don't care right now.
		//TODO: Maybe fixed in 1.6?
		m_Answer = env->addEditBox(child->Text.c_str(), btnRect, true, m_DlgWnd);
		m_Answer->setText(child->Text.c_str());
		m_Answer->setWordWrap(true);
		m_Answer->setMultiLine(true);
		m_Answer->setAutoScroll(true);
		m_Answer->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
		env->setFocus(m_Answer);

		//run NPC dialog node action
		if(child->scriptAction.actions.size() > 0)
		{
			for (u32 i=0; i < node->scriptAction.actions.size(); i++)
			{
				pGameManager->m_pScriptEngine->ExecuteScriptAction(child->scriptAction.actions[i],false,-1);
			}
		}
		
		child = child->Child;
		
		if(child == 0)
		{
			//this should not happen, but hey, we leave anyway.
			m_DlgWnd->remove();
			m_DlgWnd = 0;
			return;
		}

		while(child)
		{
			if(child->Enabled)
			{
				btnRect = rect<s32>(10, middle + m_NumberOfCurrentDialogChoices*(btnHeight+2), m_Rect.LowerRightCorner.X - m_Rect.UpperLeftCorner.X - 10, middle + btnHeight + m_NumberOfCurrentDialogChoices*(btnHeight+2));
				m_ListOfButtons[m_NumberOfCurrentDialogChoices] = env->addButton(btnRect,m_DlgWnd,m_ID+child->ID,child->Text.c_str());
				
				if(child->isRead)
				{
					m_ListOfButtons[m_NumberOfCurrentDialogChoices]->setImage(m_ImageBtnRead);
				}
				m_NumberOfCurrentDialogChoices++;
			}
			child = child->Sibling;
		}
	}
	else
	{
		//That must have been "Good bye" text - close dialog window.
		m_DlgWnd->remove();
		m_DlgWnd = 0;
	}
}

//finds dialog with a given name in array of dialogs, returns bool and stores dialog index
bool CDialogWindow::FindDialog(stringw dialogName, s32* dialogIndex)
{
	bool bDialogFound = false;

	for(u32 index = 0; index < m_NumDialogs; index++)
	{
		if(dialogName == m_LevelDialogs[index]->m_Filename)
		{
			*dialogIndex = index;
			return true;
		}
	}

	return bDialogFound;
}

bool CDialogWindow::LoadDialogFromXML(IXMLReader* xml, u32 index)
{
	//XML load
	int nodeIndex = 0;

	m_LevelDialogs[index]->m_NodeList = new TreeNode*[100];
	m_LevelDialogs[index]->m_RootNode = new TreeNode;
	m_LevelDialogs[index]->m_RootNode->Child = 0;
	m_LevelDialogs[index]->m_RootNode->ID = 0;
	m_LevelDialogs[index]->m_RootNode->Parent = 0;
	m_LevelDialogs[index]->m_RootNode->Sibling = 0;
	for(u32 i=0; i<100; i++)
	{
		m_LevelDialogs[index]->m_NodeList[i] = 0;
	}
	m_LevelDialogs[index]->m_NodeList[0] = m_LevelDialogs[index]->m_RootNode;

	u32 Parenthood[100];
	int ParenthoodDepth = 0;
	Parenthood[ParenthoodDepth] = 0;
			
	bool loadingAction = false;

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				stringw figo;
				//Insert New Node
				if (stringw(L"Node") == xml->getNodeName())
				{
					nodeIndex++;

					TreeNode* node = new TreeNode;
					node->ID = nodeIndex;
					node->Child = 0;
					node->Link = 0;
					node->isRead = false;
					node->Enabled = true;
					node->isLink = false;
					node->Parent = m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]];
					node->Sibling = NULL;
					if(!node->Parent->Child)	
					{
						// Childless parent is simple
						node->Parent->Child = node;
					}
					else
					{
						TreeNode *curnode = node->Parent->Child;
						while(curnode->Sibling)
							curnode = curnode->Sibling;
						curnode->Sibling = node;
						// Find last sibling of parent and include new sibling
					}
					ParenthoodDepth++;
					
					m_LevelDialogs[index]->m_NodeList[nodeIndex] = node;
					Parenthood[ParenthoodDepth] = nodeIndex;
				}
				else if (stringw(L"Id") == xml->getNodeName())
				{
					u32 id = xml->getAttributeValueAsInt(L"value");
					m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->ID = id;
				}
				else if (stringw(L"Speaker") == xml->getNodeName())
				{
					stringw speaker = xml->getAttributeValue(L"value");
					if (stringw(L"PLAYER") == speaker)
					{
						m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->Player = true;
					}
					else
					{
						m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->Player = false;
						m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->NPCName = speaker;
					}
				}
				else if (stringw(L"Text") == xml->getNodeName())
				{
					m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->Text = xml->getAttributeValue(L"value");
				}
				else if (stringw(L"Link") == xml->getNodeName())
				{
					m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->Link = xml->getAttributeValueAsInt(L"value");
				}
				else if (stringw(L"IsLink") == xml->getNodeName())
				{
					if(stringw(L"true") == xml->getAttributeValue(L"value"))
					{
						m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->isLink = true;
					}
					else
					{
						m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->isLink = false;
					}
				}
				else if (stringw(L"Enabled") == xml->getNodeName())
				{
					if(stringw(L"true") == xml->getAttributeValue(L"value"))
					{
						m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->Enabled = true;
					}
					else
					{
						m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->Enabled = false;
					}
				}
				else if (stringw(L"Actions") == xml->getNodeName())
				{
					m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->scriptAction.actions.clear();
					loadingAction = true;
				}
				else if (loadingAction)
				{
					TAction* action = new TAction;
					action->name = xml->getNodeName();
					action->target = xml->getAttributeValue(L"target");
					action->attribute = xml->getAttributeValue(L"attribute");
					action->value = xml->getAttributeValue(L"value");
					m_LevelDialogs[index]->m_NodeList[Parenthood[ParenthoodDepth]]->scriptAction.actions.push_back(action);
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			{
				if (stringw(L"Node") == xml->getNodeName())
				{
					//Node Closed, stepping out.
					ParenthoodDepth--;
				}
				else if (stringw(L"Actions") == xml->getNodeName())
				{
					loadingAction = false;
				}
			}
			break;
		}
	}

	if (xml)
		xml->drop(); // don't forget to delete the xml reader

	return true;
}

s32 CDialogWindow::DisplayDialog(CGameManager* pGameManager, stringw filename, stringw NPCName)
{
	s32 index = -1;

	if(FindDialog(filename,&index))
	{
		m_DlgWnd = pGameManager->getGUIEnvironment()->addWindow(m_Rect,false,0,0,m_ID);
		m_DlgWnd->setText(stringw(NPCName + pGameManager->m_pLanguages->getString(E_LANG_STRING_LEVEL_GUI_MSGBOX_CONVERSATION_TITLE)).c_str());
		//this can be moved to constructor
		m_ImageBtnRead = pGameManager->getDevice()->getVideoDriver()->getTexture("media/Icons/button_read.png");

		if(!m_LevelDialogs[index]->m_DialogRead)
		{
			m_LevelDialogs[index]->m_DialogRead = true;
			m_ListOfButtons = new IGUIButton*[100];
		}

		s32 btnHeight = m_Font->getDimension(L"Pera").Height + 4; //4 pixels offset
		s32 middle = (m_Rect.LowerRightCorner.Y - m_Rect.UpperLeftCorner.Y)/2;
		rect<s32> btnRect;

		m_NumberOfCurrentDialogChoices = 0;
		TreeNode* firstNode = m_LevelDialogs[index]->m_RootNode->Child;
		btnRect = rect<s32>(10, 30, m_Rect.LowerRightCorner.X - m_Rect.UpperLeftCorner.X - 10, middle);
		TreeNode* npcWelcome = firstNode->Child;
		TreeNode* child = 0;
		while(npcWelcome)
		{
			if(npcWelcome->Enabled)
			{
				m_Answer = pGameManager->getGUIEnvironment()->addEditBox(npcWelcome->Text.c_str(), btnRect, true, m_DlgWnd);
				m_Answer->setAutoScroll(true);
				m_Answer->setMultiLine(true);
				m_Answer->setWordWrap(true);
				m_Answer->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
				//env->addStaticText(npcWelcome->Text.c_str(),btnRect,true,true,m_DlgWnd);
				child = npcWelcome->Child;
				break;
			}
			npcWelcome = npcWelcome->Sibling;
		}
		while(child)
		{
			if(child->Enabled)
			{
				btnRect = rect<s32>(10, middle + m_NumberOfCurrentDialogChoices*(btnHeight+2), m_Rect.LowerRightCorner.X - m_Rect.UpperLeftCorner.X - 10, middle + btnHeight + m_NumberOfCurrentDialogChoices*(btnHeight+2));
				m_ListOfButtons[m_NumberOfCurrentDialogChoices] = pGameManager->getGUIEnvironment()->addButton(btnRect,m_DlgWnd,m_ID+child->ID,child->Text.c_str());
				if(child->isRead)
				{
					m_ListOfButtons[m_NumberOfCurrentDialogChoices]->setImage(m_ImageBtnRead);
				}
				m_NumberOfCurrentDialogChoices++;
			}
			child = child->Sibling;
		}
	}
	
	return index;
}

void CDialogWindow::HideDialog()
{
}

void CDialogWindow::OnEvent(s32 Value)
{
}

void CDialogWindow::SaveNPCDialogs(IFileSystem* fs, IGUIEnvironment* env)
{
	for(u32 index = 0; index < m_NumDialogs; index++)
	{
		if(m_LevelDialogs[index]->m_DialogRead)
		{
			bool isSuccessful = SaveNPCDialog(fs, stringc("save/") + m_LevelDialogs[index]->m_Filename, m_LevelDialogs[index]->m_RootNode);
			if(!isSuccessful)
			{
				stringw message = stringw("Error Saving NPC Dialog: save/") + m_LevelDialogs[index]->m_Filename;
				env->addMessageBox(L"Error", message.c_str());
			}
		}
	}

}

void CDialogWindow::LoadNPCDialogs(IFileSystem* fs)
{
	for(u32 index = 0; index < m_NumDialogs; index++)
	{
		m_LevelDialogs[index]->m_DialogRead = false;
		stringc savename = stringc("save/") + m_LevelDialogs[index]->m_Filename;
		if(fs->existFile(savename.c_str()))
		{
			TreeNode** nodeList = m_LevelDialogs[index]->m_NodeList;
			//Clean existing
			for(u32 i=0; i<100; i++)
			{
				if(nodeList[i] != 0)
				{
					delete nodeList[i];
				}
			}
			//Load from save
			IXMLReader* xml = fs->createXMLReader(savename.c_str());
			if(xml)
			{
				LoadDialogFromXML(xml, index);
			}
		}
		else
		{
			//Load from default dir
		}
	}
}

//Write dialog node attributes to XML like file (recursive)
void CDialogWindow::WriteNPCDialogNode(IXMLWriter* xml, TreeNode* node)
{
	xml->writeElement(L"Node",false); xml->writeLineBreak();
	stringw text_id = stringw(node->ID);
	if(node->Enabled)
	{
		xml->writeElement(L"Enabled",true,L"value",L"true"); xml->writeLineBreak();
	}
	else
	{
		xml->writeElement(L"Enabled",true,L"value",L"false"); xml->writeLineBreak();
	}
	xml->writeElement(L"Id",true,L"value",text_id.c_str()); xml->writeLineBreak();
	if(node->Link)
	{
		stringw text_link = stringw(node->Link);
		xml->writeElement(L"Link",true,L"value",text_link.c_str()); xml->writeLineBreak();
	}
	if(node->isLink)
	{
		xml->writeElement(L"IsLink",true,L"value",L"true"); xml->writeLineBreak();
	}
	if(node->Player)
	{
		xml->writeElement(L"Speaker",true,L"value",L"PLAYER"); xml->writeLineBreak();
	}
	else
	{
		stringw text_name = node->NPCName;
		xml->writeElement(L"Speaker",true,L"value",text_name.c_str()); xml->writeLineBreak();
	}
	xml->writeElement(L"Text",true,L"value",node->Text.c_str()); xml->writeLineBreak();
	//write actions
	if(node->scriptAction.actions.size() > 0)
	{
		xml->writeElement(L"Actions",false); xml->writeLineBreak();
		for ( u32 a = 0; a < node->scriptAction.actions.size(); a++)
		{
			xml->writeElement(node->scriptAction.actions[a]->name.c_str(),true,
				L"target",node->scriptAction.actions[a]->target.c_str(),
				L"attribute",node->scriptAction.actions[a]->attribute.c_str(),
				L"value",node->scriptAction.actions[a]->value.c_str());
			xml->writeLineBreak();
		}
		xml->writeClosingTag(L"Actions"); xml->writeLineBreak();
	}

	if (node->Child)
	{
		WriteNPCDialogNode(xml, node->Child); //continue populating child
	}

	xml->writeClosingTag(L"Node"); xml->writeLineBreak();

	if (node->Sibling)
	{
		WriteNPCDialogNode(xml, node->Sibling); //continue populating sibilings.
	}
}

//Saves NPC conversation and actions related to dialog nodes in one XML like file
bool CDialogWindow::SaveNPCDialog(IFileSystem* fs, stringc file,TreeNode* rootNode)
{
	if (file != stringc(""))
	{
		IXMLWriter* xml = fs->createXMLWriter(file.c_str());
		
		if (xml)
		{
			xml->writeXMLHeader(); xml->writeLineBreak();

			//recursive function
			WriteNPCDialogNode(xml, rootNode->Child);
			
			xml->drop(); // don't forget to delete the xml reader

			return true;
		}
	}

	return false;
}

void CDialogWindow::AddGameDialog(IFileSystem* fs, stringw language, stringw filename)
{
	stringc dialogPath = stringw(DIALOG_DIR) + language + stringw("/") + filename;
	IXMLReader* xml = fs->createXMLReader(dialogPath.c_str());
	if(xml)
	{
		m_LevelDialogs[m_NumDialogs] = new SDialog;
		m_LevelDialogs[m_NumDialogs]->m_Filename = filename;
		m_LevelDialogs[m_NumDialogs]->m_DialogRead = false;
		//XML load
		LoadDialogFromXML(xml, m_NumDialogs);
		m_NumDialogs++;
	}
}

void CDialogWindow::Init(rect<s32> Rect, IGUIElement* parent, s32 id, IGUIFont* font, ITexture* texture, SColor color)
{
	m_Rect = Rect;
	m_Parent = parent;
	m_Font = font;
	m_Texture = texture;
	m_ID = id;
	m_DlgWnd = 0;
	m_LevelDialogs = new SDialog*[100];
	m_NumDialogs = 0;
}

void CDialogWindow::SetDialogNodeEnabled(stringw filename, s32 nodeID, bool enable)
{
	s32 index = -1;

	if(FindDialog(filename,&index))
	{
		SetNodeEnable(m_LevelDialogs[index]->m_RootNode->Child, (u32)nodeID, enable);
	}
}

