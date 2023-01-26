#ifndef EFFECTS_H
#define EFFECTS_H

#define TWO_PI 6.283185307179586476925286766559

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class Effects {
    public:
        Effects(int sampleRate);
        void addNoiseFloor(vector<double> & b, double range);
        void LFO(vector<double> & b, double lfoFreq, double lfoAmp);
        void modEcho(vector<double> & b, double decay, double time, double lfoFreq, double lfoAmp);
        void echo(vector<double> & b, double decay, double time);
        void filter(vector<double> & b, double K, bool flip = false);
        double getMinMax(vector<double> & bT);
        void crush(vector<double> & b, int bitz);
        void mult(vector<double> & b, int L);
        void wavefold(vector<double> & b, double L);
        void haas(vector<double> & b, int interval);
        void normalise(vector<double> & bT);
        void fuzz(vector<double> & b, double k, double mix);
        void overdrive(vector<double> & b, double thresh, double mix);
        double mod(double n, double d);
        void distort(vector<double> & b, double gain, double mix, int mode);

    private:
        int sR;

};
#endif