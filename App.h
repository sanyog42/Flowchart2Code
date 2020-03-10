#pragma once

#include "Main.h"

class App : public wxApp {
public:
	App();
	~App();
	
	virtual bool OnInit();

private:
	Main* main_frame = nullptr;
};