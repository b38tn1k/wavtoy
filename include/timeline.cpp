#include "timeline.h"

void Timeline::setBPM(double b) {
    bpm = b;
    beatInSamples = (int)((60.0 * sampleRate)/b);
    measureInSamples = beatsInMeasure * beatInSamples;
}

void Timeline::addBeats(vector<double> & b, int count) {
    for (int i = 0; i <= count * beatInSamples; i++) {
        b.push_back(0);
    }
}

void Timeline::addSilence(vector<double> & b, double dur) {
    for (int i = 0; i <= dur * sampleRate; i++) {
        b.push_back(0);
    }
}

int Timeline::getSampleIndexFromCursor(u_int beatSubDivision, double multiples=1) {
    int offset = (int)((measureInSamples / beatSubDivision) * multiples);
    return cursor + offset;
}