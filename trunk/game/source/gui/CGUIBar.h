/** 
 * \file CGUIBar.h
 * \brief Header file containing CGUIBar class used for managing GUI health and mana bars.
 * CGUIBar class creates bars and handles health and mana changes.
 *
 * This code is taken from irrlich forum: 
 * http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=25017
 * and adjusted to our needs. 
 * It is originally written by eMgz, and all thanks goes to him.
 */


#if !defined(AFX_CGUIBarsCA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_CGUIBarsCA_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace gui;
using namespace video;

class CGUIBar {

private:
	video::IVideoDriver* driver;
	int x1Bar, y1Bar, x2Bar, y2Bar;
	int barSize;
	int   maxBarValue; //bar size
	int deltaBar;   //interpolation
	double BarValue;   //current bar value
	stringw m_Name;
	video::ITexture* BarTexture;
	//IGUIFont* font;

public:

	CGUIBar(int x1, int y1, int x2, int y2, int maxBarValue, int curValue, video::ITexture* texture, stringw name); //constructor
	~CGUIBar(); //destructor :O
	void renderGUIBars(video::IVideoDriver* driver, IGUIFont* font); //render the bars, must be called AFTER the 3D draw, or it  wont show
	void setBarValue(s32 healthValue); //change bar values
	void setMaxBarValue(s32 value);
	void changeValueBy(s32 Value);
	void setPos(int x1Bar, int y1Bar, int x2Bar, int y2Bar);
	vector2di getPos() {return vector2di(((x1Bar+x2Bar)/2),y1Bar);}
	int getBarValue();
	stringw getName() {return m_Name;}
}; 

#endif

