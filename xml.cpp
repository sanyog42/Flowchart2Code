#include "xml.h"

node* start1 = nullptr;

struct node* traverse1(struct node* temp, int id1);

xml::xml(wxString filepath, wxString codepath, int lang_sel) {
	std::string path = std::string(filepath.mb_str());
    std::ifstream file(path, std::ios::in);
    std::string line;
    int flag;
    int flagout = -1;
    int tagtype;
    node* x = nullptr;
    int r = -1, d = -1, l = -1;
    
    while (!file.eof()) {
        getline(file, line);
        if (line == "<node>") {
            flagout = 0;
            x = new node;
            continue;
        }
        else if (line == "</node>") {
            flagout = -1;
            if (start1 == NULL) {
                start1 = x;
            }
            else {
                if (d != -1) {
                    node* temp = traverse1(start1, d);
                    temp->down = x;
                }
                if (r != -1) {
                    node* temp = traverse1(start1, r);
                    temp->right = x;
                }
                if (l != -1) {
                    node* temp = traverse1(start1, l);
                    x->loop = temp;
                }
                d = -1, r = -1, l = -1;
            }
            x = NULL;
            continue;
        }

        if (flagout == -1) {
            continue;
        }
        
        flag = -1;
        std::string tag = "";
        std::string text = "";
        
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '<') {
                if (flag == -1) {
                    flag = 0;
                }
                else if (flag == 1) {
                    if (line[i + 1] == '/') {
                        flag = 2;
                    }
                    else {
                        text += line[i];
                    }
                }
                continue;
            }
            if (line[i] == '>' && flag == 0) {
                flag = 1;
                continue;
            }
            if (flag == 0) {
                tag += line[i];
            }
            else if (flag == 1) {
                text += line[i];
            }
        }

        if (tag == "type") {
            if (text == "connector") {
                x->type = 3;
            }
            else if (text == "process") {
                x->type = 4;
            }
            else if (text == "decision") {
                x->type = 5;
            }
            else if (text == "data") {
                x->type = 6;
            }
            else if (text == "terminator") {
                x->type = 7;
            }
        }
        else if (tag == "id") {
            x->id = stoi(text);
        }
        else if (tag == "text") {
            x->text = text;
        }
        else if (tag == "right") {
            r = stoi(text);
        }
        else if (tag == "down") {
            d = stoi(text);
        }
        else if (tag == "loop") {
            l = stoi(text);
        }
    }
    file.close();
	generate = new CodeGenerate(codepath, start1, lang_sel);
}

xml::~xml() {

}

struct node* traverse1(struct node* temp, int id1) {
    if (temp->id == id1) {
        return temp;
    }
    node* x = NULL;
    if (temp->right != NULL) {
        x = traverse1(temp->right, id1);
    }
    if (temp->down != NULL && x == NULL) {
        x = traverse1(temp->down, id1);
    }
    return x;
}