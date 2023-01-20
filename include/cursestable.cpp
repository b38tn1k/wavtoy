#include "cursestable.h"

Ctable::Ctable() {
    scrollX = 0;
    scrollY = 0;
}

void Ctable::addCol(bool e, int x, int w) {
    table.push_back(column(e, x, w));
}

void Ctable::addRow(int col, string myStr) {
    table[col].data.push_back(myStr);
}

void Ctable::draw(int startX, int startY){
    int x;
    int y;
    for (vector < column >::iterator i = table.begin(); i != table.end(); ++i){
        vector <string> strs = i->data;
        y = startY;
        x = startX + int(i->x);
        for (vector <string> ::iterator str = strs.begin(); str != strs.end(); ++str){
            move(y, x);
            printString(string(i->width, ' ')); //clear?
            move(y, x);
            printString(*str);
            y += 1;
        }
    }
}

void Ctable::printString(string myStr){
    char * myChar = new char[myStr.length() + 1];
    copy(myStr.begin(), myStr.end(), myChar);
    printw(myChar);
}