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
 * @file    sdio_stm32.cpp
 * @author  SO
 * @version v2.5.0
 * @date    29-Dezember-2021
 * @brief   SDIO driver for STM32 microcontrollers.
 ==============================================================================
 */

// === Includes ===
#include "sdio_stm32.h"

#ifdef STM32F4
// === Helper functions ===

// === Methods ===
/**
 * @brief Construct a new SDIO controller object
 * 
 * @param clock_rate The desired bus clock in [Hz].
 */
SD::Controller::Controller(const unsigned long clock_rate)
    : Base{IO::SDIO_}
    , peripheral{ reinterpret_cast<SDIO_TypeDef *>(SDIO_BASE) }
{
    // Enable the clock of the peripheral
    RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;

    // Set the initial clock rate
    this->set_clock(clock_rate);

    // Set the timeout limit -> This value is high, because
    // during the card identification the clock speed 
    // has to be 400 kHz.
    this->set_timeout(65000);
};

/**
 * @brief Set the clock rate for the bus communication.
 * 
 * @param clock_rate The clock rate in [Hz]. Should be [400 kHz ... 25 MHz] for SDHC-Cards.
 * @param enable_power_save If True the clock is only enabled when bus is active.
 */
void SD::Controller::set_clock(
    const unsigned long clock_rate,
    const bool enable_save_power
)
{
    // Get the prescaler value with its 8 significant bits
    // -> SDIO clock is ALWAYS 48 MHz
    const unsigned char prescaler = ((48'000'000 / clock_rate) - 2);
    
    // Save the old register value without the prescaler bits
    const unsigned long register_old = (this->peripheral->CLKCR & 0b1111101000000000);

    // Set the new clock configuration
    this->peripheral->CLKCR  = register_old | prescaler;
    if (enable_save_power)
        this->peripheral->CLKCR |= SDIO_CLKCR_PWRSAV;
};

/**
 * @brief Set the bus width for the communciation.
 * 
 * @param width The width to use.
 */
void SD::Controller::set_bus_width( const Width width)
{
    // Save the old register value without the prescaler bits
    const unsigned long register_old = (this->peripheral->CLKCR & ~SDIO_CLKCR_WIDBUS_Msk);

    // Set the new clock configuration
    this->peripheral->CLKCR = register_old | (static_cast<unsigned char>(width) << SDIO_CLKCR_WIDBUS_Pos);
};

/**
 * @brief Set the hardware timeout during data transfer in
 * SDIO clock cycles.
 * 
 * @param sdio_ticks The hardware timeout value.
 */
void SD::Controller::set_hardware_timeout( const unsigned long sdio_ticks )
{
    this->peripheral->DTIMER = sdio_ticks;
};

/**
 * @brief Set the number of bytes to transfer in one block.
 * 
 * @param number_bytes The number of bytes to transfer during one block transfer.
 */
void SD::Controller::set_data_length( const unsigned long number_bytes )
{
    this->peripheral->DLEN = number_bytes;
};

/**
 * @brief Enable the peripheral and the bus.
 * 
 */
void SD::Controller::enable(void)
{
    // Enable the bus block
    this->peripheral->CLKCR |= SDIO_CLKCR_CLKEN;

    // Enable the power
    this->peripheral->POWER = 0b11; 
};

/**
 * @brief Check whether command was sent, when no
 * response is required.
 */
bool SD::Controller::command_sent(void)
{
    return this->peripheral->STA & SDIO_STA_CMDSENT;
};

/**
 * @brief Check whether a response for the active
 * command was received.
 */
bool SD::Controller::command_response_received(void)
{
    return this->peripheral->STA & SDIO_STA_CMDREND;
};

/**
 * @brief Check whether the CRC check of response for the active
 * command failed.
 */
bool SD::Controller::command_response_crc_fail(void)
{
    return this->peripheral->STA & SDIO_STA_CCRCFAIL;
};

/**
 * @brief Check whether current data block
 * transfer is finished. 
 */
bool SD::Controller::data_block_transfer_finished(void)
{
    return this->peripheral->STA & SDIO_STA_DBCKEND;
};

/**
 * @brief Check whether RX FIFO contains valid data.
 */
bool SD::Controller::data_RX_available(void)
{
    return this->peripheral->STA & SDIO_STA_RXDAVL;
};

/**
 * @brief Check whether TX FIFO is empty.
 */
bool SD::Controller::data_TX_empty(void)
{
    return this->peripheral->STA & SDIO_STA_TXFIFOE;
};


/**
 * @brief Check whether communication is ongoing
 * on the bus.
 */
bool SD::Controller::is_busy(void)
{
    return this->peripheral->STA & (SDIO_STA_CMDACT | SDIO_STA_RXACT | SDIO_STA_TXACT);
};

/**
 * @brief Check whether a hardware timeout occurred.
 */
bool SD::Controller::hardware_timeout(void)
{
    return this->peripheral->STA & (SDIO_STA_DTIMEOUT | SDIO_STA_CTIMEOUT);
};

/**
 * @brief Clear the status flags for command transfer
 * which do not clear automatically.
 */
void SD::Controller::clear_command_flags(void)
{
    this->peripheral->ICR = (SDIO_ICR_CMDSENTC | SDIO_ICR_CMDRENDC | SDIO_ICR_CCRCFAILC);
};

/**
 * @brief Clear the status flags for data transfer
 * which do not clear automatically.
 */
void SD::Controller::clear_data_flags(void)
{
    this->peripheral->ICR = (SDIO_ICR_DBCKENDC | SDIO_ICR_DATAENDC);
};

/**
 * @brief Clear all error flags
 */
void SD::Controller::clear_error_flags(void)
{
    this->peripheral->ICR = SDIO_ICR_RXOVERRC | SDIO_ICR_TXUNDERRC | SDIO_ICR_DTIMEOUTC | SDIO_ICR_CTIMEOUTC | SDIO_ICR_DCRCFAILC | SDIO_ICR_CCRCFAILC;
};

/**
 * @brief Send a command without an expected response.
 * 
 * Sets the following errors:
 * - SDIO_Timeout
 * - SDIO_BUS_Busy_Error
 * 
 * @param command  The command number to send.
 * @param argument The argument of the command
 * @return Returns True when the command was sent successfully.
 * @details blocking-function
 */
bool SD::Controller::send_command_no_response(
    const unsigned char command,
    const unsigned long argument
)
{
    // Only when bus is not busy
    if(this->is_busy())
    {
        this->set_error(Error::Code::SDIO_BUS_Busy_Error);
        return false;
    };

    // Start Command transfer
    this->peripheral->ARG = argument;
    this->peripheral->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | (command & 0b111111);

    // Wait for transfer to finish
    this->reset_timeout();
    while(!this->command_sent())
    {
        if(this->hardware_timeout() || this->timed_out() )
        {
            this->set_error(Error::Code::SDIO_Timeout);
            return false;
        }
    }

    // transfer finished, clear flags and return
    this->clear_command_flags();
    return true;
};

/**
 * @brief Send a command with an expected R1 response.
 * 
 * Sets the following errors:
 * - SDIO_Timeout
 * - SDIO_BUS_Busy_Error
 * 
 * @param command  The command number to send.
 * @param argument The argument of the command
 * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
 * @details blocking-function
 */
std::optional<unsigned long> SD::Controller::send_command_R1_response(
    const unsigned char command,
    const unsigned long argument
)
{
    // Only when bus is not busy
    if(this->is_busy())
    {
        this->set_error(Error::Code::SDIO_BUS_Busy_Error);
        return {};
    };

    // Start Command transfer
    this->peripheral->ARG = argument;
    this->peripheral->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | SDIO_CMD_WAITRESP_0 | (command & 0b111111);

    // Wait for transfer to finish
    this->reset_timeout();
    while(!this->command_response_received())
    {
        if(this->hardware_timeout() || this->timed_out() )
        {
            this->set_error(Error::Code::SDIO_Timeout);
            return {};
        }
    }

    // transfer finished, clear flags and return
    this->clear_command_flags();
    return this->peripheral->RESP1;
};

/**
 * @brief Send a command with an expected R2 response.
 * 
 * Sets the following errors:
 * - SDIO_Timeout
 * - SDIO_BUS_Busy_Error
 * 
 * @param command  The command number to send.
 * @param argument The argument of the command
 * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
 * @details blocking-function
 */
std::optional<unsigned long> SD::Controller::send_command_R2_response(
    const unsigned char command,
    const unsigned long argument
)
{
    // Only when bus is not busy
    if(this->is_busy())
    {
        this->set_error(Error::Code::SDIO_BUS_Busy_Error);
        return {};
    };

    // Start Command transfer
    this->peripheral->ARG = argument;
    this->peripheral->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL  | SDIO_CMD_WAITRESP_1 | SDIO_CMD_WAITRESP_0 | (command & 0b111111);

    // Wait for transfer to finish
    this->reset_timeout();
    while(!this->command_response_received())
    {
        if(this->hardware_timeout() || this->timed_out() )
        {
            this->set_error(Error::Code::SDIO_Timeout);
            return {};
        }
    }

    // transfer finished, clear flags and return
    this->clear_command_flags();
    return this->peripheral->RESP1;
};

/**
 * @brief Send a command with an expected R3 response.
 * 
 * Sets the following errors:
 * - SDIO_Timeout
 * - SDIO_BUS_Busy_Error
 * 
 * @param command  The command number to send.
 * @param argument The argument of the command
 * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
 * @details blocking-function
 */
std::optional<unsigned long> SD::Controller::send_command_R3_response(
    const unsigned char command,
    const unsigned long argument
)
{
    // Only when bus is not busy
    if(this->is_busy())
    {
        this->set_error(Error::Code::SDIO_BUS_Busy_Error);
        return {};
    };

    // Start Command transfer
    this->peripheral->ARG = argument;
    this->peripheral->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | SDIO_CMD_WAITRESP_0 | (command & 0b111111);

    // Wait for transfer to finish
    this->reset_timeout();
    while(!this->command_response_crc_fail() && !this->command_response_received())
    {
        if(this->hardware_timeout() || this->timed_out() )
        {
            this->set_error(Error::Code::SDIO_Timeout);
            return {};
        }
    }

    // transfer finished, clear flags and return
    this->clear_command_flags();
    return this->peripheral->RESP1;
};

/**
 * @brief Send a command with an expected R6 response.
 * 
 * Sets the following errors:
 * - SDIO_Timeout
 * - SDIO_BUS_Busy_Error
 * 
 * @param command  The command number to send.
 * @param argument The argument of the command
 * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
 * @details blocking-function
 */
std::optional<unsigned long> SD::Controller::send_command_R6_response(
    const unsigned char command,
    const unsigned long argument
)
{
    return this->send_command_R1_response(command, argument);
};

/**
 * @brief Send a command with an expected R7 response.
 * 
 * Sets the following errors:
 * - SDIO_Timeout
 * - SDIO_BUS_Busy_Error
 * 
 * @param command  The command number to send.
 * @param argument The argument of the command
 * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
 * @details blocking-function
 */
std::optional<unsigned long> SD::Controller::send_command_R7_response(
    const unsigned char command,
    const unsigned long argument
)
{
    return this->send_command_R1_response(command, argument);
};

/**
 * @brief Read a data block.
 * The length of the block is defined by the length of the
 * given buffer.
 * 
 * The length of the buffer should be 2^N!
 * 
 * @param buffer_begin The begin iterator of the receive buffer.
 * @param buffer_end The end of the receive buffer.
 * @return Return True when the block was read successfully.
 */
bool SD::Controller::read_single_block(
    const unsigned long* buffer_begin,
    const unsigned long* buffer_end
)
{
    // Only when bus is not busy
    if (this->is_busy())
    {
        this->set_error(Error::Code::SDIO_BUS_Busy_Error);
        return false;
    }

    /*
    * Get the numer of bytes to transfer and since this is 
    * a block transfer the exponent of the number of bytes
    * for the DCTRL register.
    * The buffer has 4 bytes per entry -> n_bytes = 4 * len(buffer)
    */
    const unsigned long n_bytes = std::distance(buffer_begin, buffer_end) * 4;

    // Calculate the exponent of the byte length
    unsigned char byte_exponent = 0;
    switch (n_bytes)
    {
        case 1: byte_exponent = 0; break;
        case 2: byte_exponent = 1; break;
        case 4: byte_exponent = 2; break;
        case 8: byte_exponent = 3; break;
        case 16: byte_exponent = 4; break;
        case 32: byte_exponent = 5; break;
        case 64: byte_exponent = 6; break;
        case 128: byte_exponent = 7; break;
        case 256: byte_exponent = 8; break;
        case 512: byte_exponent = 9; break;
        case 1024: byte_exponent = 10; break;
        case 2048: byte_exponent = 11; break;
        case 4096: byte_exponent = 12; break;
        case 8192: byte_exponent = 13; break;
        case 16384: byte_exponent = 14; break;
        default:
            return false;
    }

    // Set the data length in bytes.
    this->set_data_length(n_bytes);

    // Start transfer
    this->peripheral->DCTRL = (byte_exponent << 4) | SDIO_DCTRL_DTDIR | SDIO_DCTRL_DTEN;

    // Wait for transfer to finish
    unsigned long* iter = const_cast<unsigned long*>(buffer_begin);
    this->reset_timeout();
    while(not this->data_block_transfer_finished())
    {
        // Check for timeouts
        if(this->hardware_timeout() || this->timed_out() )
        {
            this->set_error(Error::Code::SDIO_Timeout);
            return false;
        }

        // Save the FIFO data
        if(this->data_RX_available())
        {
            *iter = this->peripheral->FIFO;
            iter++;
        }
    }

    // Transer finished without errors, clear flags and exit.
    this->clear_data_flags();
    return true;
};

/**
 * @brief Write a data block.
 * The length of the block is defined by the length of the
 * given buffer.
 * 
 * The length of the buffer should be 2^N!
 * 
 * @param buffer_begin The begin iterator of the transmit data.
 * @param buffer_end The end of the transmit buffer.
 * @return Return True when the block was sent successfully.
 */
bool SD::Controller::write_single_block(
    const unsigned long* buffer_begin,
    const unsigned long* buffer_end
)
{
    // Only when bus is not busy
    if (this->is_busy())
    {
        this->set_error(Error::Code::SDIO_BUS_Busy_Error);
        return false;
    }

    /*
    * Get the numer of bytes to transfer and since this is 
    * a block transfer the exponent of the number of bytes
    * for the DCTRL register.
    * The buffer has 4 bytes per entry -> n_bytes = 4 * len(buffer)
    */
    const unsigned long n_bytes = (buffer_end - buffer_begin) * 4;

    // Calculate the exponent of the byte length
    unsigned char byte_exponent = 0;
    switch (n_bytes)
    {
        case 1: byte_exponent = 0; break;
        case 2: byte_exponent = 1; break;
        case 4: byte_exponent = 2; break;
        case 8: byte_exponent = 3; break;
        case 16: byte_exponent = 4; break;
        case 32: byte_exponent = 5; break;
        case 64: byte_exponent = 6; break;
        case 128: byte_exponent = 7; break;
        case 256: byte_exponent = 8; break;
        case 512: byte_exponent = 9; break;
        case 1024: byte_exponent = 10; break;
        case 2048: byte_exponent = 11; break;
        case 4096: byte_exponent = 12; break;
        case 8192: byte_exponent = 13; break;
        case 16384: byte_exponent = 14; break;
        default:
            return false;
    }

    // Set the data length in bytes.
    this->set_data_length(n_bytes);

    // Start transfer
    this->peripheral->DCTRL = (byte_exponent << 4) | SDIO_DCTRL_DTEN;

    // Wait for transfer to finish
    unsigned long* iter = const_cast<unsigned long*>(buffer_begin);
    this->reset_timeout();
    while(not this->data_block_transfer_finished())
    {
        // Check for timeouts
        if(this->hardware_timeout() || this->timed_out() )
        {
            this->set_error(Error::Code::SDIO_Timeout);
            return false;
        }

        // Save the FIFO data
        if(this->data_TX_empty())
        {
            this->peripheral->FIFO = *iter;
            iter++;
        }
    }

    // Transer finished without errors, clear flags and exit.
    this->clear_data_flags();
    return true;
};
#endif