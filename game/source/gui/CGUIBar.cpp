/** 
 * \file CGUIBar.cpp
 * \brief Graphic bar used to display health.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "CGUIBar.h"
CGUIBar::CGUIBar(int x1, int y1, int x2, int y2, int maxValue, int curValue, video::ITexture* texture, stringw name)
{
   //class constructor
       
   //bars size
   x1Bar = x1;
   y1Bar = y1;
   x2Bar = x2;
   y2Bar = y2;

   barSize = (x2Bar-3) - (x1Bar+3);
   //max bar values
   maxBarValue = maxValue;

   //current bar values
   BarValue = curValue;

   deltaBar = curValue;

   //bar name (asociated with skill/ability name)
   m_Name = name;

   //texture
   BarTexture = texture;

} 

void CGUIBar::renderGUIBars(video::IVideoDriver* driver, IGUIFont* font){

   double delta; //status bar variation
   double x=17; //spell bar position
   double y=35;

   //here is some cool stuff, it changes the bar value from X to Y smoothly
   if ((float)deltaBar != (float)BarValue){
      delta = BarValue - deltaBar;
	  if((deltaBar == 0)&&(BarValue < 0.3))
	  {
		  delta = 0;
		  BarValue = 0;
	  }
      if (delta > 0.1)
         BarValue -= 0.2;
      if (delta < -0.1)
         BarValue += 0.2;
   }
       
 
   //another cool stuff,  it draws the bar according to its value (green for high, decreasing to yellow till red), and some others rectangles for a cool look
	if(BarTexture)
	{
		driver->draw2DImage(BarTexture, core::position2d<s32>(x1Bar-4, y1Bar-4));
	}

	driver->draw2DRectangle(video::SColor(255, 100, 100, 100), core::rect<s32>(x1Bar, y1Bar, x2Bar, y2Bar));

	driver->draw2DRectangle(video::SColor(255, 125, 125, 125), core::rect<s32>(x1Bar+1, y1Bar+1, x2Bar-1, y2Bar-1));

	driver->draw2DRectangle(video::SColor(255, 150, 150, 150), core::rect<s32>(x1Bar+3, y1Bar+3, x2Bar-3, y2Bar-3));

	driver->draw2DRectangle(core::rect<s32>(x1Bar+3, y1Bar+3, x1Bar+3+(s32)(barSize*BarValue/maxBarValue), y2Bar-3),

						video::SColor(255, (s32)(255-BarValue*(255/maxBarValue)), (s32)(BarValue*(255/maxBarValue)), 0),

						video::SColor(255, (s32)(255-BarValue*(255/maxBarValue)), (s32)(BarValue*(255/maxBarValue)), 0),

						video::SColor(255, 180, 180, 0),

						video::SColor(255, 180, 180, 0));
	
	stringw value = stringw((s32)(BarValue+0.5)) + stringw(L"/") + stringw(maxBarValue);
	font->draw(m_Name.c_str(),rect<s32>(x1Bar+4, y1Bar,x1Bar+barSize, y2Bar-4),SColor(255,0,0,0));
	font->draw(value.c_str(),rect<s32>(x1Bar+60, y1Bar,x1Bar+barSize, y2Bar-4),SColor(255,0,0,0));

} 

void CGUIBar::setBarValue(s32 Value)
{
   deltaBar = (int)Value;
}

void CGUIBar::setMaxBarValue(s32 value)
{
	maxBarValue = value;
}

void CGUIBar::setPos(int x1, int y1, int x2, int y2)
{
	x1Bar = x1;
	x2Bar = x2;
	y1Bar = y1;
	y2Bar = y2;
}

int CGUIBar::getBarValue()
{
   return deltaBar;
}

void CGUIBar::changeValueBy(s32 Value)
{
   deltaBar += (int)Value;
   if (deltaBar < 0)
   {
	   deltaBar = 0;
   }
   if (deltaBar > maxBarValue)
   {
	   deltaBar = maxBarValue;
   }
}