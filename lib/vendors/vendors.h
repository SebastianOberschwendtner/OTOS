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

#ifndef VENDORS_H_
#define VENDORS_H_
    /* 
    * Check which microcontroller is used and 
    * include the corresponding implementation
    * of the vendors specific peripheral libraries.
    */

    #ifdef STM32F4
    /*-----------------------------
    * Controller:   STM32F4xx
    * Processor:    ARM Cortex M4
    * Vendor:       ST Microelectronics
    ------------------------------*/
    #include "stm32f4xx.h"

    #else
        // Microcontroller is not yet implemented -> throw error
        #error "OTOS: Microcontroller not supported yet!"
    #endif

#endif
