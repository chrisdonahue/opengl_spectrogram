/*
  ==============================================================================

    audio_util.h
    Created: 26 Nov 2013 3:33:41pm
    Author:  cdonahue

  ==============================================================================
*/

#ifndef AUDIO_UTIL_H_INCLUDED
#define AUDIO_UTIL_H_INCLUDED

#include <string>

#include "JuceHeader.h"
#include "dependencies/kiss_fft130/tools/kiss_fftr.h"

namespace audio_util {
    // fft helpers
    extern void calculate_fft_buffer_size(int, int, int, int*, int*, int*);
    extern void window(const char*, int n, float*);
    extern void apply_window(int, kiss_fft_scalar*, const float*);
    extern void fft_real(kiss_fftr_cfg, int, const float*, int, int, const float*, kiss_fft_scalar*, kiss_fft_cpx*, double*, double*);

    // wav file loading
    extern void get_wav_file_metadata(std::string, int*, int*, double*, double*, double*);
    extern void load_wav_file(std::string, int, int, float*);
}

#endif  // AUDIO_UTIL_H_INCLUDED
