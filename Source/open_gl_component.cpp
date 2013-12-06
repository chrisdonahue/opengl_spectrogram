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

	// openGL init
	open_gl_context.setRenderer(this);
	open_gl_context.attachTo(*this);
	open_gl_context.setContinuousRepainting(true);
	rotation = 0.0f;

	// start UI changed timer
    startTimer(1000);
}

open_gl_component::~open_gl_component()
{
	if (wav_file != nullptr)
		delete wav_file;
	open_gl_context.detach();
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
	const ScopedLock sl(wav_file_lock);
	if (wav_file == nullptr)
		return;

    jassert (OpenGLHelpers::isContextActive());

    //const float desktopScale = (float) open_gl_context.getRenderingScale();
    OpenGLHelpers::clear (Colours::black);

	// draw spectrogram
	int num_frames = wav_file->get_num_frames();
	int num_bins = wav_file->get_num_bins_per_frame();

	// orient camera
	glViewport (0, 0, (GLsizei) getWidth(), (GLsizei) getHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, (float) num_frames);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// draw axes
	/*
	glPushMatrix();
		glBegin(GL_LINE_STRIP);
			glColor3f (1.0f, 1.0f, 1.0f);
			glVertex2f(0.0f + PIXEL_EPSILON, 0.0f + PIXEL_EPSILON);
			glVertex2f(0.0f + PIXEL_EPSILON, 1.0f);
		glEnd();
	glPopMatrix();
	*/

	glPushMatrix();
		//glRotatef(rotation, 1.0f, 0.0f, 0.0f);
		for (int i = 0; i < num_frames; i++) {
			double* magnitudes = wav_file->get_fft_magnitudes_frame(i);
			float z = (float) i;
			glBegin(GL_LINE_STRIP);
				glColor3f (1.0f, 0.0f, z);
				glVertex2f(-1.0f, 0.0f);
				for (int j = 1; j < num_bins; j++) {
					float x = float(j)/float(num_bins);
					float y = (float) magnitudes[j];
					//std::cerr << "Plotting: (" << x << ", " << y << ")" << std::endl;
					glColor3f (1.0f, 0.0f, y);
					glVertex3f(x, y, z);
				}
			glEnd();
		}
	glPopMatrix();
	glFlush();

	rotation += 1.0f;

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
	const ScopedLock sl(wav_file_lock);
	if (wav_file == nullptr)
		return;

	// retrieve values from parent UI
	spectrogram_component* parent = ((spectrogram_component*) getParentComponent());
	current_fft_size = parent->get_fft_size();
	current_fft_overlap = parent->get_fft_overlap();
	current_fft_window_type = parent->get_fft_window_type();

	// update values if changed
    if (
			current_fft_size != last_fft_size ||
			current_fft_overlap != last_fft_overlap ||
			current_fft_window_type.compare(last_fft_window_type) != 0
		)
	{
        compute_fft();
    }
}

void open_gl_component::init_fft_params(int fft_size, int fft_overlap, std::string fft_window_type) {
	current_fft_size = fft_size;
	current_fft_overlap = fft_overlap;
	current_fft_window_type = fft_window_type;
	last_fft_size = fft_size;
	last_fft_overlap = fft_overlap;
	last_fft_window_type = fft_window_type;
}

void open_gl_component::set_wav_file(std::string file_path) {
	const ScopedLock sl(wav_file_lock);
	if (wav_file != nullptr) {
		delete wav_file;
	}
	wav_file = new audio_util::wav_data(file_path);

	// compute initial fft
	compute_fft();
}

void open_gl_component::compute_fft() {
	const ScopedLock sl(wav_file_lock);
	if (wav_file == nullptr)
		return;

	// use current UI values
	int fft_size = current_fft_size;
	int fft_overlap = current_fft_overlap;
	std::string fft_window_type = current_fft_window_type;

    // re-compute fft
    std::cerr << "Recomputing FFT with size " << fft_size << ", overlap " << fft_overlap << ", and window \'" << fft_window_type << "\'" << std::endl;
    wav_file->perform_fft(fft_size, fft_overlap, fft_window_type);

	//int num_bins = wav_file->get_num_bins_per_frame();
	//double* magnitudes = wav_file->get_fft_magnitudes_frame(2);

    /*
    for (int i = 1; i < num_bins; i++) {
        float x = float(i - 1)/float(num_bins - 1);
        float y = magnitudes[i];
        //std::cerr << "Plotting: (" << x << ", " << y << ")" << std::endl;
    }
    */

	// set last references
	last_fft_size = fft_size;
	last_fft_overlap = fft_overlap;
	last_fft_window_type = fft_window_type;
}
