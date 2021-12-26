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
 * @file    ili9341.cpp
 * @author  SO
 * @version v2.3.0
 * @date    23-Dezember-2021
 * @brief   Driver for the ILI9341 display controller.
 ******************************************************************************
 */

// === Includes ===
#include "ili9341.h"

// Provide valid instanciations => use this as "concepts" for the display controller
/// @todo This seems a bit much work, the template approach of the bus controllers is nice though...
template class ILI9341::Controller<SPI::Controller<IO::SPI_1>, GPIO::PIN>;
template class ILI9341::Controller<SPI::Controller<IO::SPI_2>, GPIO::PIN>;
template class ILI9341::Controller<SPI::Controller<IO::SPI_3>, GPIO::PIN>;
template class ILI9341::Controller<SPI::Controller<IO::SPI_4>, GPIO::PIN>;
template class ILI9341::Controller<SPI::Controller<IO::SPI_5>, GPIO::PIN>;

// === Functions ===
/**
 * @brief Constructor for display controller.
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param bus_used The reference to the used bus peripheral.
 */
template <class bus_controller, class gpio>
ILI9341::Controller<bus_controller, gpio>::Controller(
    bus_controller &bus_used,
    gpio &dx_used,
    gpio &cs_used)
    : mybus{bus_used}, dx_pin{&dx_used}, cs_pin{&cs_used}
{
    this->cs_pin->set_high();
};

/**
 * @brief Initialize the Display.
 * The configuration assumes the configuration of the
 * display as it is used on the STM32F429 Discovery Board.
 * 
 * The display should be waked up before calling this command.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns true when the display was initialized successfully.
 */
template <class bus_controller, class gpio>
bool ILI9341::Controller<bus_controller, gpio>::initialize(void)
{
    // Set Power Control
    if(!this->set_power_control_1(0x03))
        return false;
    
    // set VCOM control
    if(!this->set_VCOM_control_1(0x01, 0x3C))
        return false;

    // Inverse the Y Pixel mapping
    if(!this->set_memory_access((1<<7)))
        return false;

    // Set RGB format to 16 bits per pixel
    if(!this->set_16bits_per_pixel())
        return false;
    return true;
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
bool ILI9341::Controller<bus_controller, gpio>::send_command_byte(const Command cmd)
{
    // Indicate that next byte is a command
    this->dx_pin->set_low();

    // Select Chip
    this->cs_pin->set_low();
    // Send the byte
    bool response = Bus::send_byte(this->mybus, static_cast<unsigned char>(cmd));
    // deselect chip
    this->cs_pin->set_high();

    // Return responce
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
bool ILI9341::Controller<bus_controller, gpio>::send_data_byte(const unsigned char data)
{
    // Indicate that next byte is data
    this->dx_pin->set_high();

    // Select Chip
    this->cs_pin->set_low();
    // Send the byte
    bool response = Bus::send_byte(this->mybus, data);
    // deselect chip
    this->cs_pin->set_high();

    // Return responce
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
bool ILI9341::Controller<bus_controller, gpio>::send_command(
    const Command cmd,
    const unsigned char byte0
)
{
    // Select Chip
    this->cs_pin->set_low();
    // Send the command and payload
    // Send the command
    this->dx_pin->set_low();
    if(!Bus::send_byte(this->mybus, static_cast<unsigned char>(cmd)))
        return false;
    // Send the data
    this->dx_pin->set_high();
    if(!Bus::send_byte(this->mybus, byte0))
        return false;

    // deselect chip
    this->cs_pin->set_high();

    // Return responce
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
bool ILI9341::Controller<bus_controller, gpio>::send_command(
    const Command cmd,
    const unsigned char byte0,
    const unsigned char byte1
)
{
    // Select Chip
    this->cs_pin->set_low();
    // Send the command and payload
    // Send the command
    this->dx_pin->set_low();
    if(!Bus::send_byte(this->mybus, static_cast<unsigned char>(cmd)))
        return false;
    // Send the data
    this->dx_pin->set_high();
    if(!Bus::send_byte(this->mybus, byte0))
        return false;
    if(!Bus::send_byte(this->mybus, byte1))
        return false;

    // deselect chip
    this->cs_pin->set_high();

    // Return responce
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
bool ILI9341::Controller<bus_controller, gpio>::on(void)
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
bool ILI9341::Controller<bus_controller, gpio>::off(void)
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
bool ILI9341::Controller<bus_controller, gpio>::reset(void)
{
    return this->send_command_byte(Command::Reset);
};

/**
 * @brief Go out of sleep mode.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool ILI9341::Controller<bus_controller, gpio>::wake_up(void)
{
    return this->send_command_byte(Command::Sleep_Out);
};

/**
 * @brief Set the pixelformat for the communication
 * to 16 bits per pixel for all interface methods.
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool ILI9341::Controller<bus_controller, gpio>::set_16bits_per_pixel(void)
{
    constexpr unsigned char format = (0b101 << 4 ) | (0b101);
    return this->send_command(Command::Set_Pixel_Format, format);
};

/**
 * @brief Set the Power Control 1 register of the display.
 * 
 * The datasheet describes VRH as:
 * "Set the GVDD level, which is a reference level for the
 * VCOM level and grayscale voltage level."
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param VHR THE GVDD level to set.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool ILI9341::Controller<bus_controller, gpio>::set_power_control_1(
    const unsigned char VHR
)
{
    return this->send_command(Command::Power_Ctrl_1, VHR);
};

/**
 * @brief Set the memory access control of the display.
 * 
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param configuration The configuration bits as byte.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool ILI9341::Controller<bus_controller, gpio>::set_memory_access(
    const unsigned char configuration
)
{
    return this->send_command(Command::Memory_Access_Ctrl, configuration);
};

/**
 * @brief Set the VCOM Control 1 register of the display.
 * 
 * The datasheet describes VMH and VML as:
 * "VMH: Set the VCOMH voltage."
 * "VML: Set the VCOML voltage."
 * 
 * @tparam bus_controller The type of the used bus controller.
 * @tparam gpio The Pin class used for the bus communication.
 * @param VHR THE GVDD level to set.
 * @return Returns True when command was send successfully.
 */
template <class bus_controller, class gpio>
bool ILI9341::Controller<bus_controller, gpio>::set_VCOM_control_1(
    const unsigned char VMH,
    const unsigned char VML
)
{
    return this->send_command(Command::VCOM_Ctrl_1, VMH, VML);
};

// template <class bus_controller, class gpio>
// bool ILI9341::Controller<bus_controller, gpio>::fill(
//     const unsigned int color
// )
// {
//     if(!this->send_command_byte(Command::Write_Memory))
//         return false;
//     for(unsigned int i=0; i<(320*240); i++)
//         if(!this->send_data_byte(color))
//             return false;
//     return true;
// };

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
bool ILI9341::Controller<bus_controller, gpio>::draw(
    const unsigned short* buffer_begin,
    const unsigned short* buffer_end
)
{
    // Start memory write
    if(!this->send_command_byte(Command::Write_Memory))
        return false;

    // Switch to data transfer
    this->dx_pin->set_high();
    this->cs_pin->set_low();

    // For every pixel in the buffer transmit its color value
    for(const unsigned short* iter = buffer_begin; iter!=buffer_end; iter++)
    {
        if(!Bus::send_word(this->mybus, *iter))
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
bool ILI9341::Controller<bus_controller, gpio>::draw(
    const unsigned char* buffer_begin,
    const unsigned char* buffer_end,
    const unsigned int color,
    const unsigned int background
)
{
    // Start memory write
    if(!this->send_command_byte(Command::Write_Memory))
        return false;

    // Switch to data transfer
    this->dx_pin->set_high();
    this->cs_pin->set_low();

    // The BW buffer encodes 8 pixels per byte -> transform this to singular pixels
    // Split the buffer into pages which fit the width (240 pixels) of the display.
    for(const unsigned char* page = buffer_begin; page != buffer_end; page +=240)
    {
        // 8 Pixels per byte
        for (unsigned char iPixel = 0; iPixel<8; iPixel++)
        {
            // Add one page of data
            for(const unsigned char* iter = page; iter!=(page+240); iter++)
            {
                // Check whether pixel is black or white
                if(*iter & (1<<(iPixel)))
                {
                if(!Bus::send_word(this->mybus, color))
                    return false;
                }
                else
                {
                if(!Bus::send_word(this->mybus, background))
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
bool ILI9341::Controller<bus_controller, gpio>::draw(
    const unsigned char* buffer_begin,
    const unsigned char* buffer_end,
    const unsigned int color,
    const unsigned int background,
    void (*hook)()
)
{
    // Start memory write
    if(!this->send_command_byte(Command::Write_Memory))
        return false;

    // Switch to data transfer
    this->dx_pin->set_high();
    this->cs_pin->set_low();

    // The BW buffer encodes 8 pixels per byte -> transform this to singular pixels
    // Split the buffer into pages which fit the width (240 pixels) of the display.
    for(const unsigned char* page = buffer_begin; page != buffer_end; page +=240)
    {
        // 8 Pixels per byte
        for (unsigned char iPixel = 0; iPixel<8; iPixel++)
        {
            // Add one page of data
            for(const unsigned char* iter = page; iter!=(page+240); iter++)
            {
                // Check whether pixel is black or white
                if(*iter & (1<<(iPixel)))
                {
                if(!Bus::send_word(this->mybus, color))
                    return false;
                }
                else
                {
                if(!Bus::send_word(this->mybus, background))
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