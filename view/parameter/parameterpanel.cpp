#include "parameterpanel.h"

#include <QLabel>
#include <QFormLayout>
#include <QGraphicsScene>

#include "model/datainput.h"
#include "model/basedatatype.h"
#include "model/baseoperator.h"

#include "view/operatorview.h"
#include "view/parameter/intwidget.h"
#include "view/parameter/enumwidget.h"
#include "view/parameter/parametereditor.h"


ParameterPanel::ParameterPanel(ParameterEditor* e)
    : editor(e)
{
    MontSerrat.setPixelSize(14);
    setAutoFillBackground(true);

    layout = new QFormLayout();
    //layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);

    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    //setMinimumSize(QSize(100, 600));
}


void ParameterPanel::set_scene(QGraphicsScene* new_scene)
{
    if (scene != new_scene)
    {
        if (scene != nullptr)
        {
            disconnect(scene, &QGraphicsScene::selectionChanged, this, &ParameterPanel::on_selection_changed);
        }
        if (new_scene)
        {
            scene = new_scene;
            connect(scene, &QGraphicsScene::selectionChanged, this, &ParameterPanel::on_selection_changed);
        }
    }
}


void ParameterPanel::clear()
{
    for (int i = layout->rowCount() - 1; i >= 0; --i)
    {
        layout->removeRow(i);
    }
}


void ParameterPanel::on_selection_changed()
{
    auto selected_items = scene->selectedItems();

    if (selected_items.size() == 1)
    {
        auto selected = dynamic_cast<OperatorView*>(selected_items[0]);

        if (selected)
        {
            clear();
            build_ui(selected->operator_model);
            editor->setVisible(true);
            return;
        }
    }
    editor->setVisible(false);
}


void ParameterPanel::add_parameters(const std::vector<BaseParameter*>& parameters)
{
    for (auto & p : parameters)
    {
        QLabel* label = new QLabel(p->name);
        label->setFont(MontSerrat);

        if (p->type == ParameterType::Int)
        {
            layout->addRow(label, new IntWidget(this, qobject_cast<IntegerParameter*>(p)));
        }
        else if (p->type == ParameterType::Enum)
        {
            layout->addRow(label, new EnumWidget(this, qobject_cast<EnumParameter*>(p)));
        }
    }
}


void ParameterPanel::build_ui(BaseOperator& op)
{
    QLabel* l1 = new QLabel("Operator Parameters");
    //l1->setAlignment(Qt::AlignCenter);
    l1->setFont(MontSerrat);
    layout->addRow(l1);

    add_parameters(op.parameters());

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addRow(line);

    QLabel* l2 = new QLabel("Output Parameters");
    //l2->setAlignment(Qt::AlignCenter);
    l2->setFont(MontSerrat);
    layout->addRow(l2);

    for (auto o : op.data_outputs())
    {
        add_parameters(o->parameters());
    }

    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    layout->addRow(line2);

    QLabel* l3 = new QLabel("Input Parameters");
    //l3->setAlignment(Qt::AlignCenter);
    l3->setFont(MontSerrat);
    layout->addRow(l3);

    for (auto o : op.data_inputs())
    {
        add_parameters(o->parameters());
    }

}
