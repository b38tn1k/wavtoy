#ifndef CURSESTABLE_H
#define CURSESTABLE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>

using namespace std;

struct column {
    bool editable;
    int x;
    int width;
    vector <string> data;
    column(bool e, int _x, int w) : editable(e), x(_x), width(w){};
};

class Ctable {
    public:
        Ctable();
        vector < column > table;
        int scrollX;
        int scrollY;
        void addRow(int col, string myStr);
        void addCol(bool e, int x, int w);
        void draw(int maxy, int startX=0, int startY=0);
        void setData(int col, int row, string data);
    private:
        void printString(string myStr); //duplicate, weird
};

#endif