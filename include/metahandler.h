#ifndef METAHANDLER_H
#define METAHANDLER_H

#define MODE_IDLE 0
#define MODE_COMMAND 1
#define MODE_QUIT 2
#define MODE_SYNTH 3
#define MODE_INFO 4
#define MODE_NOTATE 5
#define MODE_WRITE 5

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>
#include "cursestable.h"
#include "score_handler.h"

using namespace std;

class Meta {
    public:
        Meta(WINDOW *stdscr);
        int mode;
        void draw();
        void update(char c);
        bool dontQuit;
        void attachScore(ScoreHandler scr);

    private:
        WINDOW * stdscr;
        string optionsMenuMSG;
        string synthMenuMSG;
        string infoMenuMSG;
        string saveMenuMSG;
        string cmdMSG;
        string baseMSG;
        string notateMenuMSG;
        //update
        void tableViewUpdate(char c, Ctable & table);
        void commandUpdate(char c);
        void quitUpdate(char c);
        void idleUpdate(char c);
        //draw
        void tableViewDraw(Ctable & table);
        void drawCommandBar();
        //utils
        void drawHorizontalLine(int height);
        void drawVerticallLine(int X);
        void printString(string myStr);
        void centerString(string myStr);
        void toggleMode(int newMode);
        // cursor tracker
        int cx, cy, vcy;
        // tables
        Ctable sT;
        Ctable iT;
        Ctable nT;
        // score unpacker
        ScoreHandler score;
};

#endif