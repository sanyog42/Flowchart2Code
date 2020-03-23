#include "Main.h"

wxBEGIN_EVENT_TABLE(Main, wxFrame)
    EVT_MENU(101, Main::OnMenuQuit)
wxEND_EVENT_TABLE()

Main::Main() : wxFrame(nullptr, wxID_ANY, "Flowchart2Code") {

#ifndef __WXOSX__
    wxIcon mainicon;
    mainicon.LoadFile("Flowchart2Code.ico", wxBITMAP_TYPE_ICO);
    SetIcon(mainicon);
#endif

    wxMenuBar* menubar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    
    fileMenu->Append(101, "Quit");
    menubar->Append(fileMenu, "File");
    
    SetMenuBar(menubar);

    wxFont font_head = wxFont(26, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    wxFont font_head2 = wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    wxFont font_head3 = wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);

    wxString lang[] = { "C", "C++", "Python" };
    wxString input_type[] = { "Image", "XML" };

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* heading = new wxStaticText(this, 501, "FLOWCHART2CODE", wxDefaultPosition, wxDefaultSize);
    wxStaticText* heading2 = new wxStaticText(this, 502, "Created By: Sanyog Gupta", wxDefaultPosition, wxDefaultSize);
    wxStaticText* heading3 = new wxStaticText(this, 503, "Select Input File Location:", wxDefaultPosition, wxDefaultSize);
    wxStaticText* heading4 = new wxStaticText(this, 504, "Select Code File Location:", wxDefaultPosition, wxDefaultSize);

    Main::file = new wxFileDialog(this, "Open Input File", wxEmptyString, wxEmptyString, "JPG files (*.jpg)|*.jpg", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    Main::code = new wxFileDialog(this, "Save Code As", wxEmptyString, wxEmptyString, "C files (*.c)|*.c", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    Main::inputbox = new wxRadioBox(this, 301, "Input Type", wxDefaultPosition, wxDefaultSize, 2, input_type, 2, wxRA_SPECIFY_COLS);
    Main::langbox = new wxRadioBox(this, 302, "Language", wxDefaultPosition, wxDefaultSize, 3, lang, 3, wxRA_SPECIFY_COLS);
    wxButton* gen = new wxButton(this, 201, "Generate Code");
    wxButton* guidelines = new wxButton(this, 202, "Guidelines");
    wxButton* choosefile = new wxButton(this, 203, "Select Input File");
    wxButton* choosecode = new wxButton(this, 204, "Select Output File");
    
    gen->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Main::OnCreateCode, this);
    choosefile->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Main::OnChooseInp, this);
    choosecode->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Main::OnChooseCode, this);
    inputbox->Bind(wxEVT_RADIOBOX, &Main::OnInputbox, this);
    langbox->Bind(wxEVT_RADIOBOX, &Main::OnLangbox, this);
    guidelines->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Main::OnGuidelines, this);
    
    heading->SetFont(font_head);
    heading2->SetFont(font_head2);
    heading3->SetFont(font_head3);
    heading4->SetFont(font_head3);

    sizer->Add(heading, 0, wxALIGN_CENTER | wxUP, 30);
    sizer->Add(heading2, 0, wxALIGN_CENTER | wxDOWN, 30);
    sizer->Add(guidelines, 0, wxEXPAND | wxLEFT | wxRIGHT, 20);
    sizer->Add(inputbox, 0, wxEXPAND | wxLEFT | wxRIGHT, 20);
    sizer->Add(heading3, 0, wxALIGN_LEFT | wxUP | wxLEFT, 20);
    sizer->Add(choosefile, 0, wxEXPAND | wxLEFT | wxRIGHT, 20);
    sizer->Add(langbox, 0, wxEXPAND | wxALL, 20);
    sizer->Add(heading4, 0, wxALIGN_LEFT | wxLEFT, 20);
    sizer->Add(choosecode, 0, wxEXPAND | wxLEFT | wxRIGHT | wxDOWN, 20);
    sizer->Add(gen, 0, wxEXPAND, 0);
    
    sizer->SetSizeHints(this);
    SetSizer(sizer);   
}

Main::~Main() {

}

void Main::OnMenuQuit(wxCommandEvent& evt) {
    Close();
    evt.Skip();
}

void Main::OnCreateCode(wxCommandEvent& evt) {
    Main::filepath = file->GetPath();
    Main::codepath = code->GetPath();
    std::string check1 = std::string(codepath.mb_str());
    if (check1.empty()) {
        wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Output File Not Selected!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
        dial->ShowModal();
    }
    else {
        Main::lang_sel = langbox->GetSelection();
        Main::inp_sel = inputbox->GetSelection();
        std::string path = std::string(filepath.mb_str());
        std::ifstream check(path);
        if (!check) {
            wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Input File Not Found!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
            dial->ShowModal();
        }
        else {
            if (Main::inp_sel == 0) {
                imagefile = new img(Main::filepath, Main::codepath, Main::lang_sel);
            }
            else if (Main::inp_sel == 1) {
                xmlfile = new xml(Main::filepath, Main::codepath, Main::lang_sel);
            }
        }
        check.close();
    }
    evt.Skip();
}

void Main::OnGuidelines(wxCommandEvent& evt) {
    std::ifstream check("Guidelines.pdf");
    if (!check) {
        wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Guidelines File Not Found!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
        dial->ShowModal();
    }
    else {
        wxLaunchDefaultApplication("Guidelines.pdf");
    }
    check.close();
    evt.Skip();
}

void Main::OnChooseInp(wxCommandEvent& evt) {
    Main::file->ShowModal();
    evt.Skip();
}

void Main::OnChooseCode(wxCommandEvent& evt) {
    Main::code->ShowModal();
    evt.Skip();
}

void Main::OnInputbox(wxCommandEvent& evt) {
    Main::inp_sel = inputbox->GetSelection();
    Main::file->SetPath("");
    if (Main::inp_sel == 0) {
        Main::file->SetWildcard("JPG files (*.jpg)|*.jpg");
    }
    else {
        Main::file->SetWildcard("XML files (*.xml)|*.xml");
    }
    evt.Skip();
}

void Main::OnLangbox(wxCommandEvent& evt) {
    Main::lang_sel = langbox->GetSelection();
    Main::code->SetPath("");
    if (Main::lang_sel == 0) {
        Main::code->SetWildcard("C files (*.c)|*.c");
    }
    else if (Main::lang_sel == 1) {
        Main::code->SetWildcard("C++ files (*.cpp)|*.cpp");
    }
    else {
        Main::code->SetWildcard("Python files (*.py)|*.py");
    }
    evt.Skip();
}
