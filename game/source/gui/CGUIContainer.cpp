/** 
 * \file CGUIContainer.cpp
 * \brief CGUIContainer class creates simple container component.
 * Container represents content of chest or standard inventory, displaying empty slots and 
 * icons of items placed in container. This class has array of CGameObject(s). 
 * CGUIContainer can be added to GUI window like any other irrlicht 
 * GUI component, you only have to choose how many rows and columns, and texture for empty
 * slot (if these values are not provided, default values will take place).
 *
 * This class needs some clean up and maybe aditional functionality before presented to the world
 * as standalone useful GUI component.
 *
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#include "CGUIContainer.h"
#include "IGUIEnvironment.h"
#include "IGUIFont.h"
#include "IVideoDriver.h"

using namespace irr;
using namespace gui;
using namespace core;
using namespace video;

/*
 * This function allows CGUIContainer to be added like other irrlicht GUI components.
 */
IGUIContainer *irr::gui::AddGUIContainer(
		IGUIEnvironment *env,
		const core::rect<s32> Rectangle,
		IGUIElement *parent,
		s32 id,
		core::dimension2d<s32> arrayDim,
		core::dimension2d<s32> spacing,
		video::ITexture* texture
		)
{
	if(parent == NULL)
		parent = env->getRootGUIElement();

	rect<s32> slotRect = rect<s32>(0,0,64,64); //change this
	rect<s32> rectangle = Rectangle;
	if (spacing.Width + arrayDim.Width*(slotRect.getWidth() + spacing.Width) > Rectangle.getWidth())
	{
		//icons falling out of container
		rectangle.LowerRightCorner.X = rectangle.UpperLeftCorner.X + spacing.Width + arrayDim.Width*(slotRect.getWidth() + spacing.Width);
	}

	CGUIContainer *container = new CGUIContainer(
		env, 
		parent, 
		id, 
		rectangle, 
		arrayDim,
		spacing,
		texture
		);
	return container;
}

/**
 * \brief Standard destructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGUIContainer::~CGUIContainer()
{

	m_ListOfItems.clear();
}

/**
 * \brief Standard constructor.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGUIContainer::CGUIContainer(
		IGUIEnvironment* environment, 
		IGUIElement* parent,
		s32 id,
		core::rect<s32> rectangle,
		core::dimension2d<s32> arrayDim,
		core::dimension2d<s32> spacing,
		video::ITexture* texture
		): IGUIContainer(environment, parent, id, rectangle),
		m_NumItems(arrayDim.Height * arrayDim.Width),
		m_Parent(parent),
		m_SlotTexture(texture),
		m_Width(arrayDim.Width),
		m_Height(arrayDim.Height),
		m_Spacing(spacing)
{
	slotRect = rect<s32>(0,0,64,64); //change this
	m_ItemBeingDragged = true;
	m_Overwrite = false;
	Clear();
}

/**
 * \brief Returns CGameObject in position (x,y).
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGameObject* CGUIContainer::GetItem(u32 x, u32 y)
{
	u32 i = m_Width*y + x;
	return m_ListOfItems[i].pick;
}

/**
 * \brief Returns CGameObject at array[index].
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
CGameObject* CGUIContainer::GetItem(u32 index)
{
	CGameObject* temp = m_ListOfItems[index].pick;
	return temp;
}

/**
 * \brief Sets CGameObject in position (x,y).
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGUIContainer::SetItem(u32 x, u32 y, CGameObject* pick)
{
	u32 i = m_Width*y + x;
	if (m_ListOfItems[i].pick != NULL)
	{
		//do something with existing item (switching?), or ignore setting new one
		if (m_Overwrite)
		{
			m_ListOfItems[i].pick = pick;
		}
	}
	else
	{
		//slot is empty, add pickable item
		m_ListOfItems[i].pick = pick;
	}
}

/**
 * \brief Sets CGameObject in position specified by index.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGUIContainer::InsertItem(CGameObject* pick, u32 index)
{
	if (m_ListOfItems[index].pick == NULL)
	{
		u32 y = index/m_Width;
		u32 x = index - y*m_Width;
		m_ListOfItems[index].pick = pick;
		m_ListOfItems[index].x = x;
		m_ListOfItems[index].y = y;
	}
}

/**
 * \brief Insert CGameObject to first available position.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGUIContainer::InsertItem(CGameObject* pick)
{
    u32 size = m_ListOfItems.size();
	u32 x = 0;
	u32 y = 0;
    for(u32 i = 0; i < size; i++)
    {
		if(m_ListOfItems[i].pick == NULL)
		{
			y = i/m_Width;
			x = i - y*m_Width;
			m_ListOfItems[i].pick = pick;
			m_ListOfItems[i].x = x;
			m_ListOfItems[i].y = y;
			return;
		}
    }
}

/**
 * \brief Return if container is full.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGUIContainer::isFull()
{
    u32 size = m_ListOfItems.size();
    for(u32 i = 0; i < size; i++)
    {
		if(m_ListOfItems[i].pick == NULL)
		{
			return false;
		}
    }

	return true;
}

/**
 * \brief Clears the array of CGameObject objects - m_ListOfItems.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGUIContainer::Clear()
{
	u32 x = 0;
	u32 y = 0;
	u32 size = m_Width*m_Height;
	m_ListOfItems.clear();
	TContainerItem emptySlot;

	for(u32 i = 0; i < size; i++)
	{
		y = i/m_Width;
		x = i - y*m_Width;
		emptySlot.pick = NULL;
		emptySlot.x = x;
		emptySlot.y = y;
		m_ListOfItems.push_back(emptySlot);
	}
}

/**
 * \brief Draw function draws the container component.
 * If texture for empty slot is not set, simple rectangles will be drawn.
 * This function also goes through the list of items and draws them textures in place.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGUIContainer::draw()
{
	if(!isVisible())
		return;

	//Draw Container
	IGUISkin* skin = Environment->getSkin();	
	core::rect<s32>* clipRect = 0;
	core::rect<s32> iconAbsoluteSquare;
	clipRect = &AbsoluteClippingRect;
	core::rect<s32> frameRect(AbsoluteRect);
	skin->draw3DSunkenPane(this, skin->getColor(EGDC_3D_HIGH_LIGHT), false, true, frameRect, clipRect);
	skin->draw2DRectangle(this, skin->getColor(EGDC_BUTTON_TEXT), frameRect, clipRect);

	//Draw Slots
	for(u32 x = 0; x < m_Width; x++)
	{
		for(u32 y = 0; y < m_Height; y++)
		{
			iconAbsoluteSquare.UpperLeftCorner = AbsoluteRect.UpperLeftCorner + position2d<s32>( m_Spacing.Width + x*(slotRect.getWidth() + m_Spacing.Width), m_Spacing.Height + y*(slotRect.getHeight() + m_Spacing.Height));
			iconAbsoluteSquare.LowerRightCorner = position2d<s32>(iconAbsoluteSquare.UpperLeftCorner.X + slotRect.getWidth(), iconAbsoluteSquare.UpperLeftCorner.Y + slotRect.getHeight());
			if (!m_SlotTexture)
			{
				//draw simlpe graphic squares
				Environment->getVideoDriver()->draw2DRectangle(SColor(255,50,155,0),iconAbsoluteSquare);
			}
			else
			{
				//draw fancy slot icons with textures
				Environment->getVideoDriver()->draw2DImage(
				m_SlotTexture,
				iconAbsoluteSquare.UpperLeftCorner,
				core::rect<s32>(core::position2d<s32>(0,0), m_SlotTexture->getOriginalSize()),
				&iconAbsoluteSquare);
			}
		}
	}

	//Draw Items in Container
	u32 index = 0;
	if (m_ListOfItems.size() == m_Width*m_Height)
	{
		for(u32 x = 0; x < m_Width; x++)
		{
			for(u32 y = 0; y < m_Height; y++)
			{
				index = x + y*m_Width;
				if (m_ListOfItems[index].pick != NULL)
				{
					if (m_ListOfItems[index].pick->m_IconTexture != NULL)
					{
						iconAbsoluteSquare.UpperLeftCorner = AbsoluteRect.UpperLeftCorner + position2d<s32>( m_Spacing.Width + x*(slotRect.getWidth() + m_Spacing.Width), m_Spacing.Height + y*(slotRect.getHeight() + m_Spacing.Height));
						iconAbsoluteSquare.LowerRightCorner = iconAbsoluteSquare.UpperLeftCorner + position2d<s32>(slotRect.getWidth(), slotRect.getHeight());
						//Draw item in container
						Environment->getVideoDriver()->draw2DImage(
						m_ListOfItems[index].pick->m_IconTexture,
						iconAbsoluteSquare.UpperLeftCorner,
						core::rect<s32>(core::position2d<s32>(0,0), m_ListOfItems[index].pick->m_IconTexture->getOriginalSize()),
						&iconAbsoluteSquare);
					}
					else
					{
						//TODO: draw invalid icon image in the slot
					}
				}
			}
		}
	}
}

/**
 * \brief Checks if position coordinate is inside slot with index (x,y).
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGUIContainer::isInsideSlot(u32 x, u32 y, position2d<s32> p)
{
	bool contains = false;
	core::rect<s32> iconAbsoluteSquare;
	iconAbsoluteSquare.UpperLeftCorner = AbsoluteRect.UpperLeftCorner + position2d<s32>( m_Spacing.Width + x*(slotRect.getWidth() + m_Spacing.Width), m_Spacing.Height + y*(slotRect.getHeight() + m_Spacing.Height));
	iconAbsoluteSquare.LowerRightCorner = iconAbsoluteSquare.UpperLeftCorner + position2d<s32>(slotRect.getWidth(), slotRect.getHeight());
	contains = iconAbsoluteSquare.isPointInside(p);
	
	return contains;
}

bool CGUIContainer::isInsideSlot(u32 i, position2d<s32> p)
{
	u32 y = i/m_Width;
	u32 x = i - y*m_Width;
	bool contains = false;
	core::rect<s32> iconAbsoluteSquare;
	iconAbsoluteSquare.UpperLeftCorner = AbsoluteRect.UpperLeftCorner + position2d<s32>( m_Spacing.Width + x*(slotRect.getWidth() + m_Spacing.Width), m_Spacing.Height + y*(slotRect.getHeight() + m_Spacing.Height));
	iconAbsoluteSquare.LowerRightCorner = iconAbsoluteSquare.UpperLeftCorner + position2d<s32>(slotRect.getWidth(), slotRect.getHeight());
	contains = iconAbsoluteSquare.isPointInside(p);
	
	return contains;
}
/**
 * \brief Removes item in slot with coordinates (x,y).
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGUIContainer::RemoveItem(u32 x, u32 y)
{
	u32 i = m_Width*y + x;
	m_ListOfItems[i].pick = NULL;
}

/**
 * \brief Removes item in slot with coordinates (x,y).
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
void CGUIContainer::RemoveItem(u32 i)
{
	if (i < m_ListOfItems.size())
	{
		m_ListOfItems[i].pick = NULL;
	}
}

/**
 * \brief OnEvent handles user cliks on this component.
 * So far, simple mouse clicks are interpreted like "removing item from the slot".
 * Here can be added icon drag and drop, as well as keyboard event reactions.
 * \author Petar Bajic 
 * \date July, 21 2008.
 */
bool CGUIContainer::OnEvent(const SEvent& event)
{
	switch(event.EventType)
	{
	case EET_KEY_INPUT_EVENT:
		{
			if(event.KeyInput.PressedDown)
			{
				switch(event.KeyInput.Key)
				{
					case KEY_LEFT:
					case KEY_UP:
						break;
					case KEY_RIGHT:
					case KEY_DOWN:
						break;
				}
			}
		}
		break;
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		case gui::EGET_SCROLL_BAR_CHANGED:
			break;
		case gui::EGET_ELEMENT_FOCUS_LOST:
			break;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		{
			core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);

			switch(event.MouseInput.Event)
			{
				case EMIE_MOUSE_WHEEL:
					break;
				case EMIE_LMOUSE_PRESSED_DOWN:
					{
						m_ItemBeingDragged = true;
					}
					break;
				case EMIE_RMOUSE_PRESSED_DOWN:
					{
						m_ItemBeingDragged = true;
					}
					break;
				case EMIE_LMOUSE_LEFT_UP:
					if (m_ItemBeingDragged)
					{
						m_ItemBeingDragged = false;
					}
					break;
				case EMIE_MOUSE_MOVED:
					break;
			}
		}
		break;
	}
	return m_Parent ? m_Parent->OnEvent(event) : false;
}

