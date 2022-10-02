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

#ifndef GC9A01A_H_
#define GC9A01A_H_

// === includes ===
// #include "interface.h"
#include "drivers.h"

// === Command codes ===
namespace GC9A01A
{
    // Color Expression
    // 16 Bit RGB
    template<unsigned char Red, unsigned char Green, unsigned char Blue>
    constexpr unsigned int RGB_16bit()
    {
        unsigned int color = 0;
        // Add Red
        color |= (Red & 0b11111000) << 8;
        // Add Green
        color |= (Green & 0b11111100) << 3;
        // Add Blue
        color |= (Blue & 0b11111000) >> 3;
        return color;
    };

    // commands
    enum class Command : unsigned char
    {
        Sleep_Out           = 0x11,
        Display_Off         = 0x28,
        Display_On          = 0x29,
        Column_Addr         = 0x2A,
        Page_Addr           = 0x2B,
        Write_Memory        = 0x2C,
        // Memory_Access_Ctrl  = 0x36,
        // Set_Pixel_Format    = 0x3A,
        // RGB_Interface_Ctrl  = 0xB0,
        // Power_Ctrl_1        = 0xC0,
        // VCOM_Ctrl_1         = 0xC5
    };

    // === Classes ===
    template<class bus_controller, class gpio>
    class Controller
    {
    private:
        // *** properties ***
        bus_controller mybus;
        gpio* const dx_pin;
        gpio* const cs_pin;
        gpio* const bl_pin;

        // *** methods ***
        bool send_command_byte(const Command cmd);
        bool send_register_byte(const unsigned char reg);
        bool send_data_byte(const unsigned char data);
        bool send_command(const Command cmd, const unsigned char byte0);
        bool send_command(const Command cmd, const unsigned int word0);
        bool send_command(const Command cmd, const unsigned char byte0, const unsigned char byte1);
        bool send_command(const Command cmd, const unsigned int word0, const unsigned int word1);

    public:
        // *** Constructor ***
        Controller() = delete;
        Controller(bus_controller& bus_used, gpio& dx_used, gpio& cs_used, gpio& bl_used);

        // *** Methods ***
        bool initialize_custom(void);
        bool on(void);
        bool off(void);
        bool reset(void);
        bool wake_up(void);
        bool draw(const unsigned short *buffer_begin, const unsigned short* buffer_end);
        bool draw(const unsigned char *buffer_begin, const unsigned char* buffer_end, const unsigned int color, const unsigned int background);
        bool draw(const unsigned char *buffer_begin, const unsigned char* buffer_end, const unsigned int color, const unsigned int background, void (*hook)());
        bool draw_pixel(const unsigned int x, const unsigned int y, const unsigned int color);
    };

    // === Functions ===
    /**
     * @brief Setup the SPI bus object.
     * 
     * @tparam spi_bus The SPI bus instance used
     * @param bus_used The SPI bus object which will be used by the display controller.
     */
    template<IO spi_bus>
    void setup_spi_bus(SPI::Controller<spi_bus>& bus_used)
    {
        bus_used.set_clock_timing(Level::High, Edge::Rising);
        bus_used.set_use_hardware_chip_select(false);
        bus_used.enable();
    };
};

#endif
