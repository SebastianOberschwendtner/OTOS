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
 * @file    ssd1306.cpp
 * @author  SO
 * @version v2.0.0
 * @date    14-November-2021
 * @brief   Driver for the SSD1306 display controller.
 ******************************************************************************
 */

// === Includes ===
#include "ssd1306.h"

// template class SSD1306::Controller<I2C::Controller>;

// === Functions ===

/**
 * @brief Constructor for display controller.
 * @param bus_used The reference to the used bus peripheral.
 */
SSD1306::Controller::Controller(bus bus_used):
mybus{bus_used}
{
};

/**
 * @brief Initialize the display controller.
 * @return Returns True when the display was initialized without any errors.
 */
// template<class bus_controller>
bool SSD1306::Controller::initialize(void) const
{
    // Set the target address for the i2c controller
    // this->mybus->set_target_address(i2c_address);

    // send the initialization data
    if (!this->send_command_byte(Command::display_off))          return false;
    if (!this->send_command_byte(Command::set_disp_clock_div))   return false;
    if (!this->send_command_data(0x80))                          return false;
    if (!this->send_command_byte(Command::set_multiplex))        return false;
    if (!this->send_command_data(0x1F))                          return false;
    if (!this->send_command_byte(Command::set_display_offset))   return false;
    if (!this->send_command_data(0x00))                          return false;
    if (!this->send_command_byte(Command::set_startline))        return false;
    if (!this->send_command_byte(Command::charge_pump))          return false;
    if (!this->send_command_data(0x14))                          return false;
    if (!this->send_command_byte(Command::memory_mode))          return false;
    if (!this->send_command_data(0x00))                          return false;
    if (!this->send_command_data(0xA1))                          return false;
    if (!this->send_command_byte(Command::com_scan_dec))         return false;
    if (!this->send_command_byte(Command::set_com_pins))         return false;
    if (!this->send_command_data(0x02))                          return false;
    if (!this->send_command_byte(Command::set_contrast))         return false;
    if (!this->send_command_data(0x8F))                          return false;
    if (!this->send_command_byte(Command::set_precharge))        return false;
    if (!this->send_command_data(0xF1))                          return false;
    if (!this->send_command_byte(Command::set_vcom_detect))      return false;
    if (!this->send_command_data(0x40))                          return false;
    if (!this->send_command_byte(Command::display_ram))          return false;
    if (!this->send_command_byte(Command::display_normal))       return false;
    if (!this->send_command_byte(Command::scroll_deactivate))    return false;
    if (!this->send_command_byte(Command::column_address))       return false;
    if (!this->send_command_data(0x00))                          return false;
    if (!this->send_command_data(0x7F))                          return false;
    if (!this->send_command_byte(Command::page_address))         return false;
    if (!this->send_command_data(0x00))                          return false;
    if (!this->send_command_data(0x03))                          return false;

    return true;
};

/**
 * @brief Send a command with a single byte to the display
 * @param cmd The command byte to send.
 * @return Returns True when the command was sent successfully.
 */
bool SSD1306::Controller::send_command_byte(const Command cmd) const
{
    // Set the payload, a single byte sends always two bytes:
    // The first byte (0x00) indicates that the following data
    // is a command
    return this->send_command_data(static_cast<unsigned char>(cmd));
};

/**
 * @brief Send a command with a single data byte to the display
 * @param cmd The command byte to send.
 * @return Returns True when the command was sent successfully.
 */
bool SSD1306::Controller::send_command_data(const unsigned char cmd) const
{
    // Set the payload, a single byte sends always two bytes:
    // The first byte (0x00) indicates that the following data
    // is a command
    I2C::Data_t payload{0};
    payload.byte[0] = cmd;
    return true;
    // return Bus::send_word(this->mybus, payload.word[0]);
};

/**
 * @brief Turn the display on.
 * @return Returns True when the display did respond correctly.
 */
bool SSD1306::Controller::on(void) const
{
    return this->send_command_byte(Command::display_on);
};

/**
 * @brief Turn the display off.
 * @return Returns True when the dispaly did respond correctly.
 */
bool SSD1306::Controller::off(void) const
{
    return this->send_command_byte(Command::display_off);
};

/**
 * @brief Send a buffer with the display content.
 * @param buffer Pointer to the buffer with the content.
 * @return Returns True when the buffer was sent successfully.
 */
bool SSD1306::Controller::draw(const unsigned char* buffer) const
{
    // Send all 4 pages of the buffer
    // for (unsigned char iPage=0; iPage<4; iPage++)
    //     if(!this->bus->send_array_leader(0x40, (buffer + 128*iPage), 128))
    //         return false;
    
    // Sending was successfull
    return true;
};
