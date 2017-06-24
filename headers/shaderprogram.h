#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "shader.h"

namespace makai
{
    class ShaderProgram
    {
    public:
        ShaderProgram();
        ~ShaderProgram();

        // Adds a compiled shader to this shader program.
        // Returns true if the shader could be added, or false otherwise.
        // Ownership of the shader object remains with the caller.
        // It will not be deleted when this ShaderProgram instance is deleted.
        // This allows the caller to add the same shader to multiple shader programs.
        bool addShader(Shader *shader);
        bool addShaderFromFile(Shader::ShaderType type, const std::string &fileName);

        //remove the shader added by addShader()
        void removeShader(Shader *shader);

        // Links together the shaders that were added to this program with addShader().
        // Returns true if the link was successful or false otherwise. If the link failed,
        // the error messages can be retrieved with log().
        bool link();

        // glUseProgram(this->program);
        void bind();

        // glUseProgram(0);
        void release();

        std::string log() const;
        GLuint programId() const;

        // @result The attribute index for the given name, as returned from glGetAttribLocation.
        GLint attrib(const GLchar* attribName) const;

        // @result The uniform index for the given name, as returned from glGetUniformLocation.
        GLint uniform(const GLchar* uniformName) const;

        /**
         Setters for attribute and uniform variables.

         These are convenience methods for the glVertexAttrib* and glUniform* functions.
         */
#define _MAKAI_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(TYPE) \
        void setAttrib(const GLchar* attribName, TYPE v0); \
        void setAttrib(const GLchar* attribName, TYPE v0, TYPE v1); \
        void setAttrib(const GLchar* attribName, TYPE v0, TYPE v1, TYPE v2); \
        void setAttrib(const GLchar* attribName, TYPE v0, TYPE v1, TYPE v2, TYPE v3); \
\
        void setAttrib1v(const GLchar* attribName, const TYPE* v); \
        void setAttrib2v(const GLchar* attribName, const TYPE* v); \
        void setAttrib3v(const GLchar* attribName, const TYPE* v); \
        void setAttrib4v(const GLchar* attribName, const TYPE* v); \
\
        void setUniform(const GLchar* uniformName, TYPE v0); \
        void setUniform(const GLchar* uniformName, TYPE v0, TYPE v1); \
        void setUniform(const GLchar* uniformName, TYPE v0, TYPE v1, TYPE v2); \
        void setUniform(const GLchar* uniformName, TYPE v0, TYPE v1, TYPE v2, TYPE v3); \
\
        void setUniform1v(const GLchar* uniformName, const TYPE* v, GLsizei count=1); \
        void setUniform2v(const GLchar* uniformName, const TYPE* v, GLsizei count=1); \
        void setUniform3v(const GLchar* uniformName, const TYPE* v, GLsizei count=1); \
        void setUniform4v(const GLchar* uniformName, const TYPE* v, GLsizei count=1);

        _MAKAI_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLfloat)
        _MAKAI_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLdouble)
        _MAKAI_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLint)
        _MAKAI_PROGRAM_ATTRIB_N_UNIFORM_SETTERS(GLuint)

        void setUniformMatrix2(const GLchar* uniformName, const GLfloat* v, GLsizei count=1, GLboolean transpose=GL_FALSE);
        void setUniformMatrix3(const GLchar* uniformName, const GLfloat* v, GLsizei count=1, GLboolean transpose=GL_FALSE);
        void setUniformMatrix4(const GLchar* uniformName, const GLfloat* v, GLsizei count=1, GLboolean transpose=GL_FALSE);
        void setUniform(const GLchar* uniformName, const glm::mat2& m, GLboolean transpose=GL_FALSE);
        void setUniform(const GLchar* uniformName, const glm::mat3& m, GLboolean transpose=GL_FALSE);
        void setUniform(const GLchar* uniformName, const glm::mat4& m, GLboolean transpose=GL_FALSE);
        void setUniform(const GLchar* uniformName, const glm::vec3& v);
        void setUniform(const GLchar* uniformName, const glm::vec4& v);

        ShaderProgram(const ShaderProgram &other) = delete;
        const ShaderProgram& operator=(const ShaderProgram &other) = delete;
    private:
        std::vector<Shader*> m_shaders;
        //shaders added by using addShader()
        std::vector<Shader*> addedShaders;

        std::string m_log;
        GLuint m_program;
    };
}



#endif // SHADERPROGRAM_H
