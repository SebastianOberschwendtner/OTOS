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
 * @attention
 * All the peripheral type definition are taken from the peripheral library
 * files provided by ST.
 * 
 * Copyright:
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 */

#ifndef GPIO_STM32_FAKE_H_
#define GPIO_STM32_FAKE_H_

// *** Includes ***
#include "../base/fake.h"
#include <cstdint>

// *** The peripheral typedefs
/** 
  * @brief General Purpose I/O
  */

class GPIO_TypeDef: public Fake::Peripheral
{
public:
  // Constructor to mimic device initialization
  GPIO_TypeDef();

  // Methods for unit testing
  void registers_to_default(void) override;

  // Fake registers of port
  Fake::Register_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  Fake::Register_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
  Fake::Register_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
  Fake::Register_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  Fake::Register_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
  Fake::Register_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
  Fake::Register_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
  Fake::Register_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  Fake::Register_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
};

// *** Public references to fake peripherals
// Fake peripheral pointers
extern GPIO_TypeDef* GPIOA;
extern GPIO_TypeDef* GPIOB;
extern GPIO_TypeDef* GPIOC;
extern GPIO_TypeDef* GPIOD;
extern GPIO_TypeDef* GPIOE;
extern GPIO_TypeDef* GPIOF;
extern GPIO_TypeDef* GPIOG;
extern GPIO_TypeDef* GPIOH;
extern GPIO_TypeDef* GPIOI;
extern GPIO_TypeDef* GPIOJ;
extern GPIO_TypeDef* GPIOK;

// Fake addresses
extern std::uintptr_t GPIOA_BASE;
extern std::uintptr_t GPIOB_BASE;
extern std::uintptr_t GPIOC_BASE;
extern std::uintptr_t GPIOD_BASE;
extern std::uintptr_t GPIOE_BASE;
extern std::uintptr_t GPIOF_BASE;
extern std::uintptr_t GPIOG_BASE;
extern std::uintptr_t GPIOH_BASE;
extern std::uintptr_t GPIOI_BASE;
extern std::uintptr_t GPIOJ_BASE;
extern std::uintptr_t GPIOK_BASE;

#endif