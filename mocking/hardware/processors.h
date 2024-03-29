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

#ifndef MOCK_PROCESSORS_H
#define MOCK_PROCESSORS_H

// *** Includes ***
#include "mock.h"
#include <cstdint>

// *** Function Declarations ***
std::uintptr_t*  __otos_switch       (std::uintptr_t* ThreadStack);
void            __otos_yield        (void);
void            __otos_call_kernel  (void);
void            __otos_init_kernel  (std::uintptr_t* ThreadStack);

#endif