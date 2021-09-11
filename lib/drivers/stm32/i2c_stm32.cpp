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
 * @file    i2c_stm32.cpp
 * @author  SO
 * @version v1.0.7
 * @date    02-September-2021
 * @brief   I2C driver for STM32 microcontrollers.
 ==============================================================================
 */

// === Includes ===
#include "i2c_stm32.h"

// === Needed Clock Information ===

/**
 * @brief Get the APB clock speed in MHz and check whether it is valid for the
 * i2c peripherl
 * @return Returns the defined APB clock speed in MHz.
 * @details constexpr
 */
constexpr unsigned long get_FREQ(void) 
{
    // Check whether F_APB is valid
    static_assert(F_APB > 2000000, "Minimum APB clock speed for I2C peripheral is 2 MHz!");
    static_assert(F_APB < 50000000, "Maximum APB clock speed for I2C peripheral is 50 MHz!");

    // Return F_APB in MHz
    return F_APB/1000000;
};

// === Functions ===

/**
 * @brief Get the i2c peripheral address of the desired instance
 * of the peripheral.
 * @param instance The I2C hardware instance to be used as the controller hardware
 * @return The address of the hardware instance.
 */
static unsigned long get_peripheral_address(const I2C::Instance instance)
{
    switch (instance)
    {
    case I2C::I2C_1:
        return I2C1_BASE;
    case I2C::I2C_2:
        return I2C2_BASE;
    case I2C::I2C_3:
        return I2C3_BASE;
    default:
        return 0;
    }
};

/**
 * @brief Calculate the control bits for the clock control
 * register of the i2c peripheral.
 * @param frequency The desired clock frequency in Hz.
 * @return The control bits for the CCR register.
 * @details For i2c frequencies below 100 kHz the SM mode is used.
 * For frequencies higher than 100 kHz, the FM mode with DUTY=1 is used.
 */
static unsigned long get_CCR(const unsigned long frequency)
{
    unsigned long reg_val = 0;
    // Set F/S and DUTY bits according to outputfrequency
    if (frequency <= 100000)
    {
        // get the CCR value
        reg_val = (get_FREQ()*1000000)/(2*frequency);
        // limit CCR to 0x01 since SM is enabled with this frequency
        if (reg_val < 0x04)
            reg_val = 0x04;
        // limit CCR value to 12 bits
        reg_val &= 0xFFF;
    }
    else
    {
        // get the CCR value
        reg_val = (get_FREQ()*1000000)/(25*frequency);
        // limit CCR value to 12 bits
        reg_val &= 0xFFF;
        reg_val |= I2C_CCR_FS | I2C_CCR_DUTY;
    }
    return reg_val;
};

/**
 * @brief Get the control bits to define the rise time.
 * @param rise_time_ns The desired maximum rise time in ns.
 * @return Returns the bits for the TRISE control register.
 */
static inline unsigned long get_TRISE(unsigned long rise_time_ns)
{
    return (rise_time_ns*get_FREQ()/1000)+1;
};

// === Methods ===

/**
 * @brief constructor for i2c controller object.
 * @param instance The I2C hardware instance to be used as the controller hardware
 * @param frequency The clock frequency for the i2c communication
 */
I2C::Controller::Controller(const I2C::Instance i2c_instance, const unsigned long frequency):
target(0)
{
    // Enable the peripheral clock
    RCC->APB1ENR |= (1<< (21 + i2c_instance));

    // set the peripheral address
    peripheral = reinterpret_cast<volatile I2C_TypeDef*>
        ( get_peripheral_address(i2c_instance) );

    // Set the configuration registers
    peripheral->CR1     = 0;
    peripheral->CR2     = get_FREQ();
    peripheral->CCR     = get_CCR(frequency);
    peripheral->TRISE   = get_TRISE(500); /** @todo Rise time is fixed to 500ns for now. Adjust that later. */
    peripheral->FLTR    = 0;

    // Enable a standard timeout of 100 calls
    set_timeout(100);
};

/**
 * @brief Set the address of the target for the next communication.
 * @param address The 7-bit address of the target.
 */
void I2C::Controller::set_target_address(const unsigned char address)
{
    // only save the first 7-bits of the address.
    // the rest of the driver assumes that the last bit of
    // the address is always zero.
    this->target = (address & 0xFE);
};

/**
 * @brief Set the clock pin as an output
 * @param output_pin The PIN object for the clock pin.
 */
void I2C::Controller::assign_pin(GPIO::PIN_Base& output_pin) const
{
    output_pin.setMode(GPIO::AF_Mode);
    output_pin.setType(GPIO::OPEN_DRAIN);
    output_pin.set_alternate_function(GPIO::I2C_1);
    /**
     * @todo For now it does not matter which I2C is used for the pin,
     * since all use the same alternate function code, but this could
     * change for other hardware...
     */
};

/**
 * @brief Enable the hardware peripheral.
 */
void I2C::Controller::enable(void)
{
    this->peripheral->CR1 |= I2C_CR1_PE;
};

/**
 * @brief Enable the hardware peripheral.
 */
void I2C::Controller::disable(void)
{
    this->peripheral->CR1 &= ~I2C_CR1_PE;
};

/**
 * @brief Write a byte to the output shift register.
 * @param data Data for the ouput register
 */
void I2C::Controller::write_data_register(const unsigned char data)
{
    this->peripheral->DR = data;
};

/**
 * @brief Generate a start condition on the bus.
 */
void I2C::Controller::generate_start(void)
{
    this->peripheral->CR1 |= I2C_CR1_START;
};

/**
 * @brief Generate a stop condition on the bus after the current byte transfer
 */
void I2C::Controller::generate_stop(void)
{
    this->peripheral->CR1 |= I2C_CR1_STOP;
};

/**
 * @brief Write the current target address to the output shift register.
 */
void I2C::Controller::write_address(void)
{
    this->write_data_register(this->target);
};

/**
 * @brief Send the set target address via the I2C bus.
 * Sets the following errors:
 * - I2C_Timeout
 * - I2C_Address_Error
 * @return Returns True when the address was sent successfully, False otherwise.
 * @details blocking function
 */
bool I2C::Controller::send_address(void)
{
    // Reset timeout
    this->reset_timeout();

    // Generate Start condition
    this->generate_start();
    while(!this->start_sent())
    {
        if(this->timed_out())
        {
            this->set_error(Error::I2C_Timeout);
            return false;
        }
    }

    // send the address
    this->write_address();

    // Wait for transmission and check whether target responded
    while(!this->address_sent())
    {
        if(!this->ack_received())
        {
            // Set the error
            this->set_error(Error::I2C_Address_Error);
            // reset the error flag and stop waiting
            return false;
        }
    }

    // Address was sent successfully. Return controller mode,
    // which should be true after sending the address.
    return this->in_controller_mode();    
};

/**
 * @brief Write a byte to the data register of the i2c peripheral
 * and wait until it is sent. Does not send an address on the bus.
 * Sets the following errors:
 * - I2C_Timeout
 * - I2C_Data_ACK_Error
 * @param data The byte to be sent
 * @return Returns True when the byte was sent successfully, False otherwise
 * @details blocking function
 */
bool I2C::Controller::send_data_byte(const unsigned char data)
{
    // Wait until the peripheral can except new data
    this->reset_timeout();
    while(!this->TX_register_empty())
    {
        // Check for timeouts
        if(this->timed_out())
        {
            this->set_error(Error::I2C_Timeout);
            return false;
        }
    }

    // write the byte
    this->write_data_register(data); 

    // Wait for the byte transfer
    while (!this->transfer_finished())
    {
        // Check for ack or nack
        if (!this->ack_received())
        {
            this->set_error(Error::I2C_Data_ACK_Error);
            return false;
        }

        // Check for timeouts
        if(this->timed_out())
        {
            this->set_error(Error::I2C_Timeout);
            return false;
        }
    }
    // Byte was sent successfully
    return true;
};

/**
 * @brief Send n bytes to an i2c target
 * Sets the following errors:
 * - I2C_Timeout
 * - I2C_Data_ACK_Error
 * @param payload Data struct with maximum 4 bytes
 * @param n_bytes How many bytes should be sent
 * @return Returns True when the byte was sent successfully, False otherwise.
 * @details blocking function
 */
bool I2C::Controller::send_data(const I2C::Data_t payload, 
    const unsigned char n_bytes)
{
    if(this->send_address())
    {
        for (unsigned char n_byte = n_bytes + 1; n_byte > 0; n_byte--)
            if(!this->send_data_byte(payload.byte[n_byte - 1]))
                return false;

        // After sending all bytes generate the stop condition
        this->generate_stop();
        return true;
    }
    return false;
};

/**
 * @brief Send a byte to an i2c target
 * Sets the following errors:
 * - I2C_Timeout
 * - I2C_Data_ACK_Error
 * @param data Byte in data representation to be sent
 * @return Returns True when the byte was sent successfully, False otherwise.
 * @details blocking function
 */
bool I2C::Controller::send_byte(const unsigned char data)
{
    // set the payload data
    I2C::Data_t payload;
    payload.byte[0] = data;

    // send the data
    return this->send_data(payload, 1);
};

/**
 * @brief Send a word to an i2c target
 * Sets the following errors:
 * - I2C_Timeout
 * - I2C_Data_ACK_Error
 * @param data Word (2 bytes) in data representation to be sent
 * @return Returns True when the byte was sent successfully, False otherwise.
 * @details blocking function
 */
bool I2C::Controller::send_word(const unsigned int data)
{
    // set the payload data
    I2C::Data_t payload;
    payload.word[0] = data;

    // send the data
    return this->send_data(payload, 2);
};

/**
 * @brief Return the currently active target address.
 * @return The 7-bit target address.
 */
unsigned char I2C::Controller::get_target_address(void) const
{
    return this->target;
};

/**
 * @brief Return the most recent received data.
 * @return Union which contains the received data. The union can be used
 * to use certain parts of the read values.
 */
I2C::Data_t I2C::Controller::get_rx_data(void) const
{
    I2C::Data_t val;
    val.value = 0;
    return val;
};

/**
 * @brief Check whether the hardware peripheral is in controller mode.
 * @return Returns true when the peripheral is controller mode and false when
 * it is in target mode.
 */
bool I2C::Controller::in_controller_mode(void) const
{
    return this->peripheral->SR2 & I2C_SR2_MSL;
};

/**
 * @brief Check whether the start condition was generated on the bus
 * by the controller hardware.
 * @return Returns true when start condition was generated.
 */
bool I2C::Controller::start_sent(void) const
{
    return this->peripheral->SR1 & I2C_SR1_SB;
};

/**
 * @brief Check whether the target address was sent successfully on the bus.
 * @return Returns true when address transmission is complete.
 */
bool I2C::Controller::address_sent(void) const
{
    return this->peripheral->SR1 & I2C_SR1_ADDR;
};

/**
 * @brief Check whether an acknowledge was received. When no communication is ongoing
 * on the bus this will always return true. Only when an nack was received this will
 * become false.
 * @return Returns true when an acknowledge was received in the current transaction.
 */
bool I2C::Controller::ack_received(void) const
{
    // Check whether nack was received and clear error
    bool nack = (this->peripheral->SR1 & I2C_SR1_AF);
    this->peripheral->SR1 &= ~I2C_SR1_AF;
    
    // Return whether there was NO acknowledge failure
    return !nack;
};

/**
 * @brief Check whether the TX data shift register is empty and
 * can receive new data.
 * @returns Returns True when new data can be written to the register.
 */
bool I2C::Controller::TX_register_empty(void) const
{
    return this->peripheral->SR1 & I2C_SR1_TXE;
};

/**
 * @brief Check whether the transfer of the current byte is finished.
 * @returns Returns True when the transfer is finished.
 */
bool I2C::Controller::transfer_finished(void) const
{
    return this->peripheral->SR1 & I2C_SR1_BTF;
};