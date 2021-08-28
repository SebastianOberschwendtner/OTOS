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

// *** Mock the register addresses
#define GPIOA_BASE      0x0000
#define GPIOB_BASE      0x0001
#define GPIOC_BASE      0x0002
#define GPIOD_BASE      0x0003
#define GPIOE_BASE      0x0004
#define GPIOF_BASE      0x0005
#define GPIOG_BASE      0x0006
#define GPIOH_BASE      0x0007
#define GPIOI_BASE      0x0008
#define GPIOJ_BASE      0x0009
#define GPIOK_BASE      0x0010

// *** Mock the peripheral typedefs
/** 
  * @brief General Purpose I/O
  */

class GPIO_TypeDef: public Fake::Peripheral
{
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

GPIO_TypeDef GPIO_Fake;
GPIO_TypeDef* GPIOA = &GPIO_Fake;

#endif