#include "OperatorNetworkView.hpp"
#include "application.h"

#include "view/BaseConnector.hpp"
#include "view/operatorview.h"
#include "view/cable.h"
#include "view/DataConnector.hpp"
#include "view/ParameterConnector.hpp"
#include "model/BaseOperator.hpp"
#include "model/datainput.h"
#include "model/parameter/BaseComponent.hpp"
#include "view/OperatorSelectorDialog.hpp"

#include <QPointer>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>



OperatorNetworkView::OperatorNetworkView(QObject *parent)
    : QGraphicsScene(-20000, -20000, 40000, 40000)
{
    setBackgroundBrush(QBrush(QColor(55, 55, 55)));
}


void OperatorNetworkView::set_model(OperatorNetwork *model)
{
    data_model = model;

    connect(this, &OperatorNetworkView::redo_signal, data_model, &OperatorNetwork::redo);
    connect(this, &OperatorNetworkView::undo_signal, data_model, &OperatorNetwork::undo);

    connect(data_model, &OperatorNetwork::operator_added, this, &OperatorNetworkView::on_operator_added);
    connect(data_model, &OperatorNetwork::operator_removed, this, &OperatorNetworkView::on_operator_deleted);
}


void OperatorNetworkView::bring_to_front(OperatorView* op)
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


void OperatorNetworkView::keyPressEvent(QKeyEvent *keyEvent)
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
        else if (keyEvent->matches(QKeySequence::Delete) || keyEvent->key() == Qt::Key_Backspace)
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


void OperatorNetworkView::keyReleaseEvent(QKeyEvent *keyEvent)
{
    QGraphicsScene::keyReleaseEvent(keyEvent);
}


void OperatorNetworkView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    auto dialog = new OperatorSelectorDialog(nullptr, event->scenePos().x(), event->scenePos().y());
     connect(dialog, &OperatorSelectorDialog::operatorRequested,
             data_model, &OperatorNetwork::add_operator, Qt::QueuedConnection);

     auto x = event->screenPos().x() - dialog->size().width() / 2;
     auto y = event->screenPos().y() - 100;
     auto screen = QGuiApplication::screenAt(event->screenPos());

     if (screen)
     {
         // make sure always fits in screen
     }
     dialog->move(x, y);
     dialog->show();
}


void OperatorNetworkView::on_operator_added(BaseOperator* operator_ptr)
{
    OperatorView* op_view = new OperatorView(*operator_ptr);
    operator_views.insert(operator_ptr, op_view);

    for (auto i : operator_ptr->dataInputs())
    {
        if (i)
        {
            connect(i, &DataInput::has_connected, this, &OperatorNetworkView::on_input_connected);
            connect(i, &DataInput::has_disconnected, this, &OperatorNetworkView::on_input_disconnected);
        }
    }
    connect(operator_ptr, &BaseOperator::parameter_started_importing, this, &OperatorNetworkView::on_parameters_connected);
    connect(operator_ptr, &BaseOperator::parameter_stopped_importing, this, &OperatorNetworkView::on_parameter_disconnected);

    addItem(op_view);
}


void OperatorNetworkView::on_operator_deleted(BaseOperator* operator_ptr)
{
    if (operator_views.contains(operator_ptr))
    {
        for (auto i : operator_ptr->dataInputs())
        {
            if (i)
            {
                disconnect(i, &DataInput::has_connected, this, &OperatorNetworkView::on_input_connected);
                disconnect(i, &DataInput::has_disconnected, this, &OperatorNetworkView::on_input_disconnected);
            }
        }
        disconnect(operator_ptr, &BaseOperator::parameter_started_importing, this, &OperatorNetworkView::on_parameters_connected);
        disconnect(operator_ptr, &BaseOperator::parameter_stopped_importing, this, &OperatorNetworkView::on_parameter_disconnected);

        OperatorView* op_view = operator_views[operator_ptr];
        operator_views.remove(operator_ptr);
        removeItem(op_view);
        delete op_view;
    }
}


void OperatorNetworkView::on_input_connected(BaseDataType* output, DataInput* input)
{
    auto input_op = operator_views[input->get_operator()];
    auto output_op = operator_views[output->getOperator()];

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


void OperatorNetworkView::on_input_disconnected(BaseDataType* output, DataInput* input)
{
    auto input_op = operator_views[input->get_operator()];

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


void OperatorNetworkView::on_parameters_connected(BaseComponent * exporter, BaseComponent * importer)
{
    OperatorView* export_op = operator_views[exporter->getParameter()->findParent<BaseOperator*>()];
    OperatorView* import_op = operator_views[importer->getParameter()->findParent<BaseOperator*>()];

    if (export_op && import_op)
    {
        auto export_connector = export_op->parameter_connector_out();
        auto import_connector = import_op->parameter_connector_in();

        if (export_connector && import_connector)
        {
            if (!parameter_cables.contains({ export_connector, import_connector }))
            {
                auto cable = new Cable(this, export_connector, import_connector);
                parameter_cables.insert({ export_connector, import_connector }, { cable, 1} );
                addItem(cable);
            }
            else
            {
                parameter_cables[{export_connector, import_connector}].second += 1;
            }
        }
    }
}


void OperatorNetworkView::on_parameter_disconnected(BaseComponent * exporter, BaseComponent * importer)
{
    OperatorView* export_op = operator_views[exporter->getParameter()->findParent<BaseOperator*>()];
    OperatorView* import_op = operator_views[importer->getParameter()->findParent<BaseOperator*>()];

    if (export_op && import_op)
    {
        auto export_connector = export_op->parameter_connector_out();
        auto import_connector = import_op->parameter_connector_in();

        if (export_connector && import_connector)
        {
            auto key = std::pair(export_connector, import_connector);
            Q_ASSERT(parameter_cables.contains(key));

            parameter_cables[key].second -= 1;
            if (parameter_cables[key].second < 1)
            {
                removeItem(parameter_cables[key].first);
                delete parameter_cables[key].first;
                parameter_cables.remove(key);
            }
        }
    }
}
