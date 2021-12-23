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

#ifndef ERROR_CODES_H_
#define ERROR_CODES_H_

// === Includes ===

// === Define the error codes ===
namespace Error {
    enum class Code: int
    {
        None                        = 0,
        // === Internal Errors ===
        IPC_PID_Overflow            = -10,
        IPC_Multiple_Registrations  = -11,

        // === Driver Errors ===
        I2C_Timeout                 = -100,
        I2C_Address_Error           = -101,
        I2C_Data_ACK_Error          = -102,
        I2C_BUS_Busy_Error          = -103,
        SPI_Timeout                 = -110,
        SPI_BUS_Busy_Error          = -111
    };
};

#endif