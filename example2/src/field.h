#pragma once

#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "shader_pipeline.h"

class Field
{
public:
    Field();

    using Pos = unsigned char;
    enum Item : Pos {
        Nobody = 0,
        Cross,
        Circle
    };

    void draw();
    void set(Item value, Pos x, Pos y);

private:
    // field 3x3
    std::array<std::array<Item, 3>, 3> mField;

private:
    void loadGridVertexData();
    void loadCircleVertexData(Pos x, Pos y);
    void loadCrossVertexData(Pos x, Pos y);
    void initShaders();
    void drawGrid();
    void drawValues();
    void drawCircle(Pos x, Pos y);
    void drawCross(Pos x, Pos y);
    void setColor(GLuint objId, const glm::vec3 &color);

    std::unique_ptr<ShaderPipeline> mLinePipeline;

    GLuint mGridVAO;
    GLuint mGridLinesVBO;

    std::array<std::array<GLuint, 3>, 3> mItemVAOs;
    std::array<std::array<GLuint, 3>, 3> mItemVBOs;
};
