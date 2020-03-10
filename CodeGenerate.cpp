#include "CodeGenerate.h"

CodeGenerate::CodeGenerate(wxString codepath, node* start, int lang_sel) {
	first = start;
	std::string path = std::string(codepath.mb_str());
	code.open(path, std::ios::out | std::ios::trunc);

	markloops(start);
	find_connectors(start);

	if (lang_sel == 0) {
		code << "#include <stdio.h>\n\n";
		node* init = traverse_c(start, 0);
	}
	else if (lang_sel == 1) {
		code << "#include <iostream>\n";
		code << "using namespace std;\n\n";
		node* init = traverse_cpp(start, 0);
	}
	else {
		node* init = traverse_py(start, 0);
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

void CodeGenerate::find_connectors(struct node* temp) {
	int found = 0;
	
	if (temp->type == 3) {
		for (int i = 0; i < connectors.size(); i++) {
			if (connectors[i].first == temp->id) {
				found = 1;
				break;
			}
		}
		if (found == 0) {
			std::pair<int, int> connector;
			connector.first = temp->id;
			connector.second = 0;
			connectors.push_back(connector);
		}
		else {
			return;
		}
	}

	if (temp->right != NULL) {
		find_connectors(temp->right);
	}

	if (temp->down != NULL) {
		find_connectors(temp->down);
	}
}

struct node* CodeGenerate::traverse_c(struct node* temp, int level) {
	node* run = NULL;
	int loopflag = -1;
	if (temp->type == 3) {
		int status = 0;
		for (int i = 0; i < connectors.size(); i++) {
			if (connectors[i].first == temp->id) {
				if (connectors[i].second == 0) {
					connectors[i].second = 1;
				}
				else {
					status = 1;
				}
				break;
			}
		}
		if (status == 1) {
			if (temp->right != NULL) {
				return (temp->right);
			}
			else if (temp->down != NULL) {
				return (temp->down);
			}
		}
		else {
			return NULL;
		}
	}
	else {
		for (int i = 0; i < loops.size(); i++) {
			if (loops[i].second == temp->id && used[i] == 0) {
				node* x = nodebyid(first, loops[i].first);
				if (temp->type == 5 && x->type != 5) {
					loopflag = 0;
				}
				else {
					if (temp->type != 5) {
						loopflag = 1;
					}
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
			std::string varname = "";
			int vartype = 0;
			int flag = 0;
			int stringflag = 0;
			while (var >> word) {
				if (word == "int" || word == "INT" || word == "Int") {
					vartype = 1;
					flag = 1;
					code << "int";
				}
				else if (word == "char" || word == "CHAR" || word == "Char") {
					vartype = 2;
					flag = 1;
					code << "char";
				}
				else if (word == "string" || word == "STRING" || word == "String") {
					vartype = 3;
					flag = 1;
					stringflag = 1;
				}
				else if (word == "float" || word == "FLOAT" || word == "Float") {
					vartype = 4;
					flag = 1;
					code << "float";
				}
				else {
					if (flag == 1) {
						if (stringflag == 0) {
							code << " " << word;
						}
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
			else {
				code << temp->text;
			}
			if (stringflag == 1) {
				code << "char " << varname << "[50]";
			}
			code << ";";
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
					code << "printf( ";
					type = 0;
				}
				else if (word == "INPUT") {
					code << "scanf( ";
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
				code << "\"" << expr << "\"" << arg << " ";
			}
			code << ");";
		}
		else if (temp->type == 5) {
			std::string validated = "";
			for (int i = 0; temp->text[i] != '\0'; i++) {
				if (temp->text[i] == '=') {
					if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '=' && temp->text[i - 1] != '!' && temp->text[i - 1] != '=') {
						validated += '=';
					}
				}
				if (temp->text[i] == '&') {
					if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '&' && temp->text[i - 1] != '&') {
						validated += '&';
					}
				}
				if (temp->text[i] == '|') {
					if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '|' && temp->text[i - 1] != '|') {
						validated += '|';
					}
				}
				validated += temp->text[i];
			}
			
			std::stringstream str(validated);
			std::string word;
			std::string final = "";
			if (str >> word) {
				final += word;
				while (str >> word) {
					if (word == "and") {
						final += " &&";
					}
					else if (word == "or") {
						final += " ||";
					}
					else {
						final += " ";
						final += word;
					}
				}
			}
			
			if (loopflag == -1) {
				code << "if (" << final << ") {";
			}
			else if (loopflag == 0) {
				code << "while (" << final << ") {";
			}
			else if (loopflag == 2) {
				code << "} while (" << final << ");";
			}
		}

		code << "\n";

		if (temp->type == 5) {
			if (loopflag == 2) {
				if (temp->down != NULL) {
					run = traverse_c(temp->down, level);
				}
				else if (temp->right != NULL) {
					run = traverse_c(temp->right, level);
				}
			}
			else if (loopflag == 0) {
				if (temp->down != NULL) {
					run = traverse_c(temp->down, level + 1);
					for (int i = 0; i < level; i++) {
						code << "\t";
					}
					code << "}\n";
				}
				if (temp->right != NULL) {
					run = traverse_c(temp->right, level);
				}
			}
			else {
				if (temp->down != NULL) {
					run = traverse_c(temp->down, level + 1);
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
					run = traverse_c(temp->right, level + 1);
					for (int i = 0; i < level; i++) {
						code << "\t";
					}
					code << "}\n";
					if (run != NULL) {
						run = traverse_c(run, level);
					}
				}
			}
		}
		else {
			if (temp->right != NULL) {
				run = traverse_c(temp->right, level);
			}
			else if (temp->down != NULL) {
				run = traverse_c(temp->down, level);
			}
		}
	}
	return run;
}

struct node* CodeGenerate::traverse_cpp(struct node* temp, int level) {
	int loopflag = -1;
	node* run = NULL;
	if (temp->type == 3) {
		int status = 0;
		for (int i = 0; i < connectors.size(); i++) {
			if (connectors[i].first == temp->id) {
				if (connectors[i].second == 0) {
					connectors[i].second = 1;
				}
				else {
					status = 1;
				}
				break;
			}
		}
		if (status == 1) {
			if (temp->right != NULL) {
				return (temp->right);
			}
			else if (temp->down != NULL) {
				return (temp->down);
			}
		}
		else {
			return NULL;
		}
	}
	else {
		for (int i = 0; i < loops.size(); i++) {
			if (loops[i].second == temp->id && used[i] == 0) {
				node* x = nodebyid(first, loops[i].first);
				if (temp->type == 5 && x->type != 5) {
					loopflag = 0;
				}
				else {
					if (temp->type != 5) {
						loopflag = 1;
					}
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
			int flag = 0;
			while (var >> word) {
				if (word == "int" || word == "INT" || word == "Int") {
					flag = 1;
					code << "int";
				}
				else if (word == "char" || word == "CHAR" || word == "Char") {
					flag = 1;
					code << "char";
				}
				else if (word == "string" || word == "STRING" || word == "String") {
					flag = 1;
				}
				else if (word == "float" || word == "FLOAT" || word == "Float") {
					flag = 1;
					code << "float";
				}
				else {
					if (flag == 1) {
						code << " " << word;
					}
				}
			}
			if (flag == 0) {
				code << temp->text;
			}
			code << ";";
		}
		else if (temp->type == 4) {
			std::stringstream str(temp->text);
			std::string word;
			while (str >> word) {
				if (word == "OUTPUT") {
					code << "cout <<";
				}
				else if (word == "INPUT") {
					code << "cin >>";
				}
				else {
					code << " " << word;
				}
			}
			code << ";";
		}
		else if (temp->type == 5) {
			std::string validated = "";
			for (int i = 0; temp->text[i] != '\0'; i++) {
				if (temp->text[i] == '=') {
					if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '=' && temp->text[i - 1] != '!' && temp->text[i - 1] != '=') {
						validated += '=';
					}
				}
				if (temp->text[i] == '&') {
					if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '&' && temp->text[i - 1] != '&') {
						validated += '&';
					}
				}
				if (temp->text[i] == '|') {
					if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '|' && temp->text[i - 1] != '|') {
						validated += '|';
					}
				}
				validated += temp->text[i];
			}

			std::stringstream str(validated);
			std::string word;
			std::string final = "";
			if (str >> word) {
				final += word;
				while (str >> word) {
					if (word == "and") {
						final += " &&";
					}
					else if (word == "or") {
						final += " ||";
					}
					else {
						final += " ";
						final += word;
					}
				}
			}

			if (loopflag == -1) {
				code << "if (" << final << ") {";
			}
			else if (loopflag == 0) {
				code << "while (" << final << ") {";
			}
			else if (loopflag == 2) {
				code << "} while (" << final << ");";
			}
		}

		code << "\n";

		if (temp->type == 5) {
			if (loopflag == 2) {
				if (temp->down != NULL) {
					run = traverse_cpp(temp->down, level);
				}
				else if (temp->right != NULL) {
					run = traverse_cpp(temp->right, level);
				}
			}
			else if (loopflag == 0) {
				if (temp->down != NULL) {
					run = traverse_cpp(temp->down, level + 1);
					for (int i = 0; i < level; i++) {
						code << "\t";
					}
					code << "}\n";
				}
				if (temp->right != NULL) {
					run = traverse_cpp(temp->right, level);
				}
			}
			else {
				if (temp->down != NULL) {
					run = traverse_cpp(temp->down, level + 1);
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
					run = traverse_cpp(temp->right, level + 1);
					for (int i = 0; i < level; i++) {
						code << "\t";
					}
					code << "}\n";
					if (run != NULL) {
						run = traverse_cpp(run, level);
					}
				}
			}
		}
		else {
			if (temp->right != NULL) {
				run = traverse_cpp(temp->right, level);
			}
			else if (temp->down != NULL) {
				run = traverse_cpp(temp->down, level);
			}
		}
	}
	return run;
}

struct node* CodeGenerate::traverse_py(struct node* temp, int level) {
	int loopflag = -1;
	int nl = 1;
	node* run = NULL;
	if (temp->type == 3) {
		int status = 0;
		for (int i = 0; i < connectors.size(); i++) {
			if (connectors[i].first == temp->id) {
				if (connectors[i].second == 0) {
					connectors[i].second = 1;
				}
				else {
					status = 1;
				}
				break;
			}
		}
		if (status == 1) {
			if (temp->right != NULL) {
				return (temp->right);
			}
			else if (temp->down != NULL) {
				return (temp->down);
			}
		}
		else {
			return NULL;
		}
	}
	else {
		for (int i = 0; i < loops.size(); i++) {
			if (loops[i].second == temp->id && used[i] == 0) {
				node* x = nodebyid(first, loops[i].first);
				if (temp->type == 5 && x->type != 5) {
					loopflag = 0;
				}
				else {
					if (temp->type != 5) {
						loopflag = 1;
					}
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
				if (word == "int" || word == "INT" || word == "Int") {
					vartype = 1;
					flag = 1;
				}
				else if (word == "char" || word == "CHAR" || word == "Char") {
					vartype = 2;
					flag = 1;
				}
				else if (word == "string" || word == "STRING" || word == "String") {
					vartype = 3;
					flag = 1;
				}
				else if (word == "float" || word == "FLOAT" || word == "Float") {
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
			if (outflag == 1 || flag == 0) {
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
			int closeflag = 0;
			while (str >> word) {
				if (word == "OUTPUT") {
					code << "print( ";
					type = 0;
					closeflag = 1;
				}
				else if (word == "INPUT") {
					type = 1;
				}
				else {
					if (type == 0) {
						code << word << " ";
					}
					else if (type == 1) {
						code << word << " = ";
						for (int i = 0; i < variables.size(); i++) {
							if (word == variables[i].first) {
								vartype = variables[i].second;
							}
						}
						if (vartype == 1) {
							code << "int(input()";
							closeflag = 1;
						}
						else if (vartype == 2) {
							code << "chr(input()";
							closeflag = 1;
						}
						else if (vartype == 3) {
							code << "str(input()";
							closeflag = 1;
						}
						else if (vartype == 4) {
							code << "float(input()";
							closeflag = 1;
						}
						else {
							code << "input()";
						}
					}
				}
			}
			if (closeflag == 1) {
				code << ")";
			}
		}
		else if (temp->type == 5) {
			std::string validated = "";
			for (int i = 0; temp->text[i] != '\0'; i++) {
				int valflag = 0;
				if (temp->text[i] == '=') {
					if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '=' && temp->text[i - 1] != '!' && temp->text[i - 1] != '=') {
						validated += '=';
					}
				}
				if (temp->text[i] == '&') {
					valflag = 1;
					if (temp->text[i - 1] != '&') {
						if (temp->text[i - 1] != ' ') {
							validated += " ";
						}
						validated += "and";
						if (temp->text[i + 1] != ' ') {
							validated += " ";
						}
					}
				}
				if (temp->text[i] == '|') {
					valflag = 1;
					if (temp->text[i - 1] != '|') {
						if (temp->text[i - 1] != ' ') {
							validated += " ";
						}
						validated += "or";
						if (temp->text[i + 1] != ' ') {
							validated += " ";
						}
					}
				}
				if (temp->text[i] == '!') {
					if (temp->text[i + 1] != '\0' && temp->text[i + 1] != '=') {
						valflag = 1;
						validated += "not";
					}
				}

				if (valflag == 0) {
					validated += temp->text[i];
				}
			}
			if (loopflag == -1) {
				code << "if (" << validated << "):";
			}
			else if (loopflag == 0) {
				code << "while (" << validated << "):";
			}
			else if (loopflag == 2) {
				code << "if not(" << validated << "):\n";
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
					run = traverse_py(temp->down, level);
				}
				else if (temp->right != NULL) {
					run = traverse_py(temp->right, level);
				}
			}
			else if (loopflag == 0) {
				if (temp->down != NULL) {
					run = traverse_py(temp->down, level + 1);
				}
				if (temp->right != NULL) {
					run = traverse_py(temp->right, level);
				}
			}
			else {
				if (temp->down != NULL) {
					run = traverse_py(temp->down, level + 1);
				}
				if (temp->right != NULL) {
					for (int i = 0; i < level; i++) {
						code << "\t";
					}
					code << "else :\n";
					run = traverse_py(temp->right, level + 1);
					if (run != NULL) {
						run = traverse_py(run, level);
					}
				}
			}
		}
		else {
			if (temp->right != NULL) {
				run = traverse_py(temp->right, level);
			}
			else if (temp->down != NULL) {
				run = traverse_py(temp->down, level);
			}
		}
	}
	return run;
}

struct node* CodeGenerate::nodebyid(struct node* temp, int id) {
	if (temp->id == id) {
		return temp;
	}
	node* x = NULL;
	if (temp->right != NULL) {
		x = nodebyid(temp->right, id);
	}
	if (temp->down != NULL && x == NULL) {
		x = nodebyid(temp->down, id);
	}
	return x;
}