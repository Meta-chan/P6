/*
	This software is distributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty

	Created by Kyrylo Sovailo (github.com/Meta-chan, k.sovailo@gmail.com)
	Reinventing bicycles since 2020
*/

#include "../header/p6_material_bar.hpp"
#include "../header/p6_frame.hpp"
#include "../header/p6_linear_material.hpp"
#include "../header/p6_nonlinear_material.hpp"

void p6::MaterialBar::_on_material_choice(wxCommandEvent &e)
{
	int c = _material_choice->GetSelection();
	if (c == wxNOT_FOUND)
	{
		_name_text->ChangeValue("");
		_nonlinear_check->SetValue(false);
		_formula_text->ChangeValue("");
	}
	else
	{
		Construction *con = _frame->construction();
		_name_text->ChangeValue(con->get_material_name(c));
		bool linear = (con->get_material_type(c) == Material::Type::linear);
		_nonlinear_check->SetValue(!linear);
		_formula_text->ChangeValue(linear ?
			real_to_string(con->get_material_modulus(c)) :
			con->get_material_formula(c));
	}
}

void p6::MaterialBar::_on_material_name(wxCommandEvent &e)
{}

void p6::MaterialBar::_on_material_new(wxCommandEvent &e)
{
	_material_choice->SetSelection(wxNOT_FOUND);
	_name_text->ChangeValue("");
	_nonlinear_check->SetValue(false);
	_formula_text->ChangeValue("");
}

void p6::MaterialBar::_on_material_apply(wxCommandEvent &e)
{
	try
	{
		Construction *con = _frame->construction();
		String name = _name_text->GetValue().ToStdString();
		String formula = _formula_text->GetValue().ToStdString();
		if (_nonlinear_check->GetValue()) con->create_nonlinear_material(name, formula);
		else con->create_linear_material(name, string_to_real(formula));
	}
	catch (std::exception &e)
	{
		wxMessageBox(e.what(), "Error", wxICON_ERROR, _frame->frame());
	}
}

void p6::MaterialBar::_on_material_delete(wxCommandEvent &e)
{
	int c = _material_choice->GetSelection();
	if (c != wxNOT_FOUND)
	{
		Construction *con = _frame->construction();
		for (uint i = 0; i < con->get_stick_count(); i++)
		{
			if (con->get_stick_material(i) == c) con->set_stick_material(i, c);
		}
		con->delete_material(c);
	}
}

void p6::MaterialBar::_on_material_nonlinear(wxCommandEvent &e)
{}

void p6::MaterialBar::_on_material_formula(wxCommandEvent &e)
{}

p6::MaterialBar::MaterialBar(Frame *frame)
{
	_frame = frame;
	wxWindow *parent = frame->side_panel()->panel();

	//Name static text
	_name_static = new wxStaticText(parent, wxID_ANY, "Name:");
	//Name edit
	_name_text = new wxTextCtrl(parent, wxID_ANY);
	parent->Bind(wxEVT_TEXT, &MaterialBar::_on_material_name, this, _name_text->GetId());
	//Nonlinear check
	_nonlinear_check = new wxCheckBox(parent, wxID_ANY, "Non-linear");
	parent->Bind(wxEVT_CHECKBOX, &MaterialBar::_on_material_nonlinear, this, _nonlinear_check->GetId());
	//Formula edit
	_formula_text = new wxTextCtrl(parent, wxID_ANY);
	parent->Bind(wxEVT_TEXT, &MaterialBar::_on_material_formula, this, _formula_text->GetId());
	//New-accept-delete buttons
	_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *button = new wxButton(parent, wxID_ANY, "New");
	_buttons_sizer->Add(button, 1, wxALL, 10);
	parent->Bind(wxEVT_BUTTON, &MaterialBar::_on_material_new, this, button->GetId());
	button = new wxButton(parent, wxID_ANY, "Apply");
	_buttons_sizer->Add(button, 1, wxALL, 10);
	parent->Bind(wxEVT_BUTTON, &MaterialBar::_on_material_apply, this, button->GetId());
	button = new wxButton(parent, wxID_ANY, "Delete");
	_buttons_sizer->Add(button, 1, wxALL, 10);
	parent->Bind(wxEVT_BUTTON, &MaterialBar::_on_material_delete, this, button->GetId());
}

void p6::MaterialBar::show()
{
	wxBoxSizer *sizer = _frame->side_panel()->sizer();
	sizer->Add(_material_choice, 0, wxLEFT | wxEXPAND, 10);
	sizer->Add(_name_static, 0, wxLEFT | wxEXPAND, 10);
	sizer->Add(_name_text, 0, wxALL | wxEXPAND, 10);
	sizer->Add(_nonlinear_check, 0, wxALL, 10);
	sizer->Add(_formula_static, 0, wxLEFT | wxEXPAND, 10);
	sizer->Add(_formula_text, 0, wxLEFT | wxEXPAND, 10);
	sizer->Add(_buttons_sizer, 0, wxALL | wxEXPAND, 10);
}

void p6::MaterialBar::refresh_materials()
{
	wxWindow *parent = _frame->side_panel()->panel();

	wxArrayString array;
	array.Alloc(_frame->construction()->get_material_count());
	for (uint i = 0; i < _frame->construction()->get_material_count(); i++)
		array.Add(_frame->construction()->get_material_name(i));
	_material_choice = new wxChoice(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, array);
	parent->Bind(wxEVT_CHOICE, &MaterialBar::_on_material_choice, this, _material_choice->GetId());
}

void p6::MaterialBar::hide()
{
	wxWindow *parent = _frame->side_panel()->panel();
	parent->Unbind(wxEVT_CHOICE, &MaterialBar::_on_material_choice, this, _material_choice->GetId());
	delete _material_choice;
}