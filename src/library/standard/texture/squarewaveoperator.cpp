#include "squarewaveoperator.h"

#include <QMatrix>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <iostream>


bool SquareWaveOperator::gpu_resources_initialized = false;
QOpenGLShaderProgram SquareWaveOperator::flat_shader;
QOpenGLShaderProgram SquareWaveOperator::radial_shader;
QOpenGLShaderProgram SquareWaveOperator::concentric_shader;
std::array<QOpenGLShaderProgram*, 3> SquareWaveOperator::shaders { &flat_shader, &radial_shader, &concentric_shader };
GLuint SquareWaveOperator::vao;
GLuint SquareWaveOperator::vbo;


// Draws the whole screen with GL_TRIANGLE_STRIP
static GLfloat const vertices[] = { -1, 1, -1, -1, 1, 1, 1, -1 };


BaseOperator * SquareWaveOperator::create(BaseZigZagObject* parent)
{
    return new SquareWaveOperator(parent);
}


const OperatorDescription SquareWaveOperator::description
{
    "Square Wave",
    "Video Synthesis",
    &create,
    {  },
    { &TextureData::description },
    &TextureView::Type
};


SquareWaveOperator::SquareWaveOperator(BaseZigZagObject* parent)
    : BaseOperator(parent, description)
{
    initializeOpenGLFunctions();
    should_update = true;
}


void SquareWaveOperator::run()
{
    if (!gpu_resources_initialized)
    {
        gpu_resources_initialized = true;

        // Flat Square wave shader
        flat_shader.create();
        flat_shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vert/basic.vert");
        flat_shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/frag/squareWaveFlat.frag");
        auto success = flat_shader.link();
        Q_ASSERT(success);

        glUseProgram(flat_shader.programId());
        flat_shader.setUniformValue(flat_shader.uniformLocation("output_range_x"), -10.0f, 10.0f);
        flat_shader.setUniformValue(flat_shader.uniformLocation("output_range_y"), -10.0f, 10.0f);

        // Radial Square wave shader
        radial_shader.create();
        radial_shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vert/basic.vert");
        radial_shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/frag/squareWaveRadial.frag");
        success = radial_shader.link();
        Q_ASSERT(success);

        glUseProgram(radial_shader.programId());
        radial_shader.setUniformValue(radial_shader.uniformLocation("output_range_x"), -10.0f, 10.0f);
        radial_shader.setUniformValue(radial_shader.uniformLocation("output_range_y"), -10.0f, 10.0f);

        // Concentric Square wave shader
        concentric_shader.create();
        concentric_shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/vert/basic.vert");
        concentric_shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/frag/squareWaveConcentric.frag");
        success = concentric_shader.link();
        Q_ASSERT(success);

        glUseProgram(concentric_shader.programId());
        concentric_shader.setUniformValue(concentric_shader.uniformLocation("output_range_x"), -10.0f, 10.0f);
        concentric_shader.setUniformValue(concentric_shader.uniformLocation("output_range_y"), -10.0f, 10.0f);


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

    if (should_update)
    {
        auto shader = shaders[wave_type.getIndex()];

        output_texture.bindFramebuffer();

        glUseProgram(shader->programId());
        glBindVertexArray(vao);

        shader->setUniformValue(shader->uniformLocation("duty_cycle"), static_cast<float>(duty_cyce.get()));
        shader->setUniformValue(shader->uniformLocation("color_a"), color_a.get());
        shader->setUniformValue(shader->uniformLocation("color_b"), color_b.get());

        auto translated = glm::translate(glm::mat3(1), glm::vec2(translation.x(), translation.y()));
        auto scaled = glm::scale(translated, glm::vec2(scale.x(), scale.y()));
        auto rotated = glm::rotate(scaled, static_cast<float>(rotation.get()));
        glUniformMatrix3fv(shader->uniformLocation("transformation"), 1, GL_FALSE, (float*)(&rotated));

        if (wave_type.getIndex()  == 1) // Radial
        {
            radial_shader.setUniformValue(radial_shader.uniformLocation("frequency"), static_cast<float>(frequency.get()));
        }
        else if (wave_type.getIndex() == 2) // Conncentric
        {
            concentric_shader.setUniformValue(concentric_shader.uniformLocation("phase"), static_cast<float>(phase.get()));
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        updateView();
        should_update = false;
    }
}


void SquareWaveOperator::parameterChangedEvent(const BaseParameter* parameter)
{
    should_update = true;
}
