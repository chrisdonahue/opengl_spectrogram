/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== graph.f.glsl ==================
static const unsigned char temp_binary_data_0[] =
"varying vec4 graph_coord;\n"
"\n"
"void main(void) {\n"
"\tgl_FragColor = graph_coord / 2.0 + 0.5;\n"
"}\n";

const char* graph_f_glsl = (const char*) temp_binary_data_0;

//================== graph.v.glsl ==================
static const unsigned char temp_binary_data_1[] =
"attribute vec2 coord2d;\n"
"varying vec4 graph_coord;\n"
"uniform mat4 texture_transform;\n"
"uniform mat4 vertex_transform;\n"
"uniform sampler2D mytexture;\n"
"\n"
"void main(void) {\n"
"\tgraph_coord = texture_transform * vec4(coord2d, 0, 1);\n"
"\tgraph_coord.z = (texture2D(mytexture, graph_coord.xy / 2.0 + 0.5).r);\n"
"\n"
"\tgl_Position = vertex_transform * vec4(coord2d, graph_coord.z, 1);\n"
"}\n";

const char* graph_v_glsl = (const char*) temp_binary_data_1;


const char* getNamedResource (const char*, int&) throw();
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes) throw()
{
    unsigned int hash = 0;
    if (resourceNameUTF8 != 0)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x5c794648:  numBytes = 88; return graph_f_glsl;
        case 0x77c6d038:  numBytes = 358; return graph_v_glsl;
        default: break;
    }

    numBytes = 0;
    return 0;
}

const int namedResourceListSize = 2;

const char* namedResourceList[] =
{
    "graph_f_glsl",
    "graph_v_glsl"
};

}
