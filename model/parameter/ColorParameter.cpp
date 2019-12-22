#include "ColorParameter.hpp"



ColorParameter::ColorParameter(BaseZigZagObject * parent, const QString& name, QColor color)
    : BaseParameter(ParameterType::Color, parent, name)
{
    m_red.setObjectName(QStringLiteral("Red"));
    m_green.setObjectName(QStringLiteral("Green"));
    m_blue.setObjectName(QStringLiteral("Blue"));
    m_alpha.setObjectName(QStringLiteral("Alpha"));
    set(color);
}


void ColorParameter::set(const QColor& color)
{
    m_red.setValue(color.redF());
    m_green.setValue(color.greenF());
    m_blue.setValue(color.blueF());
    m_alpha.setValue(color.alphaF());
}


QColor ColorParameter::get() const
{
    QColor color;
    color.setRedF(m_red.getValue());
    color.setGreenF(m_green.getValue());
    color.setBlueF(m_blue.getValue());
    color.setAlphaF(m_alpha.getValue());
    return color;
}
