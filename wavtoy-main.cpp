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

void echo(vector<int> & b, double decay, double time, int sR) {
    double invDecay = 1.0 - decay;
    time *= sR;
    for(vector<int>::iterator it = begin(b); it != end(b); ++it){
        if (it - begin(b)> time) {
            *it = invDecay * (*it) + decay * (*(it-time));
        }
    }
}

void logInst(vector<noteEvent>::iterator i, int index, vector<int> & buffer){
    cout << "new note: index\t"<< i->beatIndex << endl;
    cout << "inst\t" << i->instrument << endl;
    cout << "div\t" << i->beatDivision << endl;
    cout << "freq\t" << i->frequency << endl;
    cout << "index\t" << index << endl;
    cout << "buffer size\t" << buffer.size() << endl;
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        cout << "Specify input file" << endl;
        return 0;
    }
    ScoreHandler score(argv[1]);
    WAV wav(score.title + ".wav");
    Timeline seq(wav.sampleRate);
    seq.setBPM(score.bpm);

    cout << "Hello " << endl; 
    cout << score.bpm <<" BPM" << endl;
    // make synths
    double maxDuration = 0;
    vector <Synth> instruments;
    for (vector < vector <string> >::iterator i = begin(score.instrumentStrings); i != end(score.instrumentStrings); ++i){
        vector<string> temp = *i;
        int mode = 1;
        if (temp[0].find("SYNTH") != -1) {
            mode = 1;
        }
        if (temp[0].find("KICK") != -1) {
            mode = 3;
        }
        if (temp[0].find("NOISE") != -1) {
            mode = 2;
        }
        instruments.push_back(Synth(stof(temp[1]), stof(temp[2]), stof(temp[3]), stof(temp[4]), wav.sampleRate, stof(temp[5]), stof(temp[6]), stoi(temp[7]), mode));
        if (stof(temp[4]) > maxDuration) {
            maxDuration = stof(temp[4]); 
        }
    }
    vector<int> buffer;
    wav.openWav();
    seq.addBeats(buffer, ceil(score.length));
    seq.addSilence(buffer, maxDuration);

    // write instruments to their own buffers
    for (vector<noteEvent>::iterator i = begin(score.score); i != end(score.score); ++i){
        int index = seq.getSampleIndexFromCursor(4, i->beatIndex);
        instruments[i->instrument].addNote(buffer, index, i->frequency);
        // logInst(i, index, buffer);
    }
    int j = 0;
    // apply effects to instrument buffers and write to master buffer
    for (vector <Synth>::iterator i = begin(instruments); i!= end(instruments); ++i){
        vector <int> tempB = i->buffer;
        for (vector < vector <string> >::iterator k = begin(score.fxStrings); k != end(score.fxStrings); ++k){
            vector <string> tempFX = *k;
            if (j == stoi(tempFX[0])){
                if (tempFX[1].find("ECHO") != -1) { // can't switch on string :-<
                    echo(tempB, stof(tempFX[2]), stof(tempFX[3]), wav.sampleRate);
                }
            }
        }
        j++;
        for (int i = 0; i < tempB.size(); i++) {
            buffer[i] += tempB[i];
        }
        // transform(buffer.begin(), buffer.end(), temp.begin(), temp.end(), plus<int>());
    }

    // echo(buffer, 0.3, 0.5 * wav.sampleRate); 

    wav.writeBuffer(buffer);
    wav.closeWav();
    cout << "RENDERED: \t"<< score.title <<".wav" << endl;
    return 0;
}