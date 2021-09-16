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
 * @version v1.0.10
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
    // Check whether peripheral clock frequency is valid is valid
    static_assert(F_I2C > 2000000, "Minimum APB clock speed for I2C peripheral is 2 MHz!");
    static_assert(F_I2C < 50000000, "Maximum APB clock speed for I2C peripheral is 50 MHz!");

    // Return F_I2C in MHz
    return F_I2C/1000000;
};

/**
 * @brief Calculate the optimal prescaler value for STM32L0 devices
 * @param frequency The output frequency for the bus
 * @return Returns the 4-bit prescaler value.
 */
constexpr unsigned char get_prescaler(const unsigned long frequency)
{
    return (1 + ((F_I2C / frequency) / 255)) & 0b1111;
};

/**
 * @brief Get the control bits to define the rise time.
 * @param rise_time_ns The desired maximum rise time in ns.
 * @return Returns the bits for the TRISE control register.
 */
constexpr unsigned long get_TRISE(const unsigned long rise_time_ns)
{
    return (rise_time_ns*get_FREQ()/1000)+1;
};

// === Functions ===

/**
 * @brief Get the i2c peripheral address of the desired instance
 * of the peripheral.
 * @param instance The I2C hardware instance to be used as the controller hardware
 * @return The address of the hardware instance.
 */
constexpr unsigned long get_peripheral_address(const I2C::Instance instance) {
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
 * @return The control bits for the timing ot clock control register.
 * @details For i2c frequencies below 100 kHz the SM mode is used.
 * For frequencies higher than 100 kHz, the FM mode with DUTY=1 is used (STM32F4xx).
 */
constexpr unsigned long get_clock_control(const unsigned long frequency) {
    unsigned long reg_val = 0;
#if defined(STM32F4)
    // Set F/S and DUTY bits according to output frequency
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
#elif defined(STM32L0)
    unsigned char _pre      = get_prescaler(frequency);
    unsigned char _pre_l    = ((F_I2C / (_pre * frequency)) / 2) & 0b11111111; 
    unsigned char _pre_h    = ((F_I2C / (_pre * frequency)) / 2) & 0b11111111; 
    reg_val = (--_pre << 28) | (--_pre_h << 8) | (--_pre_l << 0);
#endif
    return reg_val;
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
#if defined(STM32F4)
    unsigned long _reg = (1<< (21 + i2c_instance)); 
#elif defined(STM32L0)
    unsigned long _reg = 0;
    switch (i2c_instance)
    {
    case I2C::I2C_1:
        _reg = RCC_APB1ENR_I2C1EN;
        break;
    case I2C::I2C_2:
        _reg = RCC_APB1ENR_I2C2EN;
        break;
    case I2C::I2C_3:
        _reg = RCC_APB1ENR_I2C3EN;
        break;
    };
#endif
    RCC->APB1ENR |= _reg;

    // set the peripheral address
    peripheral = reinterpret_cast<volatile I2C_TypeDef*>
        ( get_peripheral_address(i2c_instance) );

    // Set the configuration registers
    peripheral->CR1     = 0;
#if defined(STM32F4)
    peripheral->CR2     = get_FREQ();
    peripheral->CCR     = get_clock_control(frequency);
    peripheral->TRISE   = get_TRISE(500); /** @todo Rise time is fixed to 500ns for now. Adjust that later. */
    peripheral->FLTR    = 0;
#elif defined(STM32L0)
    peripheral->CR2     = 0;
    peripheral->TIMINGR = get_clock_control(frequency);
    peripheral->TIMEOUTR = 0;
#endif

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

    // Get the peripheral which is used, since the AF modes depend on that
    if (this->peripheral == I2C1)
        output_pin.set_alternate_function(GPIO::I2C_1);
    else if (this->peripheral == I2C2)
        output_pin.set_alternate_function(GPIO::I2C_2);
    else
        output_pin.set_alternate_function(GPIO::I2C_3);
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
#if defined(STM32F4)
    this->peripheral->DR = data;
#elif defined(STM32L0)
    this->peripheral->TXDR = data;
#endif
};

/**
 * @brief Generate a start condition on the bus.
 */
void I2C::Controller::generate_start(void)
{
#if defined(STM32F4)
    this->peripheral->CR1 |= I2C_CR1_START;
#elif defined(STM32L0)
    this->peripheral->CR2 |= I2C_CR2_START;
#endif
};

/**
 * @brief Generate a stop condition on the bus after the current byte transfer
 */
void I2C::Controller::generate_stop(void)
{
#if defined(STM32F4)
    this->peripheral->CR1 |= I2C_CR1_STOP;
#elif defined(STM32L0)
    this->peripheral->CR2 |= I2C_CR2_STOP;
#endif
};

/**
 * @brief Write the current target address to the output shift register.
 */
void I2C::Controller::write_address(void)
{
#ifdef STM32F4
    this->write_data_register(this->target);
#endif
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
            this->set_error(Error::Code::I2C_Timeout);
            return false;
        }
    }

    // send the address
    this->write_address();

    // Wait for transmission and check whether target responded
#if defined(STM32F4)
    while(!this->address_sent())
#elif defined(STM32L0)
    // the L0 devices set the TXIS flag after an successfull
    // address transmission
    while(!this->transfer_finished())
#endif
    {
        if(!this->ack_received())
        {
            // Set the error
            this->set_error(Error::Code::I2C_Address_Error);
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
    this->reset_timeout();

    // Wait until the peripheral can except new data
    while(!this->TX_register_empty())
    {
        // Check for timeouts
        if(this->timed_out())
        {
            this->set_error(Error::Code::I2C_Timeout);
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
            this->set_error(Error::Code::I2C_Data_ACK_Error);
            return false;
        }

        // Check for timeouts
        if(this->timed_out())
        {
            this->set_error(Error::Code::I2C_Timeout);
            return false;
        }
    }
    // Byte was sent successfully
    return true;
};

/**
 * @brief Send n bytes to an i2c target
 * The highest byte in the struct is transmitted first!
 * Sets the following errors:
 * - I2C_Timeout
 * - I2C_Data_ACK_Error
 * - I2C_BUS_Busy
 * @param payload Data struct with maximum 4 bytes
 * @param n_bytes How many bytes should be sent
 * @return Returns True when the byte was sent successfully, False otherwise.
 * @details blocking function
 */
bool I2C::Controller::send_data(const I2C::Data_t payload, 
    const unsigned char n_bytes)
{
    // Only start transfer when bus is idle
    if(!this->bus_busy())
    {
#ifdef STM32L0
        // Set the number of bytes and target address
        this->peripheral->CR2 = (n_bytes << 16) | this->target;
#endif

        if(this->send_address())
        {
            for (unsigned char n_byte = n_bytes; n_byte > 0; n_byte--)
                if(!this->send_data_byte(payload.byte[n_byte - 1]))
                    return false;

            // After sending all bytes generate the stop condition
            this->generate_stop();
            return true;
        }
        return false;
    }
    else
    {
        this->set_error(Error::Code::I2C_BUS_Busy_Error);
        return false;
    }
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
 * @brief Send an array with n bytes to an i2c target.
 * The first element in the array is transmitted first!
 * Sets the following errors:
 * - I2C_Timeout
 * - I2C_Data_ACK_Error
 * - I2C_BUS_Busy
 * @param data Address of array which contains the data
 * @param n_bytes How many bytes should be sent
 * @return Returns True when the array was sent successfully, False otherwise.
 * @details blocking function
 */
bool I2C::Controller::send_array(const unsigned char* data, 
    const unsigned char n_bytes)
{
    // Only start transfer when bus is idle
    if(!this->bus_busy())
    {
#ifdef STM32L0
        // Set the number of bytes and target address
        this->peripheral->CR2 = (n_bytes << 16) | this->target;
#endif

        if(this->send_address())
        {
            for (unsigned char n_byte = 0; n_byte < n_bytes; n_byte++)
                if(!this->send_data_byte(data[n_byte]))
                    return false;

            // After sending all bytes generate the stop condition
            this->generate_stop();
            return true;
        }
        return false;
    }
    else
    {
        this->set_error(Error::Code::I2C_BUS_Busy_Error);
        return false;
    }
};

/**
 * @brief Send an array with n bytes to an i2c target.
 * Includes a leading byte in front of the array data.
 * The first element in the array is transmitted first!
 * Sets the following errors:
 * - I2C_Timeout
 * - I2C_Data_ACK_Error
 * - I2C_BUS_Busy
 * @param byte The leading byte in front of the array
 * @param data Address of array which contains the data
 * @param n_bytes How many bytes should be sent
 * @return Returns True when the array was sent successfully, False otherwise.
 * @details blocking function
 */
bool I2C::Controller::send_array_leader(const unsigned char byte,
    const unsigned char* data, 
    const unsigned char n_bytes)
{
    // Only start transfer when bus is idle
    if(!this->bus_busy())
    {
#ifdef STM32L0
        // Set the number of bytes (array + leading byte) and target address
        this->peripheral->CR2 = ((n_bytes + 1) << 16) | this->target;
#endif

        if(this->send_address())
        {
            // Send the leading byte
            if(!this->send_data_byte(byte))
                return false;
            
            // proceed with the array
            for (unsigned char n_byte = 0; n_byte < n_bytes; n_byte++)
                if(!this->send_data_byte(data[n_byte]))
                    return false;

            // After sending all bytes generate the stop condition
            this->generate_stop();
            return true;
        }
        return false;
    }
    else
    {
        this->set_error(Error::Code::I2C_BUS_Busy_Error);
        return false;
    }
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
#if defined(STM32F4)
    return this->peripheral->SR2 & I2C_SR2_MSL;
#elif defined(STM32L0)
    return true;
#endif
};

/**
 * @brief Check whether the start condition was generated on the bus
 * by the controller hardware.
 * @return Returns true when start condition was generated.
 */
bool I2C::Controller::start_sent(void) const
{
#if defined(STM32F4)
    return this->peripheral->SR1 & I2C_SR1_SB;
#elif defined(STM32L0)
    return !(this->peripheral->CR2 & I2C_CR2_START);
#endif
};

/**
 * @brief Check whether the target address was sent successfully on the bus.
 * @return Returns true when address transmission is complete.
 */
bool I2C::Controller::address_sent(void) const
{
#if defined(STM32F4)
    return this->peripheral->SR1 & I2C_SR1_ADDR;
#elif defined(STM32L0)
    // This L0 devices combine the start generation an the address transmission
    // therefore the address is sent, when the start condition is generated
    return this->start_sent();
#endif
};

/**
 * @brief Check whether an acknowledge was received. When no communication is ongoing
 * on the bus this will always return true. Only when an nack was received this will
 * become false.
 * @return Returns true when an acknowledge was received in the current transaction.
 */
bool I2C::Controller::ack_received(void) const
{
#if defined(STM32F4)
    // Check whether nack was received and clear error
    bool nack = (this->peripheral->SR1 & I2C_SR1_AF);
    this->peripheral->SR1 &= ~I2C_SR1_AF;
    
    // Return whether there was NO acknowledge failure
    return !nack;
#elif defined(STM32L0)
    // Check whether nack was received and clear error
    bool nack = (this->peripheral->ISR & I2C_ISR_NACKF);
    this->peripheral->ICR |= I2C_ICR_NACKCF;
    
    // Return whether there was NO acknowledge failure
    return !nack;
#endif
};

/**
 * @brief Check whether the TX data shift register is empty and
 * can receive new data.
 * @returns Returns True when new data can be written to the register.
 */
bool I2C::Controller::TX_register_empty(void) const
{
#if defined(STM32F4)
    return this->peripheral->SR1 & I2C_SR1_TXE;
#elif defined(STM32L0)
    return this->peripheral->ISR & I2C_ISR_TXE;
#endif
};

/**
 * @brief Check whether the transfer of the current byte is finished.
 * @returns Returns True when the transfer is finished.
 */
bool I2C::Controller::transfer_finished(void) const
{
#if defined(STM32F4)
    return this->peripheral->SR1 & I2C_SR1_BTF;
#elif defined(STM32L0)
    return this->peripheral->ISR & (I2C_ISR_TXIS | I2C_ISR_TC);
#endif
};

/**
 * @brief Detect wether the bus is currently busy.
 * @return Resturns True when the bus is busy, False otherwise.
 */
bool I2C::Controller::bus_busy(void) const
{
#if defined(STM32F4)
    return this->peripheral->SR2 & I2C_SR2_BUSY;
#elif defined(STM32L0)
    return this->peripheral->ISR & I2C_ISR_BUSY;
#endif
};