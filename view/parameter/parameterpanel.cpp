#include "parameterpanel.h"
#include "parametereditor.h"
#include "parameterpanelgroup.h"

#include <QGraphicsScene>

#include "model/datainput.h"
#include "model/basedatatype.h"
#include "model/baseoperator.h"

#include "view/operatorview.h"



ParameterPanel::ParameterPanel(ParameterEditor* e)
    : editor(e)
{
    setAutoFillBackground(true);
    layout.setContentsMargins(10, 10, 10, 10);
    layout.setSpacing(10);
    layout.addStretch();
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


void ParameterPanel::on_selection_changed()
{
    auto selected_items = scene->selectedItems();

    if (selected_items.size() == 1)
    {
        auto selected = dynamic_cast<OperatorView*>(selected_items[0]);

        if (selected)
        {
            build_ui(selected->operator_model);
            editor->setVisible(true);
            return;
        }
    }
    editor->setVisible(false);
}


void ParameterPanel::build_ui(BaseOperator& op)
{
    if (parameters)
    {
        delete parameters;
        parameters = nullptr;
    }
    parameters = new ParameterPanelGroup(this, &op);
    layout.insertWidget(0, parameters);
}

