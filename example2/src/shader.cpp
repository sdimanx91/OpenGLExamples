#include "shader.h"

#include <vector>

#include <iostream>

Shader::Shader(const std::string &source, GLenum shaderType)
    : mId(0)
    , mIsValid(false)
{
    compile(source, shaderType);
}


std::shared_ptr<Shader> Shader::create(const std::string &source, GLenum shaderType)
{
    return std::make_shared<Shader>(source, shaderType);
}

Shader::~Shader()
{
    free();
}

void Shader::compile(const std::string &source, GLenum shaderType)
{
    if (source.empty())
        return ;

    GLuint shaderID = glCreateShader(shaderType);
    mId = shaderID;

    // compile
    {
        const char * sourcePtr = source.c_str();
        glShaderSource(shaderID, 1, &sourcePtr, NULL);
        glCompileShader(shaderID);
    }

    // validate
    {
        GLint result = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
        mIsValid = (result == GL_TRUE);

        GLint logLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0)
        {
            std::vector<char> log(logLength + 1);
            glGetShaderInfoLog(shaderID, logLength, nullptr, &log[0]);

            mLogMessage = &log[0];

            std::cout << mLogMessage << std::endl;
        }
    }
}

bool Shader::isValid() const
{
    return mIsValid;
}

GLuint Shader::id() const
{
    return mId;
}

const std::string &Shader::log() const
{
    return mLogMessage;
}

void Shader::free()
{
    if (glIsShader(mId) == GL_TRUE)
        glDeleteShader(mId);

    mId = 0;
    mIsValid = false;
    mLogMessage.clear();
}

