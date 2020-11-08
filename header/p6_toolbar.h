#ifndef P6_TOOLBAR
#define P6_TOOLBAR

#include "p6_frame.h"
#include <wx/wx.h>

namespace p6
{
	class ToolBar
	{
	private:
		Frame *_frame;
		wxToolBar *_toolbar;
		wxToolBarToolBase *_simulate;
		wxToolBarToolBase *_select;
		wxToolBarToolBase *_area;
		wxToolBarToolBase *_move;
		wxToolBarToolBase *_node;
		wxToolBarToolBase *_stick;
		wxToolBarToolBase *_force;
		wxToolBarToolBase *_delete;
		void _on_simulate(wxCommandEvent &e);
		void _on_select(wxCommandEvent &e);
		void _on_area(wxCommandEvent &e);
		void _on_move(wxCommandEvent &e);
		void _on_node(wxCommandEvent &e);
		void _on_stick(wxCommandEvent &e);
		void _on_force(wxCommandEvent &e);
		void _on_delete(wxCommandEvent &e);

	public:
		enum class Tool
		{
			no,
			simulate,
			select,
			area,
			move,
			node,
			stick,
			force,
			delet
		};

		Tool tool = Tool::no;
		ToolBar(Frame *frame);
	};
};

#endif