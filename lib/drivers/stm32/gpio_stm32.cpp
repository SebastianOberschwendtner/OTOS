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
/**
 ==============================================================================
 * @file    gpio_stm32.cpp
 * @author  SO
 * @version v4.2.0
 * @date    25-August-2021
 * @brief   GPIO driver for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "gpio_stm32.h"

// *** Functions ***

/**
 * @brief Return the enable bit position for the RCC register
 * for the given GPIO port.
 * @param Port The identifier of the port.
 * @return Return the bit position of the enable bit.
 */
constexpr auto get_RCCEN_position(const GPIO::Port Port) -> unsigned char
{
    // Return the bit position according to the used port
    switch (Port)
    {
        case GPIO::Port::A:
            return 0;
        case GPIO::Port::B:
            return 1;
        case GPIO::Port::C:
            return 2;
        case GPIO::Port::D:
            return 3;
        case GPIO::Port::E:
            return 4;
        case GPIO::Port::H:
            return 7;
#ifdef STM32F4
        case GPIO::Port::F:
            return 5;
        case GPIO::Port::G:
            return 6;
        case GPIO::Port::I:
            return 8;
#ifdef STM32F429xx
        case GPIO::Port::J:
            return 9;
        case GPIO::Port::K:
            return 10;
#endif
#endif
        default:
            return 32;
    }
};

/**
 * @brief Return the peripheral address for the desired port.
 * @param Port The identifier of the port.
 * @return Returns the address of the port as an integer.
 */
constexpr auto get_port_address(const GPIO::Port Port) -> std::uintptr_t
{
    switch (Port)
    {
        case GPIO::Port::A:
            return GPIOA_BASE;

        case GPIO::Port::B:
            return GPIOB_BASE;

        case GPIO::Port::C:
            return GPIOC_BASE;

        case GPIO::Port::D:
            return GPIOD_BASE;
#ifndef STM32L053xx
        case GPIO::Port::E:
            return GPIOE_BASE;
#endif
        case GPIO::Port::H:
            return GPIOH_BASE;

#ifdef STM32F4
        case GPIO::Port::F:
            return GPIOF_BASE;

        case GPIO::Port::G:
            return GPIOG_BASE;

        case GPIO::Port::I:
            return GPIOI_BASE;

#ifdef STM32F429xx
        case GPIO::Port::J:
            return GPIOJ_BASE;

        case GPIO::Port::K:
            return GPIOK_BASE;
#endif
#endif
        default:
            return 0;
    }
};

auto GPIO::PIN::get_af_code(const IO function) const -> unsigned char
{
    switch (function)
    {
#if defined(STM32F4)
        case IO::SYSTEM_:
            return 0;
        case IO::TIM_1:
        case IO::TIM_2:
            return 1;
        case IO::TIM_3:
        case IO::TIM_4:
        case IO::TIM_5:
            return 2;
        case IO::TIM_8:
        case IO::TIM_9:
        case IO::TIM_10:
        case IO::TIM_11:
            return 3;
        case IO::I2C_1:
        case IO::I2C_2:
        case IO::I2C_3:
            return 4;
        case IO::SPI_1:
        case IO::SPI_2:
        case IO::SPI_4:
        case IO::SPI_5:
            return 5;
        case IO::SPI_3:
            return 6;
        case IO::USART_1:
        case IO::USART_2:
        case IO::USART_3:
            return 7;
        case IO::CAN_1:
        case IO::CAN_2:
        case IO::TIM_12:
        case IO::TIM_13:
        case IO::TIM_14:
            return 9;
        case IO::OTG_FS_:
        case IO::OTG_HS_:
            return 10;
        case IO::ETH_:
            return 11;
        case IO::FSMC_:
        case IO::SDIO_:
            return 12;
        case IO::DCMI_:
            return 13;
        case IO::EVENTOUT_:
            return 15;
#elif defined(STM32L0)
        case IO::I2C_1:
            // The L0 devices assign different AF functions depending on the pin number
            if (this->thisPort == GPIOA)
                return 6;
            if (this->thisPort == GPIOB)
            {
                if (this->thisPin < 8)
                    return 1;
                else
                    return 4;
            }
            return 0; // This pin cannot be assigned to I2C1

        case IO::I2C_2:
            if (this->thisPort == GPIOB)
            {
                if (this->thisPin < 12)
                    return 6;
                else
                    return 5;
            }
            return 0; // This pin cannot be assigned to I2C2

        case IO::I2C_3:
            return 7;
#endif
        default:
            return 0;
    }
};

// === Constructors ===
GPIO::PIN::PIN(const Port Port, const unsigned char Pin)
    : thisPort{reinterpret_cast<volatile GPIO_TypeDef *>(get_port_address(Port))}, thisPin{Pin}, PortID{Port}
{
    // static_assert(pin_number_valid<1>());
    // enable the clock for this gpio port
#if defined(STM32F4)
    RCC->AHB1ENR |= (1 << get_RCCEN_position(Port));
#elif defined(STM32L0)
    RCC->IOPENR |= (1 << get_RCCEN_position(Port));
#endif
};

GPIO::PIN::PIN(const Port Port, const unsigned char Pin, const GPIO::Mode PinMode)
    : PIN{Port, Pin}
{
    this->set_mode(PinMode);
};

// === Methods ===
void GPIO::PIN::set_alternate_function(const IO function)
{
    // Assign the alternate function
    this->set_alternate_function(this->get_af_code(function));

    // Set AF specific options
    if ((function == IO::I2C_1) | (function == IO::I2C_2) | (function == IO::I2C_3))
        this->set_output_type(Output::Open_Drain);
};

void GPIO::PIN::set_alternate_function(const unsigned char af_code)
{
    // Set AF mode
    this->set_mode(Mode::AF_Mode);

    // Assign the alternate function
    if (this->thisPin < 8)
    {
        // Save old register state and delete the part which will change
        uint32_t _Reg = this->thisPort->AFR[0] & ~(0b1111 << (4 * this->thisPin));
        // Get the code for teh alternate function and set the register
        this->thisPort->AFR[0] = _Reg | (af_code << (4 * this->thisPin));
    }
    else
    {
        // Save old register state and delete the part which will change
        uint32_t _Reg = this->thisPort->AFR[1] & ~(0b1111 << (4 * (this->thisPin - 8)));
        // Get the code for teh alternate function and set the register
        this->thisPort->AFR[1] = _Reg | (af_code << (4 * (this->thisPin - 8)));
    }
};

void GPIO::PIN::set_high(void)
{
    // Set the BS bit
    thisPort->BSRR = (1 << this->thisPin);
};

void GPIO::PIN::set_low(void)
{
    // Set the BR bit
    thisPort->BSRR = (1 << (this->thisPin + 16));
};

void GPIO::PIN::set_mode(const GPIO::Mode NewMode)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->MODER & ~(0b11 << (2 * this->thisPin));

    // Combine the old and the new data and write the register
    this->thisPort->MODER = _Reg | (static_cast<unsigned char>(NewMode) << (2 * this->thisPin));
};

void GPIO::PIN::set_output_type(const GPIO::Output NewType)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->OTYPER & ~(1 << this->thisPin);

    // Combine the old and the new data and write the register
    this->thisPort->OTYPER = _Reg | (static_cast<unsigned char>(NewType) << this->thisPin);
};

void GPIO::PIN::set_pull(const GPIO::Pull NewPull)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->PUPDR & ~(0b11 << (2 * this->thisPin));

    // Combine the old and the new data and write the register
    this->thisPort->PUPDR = _Reg | (static_cast<unsigned char>(NewPull) << (2 * this->thisPin));
};

void GPIO::PIN::set_speed(const GPIO::Speed NewSpeed)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->OSPEEDR & ~(0b11 << (2 * this->thisPin));

    // Combine the old and the new data and write the register
    this->thisPort->OSPEEDR = _Reg | (static_cast<unsigned char>(NewSpeed) << (2 * this->thisPin));
};

void GPIO::PIN::set_state(const bool NewState)
{
    // Call the setHigh or setLow function according to NewState
    if (NewState)
        this->set_high();
    else
        this->set_low();
};

void GPIO::PIN::toggle(void)
{
    thisPort->ODR ^= (1 << this->thisPin);
};

auto GPIO::PIN::get_state(void) const -> bool
{
    return (this->thisPort->IDR & (1 << this->thisPin));
};

auto GPIO::PIN::falling_edge(void) const -> bool
{
    return this->edge_falling;
};

auto GPIO::PIN::rising_edge(void) const -> bool
{
    return this->edge_rising;
};

auto GPIO::PIN::enable_interrupt(const Edge NewEdge) const -> bool
{
    // Enable the EXTI line
    const unsigned int bitmask = (1 << this->thisPin);
    EXTI->IMR |= bitmask;
    switch (NewEdge)
    {
        case Edge::Rising:
            EXTI->RTSR |= bitmask;
            break;
        case Edge::Falling:
            EXTI->FTSR |= bitmask;
            break;
        case Edge::Both:
            EXTI->RTSR |= bitmask;
            EXTI->FTSR |= bitmask;
            break;
        default:
            return false;
    }

    // Configure the pin of the EXTI line in the
    // System configuration
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[this->thisPin / 4] |= (static_cast<unsigned char>(this->PortID) << 4 * (this->thisPin % 4));

    // Enable the EXTI line in the NVIC
    IRQn_Type ThisIRQn;
#if defined(STM32F4)
    switch (this->thisPin)
    {
        case 0:
            ThisIRQn = EXTI0_IRQn;
            break;
        case 1:
            ThisIRQn = EXTI1_IRQn;
            break;
        case 2:
            ThisIRQn = EXTI2_IRQn;
            break;
        case 3:
            ThisIRQn = EXTI3_IRQn;
            break;
        case 4:
            ThisIRQn = EXTI4_IRQn;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            ThisIRQn = EXTI9_5_IRQn;
            break;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            ThisIRQn = EXTI15_10_IRQn;
            break;
        default:
            return false;
    }
#elif defined(STM32L0)
    switch (this->thisPin)
    {
        case 0:
        case 1:
            ThisIRQn = EXTI0_1_IRQn;
            break;
        case 2:
        case 3:
            ThisIRQn = EXTI2_3_IRQn;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            ThisIRQn = EXTI4_15_IRQn;
            break;
        default:
            return false;
    }
#else
    return false;
#endif
    NVIC_EnableIRQ(ThisIRQn);
    return true;
};

void GPIO::PIN::read_edge(void)
{
    // Get the rising edge
    this->edge_rising = (this->get_state() && !this->state_old);

    // Get the falling edge
    this->edge_falling = (!this->get_state() && this->state_old);

    // Remember old state
    this->state_old = this->get_state();
};

void GPIO::PIN::reset_pending_interrupt(void) const
{
    EXTI->PR |= (1 << this->thisPin);
};