#pragma once

#include "wx/wx.h"
#include <fstream>
#include <sstream>

struct node {
	int type = -1;
	int id = -1;
	std::string text = "";
	node* right = NULL;
	node* down = NULL;
	node* loop = NULL;
};

class CodeGenerate {
public:
	CodeGenerate(wxString codepath, node* start, int lang_sel);

	std::ofstream code;

private:
    node* first = nullptr;   // pointer to the starting node
	int codeflag = 0;   // whether inside valid nodes
	std::vector<std::pair<int, int>> loops;   // for storing all loops detected
	std::vector<std::pair<int, int>> connectors;   // for storing all connectors detected
	std::vector<int> used;
	std::vector<std::pair<std::string, int>> variables;   // for storing variables and their types

	//void print(struct node* temp);   // for debugging
	void markloops(struct node* temp);
	void find_connectors(struct node* temp);
	struct node* nodebyid(struct node* temp, int id);
	struct node* traverse_c(struct node* temp, int level);
	struct node* traverse_cpp(struct node* temp, int level);
	struct node* traverse_py(struct node* temp, int level);
};
