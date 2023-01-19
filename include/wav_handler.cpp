#include "wav_handler.h"

void WAV::openWav(){
    f = ofstream(filename, ios::binary);
    // Write the file headers
    f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
    write_word( f,     16, 4 );  // no extension data
    write_word( f,      1, 2 );  // PCM - integer samples
    write_word( f,      channelCount, 2 );  // two channels (stereo file)
    write_word( f,  sampleRate, 4 );  // samples per second (Hz)
    write_word( f, (u_int)((sampleRate*bitDepth*channelCount)/8), 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
    write_word( f,      4, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
    write_word( f,     bitDepth, 2 );  // number of bits per sample (use a multiple of 8)

    // Write the data chunk header
    data_chunk_pos = f.tellp();
    f << "data----";  // (chunk size to be filled in later)
}

void WAV::writeSample(int value, int size){
    write_word(f, value, size);
}

void WAV::writeBuffer(vector<int> buffer) {
    for(vector<int>::iterator it = begin(buffer); it != end(buffer); ++it){
        writeSample(*it, 2);
    }
}

void WAV::closeWav(){
    u_int file_length = f.tellp();
    f.seekp(data_chunk_pos + 4);
    write_word( f, file_length - data_chunk_pos + 8 );
    f.seekp(0 + 4);
    write_word( f, file_length - 8, 4 );
}
