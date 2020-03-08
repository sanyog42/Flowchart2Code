#pragma once

#include <wx/filepicker.h>
#include "img.h"
#include "xml.h"

class Main : public wxFrame {
public:
	Main();
	~Main();
	wxFileDialog* file;
	wxFileDialog* code;
	wxRadioBox* inputbox;
	wxRadioBox* langbox;
	int lang_sel, inp_sel;
	wxString filepath;
	wxString codepath;

private:
	img* imagefile = nullptr;
	xml* xmlfile = nullptr;

	void OnMenuQuit(wxCommandEvent& evt);
	void OnGuidelines(wxCommandEvent& evt);
	void OnCreateCode(wxCommandEvent& evt);
	void OnChooseInp(wxCommandEvent& evt);
	void OnChooseCode(wxCommandEvent& evt);
	void OnInputbox(wxCommandEvent& evt);
	void OnLangbox(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();
};