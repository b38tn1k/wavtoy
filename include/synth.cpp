#include "synth.h"

Synth::Synth(double a, double d, double amp, double dur, int sR, double fG, double hB, int hC, int m) {
    mode = m;
    buffer.push_back(0.0); //just to put something there 
    attack = a;
    decay = d;
    amplitude = amp;
    duration = dur;
    sampleRate = sR;
    fundamentalGain = fG;
    harmonicBalance = hB;
    harmonicCount = hC;
     // can this be an init type dealy?
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
    
}

Synth::Synth(vector<string> params, int sR){
    mode = 1;
    if (params[0].find("SYNTH") != -1) {
        mode = 1;
    }
    if (params[0].find("KICK") != -1) {
        mode = 3;
    }
    if (params[0].find("NOISE") != -1) {
        mode = 2;
    }
    attack = stof(params[1]);
    decay = stof(params[2]);
    amplitude = stof(params[3]);
    duration = stof(params[4]);
    sampleRate = sR;
    fundamentalGain = stof(params[5]);
    harmonicBalance = stof(params[6]);
    harmonicCount = stoi(params[7]);
    // can this be an init type dealy?
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
    double value;
    if (n%2 == 1) {
        value = prevAdd;
    } else {
        value = fundamentalGain * sinSample(n, frequency, sampleRate);
        if (harmonicCount > 0){
            for (int i = 1; i < harmonicCount; i++) {
                double harmGain = (harmonicCount - i)/((double)harmonicCount * 2) * (1.0-fundamentalGain);
                if (i % 2 == 0) {
                    harmGain *= harmonicBalance;
                } else {
                    harmGain *= (1.0 - harmonicBalance);
                }
                value += harmGain * sinSample(n, frequency * i, sampleRate);
            }
        }
        prevAdd = value;
    }
    return value;
}

double Synth::noiseOsc(int n, double frequency) {
    double value;
    if (n%2 == 1) {
        value = prevRando;
    } else {
        prevRando = (rand() % 100)/50.0 - 1.0;
        value = fundamentalGain * sinSample(n, frequency, sampleRate);
        value = (fundamentalGain * value) + (1 - fundamentalGain) * prevRando;
    }
    
    return value;
}

double Synth::swoopOsc(int n, double frequency) {
    double value;
    if (n%2 == 1) {
        value = prevSwoop;
    } else {
        double mult = 1.0;
        double pitch = frequency;
        double random = (rand() % 100)/100.0;
        if (n <= attackInSamples/2) {
            mult -= attackIncrement * n;
            pitch  = (harmonicCount * frequency * mult);
        }
        value = fundamentalGain * (sinSample(n, pitch, sampleRate) + harmonicBalance * random);
        prevSwoop = value;
    }
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
        tempBuffer.push_back(0.0);
        tempBuffer[n] = value * amplitude;
    }
    return tempBuffer;
}

void Synth::addNote(int bs, int index, double frequency){
    vector <double> tempBuffer = synthesise(frequency);
    if(buffer.size() < bs) {
        vector <double> nb(bs, 0.0);
        buffer = nb;
    }
    // assume note fits
    // sum note into buffer;
    for(vector<double>::iterator it = begin(tempBuffer); it != end(tempBuffer); ++it){
        buffer[index] += *it;
        index++;
    }
}