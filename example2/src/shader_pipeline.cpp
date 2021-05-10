#include "shader_pipeline.h"

#include <vector>
#include <iostream>

ShaderPipeline::ShaderPipeline(const std::shared_ptr<Shader> &vertexShader, const std::shared_ptr<Shader> &fragmentShader)
    : mVertexShader(vertexShader)
    , mFragmentShader(fragmentShader)
    , mIsValid(false)
    , mProgramId(0)
{
    createGPUProgram();
}

ShaderPipeline::~ShaderPipeline()
{
    free();
}

bool ShaderPipeline::isValid() const
{
    return mIsValid && (glIsProgram(mProgramId) == GL_TRUE);
}

void ShaderPipeline::free()
{
    if (glIsProgram(mProgramId) == GL_TRUE)
        glDeleteProgram(mProgramId);

    mVertexShader.reset();
    mFragmentShader.reset();
    mIsValid = false;
    mProgramId = 0;
}

void ShaderPipeline::use()
{
    if (isValid())
        glUseProgram(mProgramId);
}

GLuint ShaderPipeline::id() const
{
    return mProgramId;
}

const std::string &ShaderPipeline::log() const
{
    return mLogMessage;
}

bool ShaderPipeline::shadersIsValid() const
{
    const bool vertexIsValid = mVertexShader && mVertexShader->isValid();
    const bool fragmentIsValid = mFragmentShader && mFragmentShader->isValid();

    return vertexIsValid && fragmentIsValid;
}

void ShaderPipeline::createGPUProgram()
{
    if (!shadersIsValid())
        return;

    // Create
    {
        mProgramId = glCreateProgram();

        glAttachShader(mProgramId, mVertexShader->id());
        glAttachShader(mProgramId, mFragmentShader->id());

        glLinkProgram(mProgramId);
    }

    // Validate
    {
        GLint result = GL_FALSE;

        // linkStatus
        glGetProgramiv(mProgramId, GL_LINK_STATUS, &result);
        mIsValid = (result == GL_TRUE);

        GLint logLength = 0;
        glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0)
        {
            std::vector<char> log(logLength + 1);
            glGetProgramInfoLog(mProgramId, logLength, nullptr, &log[0]);

            mLogMessage = &log[0];

            std::cout << mLogMessage << std::endl;
        }
    }
}
