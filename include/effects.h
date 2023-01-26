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
        void applyFX(vector<double> & bT, map <int, vector< fx> > fxMap, int index);
        void normalise(vector<double> & bT);
        void addNoiseFloor(vector<double> & b, double range);
        double mod(double n, double d);

        double procLFO(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procModEcho(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procEcho(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procHPF(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procLPF(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procCrush(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procWavefold(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procHaas(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procFuzz(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procOverdrive(vector<double> & bT, double value, int accum, vector<double> params, int sR);
        double procDistort(vector<double> & bT, double value, int accum, vector<double> params, int sR);
    private:
        int sR;

};

// class Effect {
//     public:
//         int sR;
//         vector<double> params;
//         vector<double> invParams;
//         vector<double> bT;

//         Effect(){};


// };

#endif