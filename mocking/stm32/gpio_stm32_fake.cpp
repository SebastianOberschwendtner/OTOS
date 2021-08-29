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
 * @file    gpio_stm32_fake.cpp
 * @author  SO
 * @version v1.0.7
 * @date    26-August-2021
 * @brief   Fakes for the stm32 GPIO peripheral for unit testing.
 ==============================================================================
 */

// *** Includes ***
#include "gpio_stm32_fake.h"

// *** Fakes ***
// Peripheral objects
static GPIO_TypeDef GPIOA_Fake;
static GPIO_TypeDef GPIOB_Fake;
static GPIO_TypeDef GPIOC_Fake;
static GPIO_TypeDef GPIOD_Fake;
static GPIO_TypeDef GPIOE_Fake;
static GPIO_TypeDef GPIOF_Fake;
static GPIO_TypeDef GPIOG_Fake;
static GPIO_TypeDef GPIOH_Fake;
static GPIO_TypeDef GPIOI_Fake;
static GPIO_TypeDef GPIOJ_Fake;
static GPIO_TypeDef GPIOK_Fake;

// Pointer to peripherals
GPIO_TypeDef* GPIOA = &GPIOA_Fake;
GPIO_TypeDef* GPIOB = &GPIOB_Fake;
GPIO_TypeDef* GPIOC = &GPIOC_Fake;
GPIO_TypeDef* GPIOD = &GPIOD_Fake;
GPIO_TypeDef* GPIOE = &GPIOE_Fake;
GPIO_TypeDef* GPIOF = &GPIOF_Fake;
GPIO_TypeDef* GPIOG = &GPIOG_Fake;
GPIO_TypeDef* GPIOH = &GPIOH_Fake;
GPIO_TypeDef* GPIOI = &GPIOI_Fake;
GPIO_TypeDef* GPIOJ = &GPIOJ_Fake;
GPIO_TypeDef* GPIOK = &GPIOK_Fake;

// Base addresses to peripherals
unsigned long GPIOA_BASE = reinterpret_cast<unsigned long>(GPIOA);
unsigned long GPIOB_BASE = reinterpret_cast<unsigned long>(GPIOB);
unsigned long GPIOC_BASE = reinterpret_cast<unsigned long>(GPIOC);
unsigned long GPIOD_BASE = reinterpret_cast<unsigned long>(GPIOD);
unsigned long GPIOE_BASE = reinterpret_cast<unsigned long>(GPIOE);
unsigned long GPIOF_BASE = reinterpret_cast<unsigned long>(GPIOF);
unsigned long GPIOG_BASE = reinterpret_cast<unsigned long>(GPIOG);
unsigned long GPIOH_BASE = reinterpret_cast<unsigned long>(GPIOH);
unsigned long GPIOI_BASE = reinterpret_cast<unsigned long>(GPIOI);
unsigned long GPIOJ_BASE = reinterpret_cast<unsigned long>(GPIOJ);
unsigned long GPIOK_BASE = reinterpret_cast<unsigned long>(GPIOK);

// *** Methods ***

/**
 * @brief Constructor for port object which initializes
 * the port to default values.
 */
GPIO_TypeDef::GPIO_TypeDef()
{
    // Reinit to default values
    registers_to_default();
};

/**
 * @brief Reset all the registers to the default values
 */
void GPIO_TypeDef::registers_to_default(void)
{
    this->MODER     = 0x00;
    this->OTYPER    = 0x00;
    this->OSPEEDR   = 0x00;
    this->PUPDR     = 0x00;
    this->IDR       = 0x00;
    this->ODR       = 0x00;
    this->BSRR      = 0x00;
    this->LCKR      = 0x00;
    this->AFR[0]    = 0x00;
    this->AFR[1]    = 0x00;
};