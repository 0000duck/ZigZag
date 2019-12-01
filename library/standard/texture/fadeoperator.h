#pragma once

#include <array>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#include "model/baseoperator.h"
#include "model/datainput.h"

#include "library/standard/texture/texturedata.h"
#include "library/standard/texture/textureview.h"



class FadeOperator : public BaseOperator,
                     public QOpenGLFunctions_3_3_Core
{

public:

    FadeOperator();

    void run() override;

    void parameter_changed(BaseParameterOld* parameter) override;


    static BaseOperator* create();

    static const OperatorDescription description;

private:

    FloatPar position{ this, "Fade Position", 0, 0, 4 };

    DataInput texture_0{ this, "Input 0", TextureData::Type };
    DataInput texture_1{ this, "Input 1", TextureData::Type };
    DataInput texture_2{ this, "Input 2", TextureData::Type };
    DataInput texture_3{ this, "Input 3", TextureData::Type };
    DataInput texture_4{ this, "Input 4", TextureData::Type };

    std::array<DataInput*, 5> textures = { &texture_0, &texture_1, &texture_2, &texture_3, &texture_4 };

    TextureData output_texture{ this, "Result" };

    static bool gpu_resources_initialized;
    static QOpenGLShaderProgram shader;
    static GLuint vao;
    static GLuint vbo;

};
