/*
  ==============================================================================

    open_gl_component.h
    Created: 2 Dec 2013 2:04:27pm
    Author:  Chris

  ==============================================================================
*/

#ifndef OPEN_GL_COMPONENT_H_INCLUDED
#define OPEN_GL_COMPONENT_H_INCLUDED

#define PIXEL_EPSILON 1e-3f

#include <assert.h>

#include "../JuceLibraryCode/JuceHeader.h"

#include "audio_util.h"
#include "open_gl_juce_demo_structs.h"

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

	// open gl callbacks
	void newOpenGLContextCreated();
	void renderOpenGL();
	void openGLContextClosing();

	// timer callback
    void timerCallback();
	
	// wav file methods
	void init_fft_params(int fft_size, int fft_overlap, std::string fft_window_type);
	void set_wav_file(std::string file_path);
	void compute_fft();

	// open gl methods
	void update_shader(const String&, const String&);
	
private:
	// gl state
	OpenGLContext open_gl_context;
    ScopedPointer<OpenGLShaderProgram> shader;
    ScopedPointer<Attributes> attributes;
    ScopedPointer<Uniforms> uniforms;
	
	// model state
	bool interpolate;
	bool clamp;
	bool rotate;
	float offset_x;
	float offset_y;
	float scale;
	float rotation;

	// wav file
	CriticalSection wav_file_lock;
    audio_util::wav_data* wav_file;

	// new timer state
	int current_fft_size;
	int current_fft_overlap;
	std::string current_fft_window_type;

	// old timer state
	int last_fft_size;
	int last_fft_overlap;
	std::string last_fft_window_type;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (open_gl_component)
};


#endif  // OPEN_GL_COMPONENT_H_INCLUDED
