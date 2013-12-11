/*
  ==============================================================================

    open_gl_component.cpp
    Created: 2 Dec 2013 2:04:27pm
    Author:  Chris

  ==============================================================================
*/

#include "open_gl_component.h"

//==============================================================================
open_gl_component::open_gl_component(std::string vert_shader_file_path, std::string frag_shader_file_path)
{
	// load wav file
	wav_file = nullptr;
	
	// model init
	interpolate = false;
	clamp = true;
	rotate = false;
	offset_x = 0.0f;
	offset_y = 0.0f;
	scale = 1.0f;
	rotation = 0.0f;

	// gl init
	open_gl_context.setRenderer(this);
	open_gl_context.attachTo(*this);
	open_gl_context.setContinuousRepainting(true);
	
	// glew init
	bool glew_init_success = init_glew_resources(vert_shader_file_path, frag_shader_file_path);
	assert(glew_init_success);

	// start UI changed timer
    startTimer(1000);
}

open_gl_component::~open_gl_component()
{
	// delete wav file/FFT data
	if (wav_file != nullptr)
		delete wav_file;
	
	// delete glsl program
	glDeleteProgram(program);
	
	// detach from open gl
	open_gl_context.detach();
}

bool open_gl_component::init_glew_resources(std::string vert_shader_file_path, std::string frag_shader_file_path) {
	// init glew
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return false;
	}
	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "No support for OpenGL 2.0 found\n");
		return false;
	}
	
	// check something
	GLint max_units;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_units);
	if (max_units < 1) {
		fprintf(stderr, "Your GPU does not have any vertex texture image units\n");
		return false;
	}

	// create glsl program
	program = shader_util::create_program(vert_shader_file_path.c_str(), frag_shader_file_path.c_str());
	if (program == 0) {
		return false;
	}
	
	// retrieve attributes from program
	attribute_coord2d = shader_util::get_attrib(program, "coord2d");
	uniform_vertex_transform = shader_util::get_uniform(program, "vertex_transform");
	uniform_texture_transform = shader_util::get_uniform(program, "texture_transform");
	uniform_mytexture = shader_util::get_uniform(program, "mytexture");
	if (attribute_coord2d == -1 || uniform_vertex_transform == -1 || uniform_texture_transform == -1 || uniform_mytexture == -1) {
		return false;
	}
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
    
    // wireframe display
	glUseProgram(program);
	glUniform1i(uniform_mytexture, 0);

	glm::mat4 model;

	/*
	if (rotate)
		model = glm::rotate(glm::mat4(1.0f), float (glutGet(GLUT_ELAPSED_TIME) / 100.0), glm::vec3(0.0f, 0.0f, 1.0f));
	else
	*/
		model = glm::mat4(1.0f);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0, -2.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * 640 / 480, 0.1f, 10.0f);

	glm::mat4 vertex_transform = projection * view * model;
	glm::mat4 texture_transform = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1)), glm::vec3(offset_x, offset_y, 0));

	glUniformMatrix4fv(uniform_vertex_transform, 1, GL_FALSE, glm::value_ptr(vertex_transform));
	glUniformMatrix4fv(uniform_texture_transform, 1, GL_FALSE, glm::value_ptr(texture_transform));

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/* Set texture wrapping mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	/* Set texture interpolation mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);

	/* Draw the grid using the indices to our vertices using our vertex buffer objects */
	glEnableVertexAttribArray(attribute_coord2d);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glDrawElements(GL_LINES, 100 * 101 * 4, GL_UNSIGNED_SHORT, 0);

	/* Stop using the vertex buffer object */
	glDisableVertexAttribArray(attribute_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// swap gl buffers
	open_gl_context.swapBuffers();
    
    // old stuff
    /*
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
	*/
	
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
	
	/*
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
	*/

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
	
	// create datapoint array, store it as bytes
#define N 2048
	GLbyte graph[N][N];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			float x = (i - N / 2) / (N / 2.0);
			float y = (j - N / 2) / (N / 2.0);
			float d = hypotf(x, y) * 4.0;
			float z = (1 - d * d) * expf(d * d / -2.0);

			graph[i][j] = roundf(z * 127 + 128);
		}
	}

	/* Upload the texture with our datapoints */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N, N, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

	// Create two vertex buffer objects
	glGenBuffers(2, vbo);

	// Create an array for 101 * 101 vertices
	glm::vec2 vertices[101][101];

	for (int i = 0; i < 101; i++) {
		for (int j = 0; j < 101; j++) {
			vertices[i][j].x = (j - 50) / 50.0;
			vertices[i][j].y = (i - 50) / 50.0;
		}
	}

	// Tell OpenGL to copy our array to the buffer objects
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	// Create an array of indices into the vertex array that traces both horizontal and vertical lines
	GLushort indices[100 * 101 * 4];
	int i = 0;

	for (int y = 0; y < 101; y++) {
		for (int x = 0; x < 100; x++) {
			indices[i++] = y * 101 + x;
			indices[i++] = y * 101 + x + 1;
		}
	}

	for (int x = 0; x < 101; x++) {
		for (int y = 0; y < 100; y++) {
			indices[i++] = y * 101 + x;
			indices[i++] = (y + 1) * 101 + x;
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);
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
