#ifndef EFFECTS_H
#define EFFECTS_H

#define TWO_PI 6.283185307179586476925286766559

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <chrono>
#include "score_handler.h"

using namespace std;
using namespace std::chrono;

class Effects {
    public:
        Effects(int sampleRate);
        double getMinMax(vector<double> & bT);
        double mod(double n, double d);
        void applyFX(vector<double> & bT, map <int, vector< fx> > fxMap, int index);
        void normalise(vector<double> & bT);
        void addNoiseFloor(vector<double> & b, double range);

        void LFO(vector<double> & b, vector<double> params);
        void modEcho(vector<double> & b, vector<double> params);
        void echo(vector<double> & b, vector<double> params);
        void filter(vector<double> & b, vector<double> params, bool isHPF = false);
        void crush(vector<double> & b, vector<double> params);
        void wavefold(vector<double> & b, vector<double> params);
        void haas(vector<double> & b, vector<double> params);
        void fuzz(vector<double> & b, vector<double> params);
        void overdrive(vector<double> & b, vector<double> params);
        void distort(vector<double> & b, vector<double> params);

        void printTimes();
    private:
        int sR;
        map <string, vector <int> > timers;

};
#endif