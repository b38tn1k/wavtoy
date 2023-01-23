#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>
#include "include/metahandler.h"
#include "include/score_handler.h"

using namespace std;

int main(int argc, char *argv[]){
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    Meta meta(stdscr);
    if (argc < 2) {
        cout << "No Input File" << endl;
    } else {
        ScoreHandler score(argv[1]);
        meta.attachScore(score);
    }
    meta.draw();
    while (meta.dontQuit == true) {
        char c = wgetch(stdscr);
        if (c) {
            clear();
        }
        meta.update(c);
        meta.draw();
        refresh();
    }
	endwin();
    return 0;
}; 