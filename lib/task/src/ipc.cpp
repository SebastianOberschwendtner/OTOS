/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
/**
 ******************************************************************************
 * @file    ipc.cpp
 * @author  SO
 * @version v1.0.12
 * @date    16-September-2021
 * @brief   Inter process communication for OTOS.
 ******************************************************************************
 */

// === Includes ===
#include "ipc.h"

// === Static Variables ===
std::array<void*, IPC_MAX_PID> IPC::Manager::ipc_data_addresses = {0};

// === Functions ===

/**
 * @brief Register a data object within the IPC manager.
 * @param data_address The address of the data object.
 * @return Returns an errors code which represents success or
 * the actual error code.
 */
Error::Code IPC::Manager::register_data(void* const data_address)
{
    // Check whether PID is already set
    if (IPC::Manager::ipc_data_addresses[this->Owner_PID] != 0)
        return Error::Code::IPC_Multiple_Registrations;
    
    // PID can be registered
    IPC::Manager::ipc_data_addresses[this->Owner_PID] = data_address;
    return Error::Code::None;
};

/**
 * @brief Deregister data from IPC.
 * Caution! Other tasks are not informed when data becomes invalid!
 * This function is mainly for unit testing.
 */
void IPC::Manager::deregister_data(void)
{
    IPC::Manager::ipc_data_addresses[this->Owner_PID] = 0;
};

/**
 * @brief Get the pointer to already registered data.
 * @param PID The process ID of the data source.
 * @returns Returns the pointer to the data object, has to be casted
 * to the actual data type. The returned pointer is empty, when the
 * data is not yet available via ipc.
 */
std::optional<void*> IPC::Manager::get_data(const unsigned char PID)
{
    // Check whether the PID is alread available
    if (IPC::Manager::ipc_data_addresses[PID] != 0)
        return IPC::Manager::ipc_data_addresses[PID];
    
    // Data not available
    return { };
};
