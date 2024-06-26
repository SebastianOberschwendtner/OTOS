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

#ifndef TYPES_H_
#define TYPES_H_

/* === Includes === */
#include <cstdint>
#include <vendors.h>

/* === Typedefs === */
// Function pointer for thread task
typedef void (*taskpointer_t)(void);

/*
 * Define of fundamental byte size for the OS
 * This enables the OS to be used in 8bit and
 * 32bit systems, hopefully.
 */
#if (IS_STM32F4 | IS_STM32L0)
// For ARM 32-bit microcontrollers, base is 16-bit or int.
typedef int32_t s_base_t;  // 32-bit signed base type
typedef uint32_t u_base_t; // 32-bit unsigned base type

// Define a type for the stack pointers
typedef uint32_t *stackpointer_t;

#elif defined(AVR)
// For AVR 8-bit microcontrollers, base is 8-bit or char.
typedef int8_t s_base_t;  // 8-bit signed base type
typedef uint8_t u_base_t; // 8-bit unsigned base type

// Define a type for the stack pointers
///@todo What is the base type of a function pointer of 8-bit micros? Also 16-bit?
typedef uint32_t *stackpointer_t;

#else
// for unit testing in the native environment
typedef std::intptr_t s_base_t;  // native signed base type
typedef std::uintptr_t u_base_t; // native unsigned base type

// Define a type for the stack pointers
typedef std::uintptr_t *stackpointer_t;
#endif // IS_STM32F4

#endif // TYPES_H_
