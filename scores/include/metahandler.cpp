#include "metahandler.h"

Meta::Meta(WINDOW *scr){
    stdscr = scr;
    mode = MODE_IDLE;
    cmdMSG = "press ':' for options";
    optionsMenu = "[S]ynthesis\t| [N]otate\t| [I]nfo\t| [W]rite File\t| [Q]uit";
    synthMenu = "[+] New Synth\t| Navigate with arrows | ENTER to set";
    dontQuit = true;
    curs_set(0);
    centerString("Welcome to the Wavtoy Composition Tool.");
    cx = 0;
    cy = 0;
}

void Meta::drawHorizontalLine(int height){
    move(height, 0);
    for (int i = 0; i < getmaxx(stdscr); i++) {
        printw("-");
    }
}


void Meta::centerString(string myStr){
    int x = getmaxx(stdscr);
    int y = getmaxy(stdscr);
    int xOff = (x - myStr.length())/2;
    int yOff = y / 2 - 3;
    xOff = max(0, xOff);
    yOff = max(0, yOff);
    move(yOff, xOff);
    char * myChar = new char[myStr.length() + 1];
    copy(myStr.begin(), myStr.end(), myChar);
    printw(myChar);
}

void Meta::printString(string myStr){
    char * myChar = new char[myStr.length() + 1];
    copy(myStr.begin(), myStr.end(), myChar);
    printw(myChar);
}

void Meta::draw() {
    drawHorizontalLine(getmaxy(stdscr) - 3);
    drawHorizontalLine(getmaxy(stdscr) - 1);
    move(getmaxy(stdscr) - 2, 0);
    clrtoeol();
    printString(cmdMSG);
    move(cy, cx); // reset cursor to position
}

void Meta::idleUpdate(char c) {
    switch(c) {
        case ':':
            mode = MODE_COMMAND;
            cmdMSG = optionsMenu;
            curs_set(0);
            break;
    }
}

void Meta::synthUpdate(char c) {
    cmdMSG = synthMenu;
    switch(c) {
        case ':':
            mode = MODE_COMMAND;
            cmdMSG = optionsMenu;
            curs_set(0);
            break;
        case 'a':
        case 'h':
        case char(KEY_LEFT):
            cx = max(0, cx-1);
            break;
        case 'l':
        case char(KEY_RIGHT):
            cx = min(getmaxx(stdscr), cx+1);
            break;
        case 'w':
        case 'k':
        case char(KEY_UP):
            cy = max(0, cy-1);
            break;
        case 's':
        case 'j':
        case char(KEY_DOWN):
            cy = min(cy+1, getmaxy(stdscr));
            break;
    }
}

void Meta::commandUpdate(char c) {
    switch(c) {
        case 'q':
        case 'Q':
            cmdMSG = "Quit? y/n";
            mode = MODE_QUIT;
            break;
        case 's':
        case 'S':
            cmdMSG = synthMenu;
            mode = MODE_SYNTH;
            break;
        case 10:
            mode = MODE_IDLE;
            cmdMSG =  "press ':' for options";
            break;
    }
}

void Meta::quitUpdate(char c) {
    switch(c) {
        case 'y':
        case 'Y':
            dontQuit = false;
            break;
        case 'n':
        case 'N':
            mode = MODE_IDLE;
            cmdMSG = "Quit Aborted";
            break;
    }
}

void Meta::update(char c) {
    switch(mode) {
        case MODE_IDLE:
            curs_set(1);
            idleUpdate(c);
            break;
        case MODE_SYNTH:
            curs_set(2);
            synthUpdate(c);
            break;
        case MODE_COMMAND:
            curs_set(0);
            commandUpdate(c);
            break;
        case MODE_QUIT:
            curs_set(0);
            quitUpdate(c);
            break;

    }

}