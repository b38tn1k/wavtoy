#ifndef WAV_HANDLER_H
#define WAV_HANDLER_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

template <typename Word>
std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
{
for (; size; --size, value >>= 8)
    outs.put( static_cast <char> (value & 0xFF) );
return outs;
}

class WAV {
    public:
        string filename;
        u_int channelCount;
        u_int sampleRate;
        u_int bitDepth;
        WAV(string f){
            filename = f;
            channelCount = 2;
            sampleRate = 44100;
            bitDepth = 16;
        }
        WAV(string f, u_int8_t cC, u_int8_t sR, u_int8_t bD){
            filename = f;
            channelCount = cC;
            sampleRate = sR;
            bitDepth = bD;
        }
        ofstream f;
        void openWav();
        void closeWav();
        void writeSample(int value, int size);
        void writeBuffer(vector<int> buffer);
    private:
        u_int data_chunk_pos;
        
};

#endif