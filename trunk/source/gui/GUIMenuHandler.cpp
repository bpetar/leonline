/** 
 * \file GUIWndMaker.cpp
 * \brief Handles menu events.
 * 
 * Who says it all has to be in one cpp file? :) 
 * I moved these functions from EDGui.cpp file, CEditorGUI class, to stop it from growing enormous
 * All funtions that handle menu clicks are neatly placed here.
 * Writting a winning combination of include files was a nigthmare but I did it!
 * Couldn't repeat it if I wanted to, so don't touch anything :)
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date June, 1 2009.
 */

#include "../EditorManager.h"

#include "GUIMenuHandler.h"
#include "CGUIFileSaveDialog.h"

#ifdef LE_WIN_OS
#include <windows.h> 
#endif


//Called from EDGui.cpp,
//creates Main Menu
void CreateMenu(CEditorManager* editorManager)
{
	IGUIContextMenu* menu = editorManager->getGUIEnvironment()->addMenu();
	//menu->
	menu->addItem(L"File", -1, true, true);
	menu->addItem(L"Edit", -1, true, true);
	menu->addItem(L"View", -1, true, true);
	menu->addItem(L"Insert", -1, true, true);
	menu->addItem(L"Help", -1, true, true);

	IGUIContextMenu* submenu;

	//File
	submenu = menu->getSubMenu(0);
	submenu->addItem(L"New Level", GUI_ID_MENU_NEW_LEVEL);
	submenu->addItem(L"Open Level...", GUI_ID_MENU_OPEN_LEVEL);
	submenu->addItem(L"Save Level", GUI_ID_MENU_SAVE_LEVEL);
	submenu->addItem(L"Save Level As...", GUI_ID_MENU_SAVE_LEVEL_AS);
	submenu->addSeparator();
	submenu->addItem(L"Save all to irr scene...", GUI_ID_MENU_SAVE_IRR);
	submenu->addItem(L"Load from irr scene...", GUI_ID_MENU_LOAD_IRR);
	submenu->addSeparator();
	submenu->addItem(L"Quit", GUI_ID_MENU_QUIT);

	//Edit
	submenu = menu->getSubMenu(1);
	submenu->addItem(L"Undo", GUI_ID_MENU_UNDO);
	submenu->addItem(L"Redo", GUI_ID_MENU_REDO, true, false);
	submenu->addSeparator();
	submenu->addItem(L"Select All", GUI_ID_MENU_SELECT_ALL, true, false );

	editorManager->getGUIManager()->m_ViewMenu = menu->getSubMenu(2);
	editorManager->getGUIManager()->m_ViewMenuItem_Properties = editorManager->getGUIManager()->m_ViewMenu->addItem(L"Object Properties", GUI_ID_MENU_VIEW_OBJECT_PROPERTIES, true, false, editorManager->getGUIManager()->m_bObjectPropertiesVisible);
	editorManager->getGUIManager()->m_ViewMenuItem_GameObjects = editorManager->getGUIManager()->m_ViewMenu->addItem(L"Game Objects", GUI_ID_MENU_VIEW_GAME_OBJECTS, true, false, true);
	editorManager->getGUIManager()->m_ViewMenuItem_ObjectPreview = editorManager->getGUIManager()->m_ViewMenu->addItem(L"Object Preview", GUI_ID_MENU_VIEW_OBJECT_PREVIEW, true, false, true);

	//View

	//Insert
	submenu = menu->getSubMenu(3);
	submenu->addItem(L"Cube", GUI_ID_MENU_INSERT_CUBE);
	submenu->addItem(L"Sphere", GUI_ID_MENU_INSERT_SPHERE);
	submenu->addItem(L"Water", GUI_ID_MENU_INSERT_WATER);
	submenu->addItem(L"Procedural Tree", -1, true, true);
	submenu->addItem(L"Terrain Grass", GUI_ID_MENU_INSERT_GRASS);
	submenu->addItem(L"Terrain from Heightmap", GUI_ID_MENU_INSERT_TERRAIN);
	submenu->addItem(L"Flag Start Position", GUI_ID_MENU_INSERT_FLAG);
	submenu->addItem(L"Level Music", GUI_ID_MENU_INSERT_MUSIC);
	submenu->addItem(L"Particle System", GUI_ID_MENU_INSERT_PARTICLE);
	submenu->addItem(L"Light", GUI_ID_MENU_INSERT_LIGHT);
	submenu->addItem(L"Dancing Light", GUI_ID_MENU_INSERT_DANCING_LIGHT);

	editorManager->getGUIManager()->m_TreeSubmenu = submenu->getSubMenu(3);
	editorManager->getGUIManager()->m_TreeSubmenu->addItem(L"Aspen Tree", GUI_ID_MENU_INSERT_TREE_ASPEN);
	editorManager->getGUIManager()->m_TreeSubmenu->addItem(L"Oak Tree", GUI_ID_MENU_INSERT_TREE_OAK);
	editorManager->getGUIManager()->m_TreeSubmenu->addItem(L"Pine Tree", GUI_ID_MENU_INSERT_TREE_PINE);
	editorManager->getGUIManager()->m_TreeSubmenu->addItem(L"Willow Tree", GUI_ID_MENU_INSERT_TREE_WILLOW);

	submenu = menu->getSubMenu(4);
	submenu->addItem(L"About", GUI_ID_MENU_HELP_ABOUT);
	submenu->addItem(L"Homepage", GUI_ID_MENU_HELP_HOMEPAGE);
	submenu->addSeparator();
	submenu->addItem(L"Tutorial 1", GUI_ID_MENU_HELP_TUT1);
	submenu->addItem(L"Tutorial 2", GUI_ID_MENU_HELP_TUT2);
	submenu->addItem(L"Tutorial 3", GUI_ID_MENU_HELP_TUT3);
	submenu->addItem(L"Tutorial 4", GUI_ID_MENU_HELP_TUT4);
}

//Called from EDGui.cpp,
//Handles menu event in one big switch case.
//Events are tryly different from each other, from file open/save events to
//insert 3d model to map, show help->about, or anything else you can find 
//in one programs menu
bool HandleMenuClick(CEditorManager* editorManager, s32 id)
{
	CEditorGUI* edGui = editorManager->getGUIManager();
	IrrlichtDevice* device = editorManager->getDevice();
	IGUIEnvironment* gui = editorManager->getGUIEnvironment();
	ISceneManager* smgr = editorManager->getSceneMngr();

	switch(id)
	{
		//New Map
		case GUI_ID_MENU_NEW_LEVEL:
		{
			//invoke level editors function wich clears variables for new map
			editorManager->OnNewMap();
		}
		break;
		//Load Map
		case GUI_ID_MENU_OPEN_LEVEL:
		{
			//just open file picking dialog, actual loading will be done on file pick
			edGui->m_bMapOpenDialog = true;
			device->getFileSystem()->changeWorkingDirectoryTo("maps");
			gui->addFileOpenDialog(L"Please select map to open");
		}
		break;
		//Save Map
		case GUI_ID_MENU_SAVE_LEVEL:
		{
			//if map name exists, just save it
			if(editorManager->getEdiLevel()->MapNameExists())
			{
				editorManager->getEdiLevel()->OnSaveMap();
			}
			//if map name is not set (untitled) open file picking dialog
			else
			{
				edGui->m_bMapSaveDialog = true;
				device->getFileSystem()->changeWorkingDirectoryTo("maps");
				CGUIFileSaveDialog* dialog = new CGUIFileSaveDialog(L"Save Map As", gui, gui->getRootGUIElement(),-1, L"map");
				dialog->drop();
			}
		}
		break;
		//Save Map As
		case GUI_ID_MENU_SAVE_LEVEL_AS:
		{
			//open file picking dialog; map will be saved after file is selected
			edGui->m_bMapSaveDialog = true;
			device->getFileSystem()->changeWorkingDirectoryTo("maps");
			CGUIFileSaveDialog* dialog = new CGUIFileSaveDialog(L"Save Map As", gui, gui->getRootGUIElement(),-1, L"map");
			dialog->drop();
		}
		break;
		//Save all to irr scene...
		case GUI_ID_MENU_SAVE_IRR: 
		{
			//open file choose dialog
			edGui->m_bIrrFileSaveDialog = true;
			device->getFileSystem()->changeWorkingDirectoryTo("maps");
			CGUIFileSaveDialog* dialog = new CGUIFileSaveDialog(L"Save Map As", gui, gui->getRootGUIElement(),-1, L"irr");
			dialog->drop();
		}
		break;
		//Load from irr scene...
		case GUI_ID_MENU_LOAD_IRR: 
		{
			//open file choose dialog
			edGui->m_bIrrFileOpenDialog = true;
			editorManager->getDevice()->getFileSystem()->changeWorkingDirectoryTo("maps");
			editorManager->getGUIEnvironment()->addFileOpenDialog(L"Please select map to open");
		}
		break;
		//Edit->Undo
		case GUI_ID_MENU_UNDO:
		{
			editorManager->Undo();
		}
		break;
		//View->Object Properties
		case GUI_ID_MENU_VIEW_OBJECT_PROPERTIES:
		{
			if(edGui->m_wnd_Properties)
			{
				//kill window
				edGui->m_wnd_Properties->remove();
				edGui->m_wnd_Properties = 0;
				edGui->m_ViewMenu->setItemChecked(edGui->m_ViewMenuItem_Properties,false);
				edGui->m_bObjectPropertiesVisible = false;
			}
			else
			{
				//create window
				edGui->m_wnd_Properties = MakePropertiesWindow(editorManager);
				edGui->m_ViewMenu->setItemChecked(edGui->m_ViewMenuItem_Properties,false);
				edGui->m_bObjectPropertiesVisible = true;
			}
		}
		break;
		//Insert Water
		case GUI_ID_MENU_INSERT_WATER:
		{
			edGui->AddNodeToSceneTree(editorManager->m_ID,L"Water Surface");
			editorManager->getEdiLevel()->InsertWater();
		}
		break;
		//Insert->Procedural Tree-> Aspen
		case GUI_ID_MENU_INSERT_TREE_ASPEN:
		{
			edGui->AddNodeToSceneTree(editorManager->m_ID,L"Aspen Tree");
			editorManager->getEdiLevel()->InsertTree(LE_PT_ASPEN);
		}
		break;
		//Insert->Procedural Tree -> Oak
		case GUI_ID_MENU_INSERT_TREE_OAK:
		{
			edGui->AddNodeToSceneTree(editorManager->m_ID,L"Oak Tree");
			editorManager->getEdiLevel()->InsertTree(LE_PT_OAK);
		}
		break;
		//Insert->Procedural Tree -> Pine
		case GUI_ID_MENU_INSERT_TREE_PINE:
		{
			edGui->AddNodeToSceneTree(editorManager->m_ID,L"Pine Tree");
			editorManager->getEdiLevel()->InsertTree(LE_PT_PINE);
		}
		break;
		//Insert->Procedural Tree -> Willow
		case GUI_ID_MENU_INSERT_TREE_WILLOW:
		{
			edGui->AddNodeToSceneTree(editorManager->m_ID,L"Willow Tree");
			editorManager->getEdiLevel()->InsertTree(LE_PT_WILLOW);
		}
		break;
		//Insert->Flag Start Position
		case GUI_ID_MENU_INSERT_FLAG:
		{
			edGui->AddNodeToSceneTree(editorManager->m_ID,L"Start Flag");
			editorManager->getGUIManager()->SetElementAtHand(STATIC_DIR, "start_flag", true);
		}
		break;
		//Insert->Level Music
		case GUI_ID_MENU_INSERT_MUSIC:
		{
			edGui->m_wnd_LevelMusic = MakeMusicWindow(editorManager);
		}
		break;
		//Insert->Particle System
		case GUI_ID_MENU_INSERT_PARTICLE:
		{
			edGui->m_wnd_ParticleSystem = MakeParticleWindow(editorManager);
		}
		break;
		//Insert->Light
		case GUI_ID_MENU_INSERT_LIGHT:
		{
			editorManager->getEdiLevel()->InsertLight();
			edGui->AddNodeToSceneTree(editorManager->m_ID,L"Light"); // ?? isnt m_ID increased before this call - is it valid?
			editorManager->m_ID++;
		}
		break;
		//Insert->Dancing Light
		case GUI_ID_MENU_INSERT_DANCING_LIGHT:
		{
			editorManager->getEdiLevel()->InsertDancingLight();
			edGui->AddNodeToSceneTree(editorManager->m_ID,L"Dancing Light"); // ?? isnt m_ID increased before this call - is it valid?
			editorManager->m_ID++;
		}
		break;
		//Insert->Terrain Grass
		case GUI_ID_MENU_INSERT_GRASS:
		{
			editorManager->getEdiLevel()->InsertTerrainGrass(0);
		}
		break;
		//Insert->Terrain from Heightmap
		case GUI_ID_MENU_INSERT_TERRAIN:
		{
			edGui->m_wnd_InsertTerrainHeightmap = MakeTerrainHeightmapWindow(editorManager);
		}
		break;
		case GUI_ID_MENU_HELP_ABOUT: //Help->About
		{
			stringw message  = "Level Editor v0.5 \nPetar Bajic www.mystic-peanut.com\n\n";
			gui->addMessageBox(L"About", message.c_str());
		}
		break;
		//Help->Homepage
		case GUI_ID_MENU_HELP_HOMEPAGE:
		{
			#ifdef LE_WIN_OS
			ShellExecute(NULL, L"open", L"http://www.mystic-peanut.com", NULL, NULL, SW_SHOWNORMAL); 
			#endif
		}
		break;
		//Help->Tutorial 1
		case GUI_ID_MENU_HELP_TUT1:
		{
			#ifdef LE_WIN_OS
			ShellExecute(NULL, L"open", L".\\tutorials\\Tutorial_01.html", NULL, NULL, SW_SHOWNORMAL); 
			#endif
		}
		break;
		//Help->Tutorial 2
		case GUI_ID_MENU_HELP_TUT2:
		{
			#ifdef LE_WIN_OS
			ShellExecute(NULL, L"open", L".\\tutorials\\Tutorial_02.html", NULL, NULL, SW_SHOWNORMAL); 
			#endif
		}
		break;
		//Help->Tutorial 3
		case GUI_ID_MENU_HELP_TUT3:
		{
			#ifdef LE_WIN_OS
			ShellExecute(NULL, L"open", L".\\tutorials\\Tutorial_03.html", NULL, NULL, SW_SHOWNORMAL); 
			#endif
		}
		break;
		//Help->Tutorial 4
		case GUI_ID_MENU_HELP_TUT4:
		{
			#ifdef LE_WIN_OS
			ShellExecute(NULL, L"open", L".\\tutorials\\Tutorial_04.html", NULL, NULL, SW_SHOWNORMAL); 
			#endif
		}
		default: 
			return false;
	}
	return true;
}