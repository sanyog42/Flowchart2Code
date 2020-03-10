#pragma once

#include "wx/wx.h"
#include <fstream>
#include <sstream>

struct node {
	int type;
	int id;
	std::string text = "";
	node* right = NULL;
	node* down = NULL;
	node* loop = NULL;
};

class CodeGenerate {
public:
	CodeGenerate(wxString codepath, node* start, int lang_sel);
	~CodeGenerate();
	std::ofstream code;

private:
	int codeflag = 0;
	std::vector<std::pair<int, int>> loops;
	std::vector<std::pair<int, int>> connectors;
	std::vector<int> used;
	std::vector<std::pair<std::string, int>> variables;
	//void print(struct node* temp); // for debugging
	void markloops(struct node* temp);
	void find_connectors(struct node* temp);
	struct node* traverse_c(struct node* temp, int level);
	struct node* traverse_cpp(struct node* temp, int level);
	struct node* traverse_py(struct node* temp, int level);
};

