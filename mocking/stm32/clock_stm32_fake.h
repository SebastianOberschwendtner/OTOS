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

#ifndef CLOCK_STM32_FAKE_H_
#define CLOCK_STM32_FAKE_H_

// *** Includes ***
#include "../base/fake.h"

// *** Class defines for fake peripherals

/** 
  * @brief Reset and Clock Control
  */
class RCC_TypeDef: public Fake::Peripheral
{
private:

public:
  // Constructor which initializes peripheral with default values
  RCC_TypeDef();

  // Methods for unit testing
  void registers_to_default (void) override;

  // Fake Peripheral registers
  Fake::Register_t CR;            /*!< RCC clock control register,                                  Address offset: 0x00 */
  Fake::Register_t PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
  Fake::Register_t CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
  Fake::Register_t CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
  Fake::Register_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
  Fake::Register_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
  Fake::Register_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
  Fake::Register_t RESERVED0;     /*!< Reserved, 0x1C                                                                    */
  Fake::Register_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
  Fake::Register_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
  Fake::Register_t RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
  Fake::Register_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  Fake::Register_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
  Fake::Register_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
  Fake::Register_t RESERVED2;     /*!< Reserved, 0x3C                                                                    */
  Fake::Register_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
  Fake::Register_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
  Fake::Register_t RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
  Fake::Register_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
  Fake::Register_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
  Fake::Register_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
  Fake::Register_t RESERVED4;     /*!< Reserved, 0x5C                                                                    */
  Fake::Register_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
  Fake::Register_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
  Fake::Register_t RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
  Fake::Register_t BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
  Fake::Register_t CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
  Fake::Register_t RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
  Fake::Register_t SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
  Fake::Register_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
  Fake::Register_t PLLSAICFGR;    /*!< RCC PLLSAI configuration register,                           Address offset: 0x88 */
  Fake::Register_t DCKCFGR;       /*!< RCC Dedicated Clocks configuration register,                 Address offset: 0x8C */
};

// *** Public references to fake peripherals
// Public pointer to RCC peripheral which is faked
extern RCC_TypeDef* RCC;
extern unsigned long RCC_BASE;

#endif