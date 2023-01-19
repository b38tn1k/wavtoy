#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "include/frequencies.h"
#include "include/wav_handler.h"
#include "include/timeline.h"
#include "include/synth.h"
#include "include/score_handler.h"

using namespace std;

void echo(vector<int> & b, double decay, double time) {
    double invDecay = 1.0 - decay;
    for(vector<int>::iterator it = begin(b); it != end(b); ++it){
        if (it - begin(b)> time) {
            *it = invDecay * (*it) + decay * (*(it-time));
        }
    }
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        cout << "Specify input file" << endl;
        return 0;
    }
    ScoreHandler score(argv[1]);
    WAV test(score.title + ".wav");
    Timeline seq(test.sampleRate);
    seq.setBPM(score.bpm);

    cout << "Hello " << endl; 
    cout << score.bpm <<" BPM" << endl;
    // make synths
    double maxDuration = 0;
    vector <Synth> instruments;
    for (vector < vector <string> >::iterator i = begin(score.instrumentStrings); i != end(score.instrumentStrings); ++i){
        vector<string> temp = *i;
        instruments.push_back(Synth(stof(temp[1]), stof(temp[2]), stof(temp[3]), stof(temp[4]), test.sampleRate, stof(temp[5]), stof(temp[6]), stoi(temp[7]), stoi(temp[8])));
        if (stof(temp[4]) > maxDuration) {
            maxDuration = stof(temp[4]); 
        }
    }
    vector<int> buffer;
    test.openWav();
    seq.addBeats(buffer, ceil(score.length));
    cout << score.length << endl;
    seq.addSilence(buffer, maxDuration);

    for (vector<noteEvent>::iterator i = begin(score.score); i != end(score.score); ++i){
        int index = seq.getSampleIndexFromCursor(4, i->beatIndex);
        instruments[i->instrument].addNote(buffer, index, i->frequency);
        // cout << "new note: index\t"<< i->beatIndex << endl;
        // cout << "inst\t" << i->instrument << endl;
        // cout << "div\t" << i->beatDivision << endl;
        // cout << "freq\t" << i->frequency << endl;
        // cout << "index\t" << index << endl;
        // cout << "buffer size\t" << buffer.size() << endl;
    }
    // echo(buffer, 0.3, 0.5 * test.sampleRate); 
    cout << "RENDERED: \t"<< score.title <<".wav" << endl;

    test.writeBuffer(buffer);
    test.closeWav();
    return 0;
}