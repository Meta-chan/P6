/*
	This software is distributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty

	Created by Kyrylo Sovailo (github.com/Meta-chan, k.sovailo@gmail.com)
	Reinventing bicycles since 2020
*/

#include "../header/p6_force_bar.hpp"
#include "../header/p6_frame.hpp"

void p6::Frame::_on_paint(wxPaintEvent &e)
{
	wxPaintDC dc(_frame);
	_main_panel.render(&dc, _main_panel.offset());
}

void p6::Frame::_on_size(wxSizeEvent &e)
{
	_main_panel.need_refresh();
	_frame->Layout();
}

p6::Frame::Frame() :
	_frame(new wxFrame(nullptr, wxID_ANY, "P6")),
	_sizer(new wxBoxSizer(wxHORIZONTAL)),
	_menubar(this),
	_toolbar(this),
	_main_panel(this),
	_side_panel(this),
	_mouse(this)
{
	_frame->Bind(wxEVT_PAINT, &Frame::_on_paint, this, _frame->GetId());
	_frame->Bind(wxEVT_SIZE, &Frame::_on_size, this, _frame->GetId());
}

wxFrame *p6::Frame::frame()
{
	return _frame;
}

wxBoxSizer *p6::Frame::sizer()
{
	return _sizer;
}

p6::ToolBar *p6::Frame::toolbar()
{
	return &_toolbar;
}

p6::MenuBar *p6::Frame::menubar()
{
	return &_menubar;
}

p6::MainPanel *p6::Frame::main_panel()
{
	return &_main_panel;
}

p6::SidePanel *p6::Frame::side_panel()
{
	return &_side_panel;
}

p6::Mouse *p6::Frame::mouse()
{
	return &_mouse;
}

p6::Construction *p6::Frame::construction()
{
	return &_construction;
}