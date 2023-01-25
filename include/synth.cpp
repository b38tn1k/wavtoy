#include "synth.h"

Synth::Synth(double a, double d, double amp, double dur, int sR, double fG, double hB, int hC, int m) {
    mode = m;
    attack = a;
    decay = d;
    amplitude = amp;
    duration = dur;
    sampleRate = sR;
    attackInSamples = duration * attack * sR;
    attackIncrement = 1.0/attackInSamples;
    if (attack == 0) {
        attackIncrement = 1.0;
        attackInSamples = 0;
    }
    decayInSamples = duration * decay * sR;
    decayIncrement = 1.0/decayInSamples;
    decayInSamples = (duration * sR) - decayInSamples;
    if (attack == 0) {
        attackIncrement = 1.0;
        attackInSamples = 0;
    }
    if (decay == 0) {
        decayIncrement = 0.0;
        decayInSamples = (duration * sR);
    }
    fundamentalGain = fG;
    harmonicBalance = hB;
    harmonicCount = hC;
}

double sinSample(int n, double f, int sR){
    return sin( (TWO_PI * n * f) / sR );
}

void Synth::setTimbre(double fG, double hB, int hC){
    fundamentalGain = fG;
    harmonicBalance = hB;
    harmonicCount = hC;
}

double Synth::additiveOsc(int n, double frequency) {
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
    return value;
}

double Synth::noiseOsc(int n, double frequency) {
    double random = (rand() % 100)/100.0;
    double value = fundamentalGain * sinSample(n, frequency, sampleRate);
    value = (fundamentalGain * value) + (1 - fundamentalGain) * random;
    return value;
}

double Synth::swoopOsc(int n, double frequency) {
    double mult = 1.0;
    double pitch = frequency;
    double random = (rand() % 100)/100.0;
    if (n <= attackInSamples/2) {
        mult -= attackIncrement * n;
        pitch  = (harmonicCount * frequency * mult);
    }
    double value = fundamentalGain * (sinSample(n, pitch, sampleRate) + harmonicBalance * random);
    return value;
}

vector <double> Synth::synthesise(double frequency){
    vector <double> tempBuffer;
    int N = sampleRate * duration;
    double env = 0.0;
    for (int n = 0; n < N; n++){
        // osc
        double value;
        switch(mode) {
            case 1:
                value = additiveOsc(n, frequency);
                break;
            case 2:
                value = noiseOsc(n, frequency);
                break;
            case 3:
                value = swoopOsc(n, frequency);
                break;
            default:
                value = additiveOsc(n, frequency);
        }
        
        // env
        if (n <= attackInSamples) {
            env += attackIncrement;
            if (mode == 3) { // i dont like this
                env = 1.0;
            }
        }
        if (n >= decayInSamples) {
            env -= decayIncrement;
        }
        //amp
        value *= env;
        // write to temp buffer
        // tempBuffer.push_back(value * MAX_AMP * amplitude);
        tempBuffer.push_back(value * amplitude);
    }
    return tempBuffer;
}

void Synth::addNote(vector<double> & b, int index, double frequency){
    vector <double> tempBuffer = synthesise(frequency);
    while(buffer.size() < b.size()) {
        buffer.push_back(0);
    }
    // assume note fits
    // sum note into buffer;
    for(vector<double>::iterator it = begin(tempBuffer); it != end(tempBuffer); ++it){
        buffer[index] += *it;
        index++;
    }
}