#ifndef SYNTH_H
#define SYNTH_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define TWO_PI 6.283185307179586476925286766559

class Synth {
    public:
        Synth(double a, double d, double amp, double dur, int sR, double fG, double hB, int hC, int m);
        void addNote(vector<double> & b, int index, double frequency);
        vector <double> synthesise(double frequency);
        void setTimbre(double fG, double hB, int hC);
        vector<double> buffer;
    private:
        u_int sampleRate;
        u_int mode;
        double attack;
        double decay;
        double amplitude;
        double duration;
        double attackInSamples;
        double attackIncrement;
        double decayIncrement;
        double decayInSamples;
        double fundamentalGain;
        double harmonicBalance;
        int harmonicCount;
        double additiveOsc(int n, double frequency);
        double noiseOsc(int n, double frequency);
        double swoopOsc(int n, double frequency);
};


#endif