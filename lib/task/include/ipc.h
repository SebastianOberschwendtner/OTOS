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

#ifndef IPC_H_
#define IPC_H_

/* === Includes === */
#include "task.h"
#include <array>
#include <misc/error_codes.h>
#include <misc/types.h>
#include <optional>

/* === Needed defines === */
#ifndef IPC_MAX_PID
#define IPC_MAX_PID 5
#endif

namespace ipc
{
    /* === Checks === */
    namespace check
    {
        template <uint8_t id>
        constexpr uint8_t PID()
        {
            static_assert(id < IPC_MAX_PID, "Invalid PID! PID is greater than allocated space!");
            return id;
        };
    };

    /**
     * @class Manager
     * @brief The IPC manager class is used to register data
     * for inter process communication.
     */
    class Manager
    {
      public:
        /* === Constructors === */
        /**
         * @brief Constructor for IPC manager.
         * The IPC manager is only needed, when the task needs to register data.
         * You do not need a manager when you only want to get data.
         * @param PID The PID of the owner task, this has to be unique.
         */
        explicit Manager(const uint8_t PID) : Owner_PID(PID){};

        /* === Getters === */
        /**
         * @brief Get the pointer to already registered data.
         * @param PID The process ID of the data source.
         * @returns Returns the pointer to the data object, has to be casted
         * to the actual data type. The returned pointer is empty, when the
         * data is not yet available via ipc.
         */
        static auto get_data(uint8_t PID) -> std::optional<void *>;

        /* === Methods === */
        /**
         * @brief Register a data object within the IPC manager.
         * @param data_address The address of the data object.
         * @return Returns an errors code which represents success or
         * the actual error code.
         */
        auto register_data(void *data_address) -> error::Code;

        /**
         * @brief Deregister data from IPC.
         * Caution! Other tasks are not informed when data becomes invalid!
         * This function is mainly for unit testing.
         */
        void deregister_data();

      private:
        /* === Properties === */
        uint8_t Owner_PID;                                         /**< The process ID of the thread owning this manager. */
        static std::array<void *, IPC_MAX_PID> ipc_data_addresses; /**< Array containing register ipc data addresses. */
    };

    /* === Functions === */
    /**
     * @brief Template function to wait until the data of the PID is available
     * Calls `OTOS::TASK::yield()` until the data is available.
     *
     * @tparam T The IPC interface type/class.
     * @param PID The PID of the task that owns the data.
     * @return T* Returns the pointer to the data as soon as it is available.
     */
    template <typename T>
    auto wait_for_data(const uint8_t PID) -> T *
    {
        YIELD_WHILE(!ipc::Manager::get_data(PID));
        return static_cast<T *>(ipc::Manager::get_data(PID).value());
    }
};     // namespace ipc
#endif // IPC_H_