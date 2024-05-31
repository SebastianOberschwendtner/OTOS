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

#ifndef ERROR_CODES_H_
#define ERROR_CODES_H_

/* === Includes === */
#include "types.h"

/* === Define the error codes === */
namespace error {
    enum class Code: int16_t
    {
        None                        = 0,
        /* === Internal Errors === */
        IPC_PID_Overflow            = -10,
        IPC_Multiple_Registrations  = -11,

        /* === File System errors === */
        File_System_Invalid         = -90,
        End_of_File_Reached         = -91,
        Bad_Sector                  = -93,
        FAT_Corrupted               = -95,
        File_ID_not_found           = -96,
        Not_a_Directory             = -97,
        Not_a_File                  = -98,
        No_Memory_Left              = -99,

        /* === Driver Errors === */
        I2C_Timeout                 = -100,
        I2C_Address_Error           = -101,
        I2C_Data_ACK_Error          = -102,
        I2C_BUS_Busy_Error          = -103,
        SPI_Timeout                 = -110,
        SPI_BUS_Busy_Error          = -111,
        USART_Timeout               = -120,
        USART_BUS_Busy_Error        = -121,
        SDIO_Timeout                = -130,
        SDIO_BUS_Busy_Error         = -131
    };
}; // namespace error

#endif // ERROR_CODES_H_
