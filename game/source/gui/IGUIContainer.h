/** 
 * \file IGUIContainer.h
 * \brief Header file containing IGUIContainer class used for representing inventory and various container content.
 * This class follows standard irrlicht way of creating GUI components.
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date July, 21 2008.
 */

#pragma once

#include "../level/GameObject.h"
#include "IGUIElement.h"
#include "irrTypes.h"

namespace irr
{
namespace gui
{
	const u32 EGUIET_CONTAINER = 0x100;
	const u32 EGET_CONTAINER_CHANGED_SELECTION = 20000;
	
	//! Default list box GUI element.
	class IGUIContainer : public IGUIElement
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
			~IGUIContainer() {};

			virtual u32 GetNumItems() = 0;
			virtual void Clear() = 0;
			virtual CGameObject *GetItem(u32 x, u32 y) = 0;
			virtual CGameObject *GetItem(u32 index) = 0;
			virtual void SetItem(u32 x, u32 y, CGameObject* pick) = 0;
			virtual void InsertItem(CGameObject* pick) = 0;
			virtual void InsertItem(CGameObject* pick, u32 index) = 0;
			virtual void RemoveItem(u32 x, u32 y) = 0;
			virtual void RemoveItem(u32 index) = 0;
			virtual bool isFull() = 0;
			virtual bool isInsideSlot(u32 i, position2d<s32> p) = 0;
			//virtual bool SetSelected(TreeNode *node) = 0;
			//virtual TreeNode *GetSelected() = 0;
			bool OnEvent(const SEvent& event) = 0;
			virtual void draw() = 0;

			virtual inline gui::EGUI_ELEMENT_TYPE getType() {return (EGUI_ELEMENT_TYPE)EGUIET_CONTAINER;};
			
		protected:
			//! constructor
			IGUIContainer(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement((EGUI_ELEMENT_TYPE)EGUIET_CONTAINER, environment, parent, id, rectangle) {}

	};
	
	IGUIContainer *AddGUIContainer(
		IGUIEnvironment *env,
		const core::rect<s32> Rectangle,
		IGUIElement *parent = NULL,
		s32 id = -1,
		core::dimension2d<s32> arrayDim = core::dimension2d<s32>(0,0),
		core::dimension2d<s32> spacing = core::dimension2d<s32>(0,0),
		video::ITexture* texture = NULL
		); 

} // end namespace gui
} // end namespace irr


