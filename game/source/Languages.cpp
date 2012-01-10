/** 
 * \file Languages.cpp
 * \brief This file is for Languages. 
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date January, 21 2012.
 */

#include "Languages.h"


/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date January, 21 2012.
 */
CLanguages::CLanguages()
{
	m_Language = new TLanguage();
}


/**
 * \brief Init loads list of available languages and sets default language from preferences.
 * \author Petar Bajic 
 * \date January, 21 2012.
 */
bool CLanguages::Init(IFileSystem* fs, stringc filepath)
{
	fs->changeWorkingDirectoryTo(filepath);

	IFileList* fileList = fs->createFileList();
	u32 count = fileList->getFileCount();
	for (u32 i=0; i<count; i++)
	{
		stringc file = fileList->getFileName(i);
		IXMLReader* xml = fs->createXMLReader(file);

		if (!xml)
		{
			//Display error message and exit

			//return false;
		}

		//get language name and value from file
	}
	

	/*while(xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_ELEMENT:
			{
				stringw figo;
				if (core::stringw("MapStart") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"filename");
					m_StartMap = figo.c_str();
				}
				else if (core::stringw("PlayerConfig") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"filename");
					m_PlayerConfigFile = figo.c_str();
				}
				else if (core::stringw("WorkingDir") == xml->getNodeName())
				{
					//New Folder, create node and step in.
					figo = xml->getAttributeValue(L"filename");
					m_WorkingDir = figo.c_str();
				}
				else if (core::stringw("WindowCaption") == xml->getNodeName())
				{
					//New Folder, create node and step in.
					figo = xml->getAttributeValue(L"text");
					m_WndCaption = figo.c_str();
				}
				else if (stringw("Level") == xml->getNodeName())
				{
					figo = xml->getAttributeValue(L"filename");
					m_Maps[m_NumberOfMaps] = figo;
					m_NumberOfMaps++;
				}
			}
			break;
		}
	}

	xml->drop(); // don't forget to delete the xml reader*/

	return true;
}

bool CLanguages::setLanguage(stringc lang)
{
	m_Language->name = "pera";
	bool found = false;

	for (u32 i=0; i< m_ListOfAvailableLanguages.size(); i++)
	{
		if(m_ListOfAvailableLanguages[i]->value.equals_ignore_case(lang))
		{
			m_Language->name = m_ListOfAvailableLanguages[i]->name;
			found = true;
		}
	}

	if(!found)
	{
		printf("Language '%s' not found! Setting english as default language.\n", lang.c_str());
		m_Language->value = "en";
		m_Language->name = "english";
	}

	return found;
}
