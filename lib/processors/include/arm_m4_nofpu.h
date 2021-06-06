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

#ifndef ARM_M4_NOFPU_H_
#define ARM_M4_NOFPU_H_

// *** Includes ***

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// *** Function Declarations ***
unsigned int*   __otos_switch       (unsigned int* ThreadStack);
void            __otos_yield        (void);
void            __otos_call_kernel  (void);
void            __otos_init_kernel  (unsigned int* ThreadStack);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif