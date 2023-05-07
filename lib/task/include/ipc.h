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

#ifndef IPC_H_
#define IPC_H_

// === Includes ===
#include <array>
#include <optional>
#include "task.h"
#include "types.h"
#include "error_codes.h"

// === Needed defines ===
#ifndef IPC_MAX_PID
#define IPC_MAX_PID 5
#endif

// === Declarations ===
namespace IPC {
    // === Checks ===
    namespace Check {
        template<unsigned char id>
        constexpr unsigned char PID() {
            static_assert(id < IPC_MAX_PID, "Invalid PID! PID is greater than allocated space!");
            return id;
        };
    };

    /// === Classes ===

    class Manager
    {
    private:
        // *** Properties ***
        unsigned char Owner_PID;
        static std::array<void*, IPC_MAX_PID> ipc_data_addresses; // NOLINT

    public:

        /**
         * @brief Constructor for IPC manager.
         * The IPC manager is only needed, when the task needs to register data.
         * You do not need a manager when you only want to get data.
         * @param PID The PID of the owner task, this has to be unique.
         */
        explicit Manager(const unsigned char PID): Owner_PID(PID) {};

        // *** Methods ***
        Error::Code                 register_data           (void* data_address);
        void                        deregister_data         ();
        static std::optional<void*> get_data                (unsigned char PID);
    };

    /**
     * @brief Template function to wait until the data of the PID is available
     * Calls `OTOS::TASK::yield()` until the data is available.
     * 
     * @tparam T The IPC interface type/class.
     * @param PID The PID of the task that owns the data.
     * @return T* Returns the pointer to the data as soon as it is available.
     */
    template<typename T>
    auto wait_for_data(const uint8_t PID) -> T*
    {
        YIELD_WHILE(!IPC::Manager::get_data(PID));
        return static_cast<T*>(IPC::Manager::get_data(PID).value());
    }
}; // namespace IPC
#endif