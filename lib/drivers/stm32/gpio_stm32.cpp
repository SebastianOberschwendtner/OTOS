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
 * @version v1.0.8
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
constexpr unsigned char get_RCCEN_position(const GPIO::PinPort Port) {
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
constexpr unsigned long get_port_address(const GPIO::PinPort Port) {
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

/**
 * @brief Get the code for the alternate function for the AFR register.
 * @param function The desired alternate function of a pin.
 * @return The AF code of the alternate function to put into the AF register.
 */
unsigned char GPIO::PIN::get_af_code(const GPIO::Alternate function) const
{
    switch (function)
    {
#if defined(STM32F4)
    case GPIO::SYSTEM_:
        return 0;
    case GPIO::TIM_1:
    case GPIO::TIM_2:
        return 1;
    case GPIO::TIM_3:
    case GPIO::TIM_4:
    case GPIO::TIM_5:
        return 2;
    case GPIO::TIM_8:
    case GPIO::TIM_9:
    case GPIO::TIM_10:
    case GPIO::TIM_11:
        return 3;
    case GPIO::I2C_1:
    case GPIO::I2C_2:
    case GPIO::I2C_3:
        return 4;
    case GPIO::SPI_1:
    case GPIO::SPI_2:
        return 5;
    case GPIO::SPI_3:
        return 6;
    case GPIO::USART_1:
    case GPIO::USART_2:
    case GPIO::USART_3:
        return 7;
    case GPIO::CAN_1:
    case GPIO::CAN_2:
    case GPIO::TIM_12:
    case GPIO::TIM_13:
    case GPIO::TIM_14:
        return 9;
    case GPIO::OTG_FS_:
    case GPIO::OTG_HS_:
        return 10;
    case GPIO::ETH_:
        return 11;
    case GPIO::FSMC_:
    case GPIO::SDIO_:
        return 12;
    case GPIO::DCMI_:
        return 13;
    case GPIO::EVENTOUT_:
        return 15;
#elif defined(STM32L0)
    case GPIO::I2C_1:
        if (this->thisPort == GPIOA)
            return 6;
        if (this->thisPort == GPIOB)
        {
            if(this->thisPin < 8)
                return 1;
            else
                return 4;
        }
        return 0; //This pin cannot be assigned to I2C1
    
    case GPIO::I2C_2:
        if (this->thisPort == GPIOB)
        {
            if (this->thisPin < 12)
                return 6;
            else
                return 5;
        }
        return 0; //This pin cannot be assigned to I2C2
    
    case GPIO::I2C_3:
        return 7;
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
GPIO::PIN::PIN(const GPIO::PinPort Port, const GPIO::PinNumber Pin):
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
    this->thisPort = reinterpret_cast<volatile GPIO_TypeDef*>(Port_addr);
};

/**
 * @brief Constructor for PIN object without specified output type.
 * @param Port The Port the pin belongs to
 * @param Pin The pin number of the pin in the port
 */
GPIO::PIN::PIN(const GPIO::PinPort Port, const GPIO::PinNumber Pin, const GPIO::Mode PinMode):
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
    this->thisPort = reinterpret_cast<volatile GPIO_TypeDef*>(Port_addr);
    this->setMode(PinMode);
};

/**
 * @brief Set the output mode of the GPIO pin.
 * @param NewMode The new mode of the pin.
 */
 void GPIO::PIN::setMode(const GPIO::Mode NewMode)
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
 void GPIO::PIN::setType(const GPIO::Type NewType) 
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
 void GPIO::PIN::setSpeed(const GPIO::Speed NewSpeed)
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
 void GPIO::PIN::setPull(const GPIO::Pull NewPull)
{
    // Save old register state and delete the part which will change
    uint32_t _Reg = this->thisPort->PUPDR & ~(0b11 << (2 * this->thisPin));

    // Combine the old and the new data and write the register
    this->thisPort->PUPDR = _Reg | (NewPull << (2 * this->thisPin));
};

/**
 * @brief Enable the desired alternate function of the pin.
 * @param function The alternate function of the pin.
 */
void GPIO::PIN::set_alternate_function(const GPIO::Alternate function)
{
    if(this->thisPin < GPIO::PIN8)
    {
        // Save old register state and delete the part which will change
        uint32_t _Reg = this->thisPort->AFR[0] & ~(0b1111 << (4 * this->thisPin));
        // Get the code for teh alternate function and set the register
        this->thisPort->AFR[0] = _Reg | (get_af_code(function) << (4 * this->thisPin));
    }
    else
    {
        // Save old register state and delete the part which will change
        uint32_t _Reg = this->thisPort->AFR[1] & ~(0b1111 << (4 * (this->thisPin - 8)));
        // Get the code for teh alternate function and set the register
        this->thisPort->AFR[1] = _Reg | (get_af_code(function) << (4 * (this->thisPin - 8)));
    }
};

/**
 * @brief Set the logic output state of the GPIO pin.
 * @param NewState The new output state of the pin.
 */
 void GPIO::PIN::set(const bool NewState)
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
 bool GPIO::PIN::get(void) const
{
    return (this->thisPort->IDR & (1 << this->thisPin));
};