#include "metahandler.h"

void populateColumn(int col, Ctable &cTab, vector<string> temp) {
    for (vector <string>::iterator i = temp.begin(); i != temp.end(); ++i){
        cTab.addRow(col, *i);
    }
}

Meta::Meta(WINDOW *scr){
    stdscr = scr;
    for (int i = 0; i < 10; i++){
        clear();
        refresh();
    }
    mode = MODE_IDLE;
    cx = 0;
    cy = 0;
    vcy = 0;
    curs_set(0);
    sT = Ctable();
    iT = Ctable();
    nT = Ctable();
    //string setup, how to do pretty?
    cmdMSG = "press ':' for options";
    optionsMenuMSG = "[S]ynthesis\t| [N]otate\t| [I]nfo\t| [W]rite File\t| [Q]uit";
    synthMenuMSG = "[+] New Synth\t| Navigate with arrows | ENTER to set";
    infoMenuMSG = "Name your project and set BPM here.";
    notateMenuMSG = "[+] New Synth\t| Navigate with arrows | ENTER to set";
    dontQuit = true;
    centerString("Welcome to the Wavtoy Composition Tool.");
    
    sT.addCol(false, 0, 12);
    const char* cs[] = {"TYPE", "ATTACK", "DECAY", "AMPLITUDE", "DURATION", "F.GAIN", "H.BAL", "OCT.COUNT", "FX"};
    vector<string> s(cs, cs+9);
    populateColumn(0, sT, s);

    iT.addCol(false, 0, 10);

    iT.addCol(true, 12, 10);
    const char* cs2[] = {"TITLE", "BPM"};
    vector<string> s2(cs2, cs2+2);
    populateColumn(0, iT, s2);
    const char* cs3[] = {"untitled", "120"};
    vector<string> s3(cs3, cs3+2);
    populateColumn(1, iT, s3);
}

void Meta::drawHorizontalLine(int height){
    move(height, 0);
    for (int i = 0; i < getmaxx(stdscr); i++) {
        printString("-");
    }
}

void Meta::drawVerticallLine(int X){
    for (int i = 0; i < getmaxy(stdscr)-3; i++) {
        move(i, X);
        printString("|");
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
    printString(myStr);
}

void Meta::printString(string myStr){
    char * myChar = new char[myStr.length() + 1];
    copy(myStr.begin(), myStr.end(), myChar);
    myChar[myStr.length()] = '\0';
    printw(myChar);
}

void Meta::draw() {
    int cmode = curs_set(0);
    switch(mode) {
        case MODE_IDLE:
            break;
        case MODE_INFO:
            tableViewDraw(iT);
            break;
        case MODE_SYNTH:
            tableViewDraw(sT);
            break;
        case MODE_NOTATE:
            tableViewDraw(nT);
            break;
        case MODE_COMMAND:
            break;
        case MODE_QUIT:
            break;
    }
    
    // reset cursor to position
    move(cy, cx); 
    curs_set(cmode);
    drawCommandBar();
}

void Meta::toggleMode(int newMode) {
    clear();
    mode = newMode;
    drawCommandBar();
}

void Meta::drawCommandBar(){
    // draw on top
    int cmode = curs_set(0);
    drawHorizontalLine(getmaxy(stdscr) - 3);
    drawHorizontalLine(getmaxy(stdscr) - 1);
    move(getmaxy(stdscr) - 2, 0);
    clrtoeol();
    printString(cmdMSG);
    move(getmaxy(stdscr) - 2, getmaxx(stdscr)-15);
    printString("MODE: ");
    printString(to_string(mode));
    move(cy, cx); 
    curs_set(cmode);

}

void Meta::idleUpdate(char c) {
    switch(c) {
        case ':':
            cmdMSG = optionsMenuMSG;
            toggleMode(MODE_COMMAND);
            curs_set(0);
            break;
    }
}

void Meta::tableViewDraw(Ctable & table){
    table.draw(getmaxy(stdscr), vcy);
    // for (int i = 1; i < table.table.size(); i++) {
    //     drawVerticallLine(10 * i);
    // }
}

void Meta::tableViewUpdate(char c, Ctable & table) {
    bool leave = false;
    switch(c) {
        case ':':
            cmdMSG = optionsMenuMSG;
            toggleMode(MODE_COMMAND);
            curs_set(0);
            leave = true;
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
            if (cy == 0) {
                vcy = max(0, vcy-1);
            }
            break;
        case 's':
        case 'j':
        case char(KEY_DOWN):
            cy = min(cy+1, getmaxy(stdscr) - 4);
            if (cy == getmaxy(stdscr) - 4) {
                vcy = min(vcy + 1, table.maxHeight());
            }
            break;
    }
    if (leave == false) {
        cmdMSG = baseMSG + " " + table.getEntry(cx+1, cy + vcy);
    }
}

void Meta::commandUpdate(char c) {
    switch(c) {
        case 'q':
        case 'Q':
            cmdMSG = "Quit? y/n";
            toggleMode(MODE_QUIT);
            break;
        case 's':
        case 'S':
            baseMSG = synthMenuMSG;
            cmdMSG = baseMSG;
            toggleMode(MODE_SYNTH);
            break;
        case 'i':
        case 'I':
            baseMSG = infoMenuMSG;
            cmdMSG = baseMSG;
            toggleMode(MODE_INFO);
            break;
        case 'n':
        case 'N':
            baseMSG = notateMenuMSG;
            baseMSG = notateMenuMSG;
            cmdMSG = baseMSG;
            toggleMode(MODE_NOTATE);
            break;
        case 10:
            cmdMSG =  "press ':' for options";
            toggleMode(MODE_IDLE);
            break;
    }
}

void Meta::quitUpdate(char c) {
    switch(c) {
        case 'y':
        case 'Y':
        case 10:
            dontQuit = false;
            break;
        case 'n':
        case 'N':
            cmdMSG = "Quit Aborted";
            toggleMode(MODE_IDLE);
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
            tableViewUpdate(c, sT);
            break;
        case MODE_INFO:
            curs_set(2);
            tableViewUpdate(c, iT);
            break;
        case MODE_NOTATE:
            curs_set(2);
            tableViewUpdate(c, nT);
            break;
        case MODE_COMMAND:
            vcy = 0;
            curs_set(0);
            commandUpdate(c);
            break;
        case MODE_QUIT:
            curs_set(0);
            quitUpdate(c);
            break;
    }

}

void Meta::attachScore(ScoreHandler scr){
    score = scr;
    //unpack
    iT.setData(1, 0, score.title);
    iT.setData(1, 1, to_string(score.bpm));
    int col = 1;
    int startx = 12;
    int width = 10;
    for (vector < vector <string> >::iterator i = score.instrumentStrings.begin(); i != score.instrumentStrings.end(); ++ i){
        sT.addCol(true, startx, width);
        int depthCounter = 0;
        for (vector <string> ::iterator j = i->begin(); j != i->end(); ++j){
            sT.addRow(col, *j);
            depthCounter++;
        }
        startx += (width);
        col++;
    }
    sT.eveniseColumns();
    nT.addCol(false, 0, 5);
    startx = 4;
    for (int i = 0; i < score.instrumentStrings.size(); i++){
        nT.addCol(true, startx+1, width);
        startx += (width);
    }
    
    int count = 0;
    for (vector < vector <string> >::iterator i = score.scoreStrings.begin(); i != score.scoreStrings.end(); ++ i){
        col = 1;
        if (i->size() > 0) {
            for (vector <string> ::iterator j = i->begin(); j != i->end(); ++j){
                nT.addRow(col, string(*j));
                col++;
            }
            nT.addRow(0, to_string(count));
            count++;
        }
    }

}