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

#ifndef PROCESSORS_H_
#define PROCESSORS_H_

/* === Includes === */
#include "vendors.h"

/*
 * Check which processor is used and
 * include corresponding implementation
 * of the assembler functions.
 *
 * The vendor libraries included in vendors.h
 * usually have some defines to identify the
 * used processor. At the least the microcontrollers
 * OTOS supports do that.
 */
///@todo Add check for FPU here.

#if defined(__CORTEX_M) /* -> ARM Cortex M family */
    #if __CORTEX_M == 4
    /*-----------------------------
    * Processor:    ARM Cortex M4
    * Detail:       No FPU enabled!
    ------------------------------*/
    #include "arm/arm_cm4_nofpu.h"
#elif __CORTEX_M == 0
    /*-----------------------------
    * Processor:    ARM Cortex M0+
    * Detail:       No FPU enabled!
    ------------------------------*/
    #include "arm/arm_cm0plus_nofpu.h"
#else
    /* ARM core is not yet implemented -> throw error */
    #error "OTOS: ARM core not supported yet!"
#endif // __CORTEX_M

#else
    /* Processor is not yet implemented -> throw error */
    #error "OTOS: Processor not supported yet!"
#endif // __CORTEX_M

#endif // PROCESSORS_H_
