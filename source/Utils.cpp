/** 
 * \file Utils.cpp
 * \brief Utility functions
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date June, 19 2009.
 */

#include "Utils.h"
#include "EditorManager.h"



/**
 * This  helper function is recursive.
 * It walks through the tree of dialog nodes and sets them all to value 'enabled'.
 */
void PropagateEnabled(TreeNode* node, bool enabled)
{
	node->isEnabled = enabled;

	TreeNode* child = node->Child;
	while(child)
	{
		PropagateEnabled(child,enabled);
		child = child->Sibling;
	}
}

/**
 * \brief Utility function that populates TreeNode of Game Objects with data from XML file.
 * Used in populating gui trees in Game Objects window for Monsters, Pickables and Static objects.
 * It receives dir name (Monsters, Static, etc.) and opens xml file with the same name (Monsters.xml, etc.)
 * It parses this xml wich contains game objects nodes and their tree of hierarchy.
 */
void Util_PopulateTreeNodeGameObjectsFromXML(CEditorManager* editorManager, stringc dir_filename, IGUIElement *parent, rect<s32> position, s32 id)
{
	IGUIEnvironment* env = editorManager->getGUIEnvironment();
	IrrlichtDevice* device = editorManager->getDevice();

	/* Fill Tree */
	stringw elementName = dir_filename.c_str(); //dir filename is also xml filename, and is also xml element name.
	stringc xml_filename = dir_filename + ".xml";
	int NUM_NODES = 100;
	IGUITreeCtrl *tree = AddGUITreeCtrl(env, position, parent, id, false);
	TreeNode **NodeList = new TreeNode*[NUM_NODES];
	NodeList[0] = tree->GetRootNode();
	io::IXMLReader* xml = device->getFileSystem()->createXMLReader(xml_filename.c_str());
	int nb_node = 1;
	int parent_node = 0;
	/* Don't ask how this tree populating from xml structure works. I did it, but I still can't figure out how. */
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo;
				if (elementName.equals_ignore_case(xml->getNodeName()))
				{
					figo = xml->getAttributeValue(L"filename");
					NodeList[nb_node] = tree->InsertNode(figo.c_str(),NodeList[parent_node],nb_node);
					nb_node++;
				}
				else if (stringw("Config") == xml->getNodeName())
				{
					//doing nothing for config
				}
				else
				{
					//New Folder, create node and step in.
					figo = xml->getNodeName();
					NodeList[nb_node] = tree->InsertNode(figo.c_str(),NodeList[parent_node],nb_node);
					parent_node = nb_node;
					nb_node++;
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			{
				stringw newre = xml->getNodeName();
				if (stringw("Config") != newre)
				{
					//Folder Closed, stepping out.
					parent_node = 0;
				}
			}
			break;
		}
	}

	if (xml)
		xml->drop(); // don't forget to delete the xml reader
}


stringw Util_GetRootNameFromPath(stringw path)
{
	stringw rootName = path.subString(path.findLast('/')+1,path.size());
	rootName = rootName.subString(0,rootName.findLast('.'));
	return rootName;
}

stringw Util_GetRootPathFromPath(stringw path)
{
	stringw rootName = path;
	rootName = rootName.subString(0,rootName.findLast('.'));
	return rootName;
}

vector3df Util_getVectorFromString(stringw str) 
{ 
   vector3df vector; 
   swscanf_s(str.c_str(), L"%f %f %f", &vector.X,&vector.Y,&vector.Z ); 
   return vector; 
}

stringw Util_getStringFromVector(vector3df v) 
{ 
   stringw v_str; 
   v_str = stringw(v.X) + stringw(" ") + stringw(v.Y) + stringw(" ") + stringw(v.Z); 
   return v_str; 
}