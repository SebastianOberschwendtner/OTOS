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
 * @file    gc9a01a.cpp
 * @author  SO
 * @version v2.10.0
 * @date    25-September-2022
 * @brief   Unit tests to test the GC9A01A display controller. (Used by the round 1.28" display from Waveshare)
 ******************************************************************************
 */

// === Includes ===
#include "gc9a01a.h"

// Provide valid instantiations
template class GC9A01A::Controller<SPI::Controller<IO::SPI_1>, GPIO::PIN>;
template class GC9A01A::Controller<SPI::Controller<IO::SPI_2>, GPIO::PIN>;
template class GC9A01A::Controller<SPI::Controller<IO::SPI_3>, GPIO::PIN>;
template class GC9A01A::Controller<SPI::Controller<IO::SPI_4>, GPIO::PIN>;
template class GC9A01A::Controller<SPI::Controller<IO::SPI_5>, GPIO::PIN>;

// === Functions ===
/**
 * @brief Constructor for display controller.
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param bus_used The reference to the used bus peripheral.
 */
template <class bus_controller, class gpio>
GC9A01A::Controller<bus_controller, gpio>::Controller(
    bus_controller &bus_used,
    gpio &dx_used,
    gpio &cs_used,
    gpio &bl_used)
    : mybus{bus_used}, dx_pin{&dx_used}, cs_pin{&cs_used}, bl_pin{&bl_used}
{
    this->cs_pin->set_high();
};

/**
 * @brief Send a command byte to the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param cmd The command code to send to the display.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::send_command_byte(const Command cmd)
{
    // Indicate that next byte is a command
    this->dx_pin->set_low();

    // Select Chip
    this->cs_pin->set_low();
    // Send the byte
    bool response = Bus::send_byte(this->mybus, static_cast<unsigned char>(cmd));
    // deselect chip
    this->cs_pin->set_high();

    // Return response
    return response;
};

/**
 * @brief Send a register byte to the display without checking
 * whether the register is valid.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param reg The register code to send to the display.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::send_register_byte(const unsigned char reg)
{
    // Indicate that next byte is a command
    this->dx_pin->set_low();

    // Select Chip
    this->cs_pin->set_low();
    // Send the byte
    bool response = Bus::send_byte(this->mybus, reg);
    // deselect chip
    this->cs_pin->set_high();

    // Return response
    return response;
};

/**
 * @brief Send a data byte to the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param data The data byte to send to the display.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::send_data_byte(const unsigned char data)
{
    // Indicate that next byte is data
    this->dx_pin->set_high();

    // Select Chip
    this->cs_pin->set_low();
    // Send the byte
    bool response = Bus::send_byte(this->mybus, data);
    // deselect chip
    this->cs_pin->set_high();

    // Return response
    return response;
};

/**
 * @brief Send a command and 1 payload byte to the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param cmd The command code to send to the display.
 * @param byte0 The payload byte for the command.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::send_command(
    const Command cmd,
    const unsigned char byte0)
{
    // Select Chip
    this->cs_pin->set_low();
    // Send the command and payload
    // Send the command
    this->dx_pin->set_low();
    if (!Bus::send_byte(this->mybus, static_cast<unsigned char>(cmd)))
        return false;
    // Send the data
    this->dx_pin->set_high();
    if (!Bus::send_byte(this->mybus, byte0))
        return false;

    // deselect chip
    this->cs_pin->set_high();

    // Return response
    return true;
};

/**
 * @brief Send a command and 1 payload word to the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param cmd The command code to send to the display.
 * @param word0 The payload word for the command.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::send_command(
    const Command cmd,
    const unsigned int word0)
{
    // Select Chip
    this->cs_pin->set_low();
    // Send the command and payload
    // Send the command
    this->dx_pin->set_low();
    if (!Bus::send_byte(this->mybus, static_cast<unsigned char>(cmd)))
        return false;
    // Send the data
    this->dx_pin->set_high();
    if (!Bus::send_word(this->mybus, word0))
        return false;

    // deselect chip
    this->cs_pin->set_high();

    // Return response
    return true;
};

/**
 * @brief Send a command and 2 payload bytes to the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param cmd The command code to send to the display.
 * @param byte0 The 1st payload byte for the command.
 * @param byte1 The 2nd payload byte for the command.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::send_command(
    const Command cmd,
    const unsigned char byte0,
    const unsigned char byte1)
{
    // Select Chip
    this->cs_pin->set_low();
    // Send the command and payload
    // Send the command
    this->dx_pin->set_low();
    if (!Bus::send_byte(this->mybus, static_cast<unsigned char>(cmd)))
        return false;
    // Send the data
    this->dx_pin->set_high();
    if (!Bus::send_byte(this->mybus, byte0))
        return false;
    if (!Bus::send_byte(this->mybus, byte1))
        return false;

    // deselect chip
    this->cs_pin->set_high();

    // Return response
    return true;
};

/**
 * @brief Send a command and 2 payload words to the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param cmd The command code to send to the display.
 * @param word0 The 1st payload word for the command.
 * @param word1 The 2nd payload word for the command.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::send_command(
    const Command cmd,
    const unsigned int word0,
    const unsigned int word1)
{
    // Select Chip
    this->cs_pin->set_low();
    // Send the command and payload
    // Send the command
    this->dx_pin->set_low();
    if (!Bus::send_byte(this->mybus, static_cast<unsigned char>(cmd)))
        return false;
    // Send the data
    this->dx_pin->set_high();
    if (!Bus::send_word(this->mybus, word0))
        return false;
    if (!Bus::send_word(this->mybus, word1))
        return false;

    // deselect chip
    this->cs_pin->set_high();

    // Return response
    return true;
};

/**
 * @brief Turn the display on.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::on(void)
{
    return this->send_command_byte(Command::Display_On);
};

/**
 * @brief Turn the display off.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::off(void)
{
    return this->send_command_byte(Command::Display_Off);
};

/**
 * @brief Reset the Display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::reset(void)
{
    // return this->send_command_byte(Command::Reset);
    return false;
};

/**
 * @brief Go out of sleep mode.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::wake_up(void)
{
    return this->send_command_byte(Command::Sleep_Out);
};

/**
 * @brief Draw a buffer which contains a color value for
 * every pixel of the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param buffer_begin The begin iterator of the buffer.
 * @param buffer_end The end of the buffer
 * @return Returns true when the buffer was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::draw(
    const unsigned short *buffer_begin,
    const unsigned short *buffer_end)
{
    // Start memory write
    if (!this->send_command_byte(Command::Write_Memory))
        return false;

    // Switch to data transfer
    this->dx_pin->set_high();
    this->cs_pin->set_low();

    // For every pixel in the buffer transmit its color value
    for (const unsigned short *iter = buffer_begin; iter != buffer_end; iter++)
    {
        if (!Bus::send_word(this->mybus, *iter))
            return false;
    }
    this->cs_pin->set_low();

    // Transmit successful
    return true;
};

/**
 * @brief Draw a buffer which contains only a black-white value for
 * every pixel of the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param buffer_begin The begin iterator of the buffer.
 * @param buffer_end The end of the buffer
 * @param color The foreground color for "white" pixels.
 * @param background The background color for "black" pixels.
 * @return Returns true when the buffer was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::draw(
    const unsigned char *buffer_begin,
    const unsigned char *buffer_end,
    const unsigned int color,
    const unsigned int background)
{
    // Start memory write
    if (!this->send_command_byte(Command::Write_Memory))
        return false;

    // Switch to data transfer
    this->dx_pin->set_high();
    this->cs_pin->set_low();

    // The BW buffer encodes 8 pixels per byte -> transform this to singular pixels
    // Split the buffer into pages which fit the width (240 pixels) of the display.
    for (const unsigned char *page = buffer_begin; page != buffer_end; page += 240)
    {
        // 8 Pixels per byte
        for (unsigned char iPixel = 0; iPixel < 8; iPixel++)
        {
            // Add one page of data
            for (const unsigned char *iter = page; iter != (page + 240); iter++)
            {
                // Check whether pixel is black or white
                if (*iter & (1 << (iPixel)))
                {
                    if (!Bus::send_word(this->mybus, color))
                        return false;
                }
                else
                {
                    if (!Bus::send_word(this->mybus, background))
                        return false;
                }
            }
        }
    }
    this->cs_pin->set_low();

    // Transmit successful
    return true;
};

/**
 * @brief Draw a buffer which contains only a black-white value for
 * every pixel of the display.
 *
 * This function can be given a call hook, for calling i.e. the
 * yield() function.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param buffer_begin The begin iterator of the buffer.
 * @param buffer_end The end of the buffer
 * @param color The foreground color for "white" pixels.
 * @param background The background color for "black" pixels.
 * @param hook Additional hook to function which gets called after each pixel transfer.
 * @return Returns true when the buffer was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::draw(
    const unsigned char *buffer_begin,
    const unsigned char *buffer_end,
    const unsigned int color,
    const unsigned int background,
    void (*hook)())
{
    // Start memory write
    if (!this->send_command_byte(Command::Write_Memory))
        return false;

    // Switch to data transfer
    this->dx_pin->set_high();
    this->cs_pin->set_low();

    // The BW buffer encodes 8 pixels per byte -> transform this to singular pixels
    // Split the buffer into pages which fit the width (240 pixels) of the display.
    for (const unsigned char *page = buffer_begin; page != buffer_end; page += 240)
    {
        // 8 Pixels per byte
        for (unsigned char iPixel = 0; iPixel < 8; iPixel++)
        {
            // Add one page of data
            for (const unsigned char *iter = page; iter != (page + 240); iter++)
            {
                // Check whether pixel is black or white
                if (*iter & (1 << (iPixel)))
                {
                    if (!Bus::send_word(this->mybus, color))
                        return false;
                }
                else
                {
                    if (!Bus::send_word(this->mybus, background))
                        return false;
                }
            }
            // Call the hook function after pixel is transmitted
            hook();
        }
    }
    this->cs_pin->set_low();

    // Transmit successful
    return true;
};

/**
 * @brief Draw a single pixel on the display.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param x The X coordinate of the pixel.
 * @param y The Y coordinate of the pixel.
 * @param color The color of the pixel.
 * @return Returns true when the pixel was send successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::draw_pixel(
    const unsigned int x,
    const unsigned int y,
    const unsigned int color)
{
    // Set the cursor to the given position -> Start == End
    this->send_command(Command::Column_Addr, x, x);
    this->send_command(Command::Page_Addr, y, y);

    // Write the pixel
    return this->send_command(Command::Write_Memory, color);
};

/**
 * @brief Initialize the Display.
 * The configuration assumes the configuration of the
 * display as it is used on the Waveshare 1.28" round LCD board.
 *
 * The command is quite long and there is no documentation from
 * the manufacturer to explain the commands. The commands are
 * directly taken from the Waveshare example code.
 *
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns true when the display was initialized successfully.
 */
template <class bus_controller, class gpio>
bool GC9A01A::Controller<bus_controller, gpio>::initialize_custom(void)
{
    this->send_register_byte(0xEF);
    this->send_register_byte(0xEB);
    this->send_data_byte(0x14);

    this->send_register_byte(0xFE);
    this->send_register_byte(0xEF);

    this->send_register_byte(0xEB);
    this->send_data_byte(0x14);

    this->send_register_byte(0x84);
    this->send_data_byte(0x40);

    this->send_register_byte(0x85);
    this->send_data_byte(0xFF);

    this->send_register_byte(0x86);
    this->send_data_byte(0xFF);

    this->send_register_byte(0x87);
    this->send_data_byte(0xFF);

    this->send_register_byte(0x88);
    this->send_data_byte(0x0A);

    this->send_register_byte(0x89);
    this->send_data_byte(0x21);

    this->send_register_byte(0x8A);
    this->send_data_byte(0x00);

    this->send_register_byte(0x8B);
    this->send_data_byte(0x80);

    this->send_register_byte(0x8C);
    this->send_data_byte(0x01);

    this->send_register_byte(0x8D);
    this->send_data_byte(0x01);

    this->send_register_byte(0x8E);
    this->send_data_byte(0xFF);

    this->send_register_byte(0x8F);
    this->send_data_byte(0xFF);

    this->send_register_byte(0xB6);
    this->send_data_byte(0x00);
    this->send_data_byte(0x20);

    this->send_register_byte(0x36);
    this->send_data_byte(0x08);

    this->send_register_byte(0x3A);
    this->send_data_byte(0x05);

    this->send_register_byte(0x90);
    this->send_data_byte(0x08);
    this->send_data_byte(0x08);
    this->send_data_byte(0x08);
    this->send_data_byte(0x08);

    this->send_register_byte(0xBD);
    this->send_data_byte(0x06);

    this->send_register_byte(0xBC);
    this->send_data_byte(0x00);

    this->send_register_byte(0xFF);
    this->send_data_byte(0x60);
    this->send_data_byte(0x01);
    this->send_data_byte(0x04);

    this->send_register_byte(0xC3);
    this->send_data_byte(0x13);
    this->send_register_byte(0xC4);
    this->send_data_byte(0x13);

    this->send_register_byte(0xC9);
    this->send_data_byte(0x22);

    this->send_register_byte(0xBE);
    this->send_data_byte(0x11);

    this->send_register_byte(0xE1);
    this->send_data_byte(0x10);
    this->send_data_byte(0x0E);

    this->send_register_byte(0xDF);
    this->send_data_byte(0x21);
    this->send_data_byte(0x0c);
    this->send_data_byte(0x02);

    this->send_register_byte(0xF0);
    this->send_data_byte(0x45);
    this->send_data_byte(0x09);
    this->send_data_byte(0x08);
    this->send_data_byte(0x08);
    this->send_data_byte(0x26);
    this->send_data_byte(0x2A);

    this->send_register_byte(0xF1);
    this->send_data_byte(0x43);
    this->send_data_byte(0x70);
    this->send_data_byte(0x72);
    this->send_data_byte(0x36);
    this->send_data_byte(0x37);
    this->send_data_byte(0x6F);

    this->send_register_byte(0xF2);
    this->send_data_byte(0x45);
    this->send_data_byte(0x09);
    this->send_data_byte(0x08);
    this->send_data_byte(0x08);
    this->send_data_byte(0x26);
    this->send_data_byte(0x2A);

    this->send_register_byte(0xF3);
    this->send_data_byte(0x43);
    this->send_data_byte(0x70);
    this->send_data_byte(0x72);
    this->send_data_byte(0x36);
    this->send_data_byte(0x37);
    this->send_data_byte(0x6F);

    this->send_register_byte(0xED);
    this->send_data_byte(0x1B);
    this->send_data_byte(0x0B);

    this->send_register_byte(0xAE);
    this->send_data_byte(0x77);

    this->send_register_byte(0xCD);
    this->send_data_byte(0x63);

    this->send_register_byte(0x70);
    this->send_data_byte(0x07);
    this->send_data_byte(0x07);
    this->send_data_byte(0x04);
    this->send_data_byte(0x0E);
    this->send_data_byte(0x0F);
    this->send_data_byte(0x09);
    this->send_data_byte(0x07);
    this->send_data_byte(0x08);
    this->send_data_byte(0x03);

    this->send_register_byte(0xE8);
    this->send_data_byte(0x34);

    this->send_register_byte(0x62);
    this->send_data_byte(0x18);
    this->send_data_byte(0x0D);
    this->send_data_byte(0x71);
    this->send_data_byte(0xED);
    this->send_data_byte(0x70);
    this->send_data_byte(0x70);
    this->send_data_byte(0x18);
    this->send_data_byte(0x0F);
    this->send_data_byte(0x71);
    this->send_data_byte(0xEF);
    this->send_data_byte(0x70);
    this->send_data_byte(0x70);

    this->send_register_byte(0x63);
    this->send_data_byte(0x18);
    this->send_data_byte(0x11);
    this->send_data_byte(0x71);
    this->send_data_byte(0xF1);
    this->send_data_byte(0x70);
    this->send_data_byte(0x70);
    this->send_data_byte(0x18);
    this->send_data_byte(0x13);
    this->send_data_byte(0x71);
    this->send_data_byte(0xF3);
    this->send_data_byte(0x70);
    this->send_data_byte(0x70);

    this->send_register_byte(0x64);
    this->send_data_byte(0x28);
    this->send_data_byte(0x29);
    this->send_data_byte(0xF1);
    this->send_data_byte(0x01);
    this->send_data_byte(0xF1);
    this->send_data_byte(0x00);
    this->send_data_byte(0x07);

    this->send_register_byte(0x66);
    this->send_data_byte(0x3C);
    this->send_data_byte(0x00);
    this->send_data_byte(0xCD);
    this->send_data_byte(0x67);
    this->send_data_byte(0x45);
    this->send_data_byte(0x45);
    this->send_data_byte(0x10);
    this->send_data_byte(0x00);
    this->send_data_byte(0x00);
    this->send_data_byte(0x00);

    this->send_register_byte(0x67);
    this->send_data_byte(0x00);
    this->send_data_byte(0x3C);
    this->send_data_byte(0x00);
    this->send_data_byte(0x00);
    this->send_data_byte(0x00);
    this->send_data_byte(0x01);
    this->send_data_byte(0x54);
    this->send_data_byte(0x10);
    this->send_data_byte(0x32);
    this->send_data_byte(0x98);

    this->send_register_byte(0x74);
    this->send_data_byte(0x10);
    this->send_data_byte(0x85);
    this->send_data_byte(0x80);
    this->send_data_byte(0x00);
    this->send_data_byte(0x00);
    this->send_data_byte(0x4E);
    this->send_data_byte(0x00);

    this->send_register_byte(0x98);
    this->send_data_byte(0x3e);
    this->send_data_byte(0x07);

    this->send_register_byte(0x35);
    this->send_register_byte(0x21);
    return true;
};