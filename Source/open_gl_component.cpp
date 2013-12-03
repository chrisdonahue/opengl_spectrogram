/*
  ==============================================================================

    open_gl_component.cpp
    Created: 2 Dec 2013 2:04:27pm
    Author:  Chris

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "open_gl_component.h"

//==============================================================================
open_gl_component::open_gl_component()
{
	// load wav file
	wav_file = nullptr;
#ifdef _WIN32
    //set_wav_file("C:\\Code\\opengl_spectrogram\\test_sound.wav");
	set_wav_file("D:\\My Code\\opengl_spectrogram\\test_sound.wav");
#else
    set_wav_file("../../../test_sound.wav");
#endif

	// compute initial fft
	compute_fft(1024, 0, "rectangle");

    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

	// openGL init
	open_gl_context.setRenderer(this);
	open_gl_context.attachTo(*this);
	open_gl_context.setContinuousRepainting(true);

	// start UI changed timer
    startTimer(5000);
}

open_gl_component::~open_gl_component()
{
    delete wav_file;
}

void open_gl_component::paint (Graphics& g)
{
}

void open_gl_component::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void open_gl_component::mouseDown (const MouseEvent& e)
{
    //demo.draggableOrientation.mouseDown (e.getPosition());
}

void open_gl_component::mouseDrag (const MouseEvent& e)
{
    //demo.draggableOrientation.mouseDrag (e.getPosition());
}

void open_gl_component::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& d)
{
    //sizeSlider.setValue (sizeSlider.getValue() + d.deltaY);
}

void open_gl_component::mouseMagnify (const MouseEvent&, float magnifyAmmount)
{
    //sizeSlider.setValue (sizeSlider.getValue() + magnifyAmmount - 1.0f);
}

void open_gl_component::newOpenGLContextCreated() {
}

void open_gl_component::renderOpenGL() {
    jassert (OpenGLHelpers::isContextActive());

    const float desktopScale = (float) open_gl_context.getRenderingScale();
    OpenGLHelpers::clear (Colours::black);

	int num_bins = wav_file->get_num_bins_per_frame();
	double* magnitudes = wav_file->get_fft_magnitudes_frame(0);

    glBegin(GL_LINE_STRIP);
		for (int i = 0; i < num_bins; i++) {
			glVertex2f((float) i, magnitudes[i]);
		}
    glEnd(); 

    /*
	glPushMatrix();
		// White side - BACK
		glBegin(GL_POLYGON);
		glColor3f(   1.0,  1.0, 1.0 );
		glVertex3f(  0.5, -0.5, 0.5 );
		glVertex3f(  0.5,  0.5, 0.5 );
		glVertex3f( -0.5,  0.5, 0.5 );
		glVertex3f( -0.5, -0.5, 0.5 );
		glEnd();
 
		// Purple side - RIGHT
		glBegin(GL_POLYGON);
		glColor3f(  1.0,  0.0,  1.0 );
		glVertex3f( 0.5, -0.5, -0.5 );
		glVertex3f( 0.5,  0.5, -0.5 );
		glVertex3f( 0.5,  0.5,  0.5 );
		glVertex3f( 0.5, -0.5,  0.5 );
		glEnd();
 
		// Green side - LEFT
		glBegin(GL_POLYGON);
		glColor3f(   0.0,  1.0,  0.0 );
		glVertex3f( -0.5, -0.5,  0.5 );
		glVertex3f( -0.5,  0.5,  0.5 );
		glVertex3f( -0.5,  0.5, -0.5 );
		glVertex3f( -0.5, -0.5, -0.5 );
		glEnd();
 
		// Blue side - TOP
		glBegin(GL_POLYGON);
		glColor3f(   0.0,  0.0,  1.0 );
		glVertex3f(  0.5,  0.5,  0.5 );
		glVertex3f(  0.5,  0.5, -0.5 );
		glVertex3f( -0.5,  0.5, -0.5 );
		glVertex3f( -0.5,  0.5,  0.5 );
		glEnd();
 
		// Red side - BOTTOM
		glBegin(GL_POLYGON);
		glColor3f(   1.0,  0.0,  0.0 );
		glVertex3f(  0.5, -0.5, -0.5 );
		glVertex3f(  0.5, -0.5,  0.5 );
		glVertex3f( -0.5, -0.5,  0.5 );
		glVertex3f( -0.5, -0.5, -0.5 );
		glEnd();
	glPopMatrix();
    */

	/*
	glPushMatrix();
		glBegin(GL_LINES); 
			glVertex3f( -2, -2, 0.6);       // P1
			glVertex3f( -2,  2, 0.6);       // P2
		glEnd();
	glPopMatrix();
	*/
}

void open_gl_component::openGLContextClosing() {
}

void open_gl_component::timerCallback() {
	// retrieve values from UI
	spectrogram_component* parent = ((spectrogram_component*) getParentComponent());
	int current_fft_size = parent->get_fft_size();
	int current_fft_overlap = parent->get_fft_overlap();
	std::string current_fft_window_type = parent->get_fft_window_type();

	// update values if changed
    if (
			current_fft_size != last_fft_size ||
			current_fft_overlap != last_fft_overlap ||
			current_fft_window_type.compare(last_fft_window_type) != 0
		)
	{
        compute_fft(current_fft_size, current_fft_overlap, current_fft_window_type);
    }
}

void open_gl_component::set_wav_file(std::string file_path) {
	if (wav_file != nullptr) {
		delete wav_file;
	}
	wav_file = new audio_util::wav_data(file_path);
}

void open_gl_component::compute_fft(int fft_size, int fft_overlap, std::string fft_window_type) {
    // re-compute fft
    std::cerr << "Recomputing FFT with size: " << fft_size << ", overlap: " << fft_overlap << ", and window: " << fft_window_type << std::endl;
    wav_file->perform_fft(fft_size, fft_overlap, fft_window_type);

	// set last references
	last_fft_size = fft_size;
	last_fft_overlap = fft_overlap;
	last_fft_window_type = fft_window_type;
}