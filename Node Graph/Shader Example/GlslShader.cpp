#include "GlslShader.h"

#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

void print_error(const std::string& class_name, const std::string& type, const std::string& info_log = "")
{
    std::string _class_name = boost::to_upper_copy<std::string>(class_name);
    std::string _type = boost::to_upper_copy<std::string>(type);

    std::cout << "ERROR::" << _class_name << "::" << _type << "\n" << info_log << "\n-- -------------------------------------------------- - -- " << std::endl;
}
GLint check_glsl_shader_compile_errors(GLuint shader)
{
    GLint success;
    GLchar info_log[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, info_log);
        print_error("GLSL_SHADER", "SHADER_COMPILATION_ERROR", info_log);
    }
    return success;
}
GLint check_glsl_shader_linking_errors(GLuint shader)
{
    GLint success;
    GLchar info_log[1024];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 1024, NULL, info_log);
        print_error("GLSL_SHADER", "PROGRAM_LINKING_ERROR", info_log);
    }
    return success;
}

void glsl_shdr_file::load(const std::string& file_path)
{
    auto file_ext = std::filesystem::path(file_path).extension().string();

    bool valid_type = true;
    if (file_ext.compare(ext) != 0) {
        print_error("GLSL_SHADER_FILE_load", "FILE_EXTENSION_DOES_NOT_MATCH", file_path);
        valid_type = false;
    }

    if (valid_type)
    {
        read_file(file_path);
    }
};
void glsl_shdr_file::save(const std::string& directory)
{
    std::string file_path = directory + "/" + name + ext;
    write_file(file_path);
}
void glsl_shdr_file::compile()
{
    if (gl_shdr_id != 0) {
        glDeleteShader(gl_shdr_id);
    }

    const char* _code = contents.c_str();
    gl_shdr_id = glCreateShader(gl_shdr_type);
    glShaderSource(gl_shdr_id, 1, &_code, NULL);
    glCompileShader(gl_shdr_id);
    compile_success = check_glsl_shader_compile_errors(gl_shdr_id);
}
void glsl_shdr_file::set_contents(const std::string& code)
{
    contents = code;

}
void glsl_shdr_file::set_name(const std::string& name)
{
    (this)->name = name;
}
uint32_t glsl_shdr_file::get_gl_shdr_id() const
{
    return gl_shdr_id;
}
std::string glsl_shdr_file::get_contents() const
{
    return contents;
}
bool glsl_shdr_file::compile_succeeded() const
{
    return compile_success;
}

void glsl_shdr_file::read_file(const std::string& file_path)
{
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        file.open(file_path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        contents = stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        print_error("GLSL_SHADER_CODE_read_file", "FILE_NOT_SUCCESFULLY_READ", e.what());
    }
}
void glsl_shdr_file::write_file(const std::string& file_path)
{
    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try
    {
        file.open(file_path);
        file << contents;
        file.close();
    }
    catch (std::ifstream::failure& e)
    {
        print_error("GLSL_SHADER_CODE_write_file", "FILE_NOT_SUCCESFULLY_READ", e.what());
    }
}

void GlslShader::use()
{
    glUseProgram(id);
}
void GlslShader::compile()
{
    if (id != 0) {
        glDeleteProgram(id);
    }

    id = glCreateProgram();

    vert_shd_file->compile();
    frag_shd_file->compile();
    //geom_shd_file->compile();

    if (vert_shd_file->compile_succeeded()) glAttachShader(id, vert_shd_file->get_gl_shdr_id());
    if (frag_shd_file->compile_succeeded()) glAttachShader(id, frag_shd_file->get_gl_shdr_id());
    //if (geom_shd_file->compile_succeeded()) glAttachShader(id, geom_shd_file->get_gl_shdr_id());

    glLinkProgram(id);
    compile_success = check_glsl_shader_linking_errors(id);
}
bool GlslShader::compile_successful()
{
    return compile_success;
}

glsl_shdr_file* GlslShader::get_shdr_file(const GLenum gl_shdr_type) const
{
    switch (gl_shdr_type)
    {
    case GL_VERTEX_SHADER:
        return vert_shd_file;
        break;
    case GL_FRAGMENT_SHADER:
        return frag_shd_file;
        break;
    case GL_GEOMETRY_SHADER:
        return geom_shd_file;
        break;
    default:
        return vert_shd_file;
    }
}

void GlslShader::set_name(const std::string& name)
{
    (this)->name = name;
    vert_shd_file->set_name(name);
    frag_shd_file->set_name(name);
    geom_shd_file->set_name(name);
}

void GlslShader::load_code(const std::string& file_path)
{
}

void GlslShader::load_code(const GLenum gl_shdr_type, const std::string& file_path)
{
    auto shdr_file = get_shdr_file(gl_shdr_type);
    shdr_file->load(file_path);
}
void GlslShader::set_code(const GLenum gl_shdr_type, const std::string& code)
{
    auto shdr_file = get_shdr_file(gl_shdr_type);
    shdr_file->set_contents(UTF8ToANSI(code));
}
void GlslShader::save_code(const std::string& directory)
{
    auto vert_shdr = get_shdr_file(GL_VERTEX_SHADER);
    vert_shdr->save(directory);

    auto frag_shdr = get_shdr_file(GL_FRAGMENT_SHADER);
    frag_shdr->save(directory);

    //auto geom_shdr = get_shdr_file(GL_GEOMETRY_SHADER);
    //geom_shdr->save(directory);
}
void GlslShader::save_code(const GLenum gl_shdr_type, const std::string& directory)
{
    auto shdr_file = get_shdr_file(gl_shdr_type);
    shdr_file->save(directory);
}
GLuint GlslShader::get_id() const
{
    return id;
}
std::string GlslShader::get_code(const GLenum gl_shdr_type) const
{
    auto shdr_file = get_shdr_file(gl_shdr_type);
    return shdr_file->get_contents();
}
