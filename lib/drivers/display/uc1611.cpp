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
 * @file    uc1611.cpp
 * @author  SO
 * @version v2.3.0
 * @date    26-Dezember-2021
 * @brief   Driver for the UC1611S display controller.
 ******************************************************************************
 */

// === Includes ===
#include "uc1611.h"

// Provide valid instanciations => use this as "concepts" for the display controller
/// @todo This seems a bit much work, the template approach of the bus controllers is nice though...
template class UC1611::Controller<SPI::Controller<IO::SPI_1>, GPIO::PIN>;
template class UC1611::Controller<SPI::Controller<IO::SPI_2>, GPIO::PIN>;
template class UC1611::Controller<SPI::Controller<IO::SPI_3>, GPIO::PIN>;
template class UC1611::Controller<SPI::Controller<IO::SPI_4>, GPIO::PIN>;
template class UC1611::Controller<SPI::Controller<IO::SPI_5>, GPIO::PIN>;

// === Functions ===
/**
 * @brief Constructor for display controller.
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param bus_used The reference to the used bus peripheral.
 */
template <class bus_controller, class gpio>
UC1611::Controller<bus_controller, gpio>::Controller(
    bus_controller &bus_used,
    gpio &dx_used,
    gpio &cs_used)
    : mybus{bus_used}, dx_pin{&dx_used}, cs_pin{&cs_used}
{
    this->cs_pin->set_high();
};

/**
 * @brief Send a command byte to the display controller.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param cmd The command byte to send.
 * @return Returns True when the byte was send successfully.
 */
template <class bus_controller, class gpio>
bool UC1611::Controller<bus_controller, gpio>::send_command_byte(const unsigned char cmd)
{
    // signal command
    this->dx_pin->set_low();

    // select chip
    this->cs_pin->set_low();

    // Send byte and return -> Display stays selected for now
    return Bus::send_byte(this->mybus, cmd);
};

/**
 * @brief Send a command byte to the display controller.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param cmd The command byte to send.
 * @return Returns True when the byte was send successfully.
 */
template <class bus_controller, class gpio>
bool UC1611::Controller<bus_controller, gpio>::send_command_bytes(
    const unsigned char byte0,
    const unsigned char byte1
)
{
    // signal command
    this->dx_pin->set_low();

    // select chip
    this->cs_pin->set_low();

    // Send byte and return -> Display stays selected for now
    return Bus::send_bytes(this->mybus, byte0, byte1);
};

/**
 * @brief Set the temperature compensation curve the controller
 * uses.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param curve One of the four available compensation curves.
 * @return Returns True when the command was sent successfully.
 */
template <class bus_controller, class gpio>
bool UC1611::Controller<bus_controller, gpio>::set_temperature_compensation(const TC curve)
{
    unsigned char command =
        static_cast<unsigned char>(Command::Temperature_Compensation)
        | static_cast<unsigned char>(curve);

    return this->send_command_byte(command);
};

/**
 * @brief Control the contrast of the display.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param value The contrast value to set.
 * @return Returns True when the command was sent successfully.
 */
template <class bus_controller, class gpio>
bool UC1611::Controller<bus_controller, gpio>::set_contrast(const unsigned char value)
{
    return this->send_command_bytes( static_cast<unsigned char>(Command::Set_Potentiometer), value);
};

/**
 * @brief Control the line rate of the display.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param rate The configuration bits for the line rate. Should be [0..3].
 * @return Returns True when the command was sent successfully.
 */
template <class bus_controller, class gpio>
bool UC1611::Controller<bus_controller, gpio>::set_line_rate(const unsigned char rate)
{
    return this->send_command_byte( static_cast<unsigned char>(Command::Set_Line_Rate) | (rate & 0b11) );
};

/**
 * @brief Enable the display in black-white mode.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns True when the display was enabled successfully.
 */
template <class bus_controller, class gpio>
bool UC1611::Controller<bus_controller, gpio>::enable_bw()
{
    return this->send_command_byte( static_cast<unsigned char>(Command::Set_Display_Enable) |  0b001 );
};

/**
 * @brief Select which pattern the display should show when in black-white mode.
 * In this mode the display controller can store 4 patterns individually.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param pattern The pattern to display. Should be [0..3].
 * @return Returns True when the command was sent successfully.
 */
template <class bus_controller, class gpio>
bool UC1611::Controller<bus_controller, gpio>::show_pattern(const unsigned char pattern)
{
    return this->send_command_byte(
        static_cast<unsigned char>(Command::Set_Display_Pattern) 
        | ((pattern & 0b11) << 1) | 1 );
};

/**
 * @brief Select the end COM segment which is active, when
 * the display does not use all the available COM segments.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param com_end The end of the COM segment. Should be [0..159].
 * @return Returns True when the command was sent successfully.
 */
template <class bus_controller, class gpio>
bool UC1611::Controller<bus_controller, gpio>::set_COM_end(const unsigned char com_end)
{
    return this->send_command_bytes( static_cast<unsigned char>(Command::Set_COM_End), com_end);
};