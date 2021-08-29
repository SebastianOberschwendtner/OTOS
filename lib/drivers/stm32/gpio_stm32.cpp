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
/**
 ==============================================================================
 * @file    gpio_stm32.cpp
 * @author  SO
 * @version v1.0.6
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
static unsigned char get_RCCEN_position(GPIO::PinPort Port)
{
    // Return the bit position according to the used port
    switch(Port)
    {
        case GPIO::PORTA:
            return 0;
        case GPIO::PORTB:
            return 1;
        case GPIO::PORTC:
            return 2;
        case GPIO::PORTD:
            return 3;
        case GPIO::PORTE:
            return 4;
        case GPIO::PORTH:
            return 7;
#ifdef STM32F4
        case GPIO::PORTF:
            return 5;
        case GPIO::PORTG:
            return 6;
        case GPIO::PORTI:
            return 8;
        case GPIO::PORTJ:
            return 9;
        case GPIO::PORTK:
            return 10;
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
static unsigned long get_port_address(GPIO::PinPort Port)
{
    switch (Port)
    {
    case GPIO::PORTA:
        return GPIOA_BASE;
        
    case GPIO::PORTB:
        return GPIOB_BASE;
        
    case GPIO::PORTC:
        return GPIOC_BASE;
        
    case GPIO::PORTD:
        return GPIOD_BASE;
        
    case GPIO::PORTE:
        return GPIOE_BASE;
        
    case GPIO::PORTH:
        return GPIOH_BASE;

#ifdef STM32F4
    case GPIO::PORTF:
        return GPIOF_BASE;
        
    case GPIO::PORTG:
        return GPIOG_BASE;
        
    case GPIO::PORTI:
        return GPIOI_BASE;
        
    case GPIO::PORTJ:
        return GPIOJ_BASE;
        
    case GPIO::PORTK:
        return GPIOK_BASE;
#endif
    default:
        return 0;
    }
};

// *** Methods ***

/**
 * @brief Constructor for PIN object without specified output type.
 * @param Port The Port the pin belongs to
 * @param Pin The pin number of the pin in the port
 */
GPIO::PIN::PIN(GPIO::PinPort Port, GPIO::PinNumber Pin):
    thisPin(Pin)
{
    // enable the clock for this gpio port
#if defined(STM32F4)
    RCC->AHB1ENR |= (1 << get_RCCEN_position(Port));
#elif defined(STM32L0)
    RCC->IOPENR |= (1 << get_RCCEN_position(Port));
#endif

    // set the Port configuration
    unsigned long Port_addr = get_port_address(Port);
    thisPort = reinterpret_cast<volatile GPIO_TypeDef*>(Port_addr);
};

/**
 * @brief Constructor for PIN object without specified output type.
 * @param Port The Port the pin belongs to
 * @param Pin The pin number of the pin in the port
 */
GPIO::PIN::PIN(GPIO::PinPort Port, GPIO::PinNumber Pin, GPIO::Mode PinMode):
    thisPin(Pin)
{
    // enable the clock for this gpio port
#if defined(STM32L4)
    RCC->AHB1ENR |= (1 << get_RCCEN_position(Port));
#elif defined(STM32L0)
    RCC->IOPENR |= (1 << get_RCCEN_position(Port));
#endif

    // set the Port configuration
    unsigned long Port_addr = get_port_address(Port);
    thisPort = reinterpret_cast<volatile GPIO_TypeDef*>(Port_addr);
    setMode(PinMode);
};

/**
 * @brief Set the output mode of the GPIO pin.
 * @param NewMode The new mode of the pin.
 */
 void GPIO::PIN::setMode(GPIO::Mode NewMode)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->MODER & ~(0b11 << (2 * this->thisPin));

    // Combine the old and the new data and write the register
    this->thisPort->MODER = _Reg | (NewMode << (2 * this->thisPin));
};
/**
 * @brief Set the output type of the GPIO pin.
 * @param NewType The new output type of the pin.
 */
 void GPIO::PIN::setType(GPIO::Type NewType)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->OTYPER & ~(1 << this->thisPin);

    // Combine the old and the new data and write the register
    this->thisPort->OTYPER = _Reg | (NewType << this->thisPin);
};

/**
 * @brief Set the output speed of the GPIO pin.
 * @param NewSpeed The new speed of the pin.
 */
 void GPIO::PIN::setSpeed(GPIO::Speed NewSpeed)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->OSPEEDR & ~(0b11 << (2 * this->thisPin));

    // Combine the old and the new data and write the register
    this->thisPort->OSPEEDR = _Reg | (NewSpeed << (2 * this->thisPin));
};

/**
 * @brief Set the pull type of the GPIO pin.
 * @param NewPull The new pull state of the pin.
 */
 void GPIO::PIN::setPull(GPIO::Pull NewPull)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->MODER & ~(0b11 << (2 * this->thisPin));

    // Combine the old and the new data and write the register
    this->thisPort->PUPDR = _Reg | (NewPull << (2 * this->thisPin));
};

/**
 * @brief Set the logic output state of the GPIO pin.
 * @param NewState The new output state of the pin.
 */
 void GPIO::PIN::set(bool NewState)
{
    // Call the setHigh or setLow function according to NewState
    if (NewState)
        this->setHigh();
    else
        this->setLow();
};

/**
 * @brief Set the GPIO pin high. Atomic access, sets the pin
 * always high.
 */
void GPIO::PIN::setHigh(void)
{
    // Set the BS bit
    thisPort->BSRR = (1 << this->thisPin);
};

/**
 * @brief Set the GPIO pin low. Atomic access, sets the pin
 * always low.
 */
 void GPIO::PIN::setLow(void)
{
    // Set the BR bit
    thisPort->BSRR = (1 << (this->thisPin + 16));
};

/**
 * @brief Toggle the logic state of a GPIO output pin.
 */
 void GPIO::PIN::toggle(void)
{
    thisPort->ODR ^= (1 << this->thisPin);
};

/**
 * @brief Get the logical pin state of the GPIO pin.
 * @return The logical state of the pin.
 */
 bool GPIO::PIN::get(void)
{
    return (this->thisPort->IDR & (1 << this->thisPin));
};