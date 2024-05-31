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
 * @file    uc1611.cpp
 * @author  SO
 * @version v2.3.0
 * @date    26-Dezember-2021
 * @brief   Driver for the UC1611S display controller.
 ==============================================================================
 */

/* === Includes === */
#include "uc1611.h"

/* Provide valid instantiations => use this as "concepts" for the display controller */
//* @todo This seems a bit much work, the template approach of the bus controllers is nice though... */
template class uc1611::Controller<spi::Controller, gpio::Pin>;

namespace uc1611
{
    /* === Functions === */
    template <class bus_controller, class gpio>
    Controller<bus_controller, gpio>::Controller(
        bus_controller &bus_used,
        gpio &dx_used,
        gpio &cs_used)
        : mybus{bus_used}, dx_pin{&dx_used}, cs_pin{&cs_used}
    {
        this->cs_pin->set_high();
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_COM_end(const uint8_t com_end) -> bool
    {
        return this->send_command_bytes(static_cast<uint8_t>(Command::Set_COM_End), com_end);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_contrast(const uint8_t value) -> bool
    {
        return this->send_command_bytes(static_cast<uint8_t>(Command::Set_Potentiometer), value);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_line_rate(const uint8_t rate) -> bool
    {
        return this->send_command_byte(static_cast<uint8_t>(Command::Set_Line_Rate) | (rate & 0b11));
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_mirrored(const bool x_mirror, const bool y_mirror) -> bool
    {
        const uint8_t config = (y_mirror << 2) | (x_mirror << 1);
        return this->send_command_bytes(static_cast<uint8_t>(Command::Set_LCD_Mapping_Ctrl), config);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_partial_end(const uint8_t end) -> bool
    {
        return this->send_command_bytes(static_cast<uint8_t>(Command::Set_Partial_Display_End), end);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_partial_start(const uint8_t start) -> bool
    {
        return this->send_command_bytes(static_cast<uint8_t>(Command::Set_Partial_Display_Start), start);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_temperature_compensation(const TC curve) -> bool
    {
        uint8_t command =
            static_cast<uint8_t>(Command::Temperature_Compensation) | static_cast<uint8_t>(curve);

        return this->send_command_byte(command);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::draw(
        const uint8_t *buffer_begin,
        const uint8_t *buffer_end) -> bool
    {
        /* signal data */
        this->dx_pin->set_high();

        /* select chip */
        this->cs_pin->set_low();

        /* iterate throught the given buffer and send the data */
        for (const uint8_t *iter = buffer_begin; iter != buffer_end; iter++)
        {
            /* Check whether data was send successfully */
            if (!bus::send_byte(this->mybus, *iter))
                return false;
        }

        /* Data was transmitted successfully */
        return true;
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::draw(
        const uint8_t *buffer_begin,
        const uint8_t *buffer_end,
        void (*hook)()) -> bool
    {
        /* signal data */
        this->dx_pin->set_high();

        /* select chip */
        this->cs_pin->set_low();

        /* iterate throught the given buffer and send the data */
        for (const uint8_t *iter = buffer_begin; iter != buffer_end; iter++)
        {
            /* Check whether data was send successfully */
            if (!bus::send_byte(this->mybus, *iter))
                return false;

            /* Call the hook function */
            hook();
        }

        /* Data was transmitted successfully */
        return true;
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::enable_bw() -> bool
    {
        return this->send_command_byte(static_cast<uint8_t>(Command::Set_Display_Enable) | 0b001);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::show_pattern(const uint8_t pattern) -> bool
    {
        return this->send_command_byte(
            static_cast<uint8_t>(Command::Set_Display_Pattern) | ((pattern & 0b11) << 1) | 1);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::send_command_byte(const uint8_t cmd) -> bool
    {
        /* signal command */
        this->dx_pin->set_low();

        /* select chip */
        this->cs_pin->set_low();

        /* Send byte and return -> Display stays selected for now */
        return bus::send_byte(this->mybus, cmd);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::send_command_bytes(
        const uint8_t byte0,
        const uint8_t byte1) -> bool
    {
        /* signal command */
        this->dx_pin->set_low();

        /* select chip */
        this->cs_pin->set_low();

        /* Send byte and return -> Display stays selected for now */
        return bus::send_bytes(this->mybus, byte0, byte1);
    };

    template <class bus_controller, class gpio>
    bool Controller<bus_controller, gpio>::send_data_byte(const uint8_t data)
    {
        /* signal data */
        this->dx_pin->set_high();

        /* select chip */
        this->cs_pin->set_low();

        /* Send byte and return -> Display stays selected for now */
        return bus::send_byte(this->mybus, data);
    };
}; // namespace uc1611