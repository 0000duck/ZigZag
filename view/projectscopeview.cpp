#include "projectscopeview.h"
#include "application.h"

#include "view/baseconnector.h"
#include "view/operatorview.h"
#include "view/cable.h"
#include "view/dataconnector.h"
#include "view/parameterconnector.h"
#include "model/baseoperator.h"
#include "model/datainput.h"

#include <QPointer>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include "library/standard/test/testoperator.h"


ProjectScopeView::ProjectScopeView(QObject *parent)
    : QGraphicsScene(-20000, -20000, 40000, 40000)
{
    setBackgroundBrush(QBrush(QColor(55, 55, 55)));
}


void ProjectScopeView::set_model(ProjectModel *model)
{
    data_model = model;

    connect(this, &ProjectScopeView::redo_signal, data_model, &ProjectModel::redo);
    connect(this, &ProjectScopeView::undo_signal, data_model, &ProjectModel::undo);

    connect(data_model, &ProjectModel::operator_added, this, &ProjectScopeView::on_operator_added);
    connect(data_model, &ProjectModel::operator_removed, this, &ProjectScopeView::on_operator_deleted);
}


void ProjectScopeView::bring_to_front(OperatorView* op)
{
    for (auto o : operator_views)
    {
        if (o != op)
        {
            o->setZValue(0.1);
        }
    }
    op->setZValue(1);
}


void ProjectScopeView::on_operator_added(BaseOperator* operator_ptr)
{
    OperatorView* op_view = new OperatorView(*operator_ptr);
    operator_views.insert(operator_ptr, op_view);

    for (auto i : operator_ptr->data_inputs())
    {
        if (i)
        {
            connect(i, &DataInput::has_connected, this, &ProjectScopeView::on_input_connected);
            connect(i, &DataInput::has_disconnected, this, &ProjectScopeView::on_input_disconnected);
        }
    }
    connect(operator_ptr, &ParameterOwner::parameters_connected, this, &ProjectScopeView::on_parameters_connected);
    connect(operator_ptr, &ParameterOwner::parameters_disconnected, this, &ProjectScopeView::on_parameter_disconnected);

    addItem(op_view);
}


void ProjectScopeView::on_operator_deleted(BaseOperator* operator_ptr)
{
    if (operator_views.contains(operator_ptr))
    {
        for (auto i : operator_ptr->data_inputs())
        {
            if (i)
            {
                disconnect(i, &DataInput::has_connected, this, &ProjectScopeView::on_input_connected);
                disconnect(i, &DataInput::has_disconnected, this, &ProjectScopeView::on_input_disconnected);
            }
        }
        disconnect(operator_ptr, &ParameterOwner::parameters_connected, this, &ProjectScopeView::on_parameters_connected);
        disconnect(operator_ptr, &ParameterOwner::parameters_disconnected, this, &ProjectScopeView::on_parameter_disconnected);


        OperatorView* op_view = operator_views[operator_ptr];
        operator_views.remove(operator_ptr);
        removeItem(op_view);
        delete op_view;
    }
}


void ProjectScopeView::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsScene::mousePressEvent(mouseEvent);
}


void ProjectScopeView::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    mouseEvent->setAccepted(false);
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    if (!mouseEvent->isAccepted())
    {
        //if (mouseEvent->)
    }
}


void ProjectScopeView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);

    if (!mouseEvent->isAccepted())
    {
       //data_model->add_operator(TestOperator::Type, mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
       std::cout << "data model" << data_model << "\n";
    }
}


void ProjectScopeView::keyPressEvent(QKeyEvent *keyEvent)
{
    keyEvent->setAccepted(false);
    QGraphicsScene::keyPressEvent(keyEvent);

    if (!keyEvent->isAccepted())
    {
        if (keyEvent->matches(QKeySequence::Undo))
        {
            emit undo_signal();
            keyEvent->setAccepted(true);
        }
        else if (keyEvent->matches(QKeySequence::Redo))
        {
            emit redo_signal();
            keyEvent->setAccepted(true);
        }
        else if (keyEvent->matches(QKeySequence::Delete))
        {
            application::project_model()->get_undo_stack()->beginMacro("Remove selected operators.");

            for (auto obj : selectedItems())
            {
                auto op = dynamic_cast<OperatorView*>(obj);

                if (op)
                {
                    op->operator_model.remove();
                }
            }
            application::project_model()->get_undo_stack()->endMacro();
            keyEvent->setAccepted(true);
        }
        else if (keyEvent->matches(QKeySequence::SelectAll))
        {
            for (auto op : operator_views)
            {
                op->setSelected(true);
            }
            keyEvent->setAccepted(true);
        }
    }
}


void ProjectScopeView::keyReleaseEvent(QKeyEvent *keyEvent)
{
    QGraphicsScene::keyReleaseEvent(keyEvent);
}


void ProjectScopeView::on_input_connected(BaseDataType* output, DataInput* input)
{
    auto input_op = operator_views[input->parent_operator];
    auto output_op = operator_views[output->parent_operator];

    if (input_op && output_op)
    {
        auto input_view = input_op->data_connector_in(input);
        auto output_view = output_op->data_connector_out(output);

        if (input_view && output_view)
        {
            auto cable = new Cable(this, output_view, input_view);
            data_cables.insert(input_view, cable);
            addItem(cable);
        }
    }
}


void ProjectScopeView::on_input_disconnected(BaseDataType* output, DataInput* input)
{
    auto input_op = operator_views[input->parent_operator];

    if (input_op)
    {
        auto input_view = input_op->data_connector_in(input);

        if (input_view)
        {
            auto cable = data_cables[input_view];
            data_cables.remove(input_view);
            removeItem(cable);
            delete cable;
        }
    }
}


void ProjectScopeView::on_parameters_connected(BaseParameter * exporter, BaseParameter * importer)
{
    OperatorView* export_op = operator_views[static_cast<BaseOperator*>(exporter->owner()->top_level_owner())];
    OperatorView* import_op = operator_views[static_cast<BaseOperator*>(importer->owner()->top_level_owner())];

    if (export_op && import_op)
    {
        auto export_connector = export_op->parameter_connector_out();
        auto import_connector = import_op->parameter_connector_in();

        if (export_connector && import_connector)
        {
            if (!parameter_cables.contains({ export_connector, import_connector }))
            {
                auto cable = new Cable(this, export_connector, import_connector);
                parameter_cables.insert({ import_connector, export_connector }, cable);
                addItem(cable);
            }
        }
    }

    // check if those two operators don't have a connection yet
    // That logic should happen in view, because that concept does not exist in model!
    std::cout << "par connected\n";
}


void ProjectScopeView::on_parameter_disconnected(BaseParameter * exporter, BaseParameter * importer)
{
    std::cout << "par disconnected\n";
}
