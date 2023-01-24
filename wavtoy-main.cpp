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

// I thought using loops was cheating so I programmed my own samples. Then I thought using samples was cheating so I recorded real drums.

// I then thought that programming it was cheating so I learnt to play drums for real. I then thought using bought drums was cheating so I learnt to make my own.

// I then thought that using pre-made skins was cheating so I killed a goat and skinned it. I then thought that that was cheating too, so I grew my own goat from a baby goat. I also think that is cheating but I'm not sure where to go from here. I haven't made any music lately, what with the goat farming and all.

using namespace std;

void echo(vector<double> & b, double decay, double time, int sR) {
    double invDecay = 1.0 - decay;
    time *= sR;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        if (it - begin(b)> time) {
            *it = invDecay * (*it) + decay * (*(it-time));
        }
    }
}

void filter(vector<double> & b, double K, bool flip = false) {
    double invK = 1.0 - K;
    for (int i = 1; i < b.size(); i++) {
        double v = K * b[i] + invK * b[i-1];
        if (flip == true) {
            b[i] -= v;
        } else {
            b[i] = v;
        }
    }
}

double getMinMax(vector<double> & b) {
    double minmax = 0;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (abs(*i) > minmax) {
            minmax = abs(*i);
        }
    }
    return minmax;
}

void crush(vector<double> & b, int bitz) {
    double bdmap = bitz;
    double minmax = getMinMax(b);
    //scaling bitcrush cause otherwise will be meh
    double ratio = bdmap / minmax;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        double temp = floor(*i * ratio);
        *i = temp / ratio;
    }
}

void mult(vector<double> & b, int L) {
    double mm = getMinMax(b);
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        double mlt = *i / (abs(*i));
        *i *= (*(i + L)) * mlt;
    }
    double mm2 = getMinMax(b);
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        *i *= mm/mm2;
    }

}


void wavefold(vector<double> & b, double L) {
    double minmax = getMinMax(b);
    double uthresh = L * minmax;
    double lthresh = L * minmax * -1;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (*i > uthresh) {
            double delta = *i - uthresh;
            *i = uthresh - delta;
        } else if (*i < lthresh) {
            double delta = *i - lthresh;
            *i = lthresh - delta;
        }
        *i *= (1.0 + L);
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
    vector<double> buffer;
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
        vector <double> tempB = i->buffer;
        for (vector < vector <string> >::iterator k = begin(score.fxStrings); k != end(score.fxStrings); ++k){
            vector <string> tempFX = *k;
            if (j == stoi(tempFX[0])){
                if (tempFX[1].find("ECHO") != -1) { // can't switch on string :-<
                    echo(tempB, stof(tempFX[2]), stof(tempFX[3]), wav.sampleRate);
                }
                if (tempFX[1].find("LPF") != -1) {
                    filter(tempB, stof(tempFX[2]));
                }
                if (tempFX[1].find("HPF") != -1) {
                    filter(tempB, stof(tempFX[2]), true);
                }
                if (tempFX[1].find("WAVEFOLD") != -1) {
                    wavefold(tempB, stof(tempFX[2]));
                }
                if (tempFX[1].find("CRUSH") != -1) {
                    crush(tempB, stof(tempFX[2]));
                }
                if (tempFX[1].find("MULT") != -1) {
                    mult(tempB, stof(tempFX[2]));
                }
            }
        }
        j++;
        for (int i = 0; i < tempB.size(); i++) {
            buffer[i] += tempB[i];
        }
    }
    wav.writeBuffer(buffer);
    wav.closeWav();
    cout << "RENDERED: \t"<< score.title <<".wav" << endl;
    return 0;
}