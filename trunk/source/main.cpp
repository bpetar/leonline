/** 
 * \file main.cpp
 * \brief Irrlicht Simple game level editor. Main function creates editor class and runs the loop. 
 * Nothing more.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "Editor.h"

using namespace irr;

#pragma comment(lib, "Irrlicht.lib")

//! Main game loop.
int main()
{
    CEditor editor;
    editor.run();
}



