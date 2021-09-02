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

#ifndef I2C_STM32_FAKE_H_
#define I2C_STM32_FAKE_H_

// *** Includes ***
#include "../base/fake.h"

// *** Class defines for fake peripherals

/** 
  * @brief Reset and Clock Control
  */
class I2C_TypeDef: public Fake::Peripheral
{
private:

public:
  // Constructor which initializes peripheral with default values
  I2C_TypeDef();

  // Methods for unit testing
  void registers_to_default (void) override;

  // Fake Peripheral registers
  Fake::Register_t CR1;        /*!< I2C Control register 1,     Address offset: 0x00 */
  Fake::Register_t CR2;        /*!< I2C Control register 2,     Address offset: 0x04 */
  Fake::Register_t OAR1;       /*!< I2C Own address register 1, Address offset: 0x08 */
  Fake::Register_t OAR2;       /*!< I2C Own address register 2, Address offset: 0x0C */
  Fake::Register_t DR;         /*!< I2C Data register,          Address offset: 0x10 */
  Fake::Register_t SR1;        /*!< I2C Status register 1,      Address offset: 0x14 */
  Fake::Register_t SR2;        /*!< I2C Status register 2,      Address offset: 0x18 */
  Fake::Register_t CCR;        /*!< I2C Clock control register, Address offset: 0x1C */
  Fake::Register_t TRISE;      /*!< I2C TRISE register,         Address offset: 0x20 */
  Fake::Register_t FLTR;       /*!< I2C FLTR register,          Address offset: 0x24 */
};

// *** Public references to fake peripherals
// Public pointer to RCC peripheral which is faked
extern I2C_TypeDef* I2C1;
extern unsigned long I2C1_BASE;

#endif