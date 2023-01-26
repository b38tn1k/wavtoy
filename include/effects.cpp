#include "effects.h"

Effects::Effects(int sampleRate){
    sR = sampleRate;
}
//procLFO
// param[2] = params[0] / sR

// procModEcho
// param[4] = params[2] / sR
// param[5] = params[1] * sR
// param[6] = 1.0 - params[0]

//procEcho
// param[2] = 1.0 - param[0]

//procWavefold
//params[1] = -1 * params[0]
//params[2] = 1.0 +  params[0]

//procFuzz
//params[2] = 1.0 - params[1]

//proceOverdrive
//params[2] = 1.0 - params[1]

//procDist
// params[3] = 1.0 - params[1]

void Effects::doPreMath(vector< fx> & fxInstance) {
    for (vector< fx>::iterator it = fxInstance.begin(); it != fxInstance.end(); ++it){
                if (it->type.find("ECHO") != -1) {
                    it->params.push_back(1.0 - it->params[0]);
                } else if (it->type.find("LPF") != -1) {
                } else if (it->type.find("HPF") != -1) {
                } else if (it->type.find("FOLD") != -1) {
                    it->params.push_back( -1 * it->params[0]);
                    it->params.push_back(1.0 +  it->params[0]);
                } else if (it->type.find("CRUSH") != -1) {
                } else if (it->type.find("HAAS") != -1) {
                } else if (it->type.find("FUZZ") != -1) {
                    it->params.push_back (1.0 - it->params[1]);
                } else if (it->type.find("MOD") != -1) {
                    it->params.push_back (it->params[2] / sR);
                    it->params.push_back (it->params[1] * sR);
                    it->params.push_back (1.0 - it->params[0]);
                } else if (it->type.find("LFO") != -1) {
                    it->params.push_back (it->params[0] / sR);
                } else if (it->type.find("OVD") != -1) {
                    it->params.push_back (1.0 - it->params[1]);
                } else if (it->type.find("DIST") != -1) {
                    it->params.push_back (1.0 - it->params[1]);
                }
            }

}

void Effects::applyFX(vector<double> & bT, map <int, vector< fx> > fxMap, int index){
    cout << to_string(index) << endl;
    if (fxMap.find(index) != fxMap.end()){
        doPreMath(fxMap[index]);
        int accum = 0;
        double pregain = 0.0;
        double postgain = 0.0;
        auto start = high_resolution_clock::now();
        for(vector<double>::iterator i = begin(bT); i != end(bT); ++i){
            if (abs(*i) > pregain) {
                pregain = abs(*i);
            }
            for (vector< fx>::iterator it = fxMap[index].begin(); it != fxMap[index].end(); ++it){
                if (it->type.find("ECHO") != -1) { // can't switch on string :-<
                    *i = procEcho(bT, *i, accum, it->params, sR);
                } else if (it->type.find("LPF") != -1) {
                    *i = procLPF(bT, *i, accum, it->params, sR);
                } else if (it->type.find("HPF") != -1) {
                    *i = procHPF(bT, *i, accum, it->params, sR);
                } else if (it->type.find("FOLD") != -1) {
                    *i = procWavefold(bT, *i, accum, it->params, sR);
                } else if (it->type.find("CRUSH") != -1) {
                    *i = procCrush(bT, *i, accum, it->params, sR);
                } else if (it->type.find("HAAS") != -1) {
                    *i = procHaas(bT, *i, accum, it->params, sR);
                } else if (it->type.find("FUZZ") != -1) {
                    *i = procFuzz(bT, *i, accum, it->params, sR);
                } else if (it->type.find("MOD") != -1) {
                    *i = procModEcho(bT, *i, accum, it->params, sR);
                } else if (it->type.find("LFO") != -1) {
                    *i = procLFO (bT, *i, accum, it->params, sR);
                } else if (it->type.find("OVD") != -1) {
                    *i = procOverdrive(bT, *i, accum, it->params, sR);
                } else if (it->type.find("DIST") != -1) {
                    *i = procDistort(bT, *i, accum, it->params, sR);
                }
            }
            if (abs(*i) > postgain) {
                postgain = abs(*i);
            }
            accum++;
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "fx: " << duration.count()/1000000.0 << endl;
        start = high_resolution_clock::now();
        double attenuate = pregain / postgain;
        for (vector <double>::iterator i = bT.begin(); i!= bT.end(); ++i) {
            *i *= attenuate;
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cout << "atten: " << duration.count()/1000000.0 << endl;
    }
}

void Effects::addNoiseFloor(vector<double> & b, double range){
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        *it = *it + (((rand() % 1000000)/500000.0) - 1.0) * range;
    }
}

double Effects::procLFO (vector<double> & bT, double value, int accum, vector<double> params, int sR) {
    double LFO = params[1] * sin( (TWO_PI * accum * params[0]) / sR );
    return value * LFO;
}

double Effects::procModEcho(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    double LFO = params[3] * sin( (TWO_PI * accum * params[2]) / sR );
    int current = int(LFO * params[5]);
    if ((accum > current) && ((accum - current) < bT.size() )) {
        value = (params[6]) * (value) + params[0] * (bT[accum-current]);
    }
    return value;
}

double Effects::procEcho(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    if (accum > params[1] * sR) {
        if (bT[accum - params[1] * sR] != 0.0) {
            value = (params[2]) * (value) + params[0] * bT[accum - params[1] * sR];
        }
    }
    return value;
}

double Effects::procHPF(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    if (accum >= 1) {
        value -= ((1.0 - params[0]) * bT[accum] +  params[0] * bT[accum-1]);
    }
    return value;
}

double Effects::procLPF(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    if (accum >= 1) {
        value = (params[0] * bT[accum] + (1.0 - params[0]) * bT[accum-1]);
    }
    return value;
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


double Effects::procCrush(vector<double> & bT, double value, int accum, vector<double> params, int sR) {
    return (floor(value * params[0]))/params[0];
}


double Effects::procWavefold(vector<double> & bT, double value, int accum, vector<double> params, int sR) {
    double uthresh = params[0];
    double lthresh = params[1];
    if (value > uthresh) {
            double delta = value - uthresh;
            value = uthresh - delta;
        } else if (value < lthresh) {
            double delta = value - lthresh;
            value = lthresh - delta;
        }
    value *= (params[2]);
    return value;

}

double Effects::procHaas(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    if (accum % 2 == 0 && accum < bT.size() - (params[0] + 1)) {
            value = bT[accum + int(params[0])];
    } else {
        value = bT[accum];
    }
    return value;
}

void Effects::normalise(vector<double> & bT) {
    double mA = getMinMax(bT);
    double gain = 0.95/mA;
    for (vector <double>::iterator i = bT.begin(); i!= bT.end(); ++i) {
        *i *= gain;
    }
}

double Effects::procFuzz(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    int sign = value / abs(value);
    sign = min(1, sign);
    sign = max(-1, sign);
    double fz = sign * (1 - exp (params[0] * sign * value));
    value = ((params[2]) * value) + params[1] *(fz);
    return value;
}


double Effects::procOverdrive(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    double temp = value;
    if (abs(value) < params[0]){
        temp = 2 * value;
    }
    if (abs(value) >= params[0]){
        double mv = 2 - (3 * value) * (3 * value);
        if (value > 0) {
            temp = 3 - (mv/3);
        }
        if (value < 0) {
            temp = -1 * (3 - (mv/3));
        }
    }
    value = (params[2]) * value + params[1] * temp;
    return value;
}

double Effects::mod(double n, double d) {
  n = fmod(n, d);
  if (n<0) n += d;
  return n;
}

double Effects::procDistort(vector<double> & bT, double value, int accum, vector<double> params, int sR){
    double temp = value;
    int mode = int(params[2]);
    switch (mode) {
        case 1: 
            temp = sin(params[0] * value);
            break;
        case 2: 
            temp = abs(mod(2*params[0] * value +2,4)-2)-1;
            break;
        case 3: 
            temp = mod(params[0]* value +1,2)-1;
            break;
        case 4: 
            temp = 2 * value * value;
            break;
        default:
            break;
    }
    value = (params[3]) * value + params[1] * temp;
    return value;
}