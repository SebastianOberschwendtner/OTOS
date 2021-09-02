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
 ==============================================================================
 * @file    i2c_stm32_fake.cpp
 * @author  SO
 * @version v1.0.7
 * @date    02-September-2021
 * @brief   Fakes the i2c interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "i2c_stm32_fake.h"

// *** Fakes ***
static I2C_TypeDef I2C1_Fake;    // Fake Peripheral
I2C_TypeDef* I2C1 = &I2C1_Fake;   // Public Pointer to fake which mimics peripheral behaviour.
unsigned long I2C1_BASE = reinterpret_cast<unsigned long>(I2C1); // Fake register address

/**
 * @brief Constructor for the fake I2C peripheral. Initializes the
 * register values to the default values as defined in the datasheet
 * of the STM32.
 * @details There should only be one instance of this object during testing.
 * This static instance is made available by the pointer to the object, which
 * fakes the peripheral during testing.
 */
I2C_TypeDef::I2C_TypeDef()
{
    // Reinit to default
    registers_to_default();
};

/**
 * @brief Reset all the registers to the default values
 */
void I2C_TypeDef::registers_to_default(void)
{
    this->CR1       = 0x83;
};