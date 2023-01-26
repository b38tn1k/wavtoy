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
        cout << endl << "AVG\t" << to_string((summer/it->second.size())/1000000.0) << endl << endl;
    }
}

void Effects::addNoiseFloor(vector<double> & b, double range){
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        *it = *it + (((rand() % 1000000)/500000.0) - 1.0) * range;
    }
}

double procLFO (double value, int accum, vector<double> params, int sR) {
    double LFO = params[1] * sin( (TWO_PI * accum * params[0]) / sR );
    return value * LFO;
}

void Effects::LFO(vector<double> & b, vector<double> params) {
    // double lfoFreq = params[0];
    // double lfoAmp = params[1];
    int n = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        *it = procLFO(*it, n, params, sR);
        n++;
    }
}

double procModEcho(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    double LFO = params[3] * sin( (TWO_PI * accum * params[2]) / sR );
    int current = int(LFO * params[1] * sR);
    if ((accum > current) && ((accum - current) < bT.size() )) {
        value = (1.- params[0]) * (value) + params[0] * (bT[accum-current]);
    }
    return value;

}
// void Effects::modEcho(vector<double> & b,double decay, double time, double lfoFreq, double lfoAmp) {
void Effects::modEcho(vector<double> & b, vector<double> params) {
    // double decay = params[0];
    // double time = params[1] * sR;
    // double lfoFreq  = params[2];
    // double lfoAmp  = params[3];
    int n = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        *it = procModEcho(b, *it, n, params, sR);
        n++;
    }
}

double procEcho(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    if (accum > params[1] * sR) {
        if (bT[accum - params[1] * sR] != 0.0) {
            value = (1.0 - params[0]) * (value) + params[0] * bT[accum - params[1] * sR];
        }
    }
    return value;
}

// void Effects::echo(vector<double> & b,double decay, double time) {
void Effects::echo(vector<double> & b, vector<double> params) {
    // double decay = params[0];
    // double time = params[1] * sR;
    // double invDecay = 1.0 - decay;
    int n = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        *it = procEcho(b, *it, n, params, sR);
        n++;
    }
}

double procHPF(vector<double> & bT, double value, int accum, vector<double> params){
    if (accum >= 1) {
        value -= ((1.0 - params[0]) * bT[accum] +  params[0] * bT[accum-1]);
    }
    return value;
}

double procLPF(vector<double> & bT, double value, int accum, vector<double> params){
    if (accum >= 1) {
        value = (params[0] * bT[accum] + (1.0 - params[0]) * bT[accum-1]);
    }
    return value;
}
// void Effects::filter(vector<double> & b,double K, bool isHPF) {
void Effects::filter(vector<double> & b, vector<double> params, bool isHPF) {
    // double K = params[0];
    for (int i = 1; i < b.size(); i++) {
        if (isHPF == true) {
            b[i] = procHPF(b, b[i], i, params);
        } else {
            b[i] = procLPF(b, b[i], i, params);
        }
    }
}

// double Effects::getMinMax(vector<double> & bT) {
double Effects::getMinMax(vector<double> & bT) {
    double minmax = 0;
    for (vector <double>::iterator i = bT.begin(); i!= bT.end(); ++i) {
        if (abs(*i) > minmax) {
            minmax = abs(*i);
        }
    }
    return minmax;
}

double procCrush(double value, vector<double> params) {
    return (floor(value * params[0]))/params[0];
}

// void Effects::crush(vector<double> & b,int bitz) {
void Effects::crush(vector<double> & b, vector<double> params) {
    // int bitz = params[0];
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        *i = procCrush(*i, params);
    }
}

double procWavefold(double value, vector<double> params) {
    double uthresh = params[0];
    double lthresh = params[0] * -1;
    if (value > uthresh) {
            double delta = value - uthresh;
            value = uthresh - delta;
        } else if (value < lthresh) {
            double delta = value - lthresh;
            value = lthresh - delta;
        }
    value *= (1.0 + params[0]);
    return value;

}

// void Effects::wavefold(vector<double> & b,double L) {
void Effects::wavefold(vector<double> & b, vector<double> params) {
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        *i = procWavefold(*i, params);
    }
}

// void Effects::haas(vector<double> & b,int interval) { // aim for about 10ms = 441 samples a 44.1k
void Effects::haas(vector<double> & b, vector<double> params) { // aim for about 10ms = 441 samples a 44.1k
    int interval = params[0];
    for (int i = 0; i < b.size() - (interval + 1); i++) {
        if (i % 2 == 0) {
            b[i] = b[i + interval];
        }
    }
}

void Effects::normalise(vector<double> & bT) {
    double mA = getMinMax(bT);
    double gain = 0.95/mA;
    for (vector <double>::iterator i = bT.begin(); i!= bT.end(); ++i) {
        *i *= gain;
    }
}

// void Effects::fuzz(vector<double> & b,double k, double mix) {
void Effects::fuzz(vector<double> & b, vector<double> params) {
    double k = params[0];
    double mix = params[1];
    double pregain = 0;
    double postgain = 0;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (abs(*i) > pregain) {
            pregain = abs(*i);
        }
        int sign = *i / abs(*i);
        sign = min(1, sign);
        sign = max(-1, sign);

        double fz = sign * (1 - exp (k * sign * *i));
        *i = ((1.0 - mix) * *i) + mix *(fz);
        if (abs(*i) > postgain) {
            postgain = abs(*i);
        }
    }
    double attenuate = pregain / postgain;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        *i *= attenuate;
    }
}

// void Effects::overdrive(vector<double> & b,double thresh, double mix){
void Effects::overdrive(vector<double> & b, vector<double> params){
    double thresh = params[0];
    double mix = params[1];
    double pregain = 0;
    double postgain = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        if (abs(*it) > pregain) {
            pregain = abs(*it);
        }
        double temp = *it;
        if (abs(*it) < thresh){
            temp = 2 * *it;
        }
        if (abs(*it) >= thresh){
            double mv = 2 - (3 * *it) * (3 * *it);
            if (*it > 0) {
                temp = 3 - (mv/3);
            }
            if (*it < 0) {
                temp = -1 * (3 - (mv/3));
            }
        }
        *it = (1.0 - mix) * *it + mix * temp;
        if (abs(*it) > postgain) {
            postgain = abs(*it);
        }
    }
    double attenuate = pregain / postgain;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        *i *= attenuate;
    }
}

// double Effects::mod(double n, double d) {
double Effects::mod(double n, double d) {
  n = fmod(n, d);
  if (n<0) n += d;
  return n;
}

// void Effects::distort(vector<double> & b,double gain, double mix, int mode){
void Effects::distort(vector<double> & b, vector<double> params){
    double gain = params[0];
    double mix  = params[1];
    int mode = params[2];
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        double temp = *i;
        switch (mode) {
            case 1: 
                temp = sin(gain * *i);
                break;
            case 2: 
                temp = abs(mod(2*gain * *i +2,4)-2)-1;
                break;
            case 3: 
                temp = mod(gain* *i +1,2)-1;
                break;
            case 4: 
                temp = 2 * *i * *i;
                break;
            default:
                break;
        }
        *i = (1.0 - mix) * *i + mix * temp;
    }
}