#ifndef SCORE_HANDLER_H
#define SCORE_HANDLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "frequencies.h"
#include "add_synth.h"

using namespace std;

struct noteEvent {
    double frequency;
    u_int beatIndex;
    u_int beatDivision;
    u_int instrument;
    noteEvent(double f, int bI, int bD, int inst) : frequency(f), beatIndex(bI), beatDivision(bD), instrument(inst) {}
};

class ScoreHandler {
    public:
        ScoreHandler(string filename);
        string title;
        double bpm;
        vector < vector <string> > instrumentStrings;
        vector < noteEvent > score;
        map <string, int> notes;
        double length;
    private:
        void makeNoteMap();

};

#endif