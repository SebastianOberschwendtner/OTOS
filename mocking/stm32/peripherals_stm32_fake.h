/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
#ifndef PERIPHERALS_STM32_H_
#define PERIPHERALS_STM32_H_

/* === Includes === */

namespace stm32
{
/* === Enum defining the available peripherals of each device family */
    enum class Peripheral : unsigned char
    {
        SYSTEM_ = 0,
        TIM_1,
        TIM_2,
        TIM_3,
        TIM_4,
        TIM_5,
        TIM_6,
        TIM_7,
        TIM_8,
        TIM_9,
        TIM_10,
        TIM_11,
        TIM_12,
        TIM_13,
        TIM_14,
        I2C_1,
        I2C_2,
        I2C_3,
        SPI_1,
        SPI_2,
        SPI_3,
        SPI_4,
        SPI_5,
        SPI_6,
        USART_1,
        USART_2,
        USART_3,
        USART_4,
        USART_5,
        USART_6,
        USART_7,
        USART_8,
        CAN_1,
        CAN_2,
        OTG_FS_,
        OTG_HS_,
        ETH_,
        FSMC_,
        SDIO_,
        DCMI_,
        EVENTOUT_
    };
};     // namespace stm32
#endif // PERIPHERALS_STM32_H_