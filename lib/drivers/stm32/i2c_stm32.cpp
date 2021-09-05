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
    // get the CCR value
    reg_val = (get_FREQ()*1000000)/frequency;
    // limit CCR value to 12 bits
    reg_val &= 0xFFF;
    // Set F/S and DUTY bits according to outputfrequency
    if (frequency > 100000)
        reg_val |= I2C_CCR_FS | I2C_CCR_DUTY;

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
 * @brief Start the I2C transaction by sending the target address
 * on the bus.
 */
void I2C::Controller::send_address(void)
{
    this->write_data_register(this->target);
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
 * @brief Return the most recent occurred error
 * @return Returns most recent error code.
 */
int I2C::Controller::get_error(void) const
{
    return 0;
};