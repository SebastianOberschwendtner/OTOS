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

#ifndef EXTI_STM32_FAKE_H_
#define EXTI_STM32_FAKE_H_

// *** Includes ***
#include "../base/fake.h"

// *** The peripheral typedefs
/** 
  * @brief External Interrupt/Event Controller
  */

 class EXTI_TypeDef: public Fake::Peripheral
{
public:
  // Constructor to mimic device initialization
  EXTI_TypeDef();

  // Methods for unit testing
  void registers_to_default(void) override;

  // Fake registers of peripheral
  Fake::Register_t IMR;    /*!< EXTI Interrupt mask register,            Address offset: 0x00 */
  Fake::Register_t EMR;    /*!< EXTI Event mask register,                Address offset: 0x04 */
  Fake::Register_t RTSR;   /*!< EXTI Rising trigger selection register,  Address offset: 0x08 */
  Fake::Register_t FTSR;   /*!< EXTI Falling trigger selection register, Address offset: 0x0C */
  Fake::Register_t SWIER;  /*!< EXTI Software interrupt event register,  Address offset: 0x10 */
  Fake::Register_t PR;     /*!< EXTI Pending register,                   Address offset: 0x14 */
};

// *** Public references to fake peripherals
// Fake peripheral pointers
extern EXTI_TypeDef* EXTI;

// Fake addresses
extern unsigned long EXTI_BASE;

#endif
