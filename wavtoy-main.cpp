#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "include/frequencies.h"
#include "include/wav_handler.h"
#include "include/timeline.h"
#include "include/synth.h"
#include "include/score_handler.h"
#include "include/effects.h"

// I thought using loops was cheating so I programmed my own samples. Then I thought using samples was cheating so I recorded real drums.

// I then thought that programming it was cheating so I learnt to play drums for real. I then thought using bought drums was cheating so I learnt to make my own.

// I then thought that using pre-made skins was cheating so I killed a goat and skinned it. I then thought that that was cheating too, so I grew my own goat from a baby goat. I also think that is cheating but I'm not sure where to go from here. I haven't made any music lately, what with the goat farming and all.

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]){
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

    

    // MAKE SYNTHS - ~40 microseconds
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

    

    // DO BUFFER STUFF - 6.5 SECONDS
    vector<double> buffer;
    wav.openWav();
    seq.addBeats(buffer, ceil(score.length));
    seq.addSilence(buffer, maxDuration);
    // 0.3 seconds from 6.5

    // write instruments to their own buffers
    for (vector<noteEvent>::iterator i = begin(score.score); i != end(score.score); ++i){
            
        int index = seq.getSampleIndexFromCursor(4, i->beatIndex); // 0 microseconds
        // auto start = high_resolution_clock::now();
        instruments[i->instrument].addNote(buffer, index, i->frequency); // LONG ONE
        // auto stop = high_resolution_clock::now();
        // auto duration = duration_cast<microseconds>(stop - start);
        // cout << "time: " << duration.count() << " microseconds" << endl;
    }
    
    // DO EFFECTS AND PRINT TO MASTER BUFFER - 12 SECONDS!
    int j = 0;
    for (vector <Synth>::iterator it = begin(instruments); it!= end(instruments); ++it){
        fx.applyFX(it->buffer, score.fxMap, j); // LONG ONE!
        j++;
        for (int i = 0; i < it->buffer.size(); i++) {
            buffer[i] += double(it->buffer[i]);
        }
    }
    
    fx.normalise(buffer);  // LONG ONE!
    // fx.addNoiseFloor(buffer, 0.001);  // LONG ONE!
    wav.writeBuffer(buffer);  // LONG ONE!
    
    wav.closeWav(); 
    cout << "RENDERED: \t"<< score.title <<".wav" << endl;
    auto stopM = high_resolution_clock::now();
    auto durationM = duration_cast<microseconds>(stopM - startM);
    cout << "Processing Time: " << durationM.count()/1000000.0 << " seconds" << endl;
    return 0;
}