#include "effects.h"

Effects::Effects(int sampleRate){
    sR = sampleRate;
}

void Effects::addNoiseFloor(vector<double> & b,double range){
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        *it = *it + (((rand() % 1000000)/500000.0) - 1.0) * range;
    }
}

void Effects::LFO(vector<double> & b,double lfoFreq, double lfoAmp) {
    int n = 0;
    double LFO = 0;
    for(vector<double>::iterator it = begin(b); it != end(b); ++it){
        LFO = lfoAmp * sin( (TWO_PI * n * lfoFreq) / sR );
        *it *= LFO;
        n++;
    }
}

void Effects::modEcho(vector<double> & b,double decay, double time, double lfoFreq, double lfoAmp) {
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

void Effects::echo(vector<double> & b,double decay, double time) {
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

void Effects::filter(vector<double> & b,double K, bool flip) {
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

double Effects::getMinMax(vector<double> & bT) {
    double minmax = 0;
    for (vector <double>::iterator i = bT.begin(); i!= bT.end(); ++i) {
        if (abs(*i) > minmax) {
            minmax = abs(*i);
        }
    }
    return minmax;
}

void Effects::crush(vector<double> & b,int bitz) {
    double bdmap = bitz;
    double minmax = getMinMax(b);
    //scaling bitcrush cause otherwise will be meh
    double ratio = bdmap / minmax;
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        double temp = floor(*i * ratio);
        *i = temp / ratio;
    }
}

void Effects::mult(vector<double> & b,int L) {
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


void Effects::wavefold(vector<double> & b,double L) {
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

void Effects::haas(vector<double> & b,int interval) { // aim for about 10ms = 441 samples a 44.1k
    for (int i = 0; i < b.size() - (interval + 1); i++) {
        if (i % 2 == 0) {
            b[i] = b[i + interval];
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

void Effects::fuzz(vector<double> & b,double k, double mix) {
    double pregain = getMinMax(b);
    normalise(b);
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        int sign = *i / abs(*i);
        sign = min(1, sign);
        sign = max(-1, sign);
        double fz = sign * (1 - exp (k * sign * *i));
        *i = ((1.0 - mix) * *i) + mix *(fz);
    }
    normalise(b);
    for (vector <double>::iterator i = b.begin(); i!= b.end(); ++i) {
        *i *= pregain;
    }
}

void Effects::overdrive(vector<double> & b,double thresh, double mix){
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

void Effects::distort(vector<double> & b,double gain, double mix, int mode){
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