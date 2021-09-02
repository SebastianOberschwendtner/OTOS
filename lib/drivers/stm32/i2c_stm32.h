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

#ifndef I2C_STM32_H_
#define I2C_STM32_H_

// *** Includes ***
#include "vendors.h"

// *** Declarations ***
namespace I2C {
    // *** Enums ***
    // enum Instance: unsigned char
    // {
    //     FIRST = 0, SECOND, THIRD
    // };

    // *** Classes ***
    class Controller
    {
    private:

    public:
        // Constructor
        Controller(char instance, unsigned long frequency);
    };

}
#endif