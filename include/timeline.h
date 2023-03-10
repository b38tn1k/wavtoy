#ifndef TIMELINE_H
#define TIMELINE_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Timeline {
    public:
        void setBPM(double b);
        void addBeats(vector<double> & b, int count);
        void addSilence(vector<double> & b, double dur);
        int getSampleIndexFromCursor(u_int beatSubDivision, double multiples);
        int cursor;
        int beatInSamples;
        Timeline(int sR) {
            sampleRate = sR;
            cursor = 0;
            beatsInMeasure = 4;

        }
    private:
        int beatsInMeasure;
        int sampleRate;
        double bpm;
        int measureInSamples;

};

#endif