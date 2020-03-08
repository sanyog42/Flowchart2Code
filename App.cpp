#include "App.h"

wxIMPLEMENT_APP(App);

App::App() {

}

App::~App() {

}

bool App::OnInit() {
	main_frame = new Main();
	main_frame->Show();
	return true;
}