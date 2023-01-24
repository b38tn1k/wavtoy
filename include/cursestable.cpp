#include "cursestable.h"

Ctable::Ctable() {
    scrollX = 0;
    scrollY = 0;
}

void Ctable::addCol(bool e, int x, int w) {
    table.push_back(column(e, x, w));
}

void Ctable::addRow(int col, string myStr) {
    // table[col].width = max(table[col].width, int(myStr.length() + 1)); // need to update other x's
    table[col].data.push_back(myStr);
}

void Ctable::setData(int col, int row, string data) {
    if (col < table.size()){
        if (row < table[col].data.size()){
            table[col].data[row] = data;
        }
    }
}

int Ctable::maxHeight() {
    int mh = 0;
    for (vector < column >::iterator i = table.begin(); i != table.end(); ++i){
        mh = max(mh, int(i->data.size()));
    }
    return mh;
}

void Ctable::draw(int maxy, int virtCursor, int startX, int startY){
    int x;
    int y;
    for (vector < column >::iterator i = table.begin(); i != table.end(); ++i){
        vector <string> strs = i->data;
        y = startY - virtCursor;
        x = startX + int(i->x);
        for (vector <string> ::iterator str = strs.begin(); str != strs.end(); ++str){
            if (y >= 0) {
                move(y, x);
                printString(string(i->width, ' ')); //clear?
                move(y, x+1);
                printString(*str);
                if (i->editable == true) {
                    move(y, x + i->width - 1);
                    printString("|");
                } else {
                    move(y, x + i->width - 2);
                    printString("||");
                }
                
            }
            y += 1;
            if (y == maxy) {
                break;
            }
        }
    }
}

string Ctable::getEntry(int x, int y) {
    string result = "";
    for (vector < column > :: iterator col = table.begin(); col != table.end(); ++col){
        if (x >= col->x && x <= (col->x + col->width)) {
            if (y < col->data.size()) {
                result += col->data[y];
                break;
            }
        }
    }
    return "[ " + result + " ] ";
}

void Ctable::printString(string myStr){ //duplicate, why cant I do this?
    char * myChar = new char[myStr.length() + 1];
    copy(myStr.begin(), myStr.end(), myChar);
    myChar[myStr.length()] = '\0';
    printw(myChar);
}

void Ctable::eveniseColumns(){
    int maxDepth = 0;
    for (vector < column > :: iterator col = table.begin(); col != table.end(); ++col){
        if (col->data.size() > maxDepth) {
            maxDepth = col->data.size();
        }
    }

    for (vector < column > :: iterator col = table.begin(); col != table.end(); ++col){
        while (col->data.size() < maxDepth) {
            col->data.push_back("");
        }
    }
}