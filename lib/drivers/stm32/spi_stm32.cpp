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
 * @file    spi_stm32.cpp
 * @author  SO
 * @version v2.2.2
 * @date    22-Dezember-2021
 * @brief   SPI driver for STM32 microcontrollers.
 ==============================================================================
 */

// === Includes ===
#include "spi_stm32.h"

// Provide template instanciations with all allowed bus instances
template class SPI::Controller<IO::SPI_1>;
template class SPI::Controller<IO::SPI_2>;
#ifdef STM32F4
template class SPI::Controller<IO::SPI_3>;
template class SPI::Controller<IO::SPI_4>;
template class SPI::Controller<IO::SPI_5>;
template class SPI::Controller<IO::SPI_6>;
#endif

// === Helper functions ===
namespace SPI
{
    /**
 * @brief Check whether the provided IO is a SPI peripheral
 * 
 * @tparam instance Provided IO instance 
 * @return true Instance is a valid SPI Peripheral
 * @return false Instance is not a valid SPI Peripheral
 */
    template <IO instance>
    constexpr bool is_valid_spi()
    {
        return (
            (instance == IO::SPI_1) || (instance == IO::SPI_2)
#ifdef STM32F4
            || (instance == IO::SPI_3) || (instance == IO::SPI_4) || (instance == IO::SPI_5) || (instance == IO::SPI_6)
#endif
        );
    };

    /**
 * @brief Get the peripheral address
 * 
 * @tparam instance SPI IO peripheral
 * @return constexpr unsigned long Address of the SPI peripheral
 */
    template <IO instance>
    constexpr std::uintptr_t get_peripheral_address()
    {
        // Check for valid SPI
        static_assert(is_valid_spi<instance>(), "IO Instance is not a valid SPI peripheral!");

        // Return the peripheral address
        if constexpr (instance == IO::SPI_1)
            return SPI1_BASE;
        if constexpr (instance == IO::SPI_2)
            return SPI2_BASE;
#ifdef STM32F4
        if constexpr (instance == IO::SPI_3)
            return SPI3_BASE;
        if constexpr (instance == IO::SPI_4)
            return SPI4_BASE;
        if constexpr (instance == IO::SPI_5)
            return SPI5_BASE;
        if constexpr (instance == IO::SPI_6)
            return SPI6_BASE;
#endif
    };

    /**
     * @brief Enable the coresponding clock in the RCC register.
     * 
     * @tparam instance SPI peripheral instance.
     */
    template <IO instance>
    void enable_rcc_clock()
    {
        if constexpr (instance == IO::SPI_1)
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
        if constexpr (instance == IO::SPI_2)
            RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
#ifdef STM32F4
        if constexpr (instance == IO::SPI_3)
            RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
        if constexpr (instance == IO::SPI_4)
            RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
        if constexpr (instance == IO::SPI_5)
            RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
        if constexpr (instance == IO::SPI_6)
            RCC->APB2ENR |= RCC_APB2ENR_SPI6EN;
#endif
    }

    /**
     * @brief Get the prescaler for teh desired baudrate.
     * The prescaler value depends on the APB clock which
     * is conencted to the SPI peripheral.
     * 
     * @tparam instance SPI peripheral instance.
     * @param baudrate The desired baudrate of the bus.
     * @return Returns the 3 significant bits for the prescaler.
     */
    template<IO instance>
    constexpr unsigned char calculate_prescaler(const unsigned long baudrate)
    {
        // Variable for calculation
        unsigned long prescaler = 0;

        // Get the prescaler for the according peripheral
        if constexpr (instance == IO::SPI_1)
            prescaler = (F_APB2 / baudrate);
        if constexpr (instance == IO::SPI_2)
            prescaler = (F_APB1 / baudrate);
#ifdef STM32F4
        if constexpr (instance == IO::SPI_3)
            prescaler = (F_APB1 / baudrate);
        if constexpr (instance == IO::SPI_4)
            prescaler = (F_APB2 / baudrate);
        if constexpr (instance == IO::SPI_5)
            prescaler = (F_APB2 / baudrate);
        if constexpr (instance == IO::SPI_6)
            prescaler = (F_APB2 / baudrate);
#endif
        // Compute the exponent: prescaler = 2^(N+1)
        // See how many times 2 fits in prescaler
        if (prescaler <= (1<<1))
            return 0;
        else if (prescaler <= (1<<2))
            return 1; 
        else if (prescaler <= (1<<3))
            return 2;
        else if (prescaler <= (1<<4))
            return 3;
        else if (prescaler <= (1<<5))
            return 4;
        else if (prescaler <= (1<<6))
            return 5;
        else if (prescaler <= (1<<7))
            return 6;
        else 
            return 7;
    }
};

// === Methods ===
/**
 * @brief Construct a new spi controller object
 * Only integer fractions of the CPU clock are possible for the baudrate!
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 * @param baudrate The desired bus baudrate in [Hz].
 */
template <IO spi_instance>
SPI::Controller<spi_instance>::Controller(const unsigned long baudrate)
    : Base{spi_instance}
    , peripheral{ reinterpret_cast<SPI_TypeDef *>(SPI::get_peripheral_address<spi_instance>())}
{
    // Enable the peripheral clock
    SPI::enable_rcc_clock<spi_instance>();

    // Calculate prescaler (3 significant bits) for the desired baudrate
    unsigned char prescaler = calculate_prescaler<spi_instance>(baudrate);

    // Set the configuration register
    this->peripheral->CR1 &= ~(SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0); // Clear BR bits
    this->peripheral->CR1 |= (prescaler << 3) | SPI_CR1_MSTR;

    // Set the timeout limit
    this->set_timeout(100);
};

/**
 * @brief Set the timing parameters of the SPI clock SCK.
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 * @param idle The logic level of the clock when bus is idle.
 * @param data_valid The clock edge when the data on the bus is valid.
 */
template <IO spi_instance>
void SPI::Controller<spi_instance>::set_clock_timing(const Level idle, const Edge data_valid)
{
    // Read old register value and delete the CPOL and CPHA bits
    unsigned int reg = this->peripheral->CR1 & ~(SPI_CR1_CPHA | SPI_CR1_CPOL);

    // Set the new bits
    if (idle == Level::High)
        reg |= SPI_CR1_CPOL;
    if ((idle == Level::High) xor (data_valid == Edge::Falling))
        reg |= SPI_CR1_CPHA;

    // Assign new register content
    this->peripheral->CR1 = reg;
};

/**
 * @brief Select whether to use a hardware pin as the chip select.
 * The alternate function of the hardware pin has to be assigned to it separately!
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 * @param use_hardware Set to true when a hardware chip select should be used.
 */
template <IO spi_instance>
void SPI::Controller<spi_instance>::set_use_hardware_chip_select(const bool use_hardware)
{
    // Enable hardware mode
    this->peripheral->CR1 &= ~(SPI_CR1_SSM | SPI_CR1_SSI);
    this->peripheral->CR2 |= SPI_CR2_SSOE;

    // Enable software mode
    if (not use_hardware)
    {
        this->peripheral->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);
        this->peripheral->CR2 &= ~SPI_CR2_SSOE;
    }
};

/**
 * @brief Enable the SPI peripheral
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 */
template <IO spi_instance>
void SPI::Controller<spi_instance>::enable(void)
{
    this->peripheral->CR1 |= SPI_CR1_SPE;
};

/**
 * @brief Disable the SPI peripheral
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 */
template <IO spi_instance>
void SPI::Controller<spi_instance>::disable(void)
{
    this->peripheral->CR1 &= ~SPI_CR1_SPE;
};

/**
 * @brief Check whether the TX data buffer is empty and can receive new data.
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 * @returns Returns True when new data can be written to the register.
 */
template <IO spi_instance>
bool SPI::Controller<spi_instance>::last_transmit_finished(void) const
{
    return this->peripheral->SR & SPI_SR_TXE;
};

/**
 * @brief Check whether the SPI peripheral is busy.
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 * @returns Returns True when the peripheral is busy.
 */
template <IO spi_instance>
bool SPI::Controller<spi_instance>::is_busy(void) const
{
    return this->peripheral->SR & SPI_SR_BSY;
};

/**
 * @brief Send a byte onto the SPI bus. Only when the TX buffer
 * is empty!
 * Sets the following errors:
 * - SPI_Timeout
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 * @param data The byte to send
 * @return Returns True when the byte was send successfully.
 * @details blocking function
 */
template <IO spi_instance>
bool SPI::Controller<spi_instance>::send_data_byte(const unsigned char data)
{

    // Wait until TX buffer is empty
    this->reset_timeout(); 
    while ( not this->last_transmit_finished() )
    {
        // Check for timeouts
        if(this->timed_out())
        {
            // Peripheral timed out -> set error
            this->set_error(Error::Code::SPI_Timeout);
            return false;
        }
    }
    // Send data when DR is empty
    this->peripheral->DR = data;
    
    // return success
    return true;
};

/**
 * @brief Send n bytes to a spi target
 * The highest byte in the payload is transmitted first!
 * 
 * Sets the following errors:
 * - SPI_Timeout
 * - SPI_BUS_Busy
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 * @param payload Data struct with maximum 4 bytes
 * @param n_bytes How many bytes should be sent
 * @return Returns True when the data was sent successfully, False otherwise.
 * @details blocking function
 */
template <IO spi_instance>
bool SPI::Controller<spi_instance>::send_data(const Bus::Data_t payload,
                                              const unsigned char n_bytes)
{
    // only when bus is not busy!
    if (this->is_busy())
    {
        this->set_error(Error::Code::SPI_BUS_Busy_Error);
        return false;
    }

    // send the bytes
    for (unsigned int i = n_bytes; i > 0; i--)
    {
        if(!this->send_data_byte(payload.byte[i-1]))
            return false;
    }

    // Wait for transfer to finish
    this->reset_timeout();
    while(this->is_busy())
    {
        if (this->timed_out())
        {
            this->set_error(Error::Code::SPI_Timeout);
            return false;
        }
    }

    // Send was successfull
    return true;
};

/**
 * @brief Send an array with n bytes to a spi target.
 * The first element in the array is transmitted first!
 * 
 * Sets the following errors:
 * - SPI_Timeout
 * - SPI_BUS_Busy
 * 
 * @tparam spi_instance The SPI peripheral to be used.
 * @param data Address of array which contains the data
 * @param n_bytes How many bytes should be sent
 * @return Returns True when the array was sent successfully, False otherwise.
 * @details blocking function
 */
template <IO spi_instance>
bool SPI::Controller<spi_instance>::send_array(const unsigned char* data,
                                              const unsigned char n_bytes)
{
    // only when bus is not busy!
    if (this->is_busy())
    {
        this->set_error(Error::Code::SPI_BUS_Busy_Error);
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