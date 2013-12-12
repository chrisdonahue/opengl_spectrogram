#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/shader_utils.h"

GLuint program;
GLint attribute_coord2d;
GLint uniform_vertex_transform;
GLint uniform_texture_transform;
GLuint texture_id;
GLint uniform_mytexture;

float offset_x = 0.0;
float offset_y = 0.0;
float scale = 1.0;

bool interpolate = false;
bool clamp = false;
bool rotate = false;

GLuint vbo[2];

int init_resources() {
	program = create_program("graph.v.glsl", "graph.f.glsl");
	if (program == 0)
		return 0;

	attribute_coord2d = get_attrib(program, "coord2d");
	uniform_vertex_transform = get_uniform(program, "vertex_transform");
	uniform_texture_transform = get_uniform(program, "texture_transform");
	uniform_mytexture = get_uniform(program, "mytexture");

	if (attribute_coord2d == -1 || uniform_vertex_transform == -1 || uniform_texture_transform == -1 || uniform_mytexture == -1)
		return 0;

	// Create our datapoints, store it as bytes
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

	printf("Use left/right/up/down to move.\n");
	printf("Use pageup/pagedown to change the horizontal scale.\n");
	printf("Press home to reset the position and scale.\n");
	printf("Press F1 to toggle interpolation.\n");
	printf("Press F2 to toggle clamping.\n");
	printf("Press F3 to toggle rotation.\n");

	if (init_resources()) {
		glutDisplayFunc(display);
		glutIdleFunc(display);
		glutSpecialFunc(special);
		glutMainLoop();
	}

	free_resources();
	return 0;
}
