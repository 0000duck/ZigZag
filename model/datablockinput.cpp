#include "datablockinput.h"

#include "model/basedatablock.h"
#include "model/programmodel.h"
#include "command/connectcommand.h"
#include "command/disconnectcommand.h"


DataBlockInput::DataBlockInput(const char * data_block_type)
    : type_name(data_block_type)
{

}


DataBlockInput::~DataBlockInput()
{

}


std::vector<BaseParameter*> DataBlockInput::get_parameters()
{
    return { };
}


void DataBlockInput::refresh_parameters()
{
    emit parameters_modified();
}


bool DataBlockInput::compatible_with(const BaseDataBlock* data_block) const
{
    if (data_block)
    {
        return strcmp(type_name, data_block->type_name) == 0;
    }
    return false;
}


void DataBlockInput::connect_to(BaseDataBlock* new_connection)
{
    if (new_connection != connection)
    {
        if (new_connection == nullptr)
        {
            disconnect(); // Setting connection to nullptr is same as disconnecting.
        }
        else
        {
            get_main_model()->get_undo_stack()->push(new ConnectCommand(new_connection, this));
        }
    }
}


void DataBlockInput::disconnect()
{
    if (connection)
    {
        get_main_model()->get_undo_stack()->push(new DisconnectCommand(connection, this));
    }
}


bool DataBlockInput::set_connection(BaseDataBlock *new_connection)
{
    if (new_connection == nullptr || compatible_with(new_connection))
    {
        /*
         * If the new connection between the data block, and this data input
         * is not compatible, we rather not change anything, than just disconnect.
         */

        if (new_connection != connection)
        {
            /*
             * Scenario's:
             * - there was a connection, still a connection after
             * - there was not a connection, connection after
             * - there was a connection, nullptr after
             */

            if (connection)
            {
                auto old_connection = connection;
                connection = nullptr;
                emit disconnected_from(old_connection);
            }

            Q_ASSERT(connection == nullptr); // Just for good measure.

            if (new_connection)
            {
                connection = new_connection;
                emit connected_to(new_connection);
            }

            return true;
        }
    }
    return false;
}


bool DataBlockInput::is_connected() const
{
    return static_cast<bool>(connection);
}


bool DataBlockInput::is_connected_to(const BaseDataBlock* data_block) const
{
    return connection == data_block;
}


const BaseDataBlock* DataBlockInput::get_connection() const
{
    return connection;
}
