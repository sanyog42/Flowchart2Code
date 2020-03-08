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
                if (d == -1) {
                    node* temp = traverse1(start1, r);
                    temp->right = x;
                }
                else {
                    node* temp = traverse1(start1, d);
                    temp->down = x;
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
        tagtype = 0;
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '<' && flag == -1) {
                flag = 0;
                continue;
            }
            if (line[i] == '>' && flag == 0) {
                flag = 1;
                if (tag == "type") {
                    tagtype = 1;
                }
                else if (tag == "id") {
                    tagtype = 2;
                }
                else if (tag == "text") {
                    tagtype = 3;
                }
                else if (tag == "right") {
                    tagtype = 4;
                }
                else if (tag == "down") {
                    tagtype = 5;
                }
                else if (tag == "loop") {
                    tagtype = 6;
                }
                continue;
            }
            if (line[i] == '<' && flag == 1) {
                flag = 2;
                if (tagtype == 1) {
                    if (text == "process") {
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
                else if (tagtype == 2) {
                    x->id = (int)(text[0] - '0');
                }
                else if (tagtype == 3) {
                    x->text = text;
                }
                else if (tagtype == 4) {
                    r = (int)(text[0] - '0');
                }
                else if (tagtype == 5) {
                    d = (int)(text[0] - '0');
                }
                else if (tagtype == 6) {
                    l = (int)(text[0] - '0');
                }
                continue;
            }
            if (flag == 0) {
                tag += line[i];
            }
            else if (flag == 1) {
                text += line[i];
            }
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
