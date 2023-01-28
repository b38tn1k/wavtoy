#ifndef SCORE_HANDLER_H
#define SCORE_HANDLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "frequencies.h"

using namespace std;

struct noteEvent {
    double frequency;
    double beatIndex;
    u_int beatDivision;
    u_int instrument;
    noteEvent(double f, double bI, int bD, int inst) : frequency(f), beatIndex(bI), beatDivision(bD), instrument(inst) {}
};

struct seq {
    double length;
    string name;
    vector < noteEvent > score;
    seq (string n): name(n) {}
    seq () {}
};

struct fx {
    string type;
    vector <double> params;
    fx (string t, vector <double> p) : type(t), params(p) {}
};

class ScoreHandler {
    public:
        ScoreHandler(string filename); // loading
        ScoreHandler(){}; // making
        string title;
        double bpm;
        vector < vector <string> > instrumentStrings;
        vector < vector <string> > fxStrings;
        map <int, vector< fx> > fxMap;
        vector < vector <string> > scoreStrings;
        vector < vector <string> > structureStrings;
        vector < noteEvent > score;
        vector < vector < noteEvent > > instrumentScore;
        map <string, int> notes;
        double length;
    private:
        void makeNoteMap();
        int readHead(vector<string> lines);
        void cleanScoreForEditor(string line);
        double readScoreLine(string line, double div);
        map < string, seq > sequences;
        string currentSeq;
        noteEvent offsetNEIndex(noteEvent nE, double off);
        void buildScore();
};

#endif