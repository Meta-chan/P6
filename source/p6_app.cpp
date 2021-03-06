/*
	This software is distributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty

	Created by Kyrylo Sovailo (github.com/Meta-chan, k.sovailo@gmail.com)
	Reinventing bicycles since 2020
*/

#include "../header/p6_app.hpp"
#include "../header/p6_frame.hpp"

wxIMPLEMENT_APP(p6::App);

bool p6::App::OnInit()
{
	wxImage::AddHandler(new wxPNGHandler);
	_frame = new Frame;
	return true;
}