/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 -2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
     * Macros to check for supported Microcontrollers
     */
    // Supported STM32F4 Devices
    #define IS_STM32F4 defined(STM32F429xx) || defined(STM32F405xx)
    // Supported STM32L0 Devices
    #define IS_STM32L0 defined(STM32L071xx) || defined(STM32L072xx) || defined(STM32L073xx) || defined(STM32L053xx)

    /* 
    * Check which microcontroller is used and 
    * include the corresponding implementation
    * of the vendors specific peripheral libraries.
    */

    #if IS_STM32F4
    /*-----------------------------
    * Controller:   STM32F4xx
    * Processor:    ARM Cortex M4
    * Vendor:       ST Microelectronics
    ------------------------------*/
    #include "stm32f4xx.h"

    
    #elif IS_STM32L0
    /*-----------------------------
    * Controller:   STM32L0xx
    * Processor:    ARM Cortex M0+
    * Vendor:       ST Microelectronics
    ------------------------------*/
    #include "stm32l0xx.h"


    #else
        // Microcontroller is not yet implemented -> throw error
        #error "OTOS: Microcontroller not supported yet!"
    #endif

#endif
