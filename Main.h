#pragma once

#include <wx/filepicker.h>
#include "img.h"
#include "xml.h"

class Main : public wxFrame {
public:
	Main();

    img* imagefile = nullptr;
	xml* xmlfile = nullptr;

private:
    wxFileDialog* file = nullptr;
	wxFileDialog* code = nullptr;
	wxRadioBox* inputbox = nullptr;
	wxRadioBox* langbox = nullptr;

	int lang_sel = 0, inp_sel = 0;   // language and input type selected
	wxString filepath;   // path to input file
	wxString codepath;   // path to output file

	void OnMenuQuit(wxCommandEvent& evt);
	void OnGuidelines(wxCommandEvent& evt);
	void OnCreateCode(wxCommandEvent& evt);
	void OnChooseInp(wxCommandEvent& evt);
	void OnChooseCode(wxCommandEvent& evt);
	void OnInputbox(wxCommandEvent& evt);
	void OnLangbox(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();
};
