#include "blendoperator.h"


#include <iostream>

bool BlendOperator::gpu_resources_initialized = false;
QOpenGLShaderProgram BlendOperator::shader;
GLuint BlendOperator::vao;
GLuint BlendOperator::vbo;

// Draws the whole screen with GL_TRIANGLE_STRIP
static GLfloat const vertices[] = { -1, 1, -1, -1, 1, 1, 1, -1 };




const OperatorDescription BlendOperator::description {
    "Blend",
    "Video Edit",
    &create,
    { &TextureData::description, &TextureData::description },
    { &TextureData::description },
    &TextureView::Type
};



BaseOperator* BlendOperator::create(BaseZigZagObject* parent)
{
    return new BlendOperator(parent);
}



BlendOperator::BlendOperator(BaseZigZagObject* parent)
    : BaseOperator(parent, description)
{
    initializeOpenGLFunctions();
}


void BlendOperator::run()
{
    if (!gpu_resources_initialized)
    {
        gpu_resources_initialized = true;
        shader.create();
        shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vert/basic.vert");
        shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/frag/blend.frag");
        auto success = shader.link();
        Q_ASSERT(success);

        glUseProgram(shader.programId());
        shader.setUniformValue(shader.uniformLocation("output_range_x"), 0.0f, 1.0f);
        shader.setUniformValue(shader.uniformLocation("output_range_y"), 0.0f, 1.0f);
        shader.setUniformValue(shader.uniformLocation("input_a"), 0);
        shader.setUniformValue(shader.uniformLocation("input_b"), 1);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        //unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    auto a = static_cast<const TextureData*>(m_textureA.get_connection());
    auto b = static_cast<const TextureData*>(m_textureB.get_connection());

    if (a && b)
    {
        m_outputTexture.bindFramebuffer();
        glUseProgram(shader.programId());
        glBindVertexArray(vao);
        shader.setUniformValue(shader.uniformLocation("blend_mode"), m_blendMode.getIndex());
        a->bindTexture(0);
        b->bindTexture(1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    else
    {
        m_outputTexture.bindFramebuffer();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    updateView();

}


void BlendOperator::parameterChangedEvent(const BaseParameter* parameter)
{

}


