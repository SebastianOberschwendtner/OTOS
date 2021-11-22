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
/**
 ******************************************************************************
 * @file    test_stm32.cpp
 * @author  SO
 * @version v2.0.0
 * @date    22-November-2021
 * @brief   Unit tests for testing the interface to STM32 peripherals.
 ******************************************************************************
 */

// ****** Includes ******
#include "unity.h"
#include "mock.h"
// *** Tests ***
#include "test_gpio_stm32.h"
#include "test_i2c_stm32.h"
#include "test_timer_stm32.h"


// === Main ===
int main(int argc, char** argv)
{
    test_gpio_stm32();
    test_i2c_stm32();
    test_timer_stm32();
    return 0;
};

