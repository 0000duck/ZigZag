#include "sinewavecontroloperator.h"
#include <cmath>
#include "application.h"




BaseOperator * SineWaveControlOperator::create(BaseZigZagObject* parent)
{
    return new SineWaveControlOperator(parent);
}


const OperatorDescription SineWaveControlOperator::description
{
    "Sine Wave",
    "Control",
    &create,
    { },
    { },
    &ParameterView::Type
};


SineWaveControlOperator::SineWaveControlOperator(BaseZigZagObject* parent)
    : BaseOperator(parent, description)
{
    output.getComponents()[0]->setFlag(ParameterFlags::IsEditable, false);
    output.getComponents()[0]->setFlag(ParameterFlags::CanImport, false);
    input.getComponents()[0]->setFlag(ParameterFlags::IsEditable, false);
    input.getComponents()[0]->setFlag(ParameterFlags::IsVisible, false);
}


void SineWaveControlOperator::run()
{
    double val;

    if (input.getComponents()[0]->isImporting())
    {
        val = input.get();
    }
    else
    {
        val = application::clock()->get_elapsed_seconds();
    }

    output.set(amplitude.get() * std::sin(val / period.get()) + offset.get());
    update_view();
}
