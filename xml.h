#pragma once

#include "CodeGenerate.h"

class xml {
public:
    xml(wxString filepath, wxString codepath, int lang_sel);

    node* start1 = nullptr;

private:
	CodeGenerate* generate = nullptr;

	struct node* traverse1(struct node* temp, int id1);
};
