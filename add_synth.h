#ifndef ADD_SYNTH_H
#define ADD_SYNTH_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define TWO_PI 6.283185307179586476925286766559
#define MAX_AMP 32760

class AddSynth {
    public:
        AddSynth(double a, double d, double amp, double dur, int sR, double fG, double hB, int hC);
        void addNote(vector<int> & b, int index, double frequency);
        vector <int> synthesise(double frequency);
        void setTimbre(double fG, double hB, int hC);
    private:
        u_int sampleRate;
        double attack;
        double decay;
        double amplitude;
        double duration;
        double attackInSamples;
        double attackIncrement;
        double decayIncrement;
        double fundamentalGain;
        double harmonicBalance;
        int harmonicCount;

};


#endif