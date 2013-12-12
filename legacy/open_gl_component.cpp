/*
  ==============================================================================

    open_gl_component.cpp
    Created: 2 Dec 2013 2:04:27pm
    Author:  Chris

  ==============================================================================
*/

#include "open_gl_component.h"
#include "spectrogram_component.h"

//==============================================================================
open_gl_component::open_gl_component()
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
	vert_shader = "";
	frag_shader = "";
	recompile_shader = false;

	// start UI changed timer
    startTimer(1000);
}

open_gl_component::~open_gl_component()
{
	// delete wav file/FFT data
	if (wav_file != nullptr)
		delete wav_file;
	
	// detach from open gl
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
    jassert (OpenGLHelpers::isContextActive());

    //const float desktopScale = (float) open_gl_context.getRenderingScale();
    OpenGLHelpers::clear (Colours::black);

	// update shader and check status
	update_shader();
    if (shader == nullptr)
        return;

	// acquire wav file lock
	const ScopedLock sl(wav_file_lock);
	if (wav_file == nullptr)
		return;
    /*
    // Having used the juce 2D renderer, it will have messed-up a whole load of GL state, so
    // we need to initialise some important settings before doing our normal GL 3D drawing..
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LESS);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    open_gl_context.extensions.glActiveTexture (GL_TEXTURE0);
    glEnable (GL_TEXTURE_2D);
    */

    //glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));
    //glViewport (0, 0, getWidth(), getHeight());

	// use shader
	shader->use();
    
    // wireframe display
	if (OpenGLShaderProgram::Uniform* uni = uniforms->mytexture) {
		//glUniform1i(uniform_mytexture, 0);
		open_gl_context.extensions.glUniform1i(uni->uniformID, 0);
	}

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

	// setup our vertex transformsca
	if (OpenGLShaderProgram::Uniform* uni = uniforms->vertex_transform) {
		//glUniformMatrix4fv(uniform_vertex_transform, 1, GL_FALSE, glm::value_ptr(vertex_transform));
		open_gl_context.extensions.glUniformMatrix4fv(uni->uniformID, 1, GL_FALSE, glm::value_ptr(vertex_transform));
	}

	// setup our texture transform
	if (OpenGLShaderProgram::Uniform* uni = uniforms->texture_transform) {
		//glUniformMatrix4fv(uniform_texture_transform, 1, GL_FALSE, glm::value_ptr(texture_transform));
		open_gl_context.extensions.glUniformMatrix4fv(uni->uniformID, 1, GL_FALSE, glm::value_ptr(texture_transform));
	}

	//glClearColor(0.0, 0.0, 0.0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT);

	/* Set texture wrapping mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	/* Set texture interpolation mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);

	/* Draw the grid using the indices to our vertices using our vertex buffer objects */
	if (OpenGLShaderProgram::Attribute* attr = attributes->coord2d) {
        /* Draw the grid using the indices to our vertices using our vertex buffer objects */
        open_gl_context.extensions.glEnableVertexAttribArray(attr->attributeID);

        open_gl_context.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        open_gl_context.extensions.glVertexAttribPointer(attr->attributeID, 2, GL_FLOAT, GL_FALSE, 0, 0);

        open_gl_context.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glDrawElements(GL_LINES, 100 * 101 * 4, GL_UNSIGNED_SHORT, 0);

        /* Stop using the vertex buffer object */
        open_gl_context.extensions.glDisableVertexAttribArray(attr->attributeID);
        open_gl_context.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
        open_gl_context.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

	// swap gl buffers
	open_gl_context.swapBuffers();
}

void open_gl_component::openGLContextClosing() {
	// delete shader data
    shader = nullptr;
    attributes = nullptr;
    uniforms = nullptr;
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

void open_gl_component::create_vbo() {
	std::cerr << "create_vbo attached: " << open_gl_context.isAttached() << ", active: " << open_gl_context.isActive() << std::endl;

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

	// Upload the texture with our datapoints
	open_gl_context.extensions.glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N, N, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

	// Create two vertex buffer objects
	open_gl_context.extensions.glGenBuffers(2, vbo);

	// Create an array for 101 * 101 vertices
	glm::vec2 vertices[101][101];

	for (int i = 0; i < 101; i++) {
		for (int j = 0; j < 101; j++) {
			vertices[i][j].x = (j - 50) / 50.0;
			vertices[i][j].y = (i - 50) / 50.0;
		}
	}

	// Tell OpenGL to copy our array to the buffer objects
	open_gl_context.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	open_gl_context.extensions.glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

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

	open_gl_context.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	open_gl_context.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);
}

void open_gl_component::set_new_shader(const String& v, const String& f) {
	recompile_shader = true;
	vert_shader = v;
	frag_shader = f;
}

void open_gl_component::update_shader()
{
	if (!recompile_shader)
		return;

	const String& newVertexShader = vert_shader;
	const String& newFragmentShader = frag_shader;
	std::cerr << "update_shader attached: " << open_gl_context.isAttached() << ", active: " << open_gl_context.isActive() << std::endl;
	std::cerr << "compiling vert shader: " << std::endl << newVertexShader << std::endl << std::endl;
	std::cerr << "compiling frag shader: " << std::endl << newFragmentShader << std::endl << std::endl;

    ScopedPointer<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (open_gl_context));
    String statusText;

    if (newShader->addVertexShader (newVertexShader)
            && newShader->addFragmentShader (newFragmentShader)
            && newShader->link())
    {
        attributes = nullptr;
        uniforms = nullptr;

        shader = newShader;
        shader->use();

        attributes = new Attributes (open_gl_context, *shader);
        uniforms   = new Uniforms (open_gl_context, *shader);

        #if ! JUCE_OPENGL_ES
        statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
        #else
        statusText = "GLSL ES";
        #endif
    }
    else
    {
        statusText = newShader->getLastError();
    }
    
    std::cerr << "compilation result: " << statusText << std::endl;
    
    recompile_shader = false;
}
