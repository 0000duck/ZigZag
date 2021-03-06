#include "increasingvalueoperator.h"
#include "application.h"



BaseOperator * IncreasingValueOperator::create(BaseZigZagObject* parent)
{
    return new IncreasingValueOperator(parent);
}


const OperatorDescription IncreasingValueOperator::description
{
    "Increasing Value",
    "Control",
    &create,
    { },
    { },
    &ParameterView::Type
};


IncreasingValueOperator::IncreasingValueOperator(BaseZigZagObject* parent)
    : BaseOperator(parent, description)
{

}


void IncreasingValueOperator::run()
{
    recalculate();
}


void IncreasingValueOperator::parameterChangedEvent(const BaseParameter * parameter)
{
    if (parameter == &reset)
    {
        value.set(0);
    }
}


void IncreasingValueOperator::recalculate()
{
    float time_now = application::clock()->get_elapsed_seconds();
    float dt = time_now - last_frame_time;
    double increase = dt * speed.get();
    value.set(value.get() + increase);
    last_frame_time = time_now;
    updateView();
}
