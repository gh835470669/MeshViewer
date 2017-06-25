#include "shader.h"

using namespace makai;

Shader::Shader(ShaderType type) : m_type(type), m_isCompiled(false),
    m_log(), m_id(0), m_code()
{

}

Shader::~Shader()
{
    glDeleteShader(m_id);
}

bool Shader::compileSourceCode(const std::string &source)
{
    if (m_type == ShaderType::Vertex)
        m_id = glCreateShader(GL_VERTEX_SHADER);
    else if (m_type == ShaderType::Fragment)
        m_id = glCreateShader(GL_FRAGMENT_SHADER);

    if (m_id == 0) {
        m_log += "glCreateShader failed";
        return false;
    }

    const GLchar* code = source.c_str();
    glShaderSource(m_id, 1, &code, NULL);
    glCompileShader(m_id);

    // Print compile errors if any
    GLint success;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint infoLogLength;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* infoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(m_id, infoLogLength, NULL, infoLog);
        m_log += "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n";
        m_log += infoLog;

        delete[] infoLog;
        glDeleteShader(m_id);
        m_id = 0;
        return false;
    }

    m_isCompiled = true;
    return true;
}

bool Shader::compileSourceFile(const std::string &fileName)
{
    std::ifstream shaderFile;
    std::stringstream shaderStream;
    try
    {
        // Open files
        shaderFile.open(fileName);
        // Read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();

    }
    catch (std::ifstream::failure e)
    {
        m_log += __FUNCTION__;
        m_log += ": ";
        m_log += e.what();
        m_log += "\n";
        return false;
    }

    m_code = shaderStream.str();

    return compileSourceCode(m_code);
}

bool Shader::isCompiled() const
{
    return m_isCompiled;
}

std::string Shader::log() const
{
    return m_log;
}

GLuint Shader::shaderId() const
{
    return m_id;
}

Shader::ShaderType Shader::shaderType() const
{
    return m_type;
}

std::string Shader::sourceCode() const
{
    return m_code;
}
