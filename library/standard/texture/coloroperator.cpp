#include "coloroperator.h"

ColorOperator::ColorOperator()
    : BaseOperator(Type)
{
    initializeOpenGLFunctions();
}


ColorOperator::~ColorOperator()
{
}


void ColorOperator::run()
{
    //std::cout << red_value.get() << "color run \n";
    output_texture.bind_as_framebuffer();
    glClearColor(red_value / 255.0, green_value / 255.0, blue_value / 255.0, alpha_value / 255.0);
    glClear(GL_COLOR_BUFFER_BIT);
    //auto test_t = test.get()[1];
}


BaseOperator* ColorOperator::create()
{
    return new ColorOperator();
}