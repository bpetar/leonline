/** 
 * \file CGUIContainer.h
 * \brief Header file containing CGUIContainer class used for representing inventory and various container content.
 * This class follows standard irrlicht way of creating GUI components.
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#pragma once

#include "IGUIContainer.h"
#include <irrlicht.h>

typedef struct SContainerItem
{
	CGameObject* pick;
	u32 x;
	u32 y;
} TContainerItem;

namespace irr
{
namespace gui
{
	//! Default Container GUI element.
	class CGUIContainer : public IGUIContainer
	{
		friend IGUIContainer *AddGUIContainer(
		IGUIEnvironment *env,
		const core::rect<s32> Rectangle,
		IGUIElement *parent,
		s32 id,
		core::dimension2d<s32> arrayDim,
		core::dimension2d<s32> spacing,
		video::ITexture* texture
		);
	public:
			//! destructor
			~CGUIContainer();

			virtual inline u32 GetNumItems() {return m_NumItems;}
			virtual CGameObject *GetItem(u32 x, u32 y);
			virtual CGameObject *GetItem(u32 index);
			virtual void SetItem(u32 x, u32 y, CGameObject* pick);
			virtual void InsertItem(CGameObject* pick);
			void RemoveItem(u32 x, u32 y);
			bool isInsideSlot(u32 x, u32 y, position2d<s32> p);
			virtual void Clear();
			virtual void draw();
			bool OnEvent(const SEvent& event);
			//virtual bool SetSelected(TreeNode *node);
			//virtual TreeNode *GetSelected();

		protected:
			//void DrawVerticalLine(core::position2d<s32> start, s32 length, video::SColor &color, const core::rect<s32> *ClipRect = NULL);
			//void DrawHorizontalLine(core::position2d<s32> start, s32 length, video::SColor &color, const core::rect<s32> *ClipRect = NULL);

			s32 m_IconHeight;	
			s32 m_IconWidth;
			u32 m_Height;
			u32 m_Width;
			u32 m_NumItems;
			IGUIElement* m_Parent;
			bool m_ItemBeingDragged;
			bool m_Overwrite;
			core::dimension2d<s32> m_Spacing;
			core::rect<s32> slotRect;
			core::array <TContainerItem> m_ListOfItems;

			video::ITexture* m_SlotTexture;
			
			IGUIFont *Font;
			//IGUIScrollBar* ScrollBarV;
			//IGUIScrollBar* ScrollBarH;
		protected:
			//! constructor
			CGUIContainer(
				IGUIEnvironment* environment, 
				IGUIElement* parent, 
				s32 id, 
				core::rect<s32> rectangle,
				core::dimension2d<s32> arrayDim,
				core::dimension2d<s32> spacing,
				video::ITexture* texture
				);

	};


} // end namespace gui
} // end namespace irr