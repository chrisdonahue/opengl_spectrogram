#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "boost/program_options.hpp"

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "audio_util.h"
#include "shader_utils.h"

// gl state
GLuint program;
GLint attribute_coord2d;
GLint uniform_vertex_transform;
GLint uniform_texture_transform;
GLuint texture_id;
GLint uniform_mytexture;

// wav file
audio_util::wav_data* wav_file;

// model variables
float offset_x = 0.0;
float offset_y = 0.0;
float scale = 1.0;

// display settings
bool interpolate = false;
bool clamp = true;
bool rotate = false;

// model state
//GLbyte* graph
GLuint vbo[2];

int init_resources(std::string vert_shader_file_path, std::string frag_shader_file_path) {
	program = create_program(vert_shader_file_path.c_str(), frag_shader_file_path.c_str());
	if (program == 0)
		return 0;

	attribute_coord2d = get_attrib(program, "coord2d");
	uniform_vertex_transform = get_uniform(program, "vertex_transform");
	uniform_texture_transform = get_uniform(program, "texture_transform");
	uniform_mytexture = get_uniform(program, "mytexture");

	if (attribute_coord2d == -1 || uniform_vertex_transform == -1 || uniform_texture_transform == -1 || uniform_mytexture == -1)
		return 0;

	// Create our datapoints, store it as bytes
    int num_frames = wav_file->get_num_frames();
    int num_bins_per_frame = wav_file->get_num_bins_per_frame();
    //GLbyte graph[num_frames][num_bins_per_frame - 1];
    GLbyte graph[num_bins_per_frame - 1][num_frames];
    double fft_magnitude_min = wav_file->get_fft_magnitude_min();
    double fft_magnitude_max = wav_file->get_fft_magnitude_max();
    double m, b;
    audio_util::map_ranged(fft_magnitude_min, fft_magnitude_max, -1.0, 1.0, &m, &b);

    for (int i = 0; i < num_frames; i++) {
        double* frame_magnitudes = wav_file->get_fft_magnitudes_frame(i);
        for (int j = 1; j < num_bins_per_frame; j++) {
            double bin_magnitude = frame_magnitudes[j];
            double adjusted_bin_magnitude = (bin_magnitude * m) + b;
			//graph[i][j - 1] = roundf(adjusted_bin_magnitude * 127 + 128);
			graph[j - 1][i] = roundf(adjusted_bin_magnitude * 127 + 128);
        }
    }

    /*
#define N 16
	GLbyte graph[N][N];

    // calculate data ranges
    float x_min = std::numeric_limits<float>::infinity();
    float x_max = std::numeric_limits<float>::infinity() * -1.0f;
    float y_min = std::numeric_limits<float>::infinity();
    float y_max = std::numeric_limits<float>::infinity() * -1.0f;
    float d_min = std::numeric_limits<float>::infinity();
    float d_max = std::numeric_limits<float>::infinity() * -1.0f;
    float z_min = std::numeric_limits<float>::infinity();
    float z_max = std::numeric_limits<float>::infinity() * -1.0f;
    int g_min = std::numeric_limits<int>::max();
    int g_max = std::numeric_limits<int>::min();

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
            // map i, j from -1.0 to 1.0
			float x = (i - N / 2) / (N / 2.0);
			float y = (j - N / 2) / (N / 2.0);
			float d = hypotf(x, y) * 4.0;
			float z = (1 - d * d) * expf(d * d / -2.0);
            
			graph[i][j] = roundf(z * 127 + 128);
            int g = (int) graph[i][j];
            
            // calculate ranges
            if (x < x_min)
                x_min = x;
            if (x > x_max)
                x_max = x;
            if (y < y_min)
                y_min = y;
            if (y > y_max)
                y_max = y;
            if (d < d_min)
                d_min = d;
            if (d > d_max)
                d_max = d;
            if (z < z_min)
                z_min = z;
            if (z > z_max)
                z_max = z;
            if (g < g_min)
                g_min = g;
            if (g > g_max)
                g_max = g;

            std::cerr << "(" << i << ", " << j << ") -> (" << x << ", " << y << ", " << z << "): " << (int) graph[i][j] << std::endl;
		}
	}

    // print ranges
    std::cerr << "x: [" << x_min << ", " << x_max << "]" << std::endl;
    std::cerr << "y: [" << y_min << ", " << y_max << "]" << std::endl;
    std::cerr << "d: [" << d_min << ", " << d_max << "]" << std::endl;
    std::cerr << "z: [" << z_min << ", " << z_max << "]" << std::endl;
    std::cerr << "g: [" << g_min << ", " << g_max << "]" << std::endl;
    */

	/* Upload the texture with our datapoints */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N, N, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, num_frames, num_bins_per_frame - 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

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

	return 1;
}

void display() {
	glUseProgram(program);
	glUniform1i(uniform_mytexture, 0);

	glm::mat4 model;

	if (rotate)
		model = glm::rotate(glm::mat4(1.0f), float (glutGet(GLUT_ELAPSED_TIME) / 100.0), glm::vec3(0.0f, 0.0f, 1.0f));

	else
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

	glutSwapBuffers();
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1:
		interpolate = !interpolate;
		printf("Interpolation is now %s\n", interpolate ? "on" : "off");
		break;
	case GLUT_KEY_F2:
		clamp = !clamp;
		printf("Clamping is now %s\n", clamp ? "on" : "off");
		break;
	case GLUT_KEY_F3:
		rotate = !rotate;
		printf("Rotation is now %s\n", rotate ? "on" : "off");
		break;
    /*
	case GLUT_KEY_LEFT:
		offset_x -= 0.03;
		break;
	case GLUT_KEY_RIGHT:
		offset_x += 0.03;
		break;
	case GLUT_KEY_UP:
		offset_y += 0.03;
		break;
	case GLUT_KEY_DOWN:
		offset_y -= 0.03;
		break;
    */
	case GLUT_KEY_PAGE_UP:
		scale *= 1.5;
		break;
	case GLUT_KEY_PAGE_DOWN:
		scale /= 1.5;
		break;
	case GLUT_KEY_HOME:
		offset_x = 0.0;
		offset_y = 0.0;
		scale = 1.0;
		break;
	}

	glutPostRedisplay();
}

void free_resources() {
	glDeleteProgram(program);
}

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    
    // containers for command line variables
    int fft_size;
    int fft_overlap;
    std::string fft_window_type;
    std::string vertex_shader_file_path;
    std::string fragment_shader_file_path;
    std::string audio_file_path;

    // parse command line
    po::options_description cl_desc("Available options");
    cl_desc.add_options()
        ("help", "display help message")
        ("fft_size", po::value<int>(&(fft_size))->default_value(1024), "fft size")
        ("fft_overlap", po::value<int>(&(fft_overlap))->default_value(0), "fft overlap")
        ("fft_window_type", po::value<std::string>(&fft_window_type)->default_value("rectangle"), "fft window type")
        ("vertex_shader_file_path", po::value<std::string>(&vertex_shader_file_path), "vertex shader file path")
        ("fragment_shader_file_path", po::value<std::string>(&fragment_shader_file_path), "fragment shader file path")
        ("audio_file_path", po::value<std::string>(&audio_file_path), "audio file path")
    ;
    po::variables_map cl_vm;
    po::store(po::parse_command_line(argc, argv, cl_desc), cl_vm);
    po::notify(cl_vm);
    
    // print help
    if (cl_vm.count("help")) {
        std::cout << cl_desc << std::endl;
        return 1;
    }
    
    // validate fft_size
    if (fft_size <= 0 || fft_size % 2 == 1) {
        std::cerr << "fft size must be an even number greater than 0" << std::endl;
        return 1;
    }

    // validate fft_overlap
    if (fft_overlap < 0 || fft_overlap >= fft_size) {
        std::cerr << "fft overlap must be a positive number less than fft size" << std::endl;
        return 1;
    }

    // validate fft_window_type
    if (!(fft_window_type.compare("rectangle") == 0 ||
          fft_window_type.compare("hanning") == 0)) {
        std::cerr << "fft window type must be 'rectangle' or 'hanning'" << std::endl;
        return 1;
    }

    // load wav file
    wav_file = new audio_util::wav_data(audio_file_path);

    // compuate wav file FFT
    wav_file->perform_fft(fft_size, fft_overlap, fft_window_type);
    
    // debug command line
    std::cerr << "Displaying FFT of " << audio_file_path << " with size " << fft_size << ", overlap " << fft_overlap << ", and window type " << fft_window_type << "." << std::endl;

    // graph example from here on
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("My Graph");

	GLenum glew_status = glewInit();

	if (GLEW_OK != glew_status) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "No support for OpenGL 2.0 found\n");
		return 1;
	}

	GLint max_units;

	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_units);
	if (max_units < 1) {
		fprintf(stderr, "Your GPU does not have any vertex texture image units\n");
		return 1;
	}

	//printf("Use left/right/up/down to move.\n");
	printf("Use pageup/pagedown to change the horizontal scale.\n");
	printf("Press home to reset the position and scale.\n");
	printf("Press F1 to toggle interpolation.\n");
	printf("Press F2 to toggle clamping.\n");
	printf("Press F3 to toggle rotation.\n");

	if (init_resources(vertex_shader_file_path, fragment_shader_file_path)) {
		glutDisplayFunc(display);
		glutIdleFunc(display);
		glutSpecialFunc(special);
		glutMainLoop();
	}

	free_resources();
	return 0;
}
