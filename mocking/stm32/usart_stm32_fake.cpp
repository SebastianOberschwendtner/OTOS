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
 * @file    spi_stm32_fake.cpp
 * @author  SO
 * @version v2.1.0
 * @date    22-Dezember-2021
 * @brief   Fakes the spi interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "usart_stm32_fake.h"

// *** Fakes ***
// Fake Peripheral
static USART_TypeDef USART1_Fake;
static USART_TypeDef USART2_Fake;
static USART_TypeDef USART3_Fake;
static USART_TypeDef UART4_Fake;
static USART_TypeDef UART5_Fake;
static USART_TypeDef USART6_Fake;
static USART_TypeDef UART7_Fake;
static USART_TypeDef UART8_Fake;

// Public Pointer to fake which mimics peripheral behaviour.
USART_TypeDef *USART1 = &USART1_Fake;
USART_TypeDef *USART2 = &USART2_Fake;
USART_TypeDef *USART3 = &USART3_Fake;
USART_TypeDef *UART4 = &UART4_Fake;
USART_TypeDef *UART5 = &UART5_Fake;
USART_TypeDef *USART6 = &USART6_Fake;
USART_TypeDef *UART7 = &UART7_Fake;
USART_TypeDef *UART8 = &UART8_Fake;

// Fake register address
unsigned long USART1_BASE = reinterpret_cast<unsigned long>(USART1);
unsigned long USART2_BASE = reinterpret_cast<unsigned long>(USART2);
unsigned long USART3_BASE = reinterpret_cast<unsigned long>(USART3);
unsigned long UART4_BASE = reinterpret_cast<unsigned long>(UART4);
unsigned long UART5_BASE = reinterpret_cast<unsigned long>(UART5);
unsigned long USART6_BASE = reinterpret_cast<unsigned long>(USART6);
unsigned long UART7_BASE = reinterpret_cast<unsigned long>(UART7);
unsigned long UART8_BASE = reinterpret_cast<unsigned long>(UART8);

/**
 * @brief Constructor for the fake USART peripheral. Initializes the
 * register values to the default values as defined in the datasheet
 * of the STM32.
 * @details There should only be one instance of this object during testing.
 * This static instance is made available by the pointer to the object, which
 * fakes the peripheral during testing.
 */
USART_TypeDef::USART_TypeDef()
{
    // Reinit to default
    registers_to_default();
};

/**
 * @brief Reset all the registers to the default values
 */
void USART_TypeDef::registers_to_default(void)
{
    this->SR = 0;  
    this->DR = 0;  
    this->BRR = 0; 
    this->CR1 = 0; 
    this->CR2 = 0; 
    this->CR3 = 0; 
    this->GTPR = 0;
};