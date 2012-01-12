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
CLanguages::CLanguages(IFileSystem* fs)
{
	m_Language = new TLanguage();
	m_FS = fs;
}

void CLanguages::setFS(IFileSystem* fs)
{
	m_FS = fs;
}

/**
 * \brief Init loads list of available languages and sets default language from preferences.
 * \author Petar Bajic 
 * \date January, 21 2012.
 */
bool CLanguages::Init()
{
	u32 index = 0;

	IFileList* fileList = m_FS->createFileList();
	u32 count = fileList->getFileCount();
	for (u32 i=0; i<count; i++)
	{
		stringc file = fileList->getFileName(i);
		IXMLReader* xml = m_FS->createXMLReader(file);

		if (xml)
		{
			//get language name and value from file
			while(xml->read())
			{
				switch(xml->getNodeType())
				{
				case io::EXN_ELEMENT:
					{
						if (stringw("Language").equals_ignore_case(xml->getNodeName()))
						{
							TLanguage* lang = new TLanguage();
							lang->name = xml->getAttributeValue(L"name");
							lang->value = xml->getAttributeValue(L"value");
							lang->index = index;
							m_ListOfAvailableLanguages.push_back(lang);
							index++;
						}
					}
				}
			}

			xml->drop(); // don't forget to delete the xml reader
		}

		
	}
	

	

	return true;
}


void CLanguages::LoadStringTable()
{
	//parse language xml and load strings to table

	//clear old table
	m_LanguageStringTable.clear();

	stringc file = m_Language->value + stringc("_strings.xml"); 
	IXMLReader* xml = m_FS->createXMLReader(file);

	if (xml)
	{
		//get language name and value from file
		while(xml->read())
		{
			switch(xml->getNodeType())
			{
			case io::EXN_ELEMENT:
				{
					if (stringw("string").equals_ignore_case(xml->getNodeName()))
					{
						TLanguageString lang;
						lang.id = xml->getAttributeValueAsInt(L"id");
						lang.value = xml->getAttributeValue(L"value");
						m_LanguageStringTable.push_back(lang);
					}
				}
			}
		}

		xml->drop(); // don't forget to delete the xml reader
	}
}

bool CLanguages::setLanguage(stringc lang)
{
	m_Language->value = lang;
	bool found = false;

	for (u32 i=0; i< m_ListOfAvailableLanguages.size(); i++)
	{
		if(m_ListOfAvailableLanguages[i]->value.equals_ignore_case(lang))
		{
			m_Language->name = m_ListOfAvailableLanguages[i]->name;
			m_Language->index = m_ListOfAvailableLanguages[i]->index;
			found = true;
		}
	}

	if(!found)
	{
		printf("Language '%s' not found! Setting english as default language.\n", lang.c_str());
		m_Language->value = "en";
		m_Language->name = "english";
		m_Language->index = 0;
	}

	LoadStringTable();

	return found;
}


bool CLanguages::setLanguage(s32 index)
{
	bool found = false;
	if((index >= 0)&&(index < (s32)m_ListOfAvailableLanguages.size()))
	{
		m_Language->name = m_ListOfAvailableLanguages[index]->name;
		m_Language->index = m_ListOfAvailableLanguages[index]->index;
		m_Language->value = m_ListOfAvailableLanguages[index]->value;
		found = true;

		LoadStringTable();
	}

	return found;
}

stringw CLanguages::getString(u32 id)
{
	stringw text;

	if(id < m_LanguageStringTable.size())
	{
		text = m_LanguageStringTable[id].value;
	}
	else
	{
		//index out of bounds

		text = "string not found";
	}
	return text;
}