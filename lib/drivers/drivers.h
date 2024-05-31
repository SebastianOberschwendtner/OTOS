/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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

/**=========================================================================
 * This file includes all the available device drivers for the supported
 * microcontrollers. Each driver includes 'vendors.h' which includes the
 * device specific library files provided by PlatformIO.
 * 
 * => New drivers have to be included in this header file.
 *
 *==========================================================================*/

#ifndef DRIVERS_H_
#define DRIVERS_H_

/* === Includes === */
/// @todo Check for the microcontroller type (STM32/AVR/...) before including the drivers. */
#include "stm32/core_stm32.h"
#include "stm32/dma_stm32.h"
#include "stm32/gpio_stm32.h"
#include "stm32/i2c_stm32.h"
#include "stm32/spi_stm32.h"
#include "stm32/sdio_stm32.h"
#include "stm32/timer_stm32.h"
#include "stm32/usart_stm32.h"

/* === Chip driver === */
/* BMS chips */
#include "battery/bq25700.h"
#include "battery/tps65987.h"
#include "battery/max17205.h"

/* Display driver chips */
#include "display/ssd1306.h"
#include "display/ili9341.h"
#include "display/uc1611.h"
#include "display/gc9a01a.h"

#endif
