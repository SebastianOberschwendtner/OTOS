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
 * @file    usart_stm32.cpp
 * @author  SO
 * @version v2.2.1
 * @date    23-Dezember-2021
 * @brief   USART driver for STM32 microcontrollers.
 ==============================================================================
 */

// === Includes ===
#include "usart_stm32.h"

// Provide template instanciations with all allowed bus instances
template class USART::Controller<IO::USART_1>;
template class USART::Controller<IO::USART_2>;
template class USART::Controller<IO::USART_4>;
template class USART::Controller<IO::USART_5>;
#ifdef STM32F4
template class USART::Controller<IO::USART_3>;
template class USART::Controller<IO::USART_6>;
template class USART::Controller<IO::USART_7>;
template class USART::Controller<IO::USART_8>;
#endif

// === Helper functions ===
namespace USART
{
    /**
     * @brief Get the peripheral address
     * 
     * @tparam instance USART IO peripheral
     * @return constexpr unsigned long Address of the SPI peripheral
     */
    template <IO instance>
    constexpr unsigned long get_peripheral_address()
    {
        // Return the peripheral address
        if constexpr (instance == IO::USART_1)
            return USART1_BASE;
        if constexpr (instance == IO::USART_2)
            return USART2_BASE;
#ifdef STM32L0
        if constexpr (instance == IO::USART_4)
            return USART4_BASE;
        if constexpr (instance == IO::USART_5)
            return USART5_BASE;
#endif
#ifdef STM32F4
        if constexpr (instance == IO::USART_3)
            return USART3_BASE;
        if constexpr (instance == IO::USART_4)
            return UART4_BASE;
        if constexpr (instance == IO::USART_5)
            return UART5_BASE;
        if constexpr (instance == IO::USART_6)
            return USART6_BASE;
        if constexpr (instance == IO::USART_7)
            return UART7_BASE;
        if constexpr (instance == IO::USART_8)
            return UART8_BASE;
#endif
    };

    /**
     * @brief Enable the corresponding clock in the RCC register.
     *
     * @tparam instance USART IO peripheral instance.
     */
    template <IO instance>
    void enable_rcc_clock()
    {
        if constexpr (instance == IO::USART_1)
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        if constexpr (instance == IO::USART_2)
            RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
#ifdef STM32L0
        if constexpr (instance == IO::USART_4)
            RCC->APB1ENR |= RCC_APB1ENR_USART4EN;
        if constexpr (instance == IO::USART_5)
            RCC->APB1ENR |= RCC_APB1ENR_USART5EN;
#endif
#ifdef STM32F4 
        if constexpr (instance == IO::USART_3)
            RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        if constexpr (instance == IO::USART_4)
            RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
        if constexpr (instance == IO::USART_5)
            RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
        if constexpr (instance == IO::USART_6)
            RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
        if constexpr (instance == IO::USART_7)
            RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
        if constexpr (instance == IO::USART_8)
            RCC->APB1ENR |= RCC_APB1ENR_UART8EN;
#endif
    };

    template<IO instance>
    constexpr unsigned short calculate_prescaler(const unsigned long baudrate)
    {
        // Calculation variables
        unsigned long prescaler = 0;
        unsigned short mantissa = 0;
        unsigned char fraction = 0;
        constexpr unsigned long scaling = 100;

        // Scale baudrate to get significant digits for
        // the integer prescaler
        const unsigned long baudrate_scaled = baudrate / scaling;

        // Get the prescaler according to the used clock
        if constexpr (instance == IO::USART_1)
            prescaler = F_APB2  / baudrate_scaled;
        if constexpr (instance == IO::USART_2)
            prescaler = F_APB1  / baudrate_scaled;
        if constexpr (instance == IO::USART_3)
            prescaler = F_APB1  / baudrate_scaled;
        if constexpr (instance == IO::USART_4)
            prescaler = F_APB1  / baudrate_scaled;
        if constexpr (instance == IO::USART_5)
            prescaler = F_APB1  / baudrate_scaled;
        if constexpr (instance == IO::USART_6)
            prescaler = F_APB2  / baudrate_scaled;
        if constexpr (instance == IO::USART_7)
            prescaler = F_APB1  / baudrate_scaled;
        if constexpr (instance == IO::USART_8)
            prescaler = F_APB1  / baudrate_scaled;

        // Convert the integer value to mantissa
        prescaler /= 8 * (2 - 0);
        mantissa = static_cast<unsigned short>(prescaler/scaling);

        // Get the fraction of the prescaler to calculate the fraction part
        prescaler -= static_cast<unsigned short>(mantissa) * scaling;
        prescaler *= 16;
        fraction = static_cast<unsigned char>(prescaler / scaling);

        return static_cast<unsigned short>((mantissa<<4) | (fraction & 0b1111));
    };
};

// === Methods ===
/**
 * @brief Construct a new usart controller object
 * 
 * @tparam usart_instance The USART peripheral to be used.
 * @param baudrate The desired bus baudrate in [Hz].
 * @param data_bits Number of data bits. Should be 8 or 9!
 * @param stop_bits Number of stop bits to use.
 */
template <IO usart_instance>
USART::Controller<usart_instance>::Controller(
    const unsigned long baudrate,
    const unsigned char data_bits,
    const StopBits stop_bits
    )
    : Base{usart_instance}
    , peripheral{ reinterpret_cast<USART_TypeDef *>(USART::get_peripheral_address<usart_instance>()) }
{
    // Enable the peripheral clock
    USART::enable_rcc_clock<usart_instance>();

    // Set the configuration register
    this->peripheral->CR1 |= USART_CR1_TE | USART_CR1_RE;
    this->peripheral->CR2 |= static_cast<unsigned int>(stop_bits) << USART_CR2_STOP_Pos;

    // Set the data format
    if (data_bits > 8)
        this->peripheral->CR1 |= USART_CR1_M;

    // Set prescaler
    this->peripheral->BRR = calculate_prescaler<usart_instance>(baudrate);

    // Set the timeout limit
    this->set_timeout(250);
};

/**
 * @brief Enable the USART peripheral.
 * 
 * @tparam usart_instance The USART peripheral to be used.
 */
template <IO usart_instance>
void USART::Controller<usart_instance>::enable(void)
{
    this->peripheral->CR1 |= USART_CR1_UE;
};

/**
 * @brief Disable the USART peripheral.
 * 
 * @tparam usart_instance The USART peripheral to be used.
 */
template <IO usart_instance>
void USART::Controller<usart_instance>::disable(void)
{
    this->peripheral->CR1 &= ~USART_CR1_UE;
};

/**
 * @brief Check whether the last byte transfer is finished.
 * 
 * @tparam usart_instance The USART peripheral to be used.
 * @return Returns True when the last transfer is finished, i.e.
 * the TX buffer is empty.
 */
template <IO usart_instance>
bool USART::Controller<usart_instance>::last_transmit_finished(void) const
{
#if defined(STM32F4)
    return this->peripheral->SR & USART_SR_TXE;
#elif defined(STM32L0)
    return not(this->peripheral->ISR & USART_ISR_TXE);
#endif
};

/**
 * @brief Check whether the bus is busy.
 * 
 * @tparam usart_instance The USART peripheral to be used.
 * @return Returns True when the bus is busy.
 */
template <IO usart_instance>
bool USART::Controller<usart_instance>::is_busy(void) const
{
#if defined(STM32F4)
    return not(this->peripheral->SR & USART_SR_TC);
#elif defined(STM32L0)
    return not(this->peripheral->ISR & USART_ISR_TC);
#endif
};

/**
 * @brief Send a byte onto the USART bus. Only when the TX buffer
 * is empty!
 * Sets the following errors:
 * - USART_Timeout
 * 
 * @tparam usart_instance The USART peripheral to be used.
 * @param data The byte to send
 * @return Returns True when the byte was send successfully.
 * @details blocking function
 */
template<IO usart_instance>
bool USART::Controller<usart_instance>::send_data_byte( const unsigned char data)
{
    // Wait for the TX register to become empty
    this->reset_timeout();
    while( not this->last_transmit_finished() )
    {
        // Catch timeouts
        if( this->timed_out() )
        {
            this->set_error(Error::Code::USART_Timeout);
            return false;
        }
    }
#if defined(STM32F4)
    this->peripheral->DR = data;
#elif defined(STM32L0)
    this->peripheral->TDR = data;
#endif
    return true;
}

/**
 * @brief Send n bytes to an USART target
 * The highest byte in the payload is transmitted first!
 * 
 * Be aware, that this functions exits when the last byte
 * is still beeing sent! So when you need to perform actions
 * after the last byte is sent, use the method `last_transmit_finished()`
 * to check whether the last byte is sent or not.
 * 
 * Sets the following errors:
 * - USART_Timeout
 * - USART_BUS_Busy
 * 
 * @tparam usart_instance The USART peripheral to be used.
 * @param payload Data struct with maximum 4 bytes
 * @param n_bytes How many bytes should be sent
 * @return Returns True when the data was sent successfully, False otherwise.
 * @details blocking function
 */
template<IO usart_instance>
bool USART::Controller<usart_instance>::send_data(
    const Bus::Data_t payload,
    const unsigned char n_bytes
)
{
    // only when bus is not busy
    if (this->is_busy())
    {
        this->set_error(Error::Code::USART_BUS_Busy_Error);
        return false;
    }

    // transfer bytes
    for (unsigned char i=n_bytes; i>0; i--)
    {
        if(not this->send_data_byte(payload.byte[i - 1]))
            return false;
    }

    // transfer successful
    return true;
};

/**
 * @brief Send an array with n bytes to an USART target.
 * The first element in the array is transmitted first!
 * 
 * Sets the following errors:
 * - USART_Timeout
 * - USART_BUS_Busy
 * 
 * @tparam usart_instance The USART peripheral to be used.
 * @param data Address of array which contains the data
 * @param n_bytes How many bytes should be sent
 * @return Returns True when the array was sent successfully, False otherwise.
 * @details blocking function
 */
template <IO usart_instance>
bool USART::Controller<usart_instance>::send_array(const unsigned char* data,
                                              const unsigned char n_bytes)
{
    // only when bus is not busy!
    if (this->is_busy())
    {
        this->set_error(Error::Code::USART_BUS_Busy_Error);
        return false;
    }

    // send the bytes
    for (unsigned int i = 0; i < n_bytes; i++)
    {
        if(!this->send_data_byte(*(data + i)))
            return false;
    }
    return true;
};