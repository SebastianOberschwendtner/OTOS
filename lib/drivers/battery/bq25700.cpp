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
 ******************************************************************************
 * @file    bq25700.cpp
 * @author  SO
 * @version v2.7.4
 * @date    14-November-2021
 * @brief   Driver for the BQ25700 battery charger controller.
 ******************************************************************************
 */

// === Includes ===
#include "bq25700.h"

// Provide template instanciations with allowed bus controllers
template class BQ25700::Controller<I2C::Controller>;

// === Functions ===


/**
 * @brief Read a register byte of the target and return 
 * the data sorted into the right order.
 * @return Returns True when the register was read successfully.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::read_register(const uint8_t reg)
{
    // All reads from the BQ25700 contain words
    std::optional<uint16_t> response = Bus::read_word(this->mybus, reg);
    if(!response) 
        return false;

    // Read successful, sort the data
    this->i2c_data.byte[0] = (response.value() >> 8) & 0xFF;
    this->i2c_data.byte[1] = (response.value() >> 0) & 0xFF;
    return true;
};

/**
 * @brief Write a register. The function automatically
 * converts from MSB-first to LSB-first.
 * @param reg The target register
 * @param data The data to be written to the register.
 * @return Returns True when the transfer was successful.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::write_register(const uint8_t reg,
    const uint16_t data)
{
    // send the data bytes
    return Bus::send_bytes(this->mybus, reg, (data & 0xFF), (data >> 8));
};

/**
 * @brief Initialize the charger
 * @return Returns True when the charger responds and was
 * initialized successfully.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::initialize()
{
    // Set the target address
    Bus::change_address(this->mybus, BQ25700::i2c_address);

    // check whether device is responding and the IDs match
    this->state = State::Error;
    if(this->read_register(Register::Manufacturer_ID))
    {
        if(this->i2c_data.byte[0] == manufacturer_id)
            this->state = State::Init;
        else
            this->state = State::Error;
    };

    if(this->read_register(Register::Device_ID))
    {
        if(this->i2c_data.byte[0] == device_id)
            this->state = State::Init;
        else
            this->state = State::Error;
    };
    return true;
};

/**
 * @brief Set the charge current and send the value to the target.
 * @param current The new charge current in [mA]
 * @return Returns True when the charge current was send successfully.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::set_charge_current(const uint16_t current)
{
    // Convert the current to the charger resolution
    this->current_charge = current & 0x1FC0;

    // Sent the new charger current
    return this->write_register(Register::Charge_Current, this->current_charge);
};

/**
 * @brief Set the OTG voltage and send the value to the target.
 * @param voltage The OTG voltage in mV
 * @return Returns True when the charge current was send successfully.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::set_OTG_voltage(const uint16_t voltage)
{
    // Convert the value to the charger resolution and clamp to min voltage
    if (voltage >= 4480)
        this->voltage_OTG = (voltage - 4480) & 0x1FC0;
    else
        this->voltage_OTG = 0;

    // Sent the new charger current
    return this->write_register(Register::OTG_Voltage, this->voltage_OTG);
};

/**
 * @brief Set the OTG current and send the value to the target.
 * @param current The OTG voltage in mA
 * @return Returns True when the charge current was send successfully.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::set_OTG_current(const uint16_t current)
{
    // Convert the value to the charger resolution
    this->current_OTG = (current/50) << 8;

    // Sent the new charger current
    return this->write_register(Register::OTG_Current, this->current_OTG);
};

/**
 * @brief Set the OTG voltage when no error occurred.
 * @param state Whether to turn the OTG on or off
 * @return Returns True when the action was performed without errors.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::enable_OTG(const bool state)
{
    if(state)
    {
        this->state = State::OTG;
        return this->write_register(Register::Charge_Option_3, EN_OTG);
    }
    this->state = State::Idle;
    return this->write_register(Register::Charge_Option_3, 0x00);
};
/**
 * @brief Read a register object from the controller.    
 * 
 * @param reg The register object read, the value is stored in the object.
 * @return Returns True when the register was read successfully.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::read(RegisterBase &reg)
{
    if (!this->read_register(reg.address))
        return false;
    reg.value = this->i2c_data.word[0];
    return true;
}

/**
 * @brief Write a register object to the controller.
 * 
 * @param reg The register object to be written.
 * @return Returns True when the register was written successfully.
 */
template<class bus_controller>
bool BQ25700::Controller<bus_controller>::write(RegisterBase &reg)
{
    return this->write_register(reg.address, reg.value);
}