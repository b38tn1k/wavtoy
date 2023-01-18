#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "frequencies.h"
#include "wav_handler.h"
#include "timeline.h"
#include "add_synth.h"
#include "score_handler.h"

using namespace std;

void addSilence(vector<int> & b, double d, int sR){
    for (int i = 0; i < 2 * d * sR; i++){
        b.push_back(0);
    }
}

void echo(vector<int> & b, double decay, double time) {
    double invDecay = 1.0 - decay;
    for(vector<int>::iterator it = begin(b); it != end(b); ++it){
        if (it - begin(b)> time) {
            *it = invDecay * (*it) + decay * (*(it-time));
        }
    }
}

struct nE {
    double frequency;
    u_int beatIndex;
    u_int beatDivision;
    nE(double f, int bI, int bD) : frequency(f), beatIndex(bI), beatDivision(bD) {}
};

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
    vector <AddSynth> instruments;
    for (vector < vector <string> >::iterator i = begin(score.instrumentStrings); i != end(score.instrumentStrings); ++i){
        vector<string> temp = *i;
        instruments.push_back(AddSynth(stof(temp[1]), stof(temp[2]), stof(temp[3]), stof(temp[4]), test.sampleRate, stof(temp[5]), stof(temp[6]), stoi(temp[7])));
        if (stof(temp[4]) > maxDuration) {
            maxDuration = stof(temp[4]); 
        }
    }
    // up to here
    vector<int> buffer;

    test.openWav();
    seq.addBeats(buffer, score.length * maxDuration);

    for (vector<noteEvent>::iterator i = begin(score.score); i != end(score.score); ++i){
        cout << "new note: index\t"<< i->beatIndex << endl;
        cout << "inst\t" << i->instrument << endl;
        cout << "div\t" << i->beatDivision << endl;
        cout << "freq\t" << i->frequency << endl;
        int index = seq.getSampleIndexFromCursor(i->beatDivision, i->beatIndex);
        cout << "index\t" << index << endl;
        cout << "buffer size\t" << buffer.size() << endl;
        
        instruments[i->instrument].addNote(buffer, index, i->frequency);
    }
    cout << "RENDERED: \t"<< score.title <<".wav" << endl;

    test.writeBuffer(buffer);
    test.closeWav();
    return 0;
}