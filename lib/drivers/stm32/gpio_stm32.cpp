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
 * @version v5.1.0
 * @date    25-August-2021
 * @brief   GPIO driver for STM32 microcontrollers.
 ==============================================================================
 */
/* === Includes === */
#include "gpio_stm32.h"
using stm32::Peripheral;

/* === Valid GPIO Port instantiations === */
template gpio::Pin gpio::Pin::create<gpio::Port::A>(std::uint8_t, gpio::Mode);
template gpio::Pin gpio::Pin::create<gpio::Port::B>(std::uint8_t, gpio::Mode);
template gpio::Pin gpio::Pin::create<gpio::Port::C>(std::uint8_t, gpio::Mode);
template gpio::Pin gpio::Pin::create<gpio::Port::D>(std::uint8_t, gpio::Mode);
#ifndef STM32L053xx
template gpio::Pin gpio::Pin::create<gpio::Port::E>(std::uint8_t, gpio::Mode);
#endif // STM32L053xx
template gpio::Pin gpio::Pin::create<gpio::Port::H>(std::uint8_t, gpio::Mode);
#ifdef STM32F4
template gpio::Pin gpio::Pin::create<gpio::Port::F>(std::uint8_t, gpio::Mode);
template gpio::Pin gpio::Pin::create<gpio::Port::G>(std::uint8_t, gpio::Mode);
template gpio::Pin gpio::Pin::create<gpio::Port::I>(std::uint8_t, gpio::Mode);
#ifdef STM32F429xx
template gpio::Pin gpio::Pin::create<gpio::Port::J>(std::uint8_t, gpio::Mode);
template gpio::Pin gpio::Pin::create<gpio::Port::K>(std::uint8_t, gpio::Mode);
#endif // STM32F429xx
#endif // STM32F4

namespace detail
{
    /* === Functions === */
    /**
     * @brief Return the enable bit position for the RCC register
     * for the given GPIO port.
     * @tparam port_used The identifier of the port.
     * @return Return the bit position of the enable bit.
     */
    template <gpio::Port port_used>
    constexpr auto get_RCCEN_position() -> std::uint8_t
    {
        /* Return the bit position according to the used port */
        switch (port_used)
        {
            case gpio::Port::A:
                return 0;
            case gpio::Port::B:
                return 1;
            case gpio::Port::C:
                return 2;
            case gpio::Port::D:
                return 3;
            case gpio::Port::E:
                return 4;
            case gpio::Port::H:
                return 7;
#ifdef STM32F4
            case gpio::Port::F:
                return 5;
            case gpio::Port::G:
                return 6;
            case gpio::Port::I:
                return 8;
#ifdef STM32F429xx
            case gpio::Port::J:
                return 9;
            case gpio::Port::K:
                return 10;
#endif // STM32F429xx
#endif // STM32F4
            default:
                return 32;
        }
    };

    /**
     * @brief Return the peripheral address for the desired port.
     * @tparam port_used The identifier of the port.
     * @return Returns the address of the port as an integer.
     */
    template <gpio::Port port_used>
    constexpr auto get_port_address() -> std::uintptr_t
    {
        switch (port_used)
        {
            case gpio::Port::A:
                return GPIOA_BASE;

            case gpio::Port::B:
                return GPIOB_BASE;

            case gpio::Port::C:
                return GPIOC_BASE;

            case gpio::Port::D:
                return GPIOD_BASE;
#ifndef STM32L053xx
            case gpio::Port::E:
                return GPIOE_BASE;
#endif
            case gpio::Port::H:
                return GPIOH_BASE;

#ifdef STM32F4
            case gpio::Port::F:
                return GPIOF_BASE;

            case gpio::Port::G:
                return GPIOG_BASE;

            case gpio::Port::I:
                return GPIOI_BASE;

#ifdef STM32F429xx
            case gpio::Port::J:
                return GPIOJ_BASE;

            case gpio::Port::K:
                return GPIOK_BASE;
#endif
#endif
            default:
                return 0;
        }
    };

    /**
     * @brief Get the alternate function code of the
     *
     * @param port The port of the pin
     * @param pin The pin number
     * @param function The peripheral to assign the pin to
     * @return The alternate function code of the pin.
     * If alternate function or peripheral is not valid, -1 is returned.
     */
    auto get_af_code(const gpio::Port port, const uint8_t pin, const Peripheral function) -> char
    {
#if defined(STM32F4)
        /* The *STM32F4xx* devices have constant AF map */
        switch (function)
        {
            case Peripheral::SYSTEM_:
                return 0;
            case Peripheral::TIM_1:
            case Peripheral::TIM_2:
                return 1;
            case Peripheral::TIM_3:
            case Peripheral::TIM_4:
            case Peripheral::TIM_5:
                return 2;
            case Peripheral::TIM_8:
            case Peripheral::TIM_9:
            case Peripheral::TIM_10:
            case Peripheral::TIM_11:
                return 3;
            case Peripheral::I2C_1:
            case Peripheral::I2C_2:
            case Peripheral::I2C_3:
                return 4;
            case Peripheral::SPI_1:
            case Peripheral::SPI_2:
            case Peripheral::SPI_4:
            case Peripheral::SPI_5:
                return 5;
            case Peripheral::SPI_3:
                return 6;
            case Peripheral::USART_1:
            case Peripheral::USART_2:
            case Peripheral::USART_3:
                return 7;
            case Peripheral::CAN_1:
            case Peripheral::CAN_2:
            case Peripheral::TIM_12:
            case Peripheral::TIM_13:
            case Peripheral::TIM_14:
                return 9;
            case Peripheral::OTG_FS_:
            case Peripheral::OTG_HS_:
                return 10;
            case Peripheral::ETH_:
                return 11;
            case Peripheral::FSMC_:
            case Peripheral::SDIO_:
                return 12;
            case Peripheral::DCMI_:
                return 13;
            case Peripheral::EVENTOUT_:
                return 15;
            default:
                return -1;
        }
#endif // STM32F4

#if defined(STM32L0)
        /* The *STM32L0xx* devices assign different AF functions depending on the pin number */
        switch (function)
        {
            case Peripheral::I2C_1:
                if (port == gpio::Port::A)
                    return 6;
                if (port == gpio::Port::B)
                {
                    if (pin < 8)
                        return 1;
                    else
                        return 4;
                }
                return -1; /* This pin cannot be assigned to I2C1 */
            case Peripheral::I2C_2:
                if (port == gpio::Port::B)
                {
                    if (pin < 12)
                        return 6;
                    else
                        return 5;
                }
                return -1; /* This pin cannot be assigned to I2C2 */
            case Peripheral::I2C_3:
                return 7;
            case Peripheral::TIM_2:
                if (port == gpio::Port::E)
                    return 0;
            case Peripheral::TIM_3:
                return 2;
            default:
                return -1;
        }
#endif // STM32L0
    };
}; // namespace detail

namespace gpio
{
    /* === Factory === */
    template <Port port_used>
    auto Pin::create(std::uint8_t pin, Mode mode) -> Pin
    {
        /* enable the clock for this gpio port */
#if defined(STM32F4)
        RCC->AHB1ENR |= (1 << detail::get_RCCEN_position<port_used>());
#elif defined(STM32L0)
        RCC->IOPENR |= (1 << detail::get_RCCEN_position<port_used>());
#endif

        /* Get the address of the port */
        auto address = detail::get_port_address<port_used>();

        /* Return the created pin */
        return Pin{address, port_used, pin, mode};
    }

    /* === Constructors === */
    Pin::Pin(const std::uintptr_t port_address, const Port port_id, const std::uint8_t Pin, Mode mode)
        : port{reinterpret_cast<volatile GPIO_TypeDef *>(port_address)}, pin{Pin}, port_id{port_id}
    {
        /* Set the mode of the pin */
        this->set_mode(mode);

#ifndef OTOS_REDUCE_MEMORY_USAGE
        /*
         * Get the bit shifted masks to set and reset
         * the pin here to save clock cycles later
         */
        this->set_mask = 1 << this->pin;
        this->reset_mask = 1 << 16 << this->pin;
#endif // OTOS_REDUCE_MEMORY_USAGE
    };

    /* === Methods === */
    auto Pin::set_alternate_function(const Peripheral function) -> Pin &
    {
        /* Get the alternate function code and check if it is valid */
        const auto af_code = this->get_af_code(function);
        if (af_code >= 0)
        {
            /* Assign the alternate function */
            this->set_alternate_function(this->get_af_code(function));

            /* Set AF specific options */
            if ((function == Peripheral::I2C_1) | (function == Peripheral::I2C_2) | (function == Peripheral::I2C_3))
                this->set_output_type(Output::Open_Drain);
        }
        else
        {
            /* Invalid alternate function -> reset the pin mode */
            this->set_mode(Mode::Input);
        }

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::set_alternate_function(const uint8_t af_code) -> Pin &
    {
        /* Set AF mode */
        this->set_mode(Mode::AF_Mode);

        /* Assign the alternate function */
        if (this->pin < 8)
        {
            /* Save old register state and delete the part which will change */
            uint32_t reg = this->port->AFR[0] & ~(0b1111 << (4 * this->pin));
            /* Get the code for teh alternate function and set the register */
            this->port->AFR[0] = reg | (af_code << (4 * this->pin));
        }
        else
        {
            /* Save old register state and delete the part which will change */
            uint32_t reg = this->port->AFR[1] & ~(0b1111 << (4 * (this->pin - 8)));
            /* Get the code for the alternate function and set the register */
            this->port->AFR[1] = reg | (af_code << (4 * (this->pin - 8)));
        }

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::set_high() -> Pin &
    {
        /* Set the BS bit */
#ifndef OTOS_REDUCE_MEMORY_USAGE
        this->port->BSRR = this->set_mask;
#else
        this->port->BSRR = (1 << this->pin);
#endif // OTOS_REDUCE_MEMORY_USAGE

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::set_low() -> Pin &
    {
        /* Set the BR bit */
#ifndef OTOS_REDUCE_MEMORY_USAGE
        this->port->BSRR = this->reset_mask;
#else
        this->port->BSRR = 1 << 16 << this->pin;
#endif // OTOS_REDUCE_MEMORY_USAGE

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::set_mode(const Mode NewMode) -> Pin &
    {
        /* Save old register state and delete the part which will change */
        uint32_t reg = this->port->MODER & ~(0b11 << (2 * this->pin));

        /* Combine the old and the new data and write the register */
        this->port->MODER = reg | (static_cast<uint8_t>(NewMode) << (2 * this->pin));

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::set_output_type(const Output NewType) -> Pin &
    {
        /* Save old register state and delete the part which will change */
        uint32_t reg = this->port->OTYPER & ~(1 << this->pin);

        /* Combine the old and the new data and write the register */
        this->port->OTYPER = reg | (static_cast<uint8_t>(NewType) << this->pin);

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::set_pull(const Pull NewPull) -> Pin &
    {
        /* Save old register state and delete the part which will change */
        uint32_t reg = this->port->PUPDR & ~(0b11 << (2 * this->pin));

        /* Combine the old and the new data and write the register */
        this->port->PUPDR = reg | (static_cast<uint8_t>(NewPull) << (2 * this->pin));

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::set_speed(const Speed NewSpeed) -> Pin &
    {
        /* Save old register state and delete the part which will change */
        uint32_t reg = this->port->OSPEEDR & ~(0b11 << (2 * this->pin));

        /* Combine the old and the new data and write the register */
        this->port->OSPEEDR = reg | (static_cast<uint8_t>(NewSpeed) << (2 * this->pin));

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::set_state(const bool NewState) -> Pin &
    {
        /* Call the setHigh or setLow function according to NewState */
        if (NewState)
            this->set_high();
        else
            this->set_low();

        /* Return the reference to the pin object */
        return *this;
    };

    auto Pin::toggle() -> Pin &
    {
#ifndef OTOS_REDUCE_MEMORY_USAGE
        this->port->ODR ^= this->set_mask;
#else
        this->port->ODR ^= (1 << this->pin);
#endif // OTOS_REDUCE_MEMORY_USAGE
        return *this;
    };

    auto Pin::get_state() const volatile -> bool
    {
#ifndef OTOS_REDUCE_MEMORY_USAGE
        return (this->port->IDR & this->set_mask);
#else
        return (this->port->IDR & (1 << this->pin));
#endif // OTOS_REDUCE_MEMORY_USAGE
    };

    auto Pin::falling_edge() const -> bool
    {
        return this->edge_falling;
    };

    auto Pin::rising_edge() const -> bool
    {
        return this->edge_rising;
    };

    auto Pin::enable_interrupt(const Edge NewEdge) const -> bool
    {
        /* Enable the EXTI line */
        const uint32_t bitmask = (1 << this->pin);
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

        /* Configure the pin of the EXTI line in the */
        /* System configuration */
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
        SYSCFG->EXTICR[this->pin / 4] |= (static_cast<uint8_t>(this->port_id) << 4 * (this->pin % 4));

        /* Enable the EXTI line in the NVIC */
        IRQn_Type ThisIRQn;
#if defined(STM32F4)
        switch (this->pin)
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
        switch (this->pin)
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

    void Pin::read_edge()
    {
        /* Get the rising edge */
        this->edge_rising = (this->get_state() && !this->state_old);

        /* Get the falling edge */
        this->edge_falling = (!this->get_state() && this->state_old);

        /* Remember old state */
        this->state_old = this->get_state();
    };

    void Pin::reset_pending_interrupt() const
    {
        EXTI->PR |= (1 << this->pin);
    };

    auto Pin::get_af_code(const Peripheral function) const -> char
    {
        return detail::get_af_code(this->port_id, this->pin, function);
    };

}; // namespace gpio
