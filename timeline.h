#ifndef TIMELINE_H
#define TIMELINE_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Timeline {
    public:
        void setBPM(double b);
        void addBeats(vector<int> & b, int count);
        int getSampleIndexFromCursor(u_int beatSubDivision, u_int multiples);
        int cursor;
        Timeline(int sR) {
            sampleRate = sR;
            cursor = 0;
            beatsInMeasure = 4;

        }
    private:
        int beatsInMeasure;
        int sampleRate;
        double bpm;
        int beatInSamples;
        int measureInSamples;

};

#endif