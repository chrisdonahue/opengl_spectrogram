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

#include <assert.h>
#include <cmath>
#include <limits>
#include <string>

#include <GL/glew.h>

#include "JuceHeader.h"
#include "dependencies/kiss_fft130/tools/kiss_fftr.h"

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
        
        int get_num_samples() {
        	return num_samples;
        }
        
        int get_bits_per_sample() {
        	return bits_per_sample;
        }
        
        double get_frequency_sampling() {
        	return frequency_sampling;
        }
        
        float* get_samples() {
        	return samples;
        }

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
        
        int get_fft_size() {
			assert(fft_performed);
			return fft_size;
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
    private:
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

    class wav_data_player : public juce::AudioIODeviceCallback {
    public: 
        wav_data_player(const juce::CriticalSection& file_lock, wav_data* file, int num_frames_on_screen, float compress_factor, float spectrum_display_percent, float* offset_x) :
            file_lock(file_lock), file(file),
            num_frames_on_screen(num_frames_on_screen), compress_factor(compress_factor),
            spectrum_display_percent(spectrum_display_percent),
            offset_x(offset_x) {
        	file_num_samples = file->get_num_samples();
        	file_samples = file->get_samples();
        	file_num_samples_played = 0;
        	
        	file_fft_size = file->get_fft_size();
            file_num_frames = file->get_num_frames();
            file_num_bins_per_frame = file->get_num_bins_per_frame();
			file_fft_magnitude_min = file->get_fft_magnitude_min();
			file_fft_magnitude_max = file->get_fft_magnitude_max();
			
			num_bins_on_screen = (int) (file_num_bins_per_frame - 1) * spectrum_display_percent;
			
            texture = (GLbyte*) malloc(sizeof(GLbyte) * (file_num_bins_per_frame - 1) * file_num_frames);
        }
        
        ~wav_data_player() {
            free(texture);
        }
        
        void audioDeviceIOCallback(const float **inputChannelData, int numInputChannels, float **outputChannelData, int numOutputChannels, int numSamples) {
			const juce::ScopedLock sl(file_lock);
        
			// fill audio buffer
			bool silence = file_num_samples_played >= file_num_samples;
        	for (int i = 0; i < numOutputChannels; i++) {
        		float* buffer = outputChannelData[i];
        		int buffer_num_filled = 0;
        		while (file_num_samples_played < file_num_samples && buffer_num_filled < numSamples) {
        			buffer[buffer_num_filled] = file_samples[file_num_samples_played];
        			buffer_num_filled++;
        			file_num_samples_played++;
        		}
        		for (; buffer_num_filled < numSamples; buffer_num_filled++) {
        			buffer[buffer_num_filled] = 0.0f;
        		}
        	}
        	
        	// stop updating visualizer if we're done playing
        	if (silence)
        		return;
        	
        	// scroll visualizer
       		int num_fft_frames_completed = file_num_samples_played / file_fft_size;
       		float offset_per_frame = 2.0f / float(file_num_frames);
       		*offset_x = -2.0f + (num_fft_frames_completed * offset_per_frame);
        }
        
        void audioDeviceAboutToStart(AudioIODevice *device) {
        	return;
        }
        
        void audioDeviceStopped() {
        	file_num_samples_played = 0;
        }
        
        void init_gfx_resources() {
        	const juce::ScopedLock sl(file_lock);
        
			// Create our datapoints, store it as bytes
			int num_frames = file_num_frames;
			int num_bins_per_frame = file_num_bins_per_frame;
			//GLbyte graph[num_frames][num_bins_per_frame - 1];
			//GLbyte graph[num_bins_per_frame - 1][num_frames];
			double fft_magnitude_min = file_fft_magnitude_min;
			double fft_magnitude_max = file_fft_magnitude_max;
			double fft_magnitude_cieling = fft_magnitude_max * compress_factor;
			double m, b;
			audio_util::map_ranged(fft_magnitude_min, fft_magnitude_cieling, -1.0, 1.0, &m, &b);

			// init data array of GLbytes
			for (int i = 0; i < num_frames; i++) {
				double* frame_magnitudes = file->get_fft_magnitudes_frame(i);
				for (int j = 1; j < num_bins_per_frame; j++) {
				    double bin_magnitude = frame_magnitudes[j];
				    if (bin_magnitude > fft_magnitude_cieling) {
						bin_magnitude = fft_magnitude_cieling;
					}
				    double adjusted_bin_magnitude = (bin_magnitude * m) + b;
				    //graph[j - 1][i] = roundf(adjusted_bin_magnitude * 127 + 128);
					set_texture_data(j - 1, i, roundf(adjusted_bin_magnitude * 127 + 128));
				}
			}

			/* Upload the texture with our datapoints */
			glActiveTexture(GL_TEXTURE0);
			glGenTextures(1, &texture_id);
			glBindTexture(GL_TEXTURE_2D, texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, num_frames, num_bins_per_frame - 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texture);
        } 

        void set_texture_data(int i, int j, GLbyte b) {
        	*(texture + (i * file_num_frames) + j) = b;
        }
        
    private:
    	// wav file
    	const juce::CriticalSection& file_lock;
        wav_data* file;
        
        // time domain
        int file_num_samples;
        float* file_samples;
        int file_num_samples_played;
        
        // frequency domain
        int file_fft_size;
        int file_num_frames;
        int file_num_bins_per_frame;
        double file_fft_magnitude_min;
        double file_fft_magnitude_max;
        
        // gfx state
        int num_frames_on_screen;
        int num_bins_on_screen;
        float compress_factor;
        float spectrum_display_percent;
        GLbyte* texture;
        GLuint texture_id;
        float* offset_x;
    };
}

#endif  // AUDIO_UTIL_H_INCLUDED
