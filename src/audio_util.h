/*
  ==============================================================================

    audio_util.h
    Created: 26 Nov 2013 3:33:41pm
    Author:  cdonahue

  ==============================================================================
*/

#ifndef AUDIO_UTIL_H_INCLUDED
#define AUDIO_UTIL_H_INCLUDED

#define LOAD_WAV_BUFFER_SIZE 512

#define _USE_MATH_DEFINES

#include "JuceHeader.h"
#include "dependencies/kiss_fft130/tools/kiss_fftr.h"

#include <assert.h>
#include <cmath>
#include <limits>
#include <string>

namespace audio_util {
    class wav_data;

    // fft helpers
    extern void calculate_fft_buffer_size(int, int, int, int*, int*, int*);
    extern void fill_window_buffer(std::string, int n, float*);
    extern void apply_window(int, kiss_fft_scalar*, const float*);
    extern void fft_real(kiss_fftr_cfg, int, const float*, int, int, const float*, kiss_fft_scalar*, kiss_fft_cpx*, double*, double*);

    // wav file loading
    extern void get_wav_file_metadata(std::string, int*, int*, double*, double*, double*);
    extern void load_wav_file(std::string, int, int, float*);
    extern void load_wav_file(std::string, wav_data*);

    // other helpers
    extern void map_ranged(double, double, double, double, double*, double*);

    // class to hold a wave file
    class wav_data {
        public:
        wav_data(std::string source_file_path) :
            source_file_path(source_file_path),
            fft_performed(false) {
            get_wav_file_metadata(source_file_path, &num_samples, &bits_per_sample, &length_seconds, &frequency_sampling, &frequency_nyquist);
            samples = (float*) malloc(sizeof(float) * num_samples);
            load_wav_file(source_file_path, LOAD_WAV_BUFFER_SIZE, num_samples, samples);
            last_sample_start_time = (double) (double(num_samples - 1)/frequency_sampling);
        };

        ~wav_data() {
            free(samples);
            delete_fft_data();
        };

        // takes the fft of time domain data
        void perform_fft(int size, int overlap, std::string window) {
            delete_fft_data();

            // calculate fft info
            fft_size = size;
            fft_overlap = overlap;
            int fft_num_bins;
            int fft_output_buffer_length;
            calculate_fft_buffer_size(num_samples, fft_size, fft_overlap, &fft_num_bins, &fft_num_frames, &fft_output_buffer_length);

            // allocate temporary buffers
            kiss_fftr_cfg fft_config = kiss_fftr_alloc(fft_size, 0/*is_inverse_fft*/, NULL, NULL);
            kiss_fft_scalar* fft_amplitude_buffer = (kiss_fft_scalar*) malloc(sizeof(kiss_fft_scalar) * fft_size);
            float* fft_window = (float*) malloc(sizeof(float) * fft_size);

            // allocate persistent buffers
            fft_spectra = (kiss_fft_cpx*) malloc(sizeof(kiss_fft_cpx) * fft_output_buffer_length);
            fft_magnitudes = (double*) malloc(sizeof(double) * fft_output_buffer_length);
            fft_phases = (double*) malloc(sizeof(double) * fft_output_buffer_length);

            // create window
            fill_window_buffer(window, fft_size, fft_window);
           
            // perform fft
            fft_real(fft_config, num_samples, samples, fft_size, fft_overlap, fft_window, fft_amplitude_buffer, fft_spectra, fft_magnitudes, fft_phases);

            // free buffers
            free(fft_config);
            free(fft_amplitude_buffer);
            free(fft_window);

            // calculate min/max
            fft_magnitude_min = std::numeric_limits<double>::infinity();
            fft_magnitude_max = std::numeric_limits<double>::infinity() * -1.0;
            fft_magnitude_min_per_frame = (double*) malloc(sizeof(double) * fft_num_frames);
            fft_magnitude_max_per_frame = (double*) malloc(sizeof(double) * fft_num_frames);
            for (int i = 0; i < fft_num_frames; i++) {
                // init temp variables for frame_min, frame_max
                double frame_min = std::numeric_limits<double>::infinity();
                double frame_max = std::numeric_limits<double>::infinity() * -1.0;

                // find frame min/max
                for (int j = 1; j < fft_num_bins; j++) {
                    double bin_val = *(fft_magnitudes + (fft_num_bins * i) + j);
                    if (bin_val < frame_min)
                        frame_min = bin_val;
                    if (bin_val > frame_max)
                        frame_max = bin_val;
                }

                // set min/max array value
                fft_magnitude_min_per_frame[i] = frame_min;
                fft_magnitude_max_per_frame[i] = frame_max;

                // update global min/max
                if (frame_min < fft_magnitude_min)
                    fft_magnitude_min = frame_min;
                if (frame_max > fft_magnitude_max)
                    fft_magnitude_max = frame_max;
            }

            // set fft performed flag to true
            fft_performed = true;
        };
        
        void delete_fft_data() {
            if (fft_performed) {
                free(fft_spectra);
                free(fft_magnitudes);
                free(fft_phases);
                free(fft_magnitude_min_per_frame);
                free(fft_magnitude_max_per_frame);
                fft_performed = false;
            }
        }

        int get_num_frames() {
            assert(fft_performed);
            return fft_num_frames;
        }

        int get_num_bins_per_frame() {
            assert(fft_performed);
            return (fft_size / 2 + 1);
        }

        double* get_fft_magnitudes_frame(int i) {
            assert(fft_performed);
            assert(i < fft_num_frames);
            return fft_magnitudes + (get_num_bins_per_frame() * i);
        }

        double* get_fft_phases_frame(int i) {
            assert(fft_performed);
            assert(i < fft_num_frames);
            return fft_phases + (get_num_bins_per_frame() * i);
        }

        double get_fft_magnitude_min_frame(int i) {
            assert(fft_performed);
            return fft_magnitude_min_per_frame[i];
        }

        double get_fft_magnitude_max_frame(int i) {
            assert(fft_performed);
            return fft_magnitude_max_per_frame[i];
        }

        double get_fft_magnitude_min() {
            assert(fft_performed);
            return fft_magnitude_min;
        }

        double get_fft_magnitude_max() {
            assert(fft_performed);
            return fft_magnitude_max;
        }

        // source file path for this wav file
        std::string source_file_path;
        
        // time domain
        int num_samples;
        int bits_per_sample;
        double length_seconds;
        double last_sample_start_time;
        double frequency_sampling;
        double frequency_nyquist;
        float* samples;

        // frequency domain
        bool fft_performed;
        int fft_size;
        int fft_overlap;
        int fft_num_frames;
        kiss_fft_cpx* fft_spectra;
        double* fft_magnitudes;
        double* fft_magnitude_min_per_frame;
        double* fft_magnitude_max_per_frame;
        double fft_magnitude_min;
        double fft_magnitude_max;
        double* fft_phases;
        kiss_fft_scalar* fft_amplitude_buffer;
        kiss_fft_cpx* fft_spectra_buffer;
        double* fft_magnitude_buffer;
    };
}

#endif  // AUDIO_UTIL_H_INCLUDED
