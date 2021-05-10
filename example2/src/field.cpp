#include "field.h"

#include <algorithm>
#include <math.h>

namespace  {

static const int circlePointsCount = 40;

}

Field::Field()
{
    loadGridVertexData();
    initShaders();

    mField.fill({Nobody, Nobody, Nobody});
    mItemVAOs.fill({0,0,0});
    mItemVBOs.fill({0,0,0});
}

void Field::draw()
{
    drawGrid();
    drawValues();
}

void Field::set(Field::Item value, Field::Pos x, Field::Pos y)
{
    if (x >= 3 || y >= 3)
        return;

    if (mField[x][y] != Nobody)
        return;

    switch (value) {
        case Cross:
            loadCrossVertexData(x, y);
            break;

        case Circle:
            loadCircleVertexData(x, y);
            break;

        case Nobody:
            return;
            break;
    }


    mField[x][y] = value;
}

void Field::loadGridVertexData()
{
    // Обязательно ли удалять каждый VBO , если удаляется VAO?
    // Будет ли утечка, если этого не делать?
    glGenVertexArrays(1, &mGridVAO);
    glBindVertexArray(mGridVAO);

    const GLfloat lines[] = {
        -0.34f, -1.f, 0.f,   // [0]
        -0.34f,  1.f, 0.f,   // [1]
        0.34f,  -1.f, 0.f,   // [2]
        0.34f,   1.f, 0.f,   // [3]
        -1.f,  -0.34f, 0.f,   // [4]
        1.0f,  -0.34f, 0.f,   // [5]
        -1.f,   0.34f, 0.f,   // [6]
        1.0f,   0.34f, 0.f,   // [7]
    };

    glGenBuffers(1, &mGridLinesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mGridLinesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
}

void Field::loadCircleVertexData(Pos x, Pos y)
{
    glGenVertexArrays(1, &mItemVAOs[x][y]);
    glBindVertexArray(mItemVAOs[x][y]);

    glGenBuffers(1, &mItemVBOs[x][y]);

    const GLfloat coeff = 0.33;

    const GLfloat centerX = (coeff * (2 * x - 2));
    const GLfloat centerY = -(coeff * (2 * y - 2));
    const GLfloat radius = coeff * 0.8;

    GLfloat points[(circlePointsCount + 1) * 3];
    long double angle = 0.0;
    for (int i = 0; i < circlePointsCount * 3; i+=3)
    {
        // параметрическое уравнение окружности
        points[i + 0] = cos(angle) * radius + centerX; // X
        points[i + 1] = sin(angle) * radius + centerY; // Y
        points[i + 2] = 0.f;                           // Z

        angle += (2 * M_PI) / circlePointsCount;
    }

    points[circlePointsCount*3] = points[0];
    points[circlePointsCount*3 + 1] = points[1];
    points[circlePointsCount*3 + 2] = points[2];

    glBindBuffer(GL_ARRAY_BUFFER, mItemVBOs[x][y]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}

void Field::loadCrossVertexData(Pos x, Pos y)
{
    glGenVertexArrays(1, &mItemVAOs[x][y]);
    glBindVertexArray(mItemVAOs[x][y]);

    glGenBuffers(1, &mItemVBOs[x][y]);

    const GLfloat coeff = 0.66f;

    const GLfloat x1 = coeff*x - 1;
    const GLfloat x2 = coeff*(x+1) - 1;

    const GLfloat y1 = -(coeff*y - 1);
    const GLfloat y2 = -(coeff*(y+1) - 1);


    // TODO можно было обсчитать это с помощью шейдера на видеокарте. Было бы в несколько раз десятков быстрее!!!
    const GLfloat lines[] = {
        x1, y1, 0.f,
        x2, y2, 0.f,
        x2, y1, 0.f,
        x1, y2, 0.f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, mItemVBOs[x][y]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
}

void Field::initShaders()
{
    // simple color shader (with selection color)
    const char* colorShaderSource =
        "#version 330 core \n"
        "uniform vec3 lineColor;"
        "out vec3 color;"
        "void main() {"
        "    color = lineColor;"
        "}";

    auto fragmentShader = Shader::create(colorShaderSource, GL_FRAGMENT_SHADER);

    // simple 2d vertex shader
    const char* lineShaderSource =
        "#version 330 core \n"
        "layout(location = 0) in vec4 vPos;"
        "void main() {"
        "    gl_Position = vPos;"
        "    gl_Position.w = 1.f;"
        "}";

    auto vertexShader = Shader::create(lineShaderSource, GL_VERTEX_SHADER);

    mLinePipeline = std::make_unique<ShaderPipeline>(vertexShader, fragmentShader);
}

void Field::drawGrid()
{
    mLinePipeline->use();
    setColor(mLinePipeline->id(), {0.f, 0.f, 0.f});

    glBindVertexArray(mGridVAO);

    GLuint attrLocation = glGetAttribLocation(mLinePipeline->id(), "vPos");
    glEnableVertexAttribArray(attrLocation);

    glBindBuffer(GL_ARRAY_BUFFER, mGridLinesVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    for (int i = 0; i < 4; ++i)
        glDrawArrays(GL_LINES, i*2, 2);

    glDisableVertexAttribArray(attrLocation);

}

void Field::drawValues()
{
    for (Pos y = 0; y < 3; ++y) {
        for (Pos x = 0; x < 3; ++x) {
            if (mField[x][y] == Cross)
                drawCross(x, y);
            else if (mField[x][y] == Circle)
                drawCircle(x, y);
        }
    }

}

void Field::drawCircle(Field::Pos x, Field::Pos y)
{
    mLinePipeline->use();
    setColor(mLinePipeline->id(), {1.f, 0.f, 0.f});

    glBindVertexArray(mItemVAOs[x][y]);

    GLuint attribLocation = glGetAttribLocation(mLinePipeline->id(), "vPos");
    glEnableVertexAttribArray(attribLocation);

    glBindBuffer(GL_ARRAY_BUFFER, mItemVBOs[x][y]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    for (int i = 0; i < circlePointsCount; ++i)
        glDrawArrays(GL_LINES, i, 2);

    glDisableVertexAttribArray(attribLocation);
}

void Field::drawCross(Field::Pos x, Field::Pos y)
{
    mLinePipeline->use();
    setColor(mLinePipeline->id(), {1.f, 0.f, 1.f});

    glBindVertexArray(mItemVAOs[x][y]);

    GLuint attribLocation = glGetAttribLocation(mLinePipeline->id(), "vPos");
    glEnableVertexAttribArray(attribLocation);

    glBindBuffer(GL_ARRAY_BUFFER, mItemVBOs[x][y]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_LINES, 0, 2);
    glDrawArrays(GL_LINES, 2, 2);

    glDisableVertexAttribArray(attribLocation);
}

void Field::setColor(GLuint objId, const glm::vec3 &color)
{
    GLuint uniformLocation = glGetUniformLocation(objId, "lineColor");
    glUniform3f(uniformLocation, color.r, color.g, color.b);
}
