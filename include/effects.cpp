#include "effects.h"

Effects::Effects(int sampleRate){
    sR = sampleRate;
}

void Effects::applyFX(vector<double> & bT, map <int, vector< fx> > fxMap, int index){
    if (fxMap.find(index) != fxMap.end()){
        for (vector< fx>::iterator it = fxMap[index].begin(); it != fxMap[index].end(); ++it){
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
        }
    }
}

void Effects::addNoiseFloor(vector<double> & b,double range){
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        *it = *it + (((rand() % 1000000)/500000.0) - 1.0) * range;
    }
}

// void Effects::LFO(vector<double> & b,double lfoFreq, double lfoAmp) {
void Effects::LFO(vector<double> & b, vector<double> params) {
    double lfoFreq = params[0];
    double lfoAmp = params[1];
    int n = 0;
    double LFO = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        LFO = lfoAmp * sin( (TWO_PI * n * lfoFreq) / sR );
        *it *= LFO;
        n++;
    }
}
// void Effects::modEcho(vector<double> & b,double decay, double time, double lfoFreq, double lfoAmp) {
void Effects::modEcho(vector<double> & b, vector<double> params) {
    double decay = params[0];
    double time = params[1];
    double lfoFreq = params[2];
    double lfoAmp = params[3];
    double invDecay = 1.0 - decay;
    time *= sR;
    int n = 0;
    double LFO = 0;
    double current = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        LFO = lfoAmp * sin( (TWO_PI * n * lfoFreq) / sR );
        current = int(LFO * time);
        if ((it - begin(b)> current) && (it - current < end(b))) {
            *it = invDecay * (*it) + decay * (*(it-current));
        }
        n++;
    }
}

// void Effects::echo(vector<double> & b,double decay, double time) {
void Effects::echo(vector<double> & b, vector<double> params) {
    double decay = params[0];
    double time = params[1];
    double invDecay = 1.0 - decay;
    time *= sR;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        if (it - begin(b)> time) {
            if (*(it - time) != 0.0) {
                *it = invDecay * (*it) + decay * (*(it-time));
            }
        }
    }
}
// void Effects::filter(vector<double> & b,double K, bool flip) {
void Effects::filter(vector<double> & b, vector<double> params, bool flip) {
    double K = params[0];
    double invK = 1.0 - K;
    for (int i = 1; i < b.size(); i++) {
        
        if (flip == true) {
            double v = invK * b[i] + K * b[i-1];
            b[i] -= v;
        } else {
            double v = K * b[i] + invK * b[i-1];
            b[i] = v;
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
    double minmax = getMinMax(b);
    //scaling bitcrush cause otherwise will be meh
    double ratio = bdmap / minmax;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        double temp = floor(*i * ratio);
        *i = temp / ratio;
    }
}

void Effects::wavefold(vector<double> & b, vector<double> params) {
// void Effects::wavefold(vector<double> & b,double L) {
    double minmax = getMinMax(b);
    double uthresh = params[0] * minmax;
    double lthresh = params[0] * minmax * -1;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        if (*i > uthresh) {
            double delta = *i - uthresh;
            *i = uthresh - delta;
        } else if (*i < lthresh) {
            double delta = *i - lthresh;
            *i = lthresh - delta;
        }
        *i *= (1.0 + params[0]);
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
    double pregain = getMinMax(b);
    normalise(b);
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        int sign = *i / abs(*i);
        sign = min(1, sign);
        sign = max(-1, sign);
        double fz = sign * (1 - exp (k * sign * *i));
        *i = ((invMix) * *i) + mix *(fz);
    }
    normalise(b);
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        *i *= pregain;
    }
}
// void Effects::overdrive(vector<double> & b,double thresh, double mix){
void Effects::overdrive(vector<double> & b, vector<double> params) {
    double thresh = params[0];
    double mix = params[1];
    double pregain = getMinMax(b);
    normalise(b);
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
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
    }
    double postgain = getMinMax(b);
    double attenuate = pregain / postgain;

    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        *i *= attenuate;
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