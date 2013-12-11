#ifndef OPEN_GL_JUCE_DEMO_STRUCTS
#define OPEN_GL_JUST_DEMO_STRUCTS

#include "JuceHeader.h"

/** Vertex data to be passed to the shaders.
    For the purposes of this demo, each vertex will have a 3D position, a colour and a
    2D texture co-ordinate. Of course you can ignore these or manipulate them in the
    shader programs but are some useful defaults to work from.
    */
struct Vertex
{
    float position[3];
    float normal[3];
    float colour[4];
    float texCoord[2];
};

//==============================================================================
// This class just manages the attributes that the demo shaders use.
struct Attributes
{
    Attributes (OpenGLContext& openGLContext, OpenGLShaderProgram& shader)
    {
        position      = createAttribute (openGLContext, shader, "position");
        normal        = createAttribute (openGLContext, shader, "normal");
        sourceColour  = createAttribute (openGLContext, shader, "sourceColour");
        texureCoordIn = createAttribute (openGLContext, shader, "texureCoordIn");
    }

    void enable (OpenGLContext& openGLContext)
    {
        if (position != nullptr)
        {
            openGLContext.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
            openGLContext.extensions.glEnableVertexAttribArray (position->attributeID);
        }

        if (normal != nullptr)
        {
            openGLContext.extensions.glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
            openGLContext.extensions.glEnableVertexAttribArray (normal->attributeID);
        }

        if (sourceColour != nullptr)
        {
            openGLContext.extensions.glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
            openGLContext.extensions.glEnableVertexAttribArray (sourceColour->attributeID);
        }

        if (texureCoordIn != nullptr)
        {
            openGLContext.extensions.glVertexAttribPointer (texureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
            openGLContext.extensions.glEnableVertexAttribArray (texureCoordIn->attributeID);
        }
    }

    ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, texureCoordIn;

private:
    static OpenGLShaderProgram::Attribute* createAttribute (OpenGLContext& openGLContext,
                                                            OpenGLShaderProgram& shader,
                                                            const char* attributeName)
    {
        if (openGLContext.extensions.glGetAttribLocation (shader.programID, attributeName) < 0)
            return nullptr;

        return new OpenGLShaderProgram::Attribute (shader, attributeName);
    }
};

//==============================================================================
// This class just manages the uniform values that the demo shaders use.
struct Uniforms
{
    Uniforms (OpenGLContext& openGLContext, OpenGLShaderProgram& shader)
    {
        projectionMatrix = createUniform (openGLContext, shader, "projectionMatrix");
        viewMatrix       = createUniform (openGLContext, shader, "viewMatrix");
        texture          = createUniform (openGLContext, shader, "texture");
        lightPosition    = createUniform (openGLContext, shader, "lightPosition");
        bouncingNumber   = createUniform (openGLContext, shader, "bouncingNumber");
    }

    ScopedPointer<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix, texture, lightPosition, bouncingNumber;

private:
    static OpenGLShaderProgram::Uniform* createUniform (OpenGLContext& openGLContext,
                                                        OpenGLShaderProgram& shader,
                                                        const char* uniformName)
    {
        if (openGLContext.extensions.glGetUniformLocation (shader.programID, uniformName) < 0)
            return nullptr;

        return new OpenGLShaderProgram::Uniform (shader, uniformName);
    }
};

//==============================================================================
struct ShaderPreset
{
	const char* name;
	const char* vertexShader;
	const char* fragmentShader;
};

static Array<ShaderPreset> getPresets()
{
	#define SHADER_DEMO_HEADER \
		"/*  This is a live OpenGL Shader demo.\n" \
		"    Edit the shader program below and it will be \n" \
		"    compiled and applied to the model above!\n" \
		"*/\n\n"

	ShaderPreset presets[] =
	{
		{
			"Wireframe",
			SHADER_DEMO_HEADER
			"attribute vec2 coord2d;\n"
			"\n"
			"uniform mat4 texture_transform;\n"
			"uniform mat4 vertex_transform;\n"
			"uniform sampler2D mytexture;\n"
			"\n"
			"varying vec4 graph_coord;\n"
			"\n"
			"void main (void)\n"
			"{\n"
			"    graph_coord = texture_transform * vec4(coord2d, 0, 1);\n"
			"    graph_coord.z = (texture2D(mytexture, graph_coord.xy / 2.0 + 0.5).r);\n"
			"\n"
			"    gl_Position = vertex_transform * vec4(coord2d, graph_coord.z, 1);\n"
			"}\n",

			SHADER_DEMO_HEADER
			#if JUCE_OPENGL_ES
			"varying lowp vec4 graph_coord;\n"
			#else
			"varying vec4 graph_coord;\n"
			#endif
			"\n"
			"void main (void)\n"
			"{\n"
			"    gl_FragColor = graph_coord / 2.0 + 0.5;\n"
			"}\n"
		} /*,
		{
			"Textured",

			SHADER_DEMO_HEADER
			"attribute vec4 position;\n"
			"attribute vec4 sourceColour;\n"
			"attribute vec2 texureCoordIn;\n"
			"\n"
			"uniform mat4 projectionMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"\n"
			"varying vec4 destinationColour;\n"
			"varying vec2 textureCoordOut;\n"
			"\n"
			"void main (void)\n"
			"{\n"
			"    destinationColour = sourceColour;\n"
			"    textureCoordOut = texureCoordIn;\n"
			"    gl_Position = projectionMatrix * viewMatrix * position;\n"
			"}\n",

			SHADER_DEMO_HEADER
			#if JUCE_OPENGL_ES
			"varying lowp vec4 destinationColour;\n"
			"varying lowp vec2 textureCoordOut;\n"
			#else
			"varying vec4 destinationColour;\n"
			"varying vec2 textureCoordOut;\n"
			#endif
			"\n"
			"uniform sampler2D texture;\n"
			"\n"
			"void main (void)\n"
			"{\n"
			"    gl_FragColor = texture2D (texture, textureCoordOut);\n"
			"}\n"
		}
		*/
	};

	return Array<ShaderPreset> (presets, numElementsInArray (presets));
}

#endif