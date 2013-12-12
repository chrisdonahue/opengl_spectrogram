/*
  ==============================================================================

    audio_util.cpp
    Created: 26 Nov 2013 3:33:42pm
    Author:  cdonahue

  ==============================================================================
*/

#include "audio_util.h"

/*
	fft helpers
*/

void audio_util::calculate_fft_buffer_size(int num_frames, int n, int o, int* fft_num_bins, int* fft_num_frames, int* fft_output_buffer_length) {
    int num_fft_calls = 0;
    int shift = n - o;
    for (int i = 0; i < num_frames;) {
        num_fft_calls++;
        i += shift;
    }
    *fft_num_bins = ((n/2) + 1);
    *fft_num_frames = num_fft_calls;
    *fft_output_buffer_length = num_fft_calls * (*fft_num_bins);
}

void audio_util::fill_window_buffer(std::string type, int n, float* window_buffer) {
    if (type.compare("rectangle") == 0) {
        for (int i = 0; i < n; i++) {
            window_buffer[i] = 1.0;
        }
    }
    else if (type.compare("hanning") == 0) {
        // 0.5 * (1 - cos(2*pi*n)/(N-1))
        double insideCosineValue = 0.0;
        double increment = (2 * M_PI)/(n - 1);
        for (int i = 0; i < n; i++) {
            window_buffer[i] = 0.5 * (1 - cos(insideCosineValue));
            insideCosineValue += increment;
        }
    }
    else {
        for (int i = 0; i < n; i++) {
            window_buffer[i] = 0.0f;
        }
    }
}

void audio_util::apply_window(int n, kiss_fft_scalar* buffer, const float* window) {
    for (int i = 0; i < n; i++) {
        buffer[i] *= window[i];
    }
}

void audio_util::fft_real(kiss_fftr_cfg cfg, int num_frames, const float* input, int n, int overlap, const float* window, kiss_fft_scalar* in_buffer, kiss_fft_cpx* out_buffer, double* magnitude, double* phase) {
    int fft_output_size = (n/2 + 1);
    int shift = n - overlap;
    int num_completed = 0;
    int num_remaining = num_frames;
    int num_fft_output_used = 0;
    while (num_remaining > 0) {
        // fill the input buffer
        int num_to_transform = num_remaining > n ? n : num_remaining;
        for (size_t i = 0; i < num_to_transform; i++) {
            in_buffer[i] = input[num_completed + i];
        }
        num_completed += shift;
        num_remaining -= shift;
        // 0 out rest of input buffer if we're out of frames
        for (size_t i = num_to_transform; i < n; i++) {
            in_buffer[i] = 0;
        }

        // apply window
        audio_util::apply_window(n, in_buffer, window);

        // perform fft
        kiss_fftr(cfg, in_buffer, out_buffer + num_fft_output_used);

        // analyze output
        //printf("FREQ\t\tREAL\tIMAG\tMAG\tPHASE\n");
        for (size_t bin = num_fft_output_used; bin < num_fft_output_used + fft_output_size; bin++) {
            magnitude[bin] = sqrt(out_buffer[bin].r * out_buffer[bin].r + out_buffer[bin].i * out_buffer[bin].i);
            if (out_buffer[bin].r == 0 && out_buffer[bin].i == 0) {
                phase[bin] = 0;
            }
            else {
                phase[bin] = atan(out_buffer[bin].i / out_buffer[bin].r);
            }
            //printf("%.1lf\t\t%.2lf\t%.2lf\t%.2lf\t%.2lf\n", (44100.0 / n) * bin, out_buffer[bin].r, out_buffer[bin].i, magnitude[bin], phase[bin]);
            //std::cout << "BIN: " << bin << ", REAL: " << out_buffer[bin].r << ", IMAGINARY:" << out_buffer[bin].i << ", MAG: " << magnitude[bin] << ", PHASE: " << phase[bin] << std::endl;
        }
        num_fft_output_used += fft_output_size;
    }
}

/*
	wav file interface
*/

void audio_util::get_wav_file_metadata(std::string path, int* num_frames, int* bits_per_sample, double* length_seconds, double* sampling_frequency, double* nyquist_frequency) {
    using namespace juce;
    File input(path);
    if (!(input.existsAsFile())) {
        std::cerr << "Invalid input file: " << path << std::endl;
        return;
    }
    FileInputStream* fis = input.createInputStream();
    ScopedPointer<WavAudioFormat> wavFormat(new WavAudioFormat());
    ScopedPointer<AudioFormatReader> afr(wavFormat->createReaderFor(fis, true));

    // get info on target from JUCE
    *num_frames = afr->lengthInSamples;
    *sampling_frequency = afr->sampleRate;
    *bits_per_sample = afr->bitsPerSample;

    // calculate more info on target
    *length_seconds = (*num_frames) / (*sampling_frequency);
    *nyquist_frequency = (*sampling_frequency) / 2;
}

void audio_util::load_wav_file(std::string path, int chunk_size, int num_frames, float* buffer) {
    File input(path);
    if (!(input.existsAsFile())) {
        std::cerr << "Invalid input file: " << path << std::endl;
        return;
    }
    FileInputStream* fis = input.createInputStream();
    ScopedPointer<WavAudioFormat> wavFormat(new WavAudioFormat());
    ScopedPointer<AudioFormatReader> afr(wavFormat->createReaderFor(fis, true));

    // get waveform of target
    AudioSampleBuffer asb(1, num_frames);
    afr->read(&asb, 0, num_frames, 0, false, true);
    float* chanData = asb.getSampleData(0);
    memcpy(buffer, chanData, sizeof(float) * num_frames);
}

void audio_util::load_wav_file(std::string path, wav_data* container) {

}
