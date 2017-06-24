#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include "GL/glew.h"

namespace makai
{
    class Shader
    {
    public:
        enum ShaderType
        {
            Vertex = 0,
            Fragment = 1
        };

        Shader(Shader::ShaderType type);
        ~Shader();
        bool compileSourceCode(const std::string &source);
        bool compileSourceFile(const std::string &fileName);
        bool isCompiled() const;
        std::string log() const;
        GLuint shaderId() const;
        Shader::ShaderType shaderType() const;
        std::string sourceCode() const;

        Shader(const Shader &other) = delete;
        const Shader& operator=(const Shader &other) = delete;
    private:
        ShaderType m_type;
        bool m_isCompiled;
        std::string m_log;
        GLuint m_id;
        std::string m_code;

    };
}


#endif // SHADER_H
