#include "effects.h"

Effects::Effects(int sampleRate){
    sR = sampleRate;
}

void Effects::applyFX(vector<double> & bT, map <int, vector< fx> > fxMap, int index){
    if (fxMap.find(index) != fxMap.end()){
        for (vector< fx>::iterator it = fxMap[index].begin(); it != fxMap[index].end(); ++it){
            auto start = high_resolution_clock::now();
            if (it->type.find("ECHO") != -1) { // can't switch on string :-<
                echo(bT, it->params);
            }
            if (it->type.find("LPF") != -1) {
                filter(bT, it->params, false);
            }
            if (it->type.find("HPF") != -1) {
                filter(bT, it->params, true);
            }
            if (it->type.find("FOLD") != -1) {
                wavefold(bT, it->params);
            }
            if (it->type.find("CRUSH") != -1) {
                crush(bT, it->params);
            }
            if (it->type.find("HAAS") != -1) {
                haas(bT, it->params);
            }
            if (it->type.find("FUZZ") != -1) {
                fuzz(bT, it->params);
            }
            if (it->type.find("MOD") != -1) {
                modEcho(bT, it->params);
            }
            if (it->type.find("LFO") != -1) {
                LFO(bT, it->params);
            }
            if (it->type.find("OVD") != -1) {
                overdrive(bT, it->params);
            }
            if (it->type.find("DIST") != -1) {
                distort(bT, it->params);
            }
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            timers[it->type].push_back(int(duration.count()));
        }
    }
}

void Effects::printTimes(){
    map<string, vector <int> >::iterator it;
    for (it = timers.begin(); it != timers.end(); ++it) {
        cout << it->first << "\t";
        vector <int> :: iterator blah;
        long summer = 0;
        for (blah = it->second.begin(); blah != it->second.end(); ++ blah){
            summer += *blah;
            cout << to_string(*blah) << " ";
        }
        cout << endl << "AVG\t" << to_string((summer/it->second.size())/1000000.0) << " sec" << endl << endl;
    }
}

void Effects::addNoiseFloor(vector<double> & b,double range){
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        *it = *it + (((rand() % 1000000)/500000.0) - 1.0) * range;
    }
}

// void Effects::LFO(vector<double> & b,double lfoFreq, double lfoAmp) {
void Effects::LFO(vector<double> & b, vector<double> params) {
    double lfoFreq = (TWO_PI * params[0]) / (2*sR);
    double lfoAmp = params[1];
    int n = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        if (*it != 0.0) {
            *it *= lfoAmp * sin( ( n * lfoFreq));
            n++;
        }
    }
}
// void Effects::modEcho(vector<double> & b,double decay, double time, double lfoFreq, double lfoAmp) {
void Effects::modEcho(vector<double> & b, vector<double> params) {
    double decay = params[0];
    double time = params[1] * sR;
    double lfoFreq = TWO_PI * params[2]  / (2*sR);;
    double lfoAmp = params[3];
    double invDecay = 1.0 - decay;
    int n = 0;
    double LFO = 0;
    double current = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        if ((it - begin(b)> current) && (it - current < end(b))) {
            if ((*(it-current)) != 0) {
                LFO = lfoAmp * sin(n * lfoFreq);
                current = int(LFO * time);
                *it = invDecay * (*it) + decay * (*(it-current));
            }
        }
        n++;
    }
}
// void Effects::echo(vector<double> & b,double decay, double time) {
void Effects::echo(vector<double> & b, vector<double> params) {
    double decay = params[0];
    double time = params[1] * sR;
    double invDecay = 1.0 - decay;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        if (it - begin(b)> time) {
            if ((*(it-time)) != 0) {
                if (*(it - time) != 0.0) {
                    *it = invDecay * (*it) + decay * (*(it-time));
                }
            }
        }
    }
}
// void Effects::filter(vector<double> & b,double K, bool flip) {
void Effects::filter(vector<double> & b, vector<double> params, bool flip) {
    double K = params[0];
    double invK = (1.0 - K) * 0.66;
    double invK2 = (1.0 - K) * 0.33;
    double pregain = 0;
    double postgain = 0;
    for (int i = 4; i < b.size(); i++) {
        if (b[i] != 0.0) {
            if (abs(b[i]) > pregain) {
                pregain = b[i];
            }
            if (flip == true) {
                double v = invK2 * b[i] + invK * b[i-2] + K * b[i-4];
                b[i] -= v;
            } else {
                double v = K * b[i] + invK * b[i-2] + invK2 * b[i-4];
                b[i] = v;
            }
            if (abs(b[i]) > postgain) {
                postgain = b[i];
            }
        }
    }
    double attenuate = pregain / postgain;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (*i != 0.0){
            *i *= attenuate;
        }
    }
}

double Effects::getMinMax(vector<double> & bT) {
    double minmax = 0;
    for (vector <double>::iterator i = bT.begin(); i!= bT.end(); ++i) {
        if (abs(*i) > minmax) {
            minmax = abs(*i);
        }
    }
    return minmax;
}
// void Effects::crush(vector<double> & b,int bitz) {
void Effects::crush(vector<double> & b, vector<double> params) {
    double bdmap = params[0];
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (*i != 0.0){
            *i = floor(*i * bdmap) / bdmap;
        }
    }
}

void Effects::wavefold(vector<double> & b, vector<double> params) {
// void Effects::wavefold(vector<double> & b,double L) {
    double uthresh = params[0];
    double lthresh = params[0] * -1;
    double upscale = (1.0 + params[0]);
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (*i != 0.0){
            if (*i > uthresh) {
                double delta = *i - uthresh;
                *i = uthresh - delta;
            } else if (*i < lthresh) {
                double delta = *i - lthresh;
                *i = lthresh - delta;
            }
            *i *= upscale;
        }
    }
}
// void Effects::haas(vector<double> & b,int interval) { // aim for about 10ms = 441 samples a 44.1k
void Effects::haas(vector<double> & b, vector<double> params) {
    for (int i = 0; i < b.size() - (params[0] + 1); i++) {
        if (i % 2 == 0) {
            b[i] = b[i + params[0]];
        }
    }
}

void Effects::normalise(vector<double> & bT) {
    double mA = getMinMax(bT);
    double gain = 0.999999/mA;
    for (vector <double>::iterator i = bT.begin(); i!= bT.end(); ++i) {
        *i *= gain;
    }
}
// void Effects::fuzz(vector<double> & b,double k, double mix) {
void Effects::fuzz(vector<double> & b, vector<double> params) {
    double k = params[0];
    double mix = params[1];
    double invMix = 1.0 - mix;
    double pregain = 0;
    double postgain = 0;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (*i != 0.0) {
            if (abs(*i) > pregain) {
                pregain = abs(*i);
            }
            int sign = 1;
            if (*i < 0){
                sign = -1;
            }
            double fz = sign * (1 - exp (k * sign * *i));
            *i = ((invMix) * *i) + mix *(fz);
            if (abs(*i) > postgain) {
                postgain = abs(*i);
            }
        }
    }
    double attenuate = pregain / postgain;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (*i != 0.0){
            *i *= attenuate;
        }
    }
}
// void Effects::overdrive(vector<double> & b,double thresh, double mix){
void Effects::overdrive(vector<double> & b, vector<double> params) {
    double thresh = params[0];
    double mix = params[1];
    double invMix = 1.0 - params[1];
    double pregain = 0;
    double postgain = 0;
    for(vector<double>::iterator i = begin(b); i != end(b); ++i){
        if (*i != 0.0) {
            if (abs(*i) > pregain) {
                pregain = abs(*i);
            }
            double temp = *i;
            if (abs(*i) < thresh){
                temp = 2 * *i;
            }
            if (abs(*i) >= thresh){
                double mv = 2 - (9 * (*i * *i));
                if (*i > 0) {
                    temp = 3 - (mv/3);
                }
                if (*i < 0) {
                    temp = -1 * (3 - (mv/3));
                }
            }
            *i = (invMix) * *i + mix * temp;
            if (abs(*i) > postgain) {
                postgain = abs(*i);
            }
        }
    }
    double attenuate = pregain / postgain;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (*i != 0.0){
            *i *= attenuate;
        }
    }
}

double Effects::mod(double n, double d) {
  n = fmod(n, d);
  if (n<0) n += d;
  return n;
}
// void Effects::distort(vector<double> & b,double gain, double mix, int mode){
void Effects::distort(vector<double> & b, vector<double> params) {
    double gain = params[0];
    double mix = params[1];
    double invMix = 1.0 - params[1];
    int mode = params[2];        
    switch (mode) {
        case 1: 
            for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
                if (*i != 0.0) {
                    double temp = *i;
                    temp = sin(gain * *i);
                    *i = (invMix) * *i + mix * temp;
                }
            }
            break;
        case 2:
            for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
                if (*i != 0.0) {
                    double temp = *i;
                    temp = abs(mod(2*gain * *i +2,4)-2)-1;
                    *i = (invMix) * *i + mix * temp;
                }
            }
            break;
        case 3: 
            for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
                if (*i != 0.0) {
                    double temp = *i;
                    temp = mod(gain* *i +1,2)-1;
                    *i = (invMix) * *i + mix * temp;
                }
            }
            break;
        case 4: 
            for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
                if (*i != 0.0) {
                    double temp = *i;
                    temp = 2 * *i * *i;
                    *i = (invMix) * *i + mix * temp;
                }
            }
            break;
        default:
            break;
    }
}

void Effects::compress(vector<double> & bT, double thresh, double ratio){
    for (vector <double>::iterator i = bT.begin(); i!= bT.end(); ++i) {
        if (abs(*i) > thresh) {
            double res = ratio * (abs(*i) - thresh) + thresh;
            if (*i < 0) {
                *i = -1 * res;
            } else {
                *i = res;
            }
        }

    }
}