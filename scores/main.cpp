#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>
#include "include/metahandler.h"

using namespace std;

int main(int argc, char *argv[]){
    if (argc < 2) {
        cout << "No Input File" << endl;
    }
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    Meta meta(stdscr);
    meta.draw();
    while (meta.dontQuit == true) {
        char c = wgetch(stdscr);
        meta.update(c);
        meta.draw();
        refresh();
    }
	endwin();
    return 0;
}; 