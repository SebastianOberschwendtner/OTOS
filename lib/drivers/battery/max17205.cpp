/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    max17205.cpp
 * @author  SO
 * @version v5.0.0
 * @date    14-November-2021
 * @brief   Driver for the MAX17205+ battery balancer and coulomb counter.
 ==============================================================================
 */

/* === Includes === */
#include "max17205.h"

/* Provide template instantiations with allowed bus controllers */
template class max17205::Controller<i2c::Controller>;

namespace max17205
{
    /* === Methods === */
    template <class bus_controller>
    auto Controller<bus_controller>::get_age() const -> percent
    {
        return this->age;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_battery_current() const -> mA
    {
        return this->current_battery;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_battery_voltage() const -> mV
    {
        return this->voltage_battery;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_cell_voltage(const uint8_t cell) const -> mV
    {
        return this->voltage_cell.at(cell - 1);
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_ESR() const -> Ohm
    {
        return this->esr;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_number_cycles() const -> uint16_t
    {
        return this->cycles;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_remaining_capacity() const -> mAh
    {
        return this->capacity[1];
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_SOC() const -> percent
    {
        return this->soc;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_temperature() const -> degC
    {
        return this->temperature;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_total_capacity() const -> mAh
    {
        return this->capacity[0];
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_TTE() const -> seconds
    {
        return this->time2empty;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::get_TTF() const -> seconds
    {
        return this->time2full;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::initialize() -> bool
    {
        return true;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::read_register(const uint16_t reg) -> std::optional<uint16_t>
    {
        /* Set the i2c address */
        if (reg > 0xFF)
            bus::change_address(this->mybus, i2c_address_high);
        else
            bus::change_address(this->mybus, i2c_address_low);

        /* perform the read */
        const uint16_t reg_byte = (reg & 0xFF);
        std::optional<uint16_t> response = bus::read_word(this->mybus, reg_byte);
        if (!response)
            return {};

        /* Read successful, sort the data */
        this->i2c_data.byte[0] = (response.value() >> 8) & 0xFF;
        this->i2c_data.byte[1] = response.value() & 0xFF;
        return this->i2c_data.word[0];
    }

    template <class bus_controller>
    bool Controller<bus_controller>::write_register(const uint16_t reg,
                                                              const uint16_t data)
    {
        /* Set the i2c address */
        if (reg > 0xFF)
            bus::change_address(this->mybus, i2c_address_high);
        else
            bus::change_address(this->mybus, i2c_address_low);

        /* send the data */
        return bus::send_bytes(this->mybus, (reg & 0xFF), (data & 0xFF), (data >> 8));
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_battery_voltage()
    {
        /* Read the Batt Register */
        if (!this->read_register(registers::Batt_Register))
            return false;

        /* convert the data -> This register uses a custom resolution of 1.25mV/LSB */
        uint32_t temp = 0;
        temp += this->i2c_data.byte[0];
        temp += (this->i2c_data.byte[1] << 8);
        temp *= 10;
        this->voltage_battery.set_value(temp >> 3);
        return true;
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_battery_current()
    {
        /* Read the Current Register */
        if (!this->read_register(registers::Current))
            return false;

        /* convert the data */
        uint16_t temp = 0;
        temp += this->i2c_data.byte[0];
        temp += (this->i2c_data.byte[1] << 8);
        this->current_battery = temp;
        return true;
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_battery_current_avg()
    {
        /* Read the Current Register */
        if (!this->read_register(registers::Avg_Current))
            return false;

        /* convert the data */
        uint16_t temp = 0;
        temp += this->i2c_data.byte[0];
        temp += (this->i2c_data.byte[1] << 8);
        this->current_battery = temp;
        return true;
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_cell_voltage()
    {
        /* Read the raw values */
        const auto reg = static_cast<uint8_t>(registers::Cell_2);
        if (!bus::read_array(this->mybus, reg, &this->i2c_data.byte[0], 4))
            return false;

        /* Convert the data */
        /* Cell 2 */
        uint16_t temp = this->i2c_data.byte[3];
        temp += this->i2c_data.byte[2] << 8;
        this->voltage_cell[1] = temp;
        /* Cell 1 */
        temp = this->i2c_data.byte[1];
        temp += this->i2c_data.byte[0] << 8;
        this->voltage_cell[0] = temp;

        return true;
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_cell_voltage_avg()
    {
        /* Read the raw values */
        const auto reg = static_cast<uint8_t>(registers::Avg_Cell_2);
        if (!bus::read_array(this->mybus, reg, &this->i2c_data.byte[0], 4))
            return false;

        /* Convert the data */
        /* Cell 2 */
        uint16_t temp = this->i2c_data.byte[3];
        temp += this->i2c_data.byte[2] << 8;
        this->voltage_cell[1] = temp;
        /* Cell 1 */
        temp = this->i2c_data.byte[1];
        temp += this->i2c_data.byte[0] << 8;
        this->voltage_cell[0] = temp;

        return true;
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_remaining_capacity()
    {
        /* Read the raw values */
        if (!this->read_register(registers::Cap_Remaining))
            return false;

        /* Convert the data */
        uint16_t temp = 0;
        temp += this->i2c_data.byte[0];
        temp += (this->i2c_data.byte[1] << 8);
        this->capacity[1] = temp;

        return true;
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_soc()
    {
        /* Read the raw values */
        if (!this->read_register(registers::SOC))
            return false;

        /* Convert the data */
        uint16_t temp = 0;
        temp += this->i2c_data.byte[0];
        temp += (this->i2c_data.byte[1] << 8);
        this->soc = temp;

        return true;
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_TTE()
    {
        /* Read the raw values */
        if (!this->read_register(registers::TTE))
            return false;

        /* Convert the data */
        uint16_t temp = 0;
        temp += this->i2c_data.byte[0];
        temp += (this->i2c_data.byte[1] << 8);
        this->time2empty = temp;

        return true;
    }

    template <class bus_controller>
    bool Controller<bus_controller>::read_TTF()
    {
        /* Read the raw values */
        if (!this->read_register(registers::TTF))
            return false;

        /* Convert the data */
        uint16_t temp = 0;
        temp += this->i2c_data.byte[0];
        temp += (this->i2c_data.byte[1] << 8);
        this->time2full = temp;

        return true;
    }
}; // namespace max17205