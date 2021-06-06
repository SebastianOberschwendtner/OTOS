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

#ifndef PROCESSORS_H_
#define PROCESSORS_H_

// Only include when not unit testing!
#ifdef UNIT_TEST

    // Include dummy assembler functions when unit testing
    #include "stubs.h"

#else
    /* 
    * Check which processor is used and 
    * include corresponding implementation
    * of the assembler functions.
    */
    ///@todo Add check for FPU here.
    #ifdef STM32F4

    /*-----------------------------
    * Processor:    ARM Cortex M4
    * Detail:       No FPU enabled!
    ------------------------------*/
    #include "arm_cm4_nofpu.h"

    #else
        // Processor is not yet implemented -> throw error
        #error "OTOS: Processor not supported yet!"
    #endif
#endif
#endif