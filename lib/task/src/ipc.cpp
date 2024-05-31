/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 ==============================================================================
 * @file    ipc.cpp
 * @author  SO
 * @version v5.0.0
 * @date    16-September-2021
 * @brief   Inter process communication for OTOS.
 ==============================================================================
 */

/* === Includes === */
#include "ipc.h"

namespace ipc
{
    /* === Static Variables === */
    std::array<void *, IPC_MAX_PID> Manager::ipc_data_addresses = {0};

    /* === Methods === */
    auto Manager::get_data(const uint8_t PID) -> std::optional<void *>
    {
        /* Check whether the PID is already available */
        if (Manager::ipc_data_addresses[PID] != 0)
            return Manager::ipc_data_addresses[PID];

        /* Data not available */
        return {};
    };

    auto Manager::register_data(void *const data_address) -> error::Code
    {
        /* Check whether PID is already set */
        if (Manager::ipc_data_addresses[this->Owner_PID] != 0)
            return error::Code::IPC_Multiple_Registrations;

        /* PID can be registered */
        Manager::ipc_data_addresses[this->Owner_PID] = data_address;
        return error::Code::None;
    };

    void Manager::deregister_data(void)
    {
        Manager::ipc_data_addresses[this->Owner_PID] = 0;
    };
}; // namespace ipc
