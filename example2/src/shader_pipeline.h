#pragma once

#include <string>
#include <memory>

#include <GL/glew.h>

#include <GL/gl.h>

#include "shader.h"

class ShaderPipeline
{
public:
    ShaderPipeline(const std::shared_ptr<Shader>& vertexShader,
                   const std::shared_ptr<Shader>& fragmentShader);
    ~ShaderPipeline();

    bool isValid() const;
    GLuint id() const;
    const std::string& log() const;

    void free();
    void use();


private:
    bool shadersIsValid() const;
    void createGPUProgram();

private:
    std::shared_ptr<Shader> mVertexShader;
    std::shared_ptr<Shader> mFragmentShader;
    bool mIsValid;

    GLuint mProgramId;
    std::string  mLogMessage;
};
