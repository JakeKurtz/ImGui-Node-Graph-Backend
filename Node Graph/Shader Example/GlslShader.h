#pragma once

#include "../GLCommon.h"

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <windows.h>

#define vert_shd_file_ext ".vert"
#define frag_shd_file_ext ".frag"
#define geom_shd_file_ext ".geom"

static std::string UTF8ToANSI(std::string s)
{
    BSTR    bstrWide;
    char* pszAnsi;
    int     nLength;
    const char* pszCode = s.c_str();

    nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, NULL, NULL);
    bstrWide = SysAllocStringLen(NULL, nLength);

    MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

    nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
    pszAnsi = new char[nLength];

    WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
    SysFreeString(bstrWide);

    std::string r(pszAnsi);
    delete[] pszAnsi;
    return r;
}

const std::string base_vert_shader = R"(
#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texcoord;

out vec2 texcoord;

void main()
{
    texcoord = a_texcoord;
    gl_Position = vec4(a_pos, 1.0);
}
)";
const std::string base_frag_shader = R"(
#version 330 core

out vec4 frag_color;
in vec2 texcoord;

void main()
{
    frag_color = vec4(texcoord, 0.f, 1.f);
}
)";
const std::string base_geom_shader = R"()";

const std::string SHADER_DIR = std::string("F:/repos/Node Graph/shaders");

struct glsl_shdr_file
{
    ~glsl_shdr_file()
    {
        if (gl_shdr_id != 0) glDeleteShader(gl_shdr_id);
    };

    void load(const std::string& file_path);
    void save(const std::string& directory);

    void compile();

    void set_contents(const std::string& code);
    void set_name(const std::string& name);

    uint32_t get_gl_shdr_id() const;
    std::string get_contents() const;

    bool compile_succeeded() const;

protected:
    uint32_t id;

    uint32_t gl_shdr_id;
    GLenum gl_shdr_type;

    std::string name;
    std::string ext;
    std::string contents;

    bool compile_success = false;

    void read_file(const std::string& file_path);
    void write_file(const std::string& file_path);

    virtual void init() = 0;
};
struct glsl_vert_shdr_file : public glsl_shdr_file
{
    glsl_vert_shdr_file() { init(); };
    glsl_vert_shdr_file(const std::string& file_path) { load(file_path); init(); };
private:
    void init()
    {
        name = "shader";
        gl_shdr_id = 0;
        gl_shdr_type = GL_VERTEX_SHADER;
        ext = vert_shd_file_ext;
        contents = base_vert_shader;
    }
};
struct glsl_frag_shdr_file : public glsl_shdr_file
{
    glsl_frag_shdr_file() { init(); };
    glsl_frag_shdr_file(const std::string& file_path) { load(file_path); init(); };
private:
    void init()
    {
        name = "shader";
        gl_shdr_id = 0;
        gl_shdr_type = GL_FRAGMENT_SHADER;
        ext = frag_shd_file_ext;
        contents = base_frag_shader;
    }
};
struct glsl_geom_shdr_file : public glsl_shdr_file
{
    glsl_geom_shdr_file() { init(); };
    glsl_geom_shdr_file(const std::string& file_path) { load(file_path); init(); };
private:
    void init()
    {
        name = "shader";
        gl_shdr_id = 0;
        gl_shdr_type = GL_GEOMETRY_SHADER;
        ext = geom_shd_file_ext;
        contents = base_geom_shader;
    }
};

class GlslShader
{
public:
    GlslShader()
    {
        id = 0;
        vert_shd_file = new glsl_vert_shdr_file();
        frag_shd_file = new glsl_frag_shdr_file();
        geom_shd_file = new glsl_geom_shdr_file();
    };
    ~GlslShader()
    {
        delete vert_shd_file;
        delete frag_shd_file;
        delete geom_shd_file;

        vert_shd_file = nullptr;
        frag_shd_file = nullptr;
        geom_shd_file = nullptr;

        if (id != 0) glDeleteProgram(id);
    };

    void use();
    void compile();

    void set_name(const std::string& name);

    void load_code(const std::string& file_path);
    void load_code(const GLenum gl_shdr_type, const std::string& file_path);
    void set_code(const GLenum gl_shdr_type, const std::string& code);
    void save_code(const std::string& directory = SHADER_DIR);
    void save_code(const GLenum gl_shdr_type, const std::string& directory = SHADER_DIR);

    GLuint get_id() const;
    std::string get_code(const GLenum gl_shdr_type) const;

    bool compile_successful();

private:
    GLuint id;

    std::string name;

    glsl_vert_shdr_file* vert_shd_file;
    glsl_frag_shdr_file* frag_shd_file;
    glsl_geom_shdr_file* geom_shd_file;

    bool compile_success = false;

    glsl_shdr_file* get_shdr_file(const GLenum gl_shdr_type) const;
};

GLint check_compile_errors(GLuint shader, std::string type);