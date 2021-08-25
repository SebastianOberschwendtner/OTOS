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

#ifndef GPIO_STM32_H_
#define GPIO_STM32_H_

// *** Includes ***
#include "processors.h"

// *** Declarations ***
namespace GPIO {
    // *** Enums ***
    enum PinPort
    {
        PORTA = GPIOA_BASE,
        PORTB = GPIOB_BASE,
        PORTC = GPIOC_BASE,
        PORTD = GPIOD_BASE,
        PORTE = GPIOE_BASE,
        PORTF = GPIOF_BASE,
        PORTG = GPIOG_BASE,
        PORTH = GPIOH_BASE,
        PORTI = GPIOI_BASE,
        PORTJ = GPIOJ_BASE,
        PORTK = GPIOK_BASE
    };
    enum PinNumber: unsigned char
    {
        PIN0 = 0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7,
        PIN8, PIN9, PIN10, PIN11, PIN12, PIN13, PIN14, PIN15
    };
    enum Mode: unsigned char
    {
        INPUT = 0, OUTPUT, AF, ANALOG
    };
    enum Type: bool
    {
        PUSH_PULL = false, OPEN_DRAIN = true
    };
    enum Speed: unsigned char
    {
        LOW = 0, MEDIUM, HIGH, VERY_HIGH
    };
    enum Pull: unsigned char
    {
        NO_PP = 0, PULL_UP, PULL_DOWN
    };

    // *** Classes ***
    template<PinPort PinPort, PinNumber ThisPin>
    class PIN{
    private:
        // Singleton to encapsulate peripheral address
        static constexpr GPIO_TypeDef volatile* Port()
        {
            return reinterpret_cast<GPIO_TypeDef volatile*>(PinPort);
        };

        // Get the clock enable bit for the port
        constexpr unsigned char RCC_AHB1ENR_pos()
        {
            if (PinPort == PORTA)
                return 0;
            else if (PinPort == PORTB)
                return 1;
            else if (PinPort == PORTC)
                return 2;
            else if (PinPort == PORTD)
                return 3;
            else if (PinPort == PORTE)
                return 4;
            else if (PinPort == PORTF)
                return 5;
            else if (PinPort == PORTG)
                return 6;
            else if (PinPort == PORTH)
                return 7;
            else if (PinPort == PORTI)
                return 8;
            else if (PinPort == PORTJ)
                return 9;
            else
                return 10;
        };

    public:

        // Methods

        /**
         * @brief Constructor for a GPIO pin object. This leaves
         * all register values at the default. Enables the peripheral clock.
         */
        PIN()
        {
            RCC->AHB1ENR |= (1 << RCC_AHB1ENR_pos());
        };

        /**
         * @brief Constructor for a GPIO pin object with some predefined
         * options. Enables the peripheral clock.
         * @param PinMode The initial mode of the pin.
         */
        PIN(Mode PinMode)
        {
            RCC->AHB1ENR |= (1 << RCC_AHB1ENR_pos());
            Port()->MODER |= PinMode << (2 * ThisPin);
        };

        /**
         * @brief Set the output mode of the GPIO pin.
         * @param NewMode The new mode of the pin.
         */
        static inline void setMode(Mode NewMode)
        {
            // Save old register state and delete the part which will change
            const uint32_t _Reg = Port()->MODER & ~(0b11 << (2 * ThisPin));
            // Combine the old and the new data and write the register
            Port()->MODER = _Reg | (NewMode << (2 * ThisPin));
        };

        /**
         * @brief Set the output type of the GPIO pin.
         * @param NewType The new output type of the pin.
         */
        static inline void setType(Type NewType)
        {
            // Save old register state and delete the part which will change
            const uint32_t _Reg = Port()->OTYPER & ~(1 << ThisPin);
            // Combine the old and the new data and write the register
            Port()->OTYPER = _Reg | (NewType << ThisPin);
        };

        /**
         * @brief Set the output speed of the GPIO pin.
         * @param NewSpeed The new speed of the pin.
         */
        static inline void setSpeed(Speed NewSpeed)
        {
            // Save old register state and delete the part which will change
            const uint32_t _Reg = Port()->OSPEEDR & ~(0b11 << (2 * ThisPin));
            // Combine the old and the new data and write the register
            Port()->OSPEEDR = _Reg | (NewSpeed << (2 * ThisPin));
        };

        /**
         * @brief Set the pull type of the GPIO pin.
         * @param NewPull The new pull state of the pin.
         */
        static inline void setPull(Pull NewPull)
        {
            // Save old register state and delete the part which will change
            const uint32_t _Reg = Port()->MODER & ~(0b11 << (2 * ThisPin));
            // Combine the old and the new data and write the register
            Port()->PUPDR = _Reg | (NewPull << (2 * ThisPin));
        };

        /**
         * @brief Set the logic output state of the GPIO pin.
         * @param NewState The new output state of the pin.
         */
        static inline void set(bool NewState)
        {
            // Call the setHigh or setLow function according to NewState
            if (NewState)
                setHigh();
            else
                setLow();
        };

        /**
         * @brief Set the GPIO pin high. Atomic access, sets the pin
         * always high.
         */
        static inline void setHigh(void)
        {
            // Set the BS bit
            Port()->BSRR = (1 << ThisPin);
        };

        /**
         * @brief Set the GPIO pin low. Atomic access, sets the pin
         * always low.
         */
        static inline void setLow(void)
        {
            // Set the BR bit
            Port()->BSRR = (1 << (ThisPin + 16));
        };

        /**
         * @brief Toggle the logic state of a GPIO output pin.
         */
        static inline void toggle(void)
        {
            Port()->ODR ^= (1 << ThisPin);
        };

        /**
         * @brief Get the logical pin state of the GPIO pin.
         * @return The logical state of the pin.
         */
        static inline bool get(void)
        {
            return (Port()->IDR & (1 << ThisPin));
        };
    };
};

#endif