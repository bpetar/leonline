/** 
 * \file CGUITreeCtrl.h
 * \brief File CGUITreeCtrl.h is taken from irrlicht forum. Tree class is created by Veylon.
 * link: http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=17422&highlight=cguitreectrl
 * I changed that tree code little bit to suite my needs.
 * Big thanks to Veylon.
 */

#pragma once

#include "IGUITreeCtrl.h"
#include <IGUIScrollbar.h>
#include <IrrArray.h>
#include <SColor.h>
#include <IVideoDriver.h>

class CEditorManager;

namespace irr
{
namespace gui
{
	//! tree GUI element.
	class CGUITreeCtrl : public IGUITreeCtrl
	{
		friend IGUITreeCtrl *AddGUITreeCtrl(
		IGUIEnvironment *env,
		const core::rect<s32> Rectangle,
		IGUIElement *parent,
		s32 id,
		bool dialog
			);
		public:

			//! destructor
			~CGUITreeCtrl();

			virtual inline u32 GetNumItems() {return NumItems;};
			
			virtual TreeNode *FindNode(u32 ID);
			virtual TreeNode *GetNextNode(TreeNode *node);
			virtual TreeNode *GetPrevNode(TreeNode *node);

			virtual TreeNode *GetNextVisibleNode(TreeNode *node);
			virtual TreeNode *GetPrevVisibleNode(TreeNode *node);
			
			virtual TreeNode *InsertNode(core::stringw Text, TreeNode *Parent = NULL, u32 ID = 0, void *Data = NULL, wchar_t IconSingle = L'\0', wchar_t IconOpen = L'-', wchar_t IconClosed = L'+');
			
			virtual bool DeleteNode(TreeNode *node);
			virtual void Clear();

			virtual bool ChangeParent(TreeNode *node, TreeNode *newparent);

			virtual inline TreeNode *GetRootNode() {return RootNode;};

			virtual void draw();
			
			bool OnEvent(const SEvent& event);

			virtual bool SetSelected(TreeNode *node);
			virtual TreeNode *GetSelected();
			virtual void OpenNode(TreeNode *node);
			virtual void CloseNode(TreeNode *node);
			
			virtual bool SetIconFont(IGUIFont *font);
			virtual IGUIFont *GetIconFont();
			
			virtual bool SetOverrideFont(IGUIFont *font);
			virtual IGUIFont *GetOverrideFont();

			bool m_Dialog;
			bool m_picked;
			TreeNode *m_SavedNode;

		protected:
			void recalculateItemHeight();
			void recalculateScrollBars();
			
			void CutTies(TreeNode *node);
			void SetPlace(TreeNode *node, TreeNode *newparent);
			void SetPlace(TreeNode *node, TreeNode *newparent, TreeNode *prevsibling);
			
			void DrawVerticalLine(core::position2d<s32> start, s32 length, video::SColor &color, const core::rect<s32> *ClipRect = NULL);
			void DrawHorizontalLine(core::position2d<s32> start, s32 length, video::SColor &color, const core::rect<s32> *ClipRect = NULL);

			
			TreeNode *Selected;
			s32 ItemHeight;	
								//	4		8
			s32 Frac18;			//  1/4		1/8
			s32 Frac12;			//	1/2		1/2
			s32 Frac78;			//	3/4		7/8
			s32 SqSize;			//	1/2		3/4
			//s32 IconWidth;
			TreeNode *RootNode;
			u32 NumItems;
			core::array<u32> IDList;
			
			IGUIFont *Font;
			IGUIFont *IconFont;
			IGUIFont *OverrideFont;
			IGUIScrollBar* ScrollBarV;
			IGUIScrollBar* ScrollBarH;
			video::IVideoDriver *video;
		protected:
						//! constructor
			CGUITreeCtrl(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle, bool dialog);

	};


} // end namespace gui
} // end namespace irr