/** 
 * \file Utils.cpp
 * \brief Utility functions
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date June, 19 2009.
 */

#include "Utils.h"

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