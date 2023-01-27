#ifndef EFFECTS_H
#define EFFECTS_H

#define TWO_PI 6.283185307179586476925286766559

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <map>
#include "score_handler.h"

using namespace std;
using namespace std::chrono;

class Effects {
    public:
        Effects(int sampleRate);
        void applyFX(vector<double> & bT, map <int, vector< fx> > fxMap, int index);
        void addNoiseFloor(vector<double> & b, double range);
        double mod(double n, double d);
        double getMinMax(vector<double> & bT);
        void normalise(vector<double> & bT);
        void printTimes();
        void compress(vector<double> & bT, double thresh, double ratio);

        void LFO(vector<double> & b, vector<double> params);
        void modEcho(vector<double> & b, vector<double> params);
        void echo(vector<double> & b, vector<double> params);
        void filter(vector<double> & b, vector<double> params, bool flip);
        void crush(vector<double> & b, vector<double> params);
        void wavefold(vector<double> & b, vector<double> params);
        void haas(vector<double> & b, vector<double> params);
        void fuzz(vector<double> & b, vector<double> params);
        void overdrive(vector<double> & b, vector<double> params);
        void distort(vector<double> & b, vector<double> params);

    private:
        int sR;
        map <string, vector <int> > timers;

};
#endif