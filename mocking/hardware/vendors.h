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

#ifndef VENDORS_H_
#define VENDORS_H_

    // *** Includes which mock vendor specific peripherals
    // ---------- STM32 -------------
    #define STM32F4 // => The library fakes the STM32F4 microcontroller family
    // === Clock defines ===
    #define F_CPU 4'000'000
    #define F_APB1 2'000'000
    #define F_APB2 4'000'000

    #define F_I2C F_APB1    

    // === Mocked CMSIS Driver ===
    #include "../cmsis/cmsis_mock.h"

    // === Fake peripherals ===
    #include "../stm32/clock_stm32_fake.h"
    #include "../stm32/gpio_stm32_fake.h"
    #include "../stm32/i2c_stm32_fake.h"
    #include "../stm32/spi_stm32_fake.h"
    #include "../stm32/sdio_stm32_fake.h"
    #include "../stm32/exti_stm32_fake.h"
    #include "../stm32/timer_stm32_fake.h"
    #include "../stm32/usart_stm32_fake.h"

#endif

