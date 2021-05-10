#pragma once

#include <string>
#include <memory>

#include <GL/glew.h>

#include <GL/gl.h>

class Shader final
{
public:
    static std::shared_ptr<Shader> create(const std::string& source, GLenum shaderType);

    Shader(const std::string& source, GLenum shaderType);
    ~Shader();
public:
    void compile(const std::string& source, GLenum shaderType);
    bool isValid() const;
    GLuint id() const;
    const std::string& log() const;
    void free();


private:
    std::string mLogMessage;
    GLuint mId;
    bool mIsValid;
};
