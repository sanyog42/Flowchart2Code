#pragma once

#include "CodeGenerate.h"

class xml {
public:
	xml(wxString filepath, wxString codepath, int lang_sel);
	~xml();

private:
	CodeGenerate* generate = nullptr;
};