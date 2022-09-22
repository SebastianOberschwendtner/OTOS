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
 * @version v2.8.1
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
 * @return Returns True when the balancer was initialized correctly.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::initialize(void)
{
    // Get the current configuration
    if(!this->read_register(Register::PackCfg)) return false;

    // Check the configuration
    /// @todo Should the configuration just be written every time and not checked?
    unsigned int config = this->i2c_data.word[0];
    if(! (config & ChEn))
        return this->write_register(Register::PackCfg, (TdEn | ChEn | BALCFG_1 | BALCFG_0 | 2U));

    return true;
};

/**
 * @brief Convert a raw voltage value to a mV value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
template<class bus_controller>
inline unsigned int MAX17205::Controller<bus_controller>::to_voltage( const unsigned int raw )
{
    // voltage = (10 * raw / 128) mV
    return (10 * raw) >> 7;
};

/**
 * @brief Convert a raw current to a mA value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
template<class bus_controller>
inline signed int MAX17205::Controller<bus_controller>::to_current( const unsigned int raw )
{
    // current = ( raw / (640*R_sense) ) mA

    // convert to signed number
    signed long temp = 0;
    if (raw & (1<<15))
        temp = raw - 65536;
    else
        temp = raw;

    // Convert the raw data
    return (1000 * temp) / (640 * R_sense_mOhm);
};

/**
 * @brief Convert a raw capacity value to a mAh value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
template<class bus_controller>
inline unsigned int MAX17205::Controller<bus_controller>::to_capacity( const unsigned int raw )
{
    // Capacity = ( raw * 0.005 / R_sense) mAh
    return raw * (5 / R_sense_mOhm);
};

/**
 * @brief Convert a raw percentage value to a % value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
template<class bus_controller>
inline unsigned int MAX17205::Controller<bus_controller>::to_percentage( const unsigned int raw )
{
    // percentage = raw / 256
    return raw >> 8;
};

/**
 * @brief Convert a raw temperature value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
template<class bus_controller>
inline signed int MAX17205::Controller<bus_controller>::to_temperature( const unsigned int raw )
{
    // temperature = raw / 256
    return raw / 256;
};

/**
 * @brief Convert a raw resistance value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
template<class bus_controller>
inline unsigned int MAX17205::Controller<bus_controller>::to_resistance( const unsigned int raw )
{
    // resistance = raw / 4096
    return raw >> 12;
};

/**
 * @brief Convert a raw time value.
 * @param raw The raw register value.
 * @return Returns the converted value.
 */
template<class bus_controller>
inline unsigned int MAX17205::Controller<bus_controller>::to_time( const unsigned int raw )
{
    // time = raw * 5.625s = 5*raw + 5*raw/8
    return (5*raw) + (5*raw >> 3);
};

/**
 * @brief Get the voltage of the battery pack.
 * @return Returns the voltage in [mV].
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_battery_voltage(void) const
{
    return this->voltage_battery;    
};

/**
 * @brief Get the current of the battery pack.
 * @return Returns the current in [mA].
 */
template<class bus_controller>
signed int MAX17205::Controller<bus_controller>::get_battery_current(void) const
{
    return this->current_battery;    
};

/**
 * @brief Get the voltage of a specific cell.
 * @param cell The number of the cell (1 or 2)
 * @return Returns the voltage in [mV].
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_cell_voltage(const unsigned char cell) const
{
    return this->voltage_cell[cell - 1];    
};

/**
 * @brief Get the total capacity of the battery pack.
 * @return Returns the capacity in [mAh].
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_total_capacity(void) const
{
    return this->capacity[0];    
};

/**
 * @brief Get the remaining capacity of the battery pack.
 * @return Returns the capacity in [mAh].
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_remaining_capacity(void) const
{
    return this->capacity[1];    
};

/**
 * @brief Get the number of cycles of the battery pack.
 * @return Returns the number of cycles.
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_number_cycles(void) const
{
    return this->cycles;    
};

/**
 * @brief Get the age of the battery pack.
 * @return Returns the age as a percentage of the original capacity.
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_age(void) const
{
    return this->age;    
};

/**
 * @brief Get the internal series resistance of the battery pack.
 * @return Returns the resistance in [mΩ].
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_ESR(void) const
{
    return this->esr;    
};

/**
 * @brief Get the temperature of the battery pack.
 * @return Returns the voltage in [mV].
 */
template<class bus_controller>
signed int MAX17205::Controller<bus_controller>::get_temperature(void) const
{
    return this->temperature;    
};

/**
 * @brief Get the SOC of the battery pack.
 * @return Returns the SOC in [%].
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_SOC(void) const
{
    return this->soc;    
};

/**
 * @brief Get the time until the battery pack is empty.
 * @return Returns the time in [s].
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_TTE(void) const
{
    return this->time2empty;
};

/**
 * @brief Get the time until the battery pack is fully charged.
 * @return Returns the time in [s].
 */
template<class bus_controller>
unsigned int MAX17205::Controller<bus_controller>::get_TTF(void) const
{
    return this->time2full;
};

/**
 * @brief Read a register of the balancer.
 * The balancer sends LSB first. The data is stored already
 * sorted to LSB last in the i2c_data struct.
 * @param reg The register to read from.
 * @return Returns True when the register was read successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_register(const Register reg)
{
    unsigned int reg_int = static_cast<unsigned int>(reg);

    // Set the i2c address
    if (reg_int > 0xFF)
        Bus::change_address(this->mybus, i2c_address_high);
    else
        Bus::change_address(this->mybus, i2c_address_low);

    // perform the read
    unsigned char reg_byte = static_cast<unsigned char>(reg_int & 0xFF);
    std::optional<unsigned int> response = Bus::read_word(this->mybus, reg_byte);
    if (!response) return false;

    // Read successful, sort the data
    this->i2c_data.byte[0] = (response.value() >> 8) & 0xFF;
    this->i2c_data.byte[1] = response.value() & 0xFF;
    return true;
};

/**
 * @brief Write a register of the balancer.
 * The balancer expects LSB first.
 * @param reg The register to write to.
 * @param data The data for the register
 * @return Returns True when the data was sent successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::write_register(const Register reg,
    const unsigned int data)
{
    unsigned int reg_int = static_cast<unsigned int>(reg);

    // Set the i2c address
    if (reg_int > 0xFF)
        Bus::change_address(this->mybus, i2c_address_high);
    else
        Bus::change_address(this->mybus, i2c_address_low);

    // send the data
    return Bus::send_bytes(this->mybus, (reg_int & 0xFF), (data & 0xFF), (data >> 8));
};

/**
 * @brief Read the pack voltage from the balancer and convert to mV.
 * @return Returns True when the pack voltage was read successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_battery_voltage(void)
{
    // Read the Batt Register
    if(!this->read_register(Register::Batt_Register)) return false;

    // convert the data
    unsigned long temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    temp *= 10;
    this->voltage_battery = (temp >> 3);
    return true;
};

/**
 * @brief Read the pack current from the balancer and convert to mA.
 * @return Returns True when the pack current was read successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_battery_current(void)
{
    // Read the Current Register
    if(!this->read_register(Register::Current)) return false;

    // convert the data
    unsigned int temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->current_battery = this->to_current(temp);
    return true;
};

/**
 * @brief Read the average pack current from the balancer and convert to mA.
 * @return Returns True when the pack current was read successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_battery_current_avg(void)
{
    // Read the Current Register
    if(!this->read_register(Register::Avg_Current)) return false;

    // convert the data
    unsigned int temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->current_battery = this->to_current(temp);
    return true;
};

/**
 * @brief Read the cell voltages of the balancer
 * @return Returns true when the cell voltages are updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_cell_voltage(void)
{
    // Read the raw values
    unsigned char reg = static_cast<unsigned char>(Register::Cell_2);
    if(!Bus::read_array(this->mybus, reg, &this->i2c_data.byte[0], 4)) return false;

    // Convert the data
    // Cell 2
    unsigned int temp = this->i2c_data.byte[3];
    temp += this->i2c_data.byte[2] << 8;
    this->voltage_cell[0] = this->to_voltage(temp);
    // Cell 2
    temp = this->i2c_data.byte[1];
    temp += this->i2c_data.byte[0] << 8;
    this->voltage_cell[1] = this->to_voltage(temp);

    return true;
};

/**
 * @brief Read the avg cell voltages of the balancer
 * @return Returns true when the cell voltages are updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_cell_voltage_avg(void)
{
    // Read the raw values
    unsigned char reg = static_cast<unsigned char>(Register::Avg_Cell_2);
    if(!Bus::read_array(this->mybus, reg, &this->i2c_data.byte[0], 4)) return false;

    // Convert the data
    // Cell 2
    unsigned int temp = this->i2c_data.byte[3];
    temp += this->i2c_data.byte[2] << 8;
    this->voltage_cell[0] = this->to_voltage(temp);
    // Cell 2
    temp = this->i2c_data.byte[1];
    temp += this->i2c_data.byte[0] << 8;
    this->voltage_cell[1] = this->to_voltage(temp);

    return true;
};

/**
 * @brief Read the remaining capacity from the balancer and convert to mAh.
 * @return Returns true when the capacity is updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_remaining_capacity(void)
{
    // Read the raw values
    if(!this->read_register(Register::Cap_Remaining)) return false;

    // Convert the data
    unsigned int temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->capacity[1] = this->to_capacity(temp);

    return true;
};

/**
 * @brief Read the SOC from the balancer and convert to %.
 * @return Returns true when the soc is updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_soc(void)
{
    // Read the raw values
    if(!this->read_register(Register::SOC)) return false;

    // Convert the data
    unsigned int temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->soc = this->to_percentage(temp);

    return true;
};

/**
 * @brief Read the time until the battery is empty and convert to [s].
 * @return Returns true when the TTE is updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_TTE(void)
{
    // Read the raw values
    if(!this->read_register(Register::TTE)) return false;

    // Convert the data
    unsigned int temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->time2empty = this->to_time(temp);

    return true;
};

/**
 * @brief Read the time until the battery is full and convert to [s].
 * @return Returns true when the TTF is updated successfully.
 */
template<class bus_controller>
bool MAX17205::Controller<bus_controller>::read_TTF(void)
{
    // Read the raw values
    if(!this->read_register(Register::TTF)) return false;

    // Convert the data
    unsigned int temp = 0;
    temp += this->i2c_data.byte[0];
    temp += (this->i2c_data.byte[1] << 8);
    this->time2full = this->to_time(temp);

    return true;
};