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
 * @file    max17205.cpp
 * @author  SO
 * @version v4.2.0
 * @date    14-November-2021
 * @brief   Driver for the MAX17205+ battery balancer and coulomb counter.
 ******************************************************************************
 */

// === Includes ===
#include "max17205.h"

// Provide template instanciations with allowed bus controllers
template class MAX17205::Controller<I2C::Controller>;

// === Functions ===

/**
 * @brief Initialize the balancer for the application.
 * The chip does not require any initialization.
 * @return Returns True when the balancer was initialized correctly.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::initialize()
{
    return true;
}

/**
 * @brief Get the voltage of the battery pack.
 * @return Returns the voltage in [mV].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_battery_voltage() const -> mV
{
    return this->voltage_battery;    
}

/**
 * @brief Get the current of the battery pack.
 * @return Returns the current in [mA].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_battery_current() const -> mA
{
    return this->current_battery;    
}

/**
 * @brief Get the voltage of a specific cell.
 * @param cell The number of the cell (1 or 2)
 * @return Returns the voltage in [mV].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_cell_voltage(const uint8_t cell) const -> mV
{
    return this->voltage_cell.at(cell - 1);    
}

/**
 * @brief Get the total capacity of the battery pack.
 * @return Returns the capacity in [mAh].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_total_capacity() const -> mAh
{
    return this->capacity[0];    
}

/**
 * @brief Get the remaining capacity of the battery pack.
 * @return Returns the capacity in [mAh].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_remaining_capacity() const -> mAh
{
    return this->capacity[1];    
}

/**
 * @brief Get the number of cycles of the battery pack.
 * @return Returns the number of cycles.
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_number_cycles() const -> uint16_t
{
    return this->cycles;    
}

/**
 * @brief Get the age of the battery pack.
 * @return Returns the age as a percentage of the original capacity.
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_age() const -> percent
{
    return this->age;    
}

/**
 * @brief Get the internal series resistance of the battery pack.
 * @return Returns the resistance in [mΩ].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_ESR() const -> Ohm
{
    return this->esr;    
}

/**
 * @brief Get the temperature of the battery pack.
 * @return Returns the voltage in [mV].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_temperature() const -> degC
{
    return this->temperature;    
}

/**
 * @brief Get the SOC of the battery pack.
 * @return Returns the SOC in [%].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_SOC() const -> percent
{
    return this->soc;    
}

/**
 * @brief Get the time until the battery pack is empty.
 * @return Returns the time in [s].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_TTE() const -> seconds
{
    return this->time2empty;
}

/**
 * @brief Get the time until the battery pack is fully charged.
 * @return Returns the time in [s].
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::get_TTF() const -> seconds
{
    return this->time2full;
}

/**
 * @brief Read a register of the balancer.
 * The balancer sends LSB first. The data is stored already
 * sorted to LSB last in the i2c_data struct.
 * @param reg The register to read from.
 * @return Returns true and the value when the register was read successfully.
 */
template<class bus_controller>
auto MAX17205::Controller<bus_controller>::read_register(const uint16_t reg) -> std::optional<uint16_t>
{
    // Set the i2c address
    if (reg > 0xFF)
        Bus::change_address(this->mybus, i2c_address_high);
    else
        Bus::change_address(this->mybus, i2c_address_low);

    // perform the read
    const uint16_t reg_byte = (reg & 0xFF);
    std::optional<uint16_t> response = Bus::read_word(this->mybus, reg_byte);
    if (!response) return {};

    // Read successful, sort the data
    this->i2c_data.byte[0] = (response.value() >> 8) & 0xFF;
    this->i2c_data.byte[1] = response.value() & 0xFF;
    return this->i2c_data.word[0];
}

/**
 * @brief Write a register of the balancer.
 * The balancer expects LSB first.
 * @param reg The register to write to.
 * @param data The data for the register
 * @return Returns True when the data was sent successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::write_register(const uint16_t reg,
    const uint16_t data)
{
    // Set the i2c address
    if (reg > 0xFF)
        Bus::change_address(this->mybus, i2c_address_high);
    else
        Bus::change_address(this->mybus, i2c_address_low);

    // send the data
    return Bus::send_bytes(this->mybus, (reg & 0xFF), (data & 0xFF), (data >> 8));
}

/**
 * @brief Read the pack voltage from the balancer and convert to mV.
 * @return Returns True when the pack voltage was read successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_battery_voltage()
{
    // Read the Batt Register
    if(!this->read_register(Register::Batt_Register)) return false;

    // convert the data -> This register uses a custom resolution of 1.25mV/LSB
    uint32_t temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    temp *= 10;
    this->voltage_battery.set_value(temp >> 3);
    return true;
}

/**
 * @brief Read the pack current from the balancer and convert to mA.
 * @return Returns True when the pack current was read successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_battery_current()
{
    // Read the Current Register
    if(!this->read_register(Register::Current)) return false;

    // convert the data
    uint16_t temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->current_battery = temp;
    return true;
}

/**
 * @brief Read the average pack current from the balancer and convert to mA.
 * @return Returns True when the pack current was read successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_battery_current_avg()
{
    // Read the Current Register
    if(!this->read_register(Register::Avg_Current)) return false;

    // convert the data
    uint16_t temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->current_battery = temp;
    return true;
}

/**
 * @brief Read the cell voltages of the balancer
 * @return Returns true when the cell voltages are updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_cell_voltage()
{
    // Read the raw values
    const auto reg = static_cast<uint8_t>(Register::Cell_2);
    if(!Bus::read_array(this->mybus, reg, &this->i2c_data.byte[0], 4)) return false;

    // Convert the data
    // Cell 2
    uint16_t temp = this->i2c_data.byte[3];
    temp += this->i2c_data.byte[2] << 8;
    this->voltage_cell[1] = temp;
    // Cell 1
    temp = this->i2c_data.byte[1];
    temp += this->i2c_data.byte[0] << 8;
    this->voltage_cell[0] = temp;

    return true;
}

/**
 * @brief Read the avg cell voltages of the balancer
 * @return Returns true when the cell voltages are updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_cell_voltage_avg()
{
    // Read the raw values
    const auto reg = static_cast<uint8_t>(Register::Avg_Cell_2);
    if(!Bus::read_array(this->mybus, reg, &this->i2c_data.byte[0], 4)) return false;

    // Convert the data
    // Cell 2
    uint16_t temp = this->i2c_data.byte[3];
    temp += this->i2c_data.byte[2] << 8;
    this->voltage_cell[1] = temp;
    // Cell 1
    temp = this->i2c_data.byte[1];
    temp += this->i2c_data.byte[0] << 8;
    this->voltage_cell[0] = temp;

    return true;
}

/**
 * @brief Read the remaining capacity from the balancer and convert to mAh.
 * @return Returns true when the capacity is updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_remaining_capacity()
{
    // Read the raw values
    if(!this->read_register(Register::Cap_Remaining)) return false;

    // Convert the data
    uint16_t temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->capacity[1] = temp;

    return true;
}

/**
 * @brief Read the SOC from the balancer and convert to %.
 * @return Returns true when the soc is updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_soc()
{
    // Read the raw values
    if(!this->read_register(Register::SOC)) return false;

    // Convert the data
    uint16_t temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->soc = temp;

    return true;
}

/**
 * @brief Read the time until the battery is empty and convert to [s].
 * @return Returns true when the TTE is updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_TTE()
{
    // Read the raw values
    if(!this->read_register(Register::TTE)) return false;

    // Convert the data
    uint16_t temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->time2empty = temp;

    return true;
}

/**
 * @brief Read the time until the battery is full and convert to [s].
 * @return Returns true when the TTF is updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_TTF()
{
    // Read the raw values
    if(!this->read_register(Register::TTF)) return false;

    // Convert the data
    uint16_t temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->time2full = temp;

    return true;
}