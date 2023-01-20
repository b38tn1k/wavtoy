#ifndef METAHANDLER_H
#define METAHANDLER_H

#define MODE_IDLE 0
#define MODE_COMMAND 1
#define MODE_QUIT 2
#define MODE_SYNTH 3

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>

using namespace std;

class Meta {
    public:
        Meta(WINDOW *stdscr);
        int mode;
        void drawHorizontalLine(int height);
        void printString(string myStr);
        void centerString(string myStr);
        void draw();
        void update(char c);
        bool dontQuit;
    private:
        WINDOW * stdscr;
        string optionsMenu;
        string synthMenu;
        string infoMenu;
        string saveMenu;
        string cmdMSG;
        void idleUpdate(char c);
        void synthUpdate(char c);
        void commandUpdate(char c);
        void quitUpdate(char c);
        int cx, cy;
};

#endif