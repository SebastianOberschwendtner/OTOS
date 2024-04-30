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
 * @file    bq25700.cpp
 * @author  SO
 * @version v5.0.0
 * @date    14-November-2021
 * @brief   Driver for the BQ25700 battery charger controller.
 ==============================================================================
 */

/* === Includes === */
#include "bq25700.h"

/* Provide template instantiations with allowed bus controllers */
template class bq25700::Controller<i2c::Controller>;

namespace bq25700
{
    /* === Methods === */
    template <class bus_controller>
    auto Controller<bus_controller>::set_charge_current(const uint16_t current) -> bool
    {
        /* Convert the current to the charger resolution */
        this->current_charge = current & 0x1FC0;

        /* Sent the new charger current */
        return this->write_register(registers::Charge_Current, this->current_charge);
    };

    template <class bus_controller>
    auto Controller<bus_controller>::set_OTG_current(const uint16_t current) -> bool
    {
        /* Convert the value to the charger resolution */
        this->current_OTG = (current / 50) << 8;

        /* Sent the new charger current */
        return this->write_register(registers::OTG_Current, this->current_OTG);
    };

    template <class bus_controller>
    auto Controller<bus_controller>::set_OTG_voltage(const uint16_t voltage) -> bool
    {
        /* Convert the value to the charger resolution and clamp to min voltage */
        if (voltage >= 4480)
            this->voltage_OTG = (voltage - 4480) & 0x1FC0;
        else
            this->voltage_OTG = 0;

        /* Sent the new charger current */
        return this->write_register(registers::OTG_Voltage, this->voltage_OTG);
    };

    template <class bus_controller>
    auto Controller<bus_controller>::enable_OTG(const bool state) -> bool
    {
        if (state)
        {
            this->state = State::OTG;
            return this->write_register(registers::Charge_Option_3, EN_OTG);
        }
        this->state = State::Idle;
        return this->write_register(registers::Charge_Option_3, 0x00);
    };

    template <class bus_controller>
    auto Controller<bus_controller>::initialize() -> bool
    {
        /* Set the target address */
        bus::change_address(this->mybus, i2c_address);

        /* check whether device is responding and the IDs match */
        this->state = State::Error;
        if (this->read_register(registers::Manufacturer_ID))
        {
            if (this->i2c_data.byte[0] == manufacturer_id)
                this->state = State::Init;
            else
                this->state = State::Error;
        };

        if (this->read_register(registers::Device_ID))
        {
            if (this->i2c_data.byte[0] == device_id)
                this->state = State::Init;
            else
                this->state = State::Error;
        };
        return true;
    };

    template <class bus_controller>
    auto Controller<bus_controller>::read(RegisterBase &reg) -> bool
    {
        if (!this->read_register(reg.address))
            return false;
        reg.value = this->i2c_data.word[0];
        return true;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::write(RegisterBase &reg) -> bool
    {
        return this->write_register(reg.address, reg.value);
    }

    template <class bus_controller>
    auto Controller<bus_controller>::write_register(const uint8_t reg,
                                                    const uint16_t data) -> bool
    {
        /* send the data bytes */
        return bus::send_bytes(this->mybus, reg, (data & 0xFF), (data >> 8));
    };

    template <class bus_controller>
    auto Controller<bus_controller>::read_register(const uint8_t reg) -> bool
    {
        /* All reads from the BQ25700 contain words */
        std::optional<uint16_t> response = bus::read_word(this->mybus, reg);
        if (!response)
            return false;

        /* Read successful, sort the data */
        this->i2c_data.byte[0] = (response.value() >> 8) & 0xFF;
        this->i2c_data.byte[1] = (response.value() >> 0) & 0xFF;
        return true;
    };
}; // namespace bq25700