#include "parametergroup.h"

#include "model/BaseOperator.hpp"
#include "model/parameter/BaseParameter.hpp"
#include "model/parameter/ButtonParameter.hpp"

#include "EnumParameterWidget.hpp"

#include "model/parameter/Int64Component.hpp"
#include "model/parameter/Float64Component.hpp"
#include "IntParameterWidget.hpp"
#include "DoubleParameterWidget.hpp"
#include "ButtonParameterWidget.hpp"
#include "Transform2DParameterWidget.hpp"

#include <QLabel>



ParameterGroup::ParameterGroup(QWidget *parent, BaseZigZagObject* parameters_)
    : QFrame(parent), parameters(parameters_)
{
    Q_ASSERT(parent);
    Q_ASSERT(parameters);

    if (dynamic_cast<BaseOperator*>(parameters))
    {
        setFrameStyle(QFrame::NoFrame);
        layout.setMargin(0);
    }
    else
    {
        setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
        layout.addRow(new QLabel(parameters->objectName()));
        layout.setMargin(10);
    }
    layout.setVerticalSpacing(4);
    layout.setHorizontalSpacing(7);

    for (auto child : parameters->findChildren<BaseZigZagObject*>(QString(), Qt::FindDirectChildrenOnly))
    {
        BaseParameter* par = qobject_cast<BaseParameter*>(child);

        if (par)
        {
            layout.addRow(par->objectName(), new_widget_for_parameter(par));
        }
        else
        {
            layout.addRow(new ParameterGroup(this, child));
        }

    }
}


QWidget* ParameterGroup::new_widget_for_parameter(BaseParameter* par)
{
    Q_ASSERT(par);

    if (par->getParameterType() == ParameterType::Enum)
    {
        return new EnumParameterWidget(this, static_cast<EnumParameter*>(par));
    }
    else if (par->getParameterType() == ParameterType::Button)
    {
        return new ButtonParameterWidget(this, static_cast<ButtonParameter*>(par));
    }
    else if (par->getParameterType() == ParameterType::Int  ||
             par->getParameterType() == ParameterType::Int2 ||
             par->getParameterType() == ParameterType::Int3 ||
             par->getParameterType() == ParameterType::Int4)
    {
        if (par->getComponents().size() == 1)
        {
            return new IntParameterWidget(this, static_cast<Int64Component*>(par->getComponents()[0]));
        }
        else
        {
            auto widget = new QWidget(this);
            auto widget_layout = new QHBoxLayout(widget);
            widget_layout->setMargin(0);
            widget_layout->setSpacing(4);

            for (int i = 0; i < par->getComponents().size(); ++i)
            {
                widget_layout->addWidget(new IntParameterWidget(this, static_cast<Int64Component*>(par->getComponents()[i])));
            }
            return widget;
        }
    }
    else if (par->getParameterType() == ParameterType::Float  ||
             par->getParameterType() == ParameterType::Float2 ||
             par->getParameterType() == ParameterType::Float3 ||
             par->getParameterType() == ParameterType::Float4 ||
             par->getParameterType() == ParameterType::Color)
    {
        if (par->getComponents().size() == 1)
        {
            return new DoubleParameterWidget(this, static_cast<Float64Component*>(par->getComponents()[0]));
        }
        else
        {
            auto widget = new QWidget(this);
            auto widget_layout = new QHBoxLayout(widget);
            widget_layout->setMargin(0);
            widget_layout->setSpacing(4);

            for (int i = 0; i < par->getComponents().size(); ++i)
            {
                widget_layout->addWidget(new DoubleParameterWidget(this, static_cast<Float64Component*>(par->getComponents()[i])));
            }
            return widget;
        }
    }
    else if (par->getParameterType() == ParameterType::Transform2D)
    {
        return new Transform2DParameterWidget(this, static_cast<Transform2DParameter*>(par));
    }
    return nullptr;
}
