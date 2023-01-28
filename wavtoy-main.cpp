#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include "include/frequencies.h"
#include "include/wav_handler.h"
#include "include/timeline.h"
#include "include/synth.h"
#include "include/score_handler.h"
#include "include/effects.h"
/*
I thought using loops was cheating so I programmed my own samples. 
Then I thought using samples was cheating so I recorded real drums.
I then thought that programming it was cheating so I learnt to play drums for real. 
I then thought using bought drums was cheating so I learnt to make my own.
I then thought that using pre-made skins was cheating so I killed a goat and skinned it. 
I then thought that that was cheating too, so I grew my own goat from a baby goat. 
I also think that is cheating but I'm not sure where to go from here. 
I haven't made any music lately, what with the goat farming and all.
*/

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]){
    auto start = high_resolution_clock::now();
    auto startM = high_resolution_clock::now();
    if (argc < 2) {
        cout << "Specify input file" << endl;
        return 0;
    }
    ScoreHandler score(argv[1]);
    WAV wav(score.title + ".wav");
    Timeline seq(wav.sampleRate);
    Effects fx(wav.sampleRate);
    seq.setBPM(score.bpm);

    cout << "Hello " << endl; 
    cout << score.bpm <<" BPM" << endl;
    // make synths
    double maxDuration = 0;
    vector <Synth> instruments;
    for (vector < vector <string> >::iterator i = begin(score.instrumentStrings); i != end(score.instrumentStrings); ++i){
        vector<string> temp = *i;
        instruments.push_back(Synth(*i, wav.sampleRate));
        double dur = stof(temp[4]);
        if (dur > maxDuration) {
            maxDuration = dur; 
        }
    }
    wav.openWav();
    vector<double> buffer(int(ceil(score.length) * seq.beatInSamples + maxDuration*wav.sampleRate + 2), 0.0);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "LOAD & SETUP: " << duration.count()/1000000.0 << " sec" << endl;

    start = high_resolution_clock::now();
    // write instruments to their own buffers
    int s = buffer.size();
    for (int i = 0; i < instruments.size(); i++){
        for (vector<noteEvent>::iterator it = begin(score.instrumentScore[i]); it != end(score.instrumentScore[i]); ++it){    
            instruments[i].addNote(s, seq.getSampleIndexFromCursor(4, it->beatIndex), it->frequency);
        }
    }

    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "SYNTH WRITE: " << duration.count()/1000000.0 << " sec" << endl;
    int j = 0;
    // apply effects to instrument buffers 
    start = high_resolution_clock::now();
    vector <thread> fxTH;
    for (vector <Synth>::iterator i = begin(instruments); i!= end(instruments); ++i){
        function <void()> op = [& fx, i, score, j](){
            fx.applyFX(i->buffer, score.fxMap, j);
        };
        j++;
        thread th(op);
        fxTH.push_back(move(th));
    }
    for (auto & th : fxTH) {
        th.join();
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "FX WRITE: " << duration.count()/1000000.0 << " sec" << endl;
    // write to master buffer
    start = high_resolution_clock::now();
    for (vector <Synth>::iterator i = begin(instruments); i!= end(instruments); ++i){
        for (int j = 0; j < i->buffer.size(); j++) {
            buffer[j] += i->buffer[j];
        }
    }
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "MASTER WRITE: " << duration.count()/1000000.0 << " sec" << endl;
    
    fx.normalise(buffer);
    fx.compress(buffer, 0.5, 0.5);
    // fx.addNoiseFloor(buffer, 0.0001);
    wav.writeBuffer(buffer);
    wav.closeWav();
    auto stopM = high_resolution_clock::now();
    auto durationM = duration_cast<microseconds>(stopM - startM);
    cout << "RENDERED: \t"<< score.title <<".wav" << endl;
    cout << "IN " << durationM.count()/1000000.0 << " sec" << endl;
    // fx.printTimes();
    return 0;
}