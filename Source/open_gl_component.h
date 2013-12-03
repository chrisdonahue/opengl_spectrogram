/*
  ==============================================================================

    open_gl_component.h
    Created: 2 Dec 2013 2:04:27pm
    Author:  Chris

  ==============================================================================
*/

#ifndef OPEN_GL_COMPONENT_H_INCLUDED
#define OPEN_GL_COMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "audio_util.h"
#include "spectrogram_component.h"

//==============================================================================
/*
*/
class open_gl_component    : public Component,
							 private OpenGLRenderer,
							 public Timer
{
public:
    open_gl_component();
    ~open_gl_component();

    void paint (Graphics&);
    void resized();

	// mouse callbacks
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& d);
    void mouseMagnify (const MouseEvent&, float magnifyAmmount);

	// open GL callbacks
	void newOpenGLContextCreated();
	void renderOpenGL();
	void openGLContextClosing();

	// timer callback
    void timerCallback();
	
	// wav file methods
	void set_wav_file(std::string file_path);
	void compute_fft(int, int, std::string);
	
private:
	// openGL state
	OpenGLContext open_gl_context;

	// wav file
    audio_util::wav_data* wav_file;

	// old timer state
	int last_fft_size;
	int last_fft_overlap;
	std::string last_fft_window_type;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (open_gl_component)
};


#endif  // OPEN_GL_COMPONENT_H_INCLUDED
