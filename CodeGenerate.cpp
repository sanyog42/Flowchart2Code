#include "CodeGenerate.h"

CodeGenerate::CodeGenerate(wxString codepath, node* start, int lang_sel) {
	std::string path = std::string(codepath.mb_str());
	code.open(path, std::ios::out | std::ios::trunc);
	markloops(start);
	if (lang_sel == 0) {
		code << "#include <stdio.h>\n\n";
		traverse_c(start, 0);
	}
	else if (lang_sel == 1) {
		code << "#include <iostream>\n";
		code << "using namespace std;\n\n";
		traverse_cpp(start, 0);
	}
	else {
		traverse_py(start, 0);
	}
	if (codeflag == 0 && (lang_sel==0 || lang_sel==1)) {
		code << "\t" << "return 0;\n}";
	}
	code.close();
	wxLaunchDefaultApplication(path);
}

CodeGenerate::~CodeGenerate() {

}

void CodeGenerate::markloops(struct node* temp) {
	if (temp->loop != NULL) {
		std::pair<int, int> x;
		node* l = temp->loop;
		x.first = temp->id;
		x.second = l->id;
		loops.push_back(x);
		used.push_back(0);
	}
	if (temp->right != NULL) {
		markloops(temp->right);
	}
	if (temp->down != NULL) {
		markloops(temp->down);
	}
}

void CodeGenerate::traverse_c(struct node* temp, int level) {
	int loopflag = -1;
	for (int i = 0; i < loops.size(); i++) {
		if (loops[i].second == temp->id && used[i] == 0) {
			if (temp->type == 5) {
				loopflag = 0;
			}
			else {
				loopflag = 1;
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "do {\n";
				level++;
			}
			used[i] = 1;
			break;
		}
		if (loops[i].first == temp->id && used[i] == 1) {
			if (temp->type == 5) {
				loopflag = 2;
				level--;
				used[i] = 2;
			}
			break;
		}
	}
	for (int i = 0; i < level; i++) {
		code << "\t";
	}
	if (temp->type == 7) {
		if (temp->text == "START") {
			code << "int main() {";
			level++;
		}
		else if (temp->text == "END") {
			code << "return 0;";
			if (level == 1) {
				code << "\n}";
				codeflag = 1;
			}
		}
	}
	else if (temp->type == 6) {
		std::stringstream var(temp->text);
		std::string word;
		std::string varname="";
		int vartype = 0;
		int flag = 0;
		int stringflag = 0;
		while (var >> word) {
			if (word == "int") {
				vartype = 1;
				flag = 1;
			}
			else if (word == "char") {
				vartype = 2;
				flag = 1;
			}
			else if (word == "string") {
				vartype = 3;
				flag = 1;
				stringflag = 1;
			}
			else if (word == "float") {
				vartype = 4;
				flag = 1;
			}
			else {
				if (flag == 1) {
					for (int i = 0; word[i] != '\0' && word[i] != '='; i++) {
						varname += word[i];
					}
				}
			}
		}
		if (flag == 1) {
			std::pair<std::string, int> varinf;
			varinf.first = varname;
			varinf.second = vartype;
			variables.push_back(varinf);
		}
		if (stringflag == 0) {
			code << temp->text << ";";
		}
		else {
			code << "char " << varname << "[50];";
		}
	}
	else if (temp->type == 4) {
		std::stringstream str(temp->text);
		std::string word;
		std::string expr = "";
		std::string arg = "";
		int flag = 0;
		int type = -1;
		int vartype = 0;
		while (str >> word) {
			if (word == "OUTPUT") {
				code << "printf(";
				type = 0;
			}
			else if (word == "INPUT") {
				code << "scanf(";
				type = 1;
			}
			else {
				if (word[0] == '\"') {
					flag = 1;
				}

				if (flag == 1) {
					code << word << " ";
				}
				else {
					for (int i = 0; i < variables.size(); i++) {
						if (word == variables[i].first) {
							vartype = variables[i].second;
						}
					}

					if (vartype == 1) {
						expr += "%d";
					}
					else if (vartype == 2) {
						expr += "%c";
					}
					else if (vartype == 3) {
						expr += "%s";
					}
					else if (vartype == 4) {
						expr += "%f";
					}

					arg += ", ";
					if (type == 1 && vartype != 3) {
						arg += "&";
					}
					arg += word;
				}
			}
		}
		if (flag == 0) {
			code << "\"" << expr << "\"" << arg;
		}
		code << ");";
	}
	else if (temp->type == 5) {
		std::string validated = "";
		for (int i = 0; temp->text[i] != '\0'; i++) {
			if (temp->text[i] == '=') {
				if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '=') {
					validated += '=';
				}
			}
			validated += temp->text[i];
		}
		if (loopflag == -1) {
			code << "if(" << validated << ") {";
		}
		else if (loopflag == 0) {
			code << "while(" << validated << ") {";
		}
		else if (loopflag == 2) {
			code << "} while(" << validated << ");";
		}
	}
	code << "\n";
	if (temp->type == 5) {
		if (loopflag == 2) {
			if (temp->down != NULL) {
				traverse_c(temp->down, level);
			}
			else if (temp->right != NULL) {
				traverse_c(temp->right, level);
			}
		}
		else if (loopflag == 0) {
			if (temp->down != NULL) {
				traverse_c(temp->down, level + 1);
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "}\n";
			}
			if (temp->right != NULL) {
				traverse_c(temp->right, level);
			}
		}
		else {
			if (temp->down != NULL) {
				traverse_c(temp->down, level + 1);
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "}\n";
			}
			if (temp->right != NULL) {
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "else {\n";
				traverse_c(temp->right, level + 1);
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "}\n";
			}
		}
	}
	else {
		if (temp->right != NULL) {
			traverse_c(temp->right, level);
		}
		else if (temp->down != NULL) {
			traverse_c(temp->down, level);
		}
	}
}

void CodeGenerate::traverse_cpp(struct node* temp, int level) {
	int loopflag = -1;
	for (int i = 0; i < loops.size(); i++) {
		if (loops[i].second == temp->id && used[i] == 0) {
			if (temp->type == 5) {
				loopflag = 0;
			}
			else {
				loopflag = 1;
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "do {\n";
				level++;
			}
			used[i] = 1;
			break;
		}
		if (loops[i].first == temp->id && used[i] == 1) {
			if (temp->type == 5) {
				loopflag = 2;
				level--;
				used[i] = 2;
			}
			break;
		}
	}
	for (int i = 0; i < level; i++) {
		code << "\t";
	}
	if (temp->type == 7) {
		if (temp->text == "START") {
			code << "int main() {";
			level++;
		}
		else if (temp->text == "END") {
			code << "return 0;";
			if (level == 1) {
				code << "\n}";
				codeflag = 1;
			}
		}
	}
	else if (temp->type == 6) {
		code << temp->text << ";";
	}
	else if (temp->type == 4) {
		std::stringstream str(temp->text);
		std::string word;
		while (str >> word) {
			if (word == "OUTPUT") {
				code << "cout << ";
			}
			else if (word == "INPUT") {
				code << "cin >> ";
			}
			else {
				code << word << " ";
			}
		}
		code << ";";
	}
	else if (temp->type == 5) {
		std::string validated = "";
		for (int i = 0; temp->text[i] != '\0'; i++) {
			if (temp->text[i] == '=') {
				if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '=') {
					validated += '=';
				}
			}
			validated += temp->text[i];
		}
		if (loopflag == -1) {
			code << "if(" << validated << ") {";
		}
		else if (loopflag == 0) {
			code << "while(" << validated << ") {";
		}
		else if (loopflag == 2) {
			code << "} while(" << validated << ");";
		}
	}
	code << "\n";
	if (temp->type == 5) {
		if (loopflag == 2) {
			if (temp->down != NULL) {
				traverse_cpp(temp->down, level);
			}
			else if (temp->right != NULL) {
				traverse_cpp(temp->right, level);
			}
		}
		else if (loopflag == 0) {
			if (temp->down != NULL) {
				traverse_cpp(temp->down, level + 1);
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "}\n";
			}
			if (temp->right != NULL) {
				traverse_cpp(temp->right, level);
			}
		}
		else {
			if (temp->down != NULL) {
				traverse_cpp(temp->down, level + 1);
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "}\n";
			}
			if (temp->right != NULL) {
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "else {\n";
				traverse_cpp(temp->right, level + 1);
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "}\n";
			}
		}
	}
	else {
		if (temp->right != NULL) {
			traverse_cpp(temp->right, level);
		}
		else if (temp->down != NULL) {
			traverse_cpp(temp->down, level);
		}
	}
}

void CodeGenerate::traverse_py(struct node* temp, int level) {
	int loopflag = -1;
	int nl = 1;
	for (int i = 0; i < loops.size(); i++) {
		if (loops[i].second == temp->id && used[i] == 0) {
			if (temp->type == 5) {
				loopflag = 0;
			}
			else {
				loopflag = 1;
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "while True:\n";
				level++;
			}
			used[i] = 1;
			break;
		}
		if (loops[i].first == temp->id && used[i] == 1) {
			if (temp->type == 5) {
				loopflag = 2;
				used[i] = 2;
			}
			break;
		}
	}
	for (int i = 0; i < level; i++) {
		code << "\t";
	}
	if (temp->type == 6) {
		std::stringstream var(temp->text);
		std::string word;
		std::string out = "";
		std::string varname = "";
		int vartype = 0;
		int flag = 0;
		int outflag = 0;
		while (var >> word) {
			if (word == "int") {
				vartype = 1;
				flag = 1;
			}
			else if (word == "char") {
				vartype = 2;
				flag = 1;
			}
			else if (word == "string") {
				vartype = 3;
				flag = 1;
			}
			else if (word == "float") {
				vartype = 4;
				flag = 1;
			}
			else {
				for (int i = 0; word[i] != '\0'; i++) {
					if (word[i] == '=') {
						outflag = 1;
					}
					if (outflag == 0) {
						varname += word[i];
					}
					out += word[i];
				}
			}
		}
		if (flag == 1) {
			std::pair<std::string, int> varinf;
			varinf.first = varname;
			varinf.second = vartype;
			variables.push_back(varinf);
		}
		if (outflag == 1) {
			code << out;
		}
		else {
			nl = 0;
		}
	}
	else if (temp->type == 7) {
		if (temp->text == "END" && level > 0) {
			code << "exit()";
		}
	}
	else if (temp->type == 4) {
		std::stringstream str(temp->text);
		std::string word;
		int type = -1;
		int vartype = 0;
		while (str >> word) {
			if (word == "OUTPUT") {
				code << "print(";
				type = 0;
			}
			else if (word == "INPUT") {
				type = 1;
			}
			else {
				if (type == 0) {
					code << word << ")";
				}
				else if (type == 1) {
					code << word << " = ";
					for (int i = 0; i < variables.size(); i++) {
						if (word == variables[i].first) {
							vartype = variables[i].second;
						}
					}
					if (vartype == 1) {
						code << "int(input())";
					}
					else if (vartype == 2) {
						code << "chr(input())";
					}
					else if (vartype == 3) {
						code << "str(input())";
					}
					else if (vartype == 4) {
						code << "float(input())";
					}
					else {
						code << "input()";
					}
				}
			}
		}
	}
	else if (temp->type == 5) {
		std::string validated = "";
		for (int i = 0; temp->text[i] != '\0'; i++) {
			if (temp->text[i] == '=') {
				if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '=') {
					validated += '=';
				}
			}
			validated += temp->text[i];
		}
		if (loopflag == -1) {
			code << "if (" << validated << "):";
		}
		else if (loopflag == 0) {
			code << "while (" << validated << "):";
		}
		else if (loopflag == 2) {
			code << "if (" << validated << "):\n";
			for (int i = 0; i < level + 1; i++) {
				code << "\t";
			}
			code << "break";
			level--;
		}
	}
	if (temp->type != 7 && nl == 1) {
		code << "\n";
	}
	if (temp->type == 5) {
		if (loopflag == 2) {
			if (temp->down != NULL) {
				traverse_py(temp->down, level);
			}
			else if (temp->right != NULL) {
				traverse_py(temp->right, level);
			}
		}
		else if (loopflag == 0) {
			if (temp->down != NULL) {
				traverse_py(temp->down, level + 1);
			}
			if (temp->right != NULL) {
				traverse_py(temp->right, level);
			}
		}
		else {
			if (temp->down != NULL) {
				traverse_py(temp->down, level + 1);
			}
			if (temp->right != NULL) {
				for (int i = 0; i < level; i++) {
					code << "\t";
				}
				code << "else :\n";
				traverse_py(temp->right, level + 1);
			}
		}
	}
	else {
		if (temp->right != NULL) {
			traverse_py(temp->right, level);
		}
		else if (temp->down != NULL) {
			traverse_py(temp->down, level);
		}
	}
}