#include "shaderprogram.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>

using namespace makai;

ShaderProgram::ShaderProgram()
{
    m_program = 0;
    m_log = std::string();
    m_shaders = std::vector<Shader*>();
    addedShaders = std::vector<Shader*>();
}

ShaderProgram::~ShaderProgram()
{
    for (unsigned i = 0; i < m_shaders.size(); i++)
        delete m_shaders.at(i);
    addedShaders.clear();
    m_shaders.clear();
    glDeleteProgram(m_program);
}

bool ShaderProgram::addShader(Shader *shader)
{
    if (!shader->isCompiled()) {
        return false;
    }
    addedShaders.push_back(shader);
    return true;
}

bool ShaderProgram::addShaderFromFile(Shader::ShaderType type, const std::string &fileName)
{

    Shader* shader = new Shader(type);
    bool success = shader->compileSourceFile(fileName);
    if (success) {
        m_shaders.push_back(shader);
    }
    return success;
}

void ShaderProgram::removeShader(Shader *shader)
{
    for (unsigned i = 0; i < addedShaders.size(); i++) {
        if (addedShaders.at(i) == shader) {
            addedShaders.erase(addedShaders.begin() + i);
            break;
        }
    }
}

bool ShaderProgram::link()
{
    // Shader Program
    m_program = glCreateProgram();

    for (unsigned i = 0; i < addedShaders.size(); i++)
        glAttachShader(m_program, addedShaders.at(i)->shaderId());

    for (unsigned i = 0; i < m_shaders.size(); i++)
        glAttachShader(m_program, m_shaders.at(i)->shaderId());

    glLinkProgram(m_program);

    // Print linking errors if any
    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint infoLogLength;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* infoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(m_program, infoLogLength, NULL, infoLog);
        m_log += "ERROR::SHADER::PROGRAM::LINKING_FAILED\n";
        m_log += infoLog;

        delete[] infoLog;
        glDeleteProgram(m_program);
        m_program = 0;
        return false;
    }

    for (unsigned i = 0; i < addedShaders.size(); i++)
        glDetachShader(m_program, addedShaders.at(i)->shaderId());

    for (unsigned i = 0; i < m_shaders.size(); i++)
        glDetachShader(m_program, m_shaders.at(i)->shaderId());

    return true;
}

void ShaderProgram::bind()
{
    glUseProgram(this->m_program);
}

void ShaderProgram::release()
{
    glUseProgram(0);
}

std::string ShaderProgram::log() const
{
    return m_log;
}

GLuint ShaderProgram::programId() const
{
    return m_program;
}

GLint ShaderProgram::attrib(const GLchar *attribName) const
{
    if(!attribName)
        throw std::runtime_error("attribName was NULL");

    GLint attrib = glGetAttribLocation(m_program, attribName);
    if(attrib == -1)
        throw std::runtime_error(std::string("Program attribute not found: ") + attribName);

    return attrib;
}

GLint ShaderProgram::uniform(const GLchar *uniformName) const
{
    if(!uniformName)
            throw std::runtime_error("uniformName was NULL");

    GLint uniform = glGetUniformLocation(m_program, uniformName);
    if(uniform == -1)
        throw std::runtime_error(std::string("Program uniform not found: ") + uniformName);

    return uniform;
}

#define ATTRIB_N_UNIFORM_SETTERS(TYPE, TYPE_PREFIX, TYPE_SUFFIX) \
\
    void ShaderProgram::setAttrib(const GLchar* name, TYPE v0) \
        { glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX (attrib(name), v0); } \
    void ShaderProgram::setAttrib(const GLchar* name, TYPE v0, TYPE v1) \
        { glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX (attrib(name), v0, v1); } \
    void ShaderProgram::setAttrib(const GLchar* name, TYPE v0, TYPE v1, TYPE v2) \
        { glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX (attrib(name), v0, v1, v2); } \
    void ShaderProgram::setAttrib(const GLchar* name, TYPE v0, TYPE v1, TYPE v2, TYPE v3) \
        { glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX (attrib(name), v0, v1, v2, v3); } \
\
    void ShaderProgram::setAttrib1v(const GLchar* name, const TYPE* v) \
        { glVertexAttrib ## TYPE_PREFIX ## 1 ## TYPE_SUFFIX ## v (attrib(name), v); } \
    void ShaderProgram::setAttrib2v(const GLchar* name, const TYPE* v) \
        { glVertexAttrib ## TYPE_PREFIX ## 2 ## TYPE_SUFFIX ## v (attrib(name), v); } \
    void ShaderProgram::setAttrib3v(const GLchar* name, const TYPE* v) \
        { glVertexAttrib ## TYPE_PREFIX ## 3 ## TYPE_SUFFIX ## v (attrib(name), v); } \
    void ShaderProgram::setAttrib4v(const GLchar* name, const TYPE* v) \
        { glVertexAttrib ## TYPE_PREFIX ## 4 ## TYPE_SUFFIX ## v (attrib(name), v); } \
\
    void ShaderProgram::setUniform(const GLchar* name, TYPE v0) \
        { glUniform1 ## TYPE_SUFFIX (uniform(name), v0); } \
    void ShaderProgram::setUniform(const GLchar* name, TYPE v0, TYPE v1) \
        { glUniform2 ## TYPE_SUFFIX (uniform(name), v0, v1); } \
    void ShaderProgram::setUniform(const GLchar* name, TYPE v0, TYPE v1, TYPE v2) \
        { glUniform3 ## TYPE_SUFFIX (uniform(name), v0, v1, v2); } \
    void ShaderProgram::setUniform(const GLchar* name, TYPE v0, TYPE v1, TYPE v2, TYPE v3) \
        { glUniform4 ## TYPE_SUFFIX (uniform(name), v0, v1, v2, v3); } \
\
    void ShaderProgram::setUniform1v(const GLchar* name, const TYPE* v, GLsizei count) \
        { glUniform1 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
    void ShaderProgram::setUniform2v(const GLchar* name, const TYPE* v, GLsizei count) \
        { glUniform2 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
    void ShaderProgram::setUniform3v(const GLchar* name, const TYPE* v, GLsizei count) \
        { glUniform3 ## TYPE_SUFFIX ## v (uniform(name), count, v); } \
    void ShaderProgram::setUniform4v(const GLchar* name, const TYPE* v, GLsizei count) \
        { glUniform4 ## TYPE_SUFFIX ## v (uniform(name), count, v); }

ATTRIB_N_UNIFORM_SETTERS(GLfloat, , f)
ATTRIB_N_UNIFORM_SETTERS(GLdouble, , d)
ATTRIB_N_UNIFORM_SETTERS(GLint, I, i)
ATTRIB_N_UNIFORM_SETTERS(GLuint, I, ui)

void ShaderProgram::setUniformMatrix2(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    glUniformMatrix2fv(uniform(name), count, transpose, v);
}

void ShaderProgram::setUniformMatrix3(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    glUniformMatrix3fv(uniform(name), count, transpose, v);
}

void ShaderProgram::setUniformMatrix4(const GLchar* name, const GLfloat* v, GLsizei count, GLboolean transpose) {
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    assert(currentProgram == (GLint)m_program);
    glUniformMatrix4fv(uniform(name), count, transpose, v);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::mat2& m, GLboolean transpose) {
    glUniformMatrix2fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const GLchar* name, const glm::mat3& m, GLboolean transpose) {
    glUniformMatrix3fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const GLchar* name, const glm::mat4& m, GLboolean transpose) {
    glUniformMatrix4fv(uniform(name), 1, transpose, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const GLchar* uniformName, const glm::vec3& v) {
    setUniform3v(uniformName, glm::value_ptr(v));
}

void ShaderProgram::setUniform(const GLchar* uniformName, const glm::vec4& v) {
    setUniform4v(uniformName, glm::value_ptr(v));
}
