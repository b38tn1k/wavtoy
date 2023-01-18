#include "add_synth.h"

AddSynth::AddSynth(double a, double d, double amp, double dur, int sR, double fG, double hB, int hC) {
    attack = a;
    decay = d; // doesn't work yet
    amplitude = amp;
    duration = dur;
    sampleRate = sR;
    attackInSamples = duration * attack * sR;
    attackIncrement = 1.0/attackInSamples;
    decayIncrement = 1.0/((sR * duration) - attackInSamples);
    fundamentalGain = fG;
    harmonicBalance = hB;
    harmonicCount = hC;
}

double sinSample(int n, double f, int sR){
    return sin( (TWO_PI * n * f) / sR );
}

void AddSynth::setTimbre(double fG, double hB, int hC){
    fundamentalGain = fG;
    harmonicBalance = hB;
    harmonicCount = hC;
}

vector <int> AddSynth::synthesise(double frequency){
    vector <int> tempBuffer;
    int N = sampleRate * duration;
    double env = 0.0;
    for (int n = 0; n < N; n++){
        // osc
        double value = fundamentalGain * sinSample(n, frequency, sampleRate);
        for (int i = 1; i < harmonicCount; i++) {
            double harmGain = (harmonicCount - i)/((double)harmonicCount * 2) * (1.0-fundamentalGain);
            if (i % 2 == 0) {
                harmGain *= harmonicBalance;
            } else {
                harmGain *= (1.0 - harmonicBalance);
            }
            value += harmGain * sinSample(n, frequency * i, sampleRate);
        }

        // env
        if (n < attackInSamples) {
            env += attackIncrement;
        } else {
            env -= decayIncrement;
        }
        //amp
        value *= env;
        // write to temp buffer
        tempBuffer.push_back(value * MAX_AMP * amplitude);
    }
    return tempBuffer;
}

void AddSynth::addNote(vector<int> & b, int index, double frequency){
    vector <int> tempBuffer = synthesise(frequency);
    //make sure note fits, makes space

    
    int remainingSpace = b.size() - index;
    if (remainingSpace < tempBuffer.size() || index > b.size()){
        for (int i = 0; i < tempBuffer.size(); i++) {
            b.push_back(0);
        }
    }
    // sum note into buffer;
    for(vector<int>::iterator it = begin(tempBuffer); it != end(tempBuffer); ++it){
        b[index] += *it;
        index++;
    }
}