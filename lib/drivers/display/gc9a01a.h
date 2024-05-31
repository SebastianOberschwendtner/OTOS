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

#ifndef GC9A01A_H_
#define GC9A01A_H_

/* === includes === */
/* #include "interface.h" */
#include "drivers.h"

/* === Command codes === */
namespace gc9a01a
{
    /* Color Expression */
    /* 16 Bit RGB */
    template<uint8_t Red, uint8_t Green, uint8_t Blue>
    constexpr uint32_t RGB_16bit()
    {
        uint32_t color = 0;
        /* Add Red */
        color |= (Red & 0b11111000) << 8;
        /* Add Green */
        color |= (Green & 0b11111100) << 3;
        /* Add Blue */
        color |= (Blue & 0b11111000) >> 3;
        return color;
    };

    /* commands */
    enum class Command : uint8_t
    {
        Sleep_Out           = 0x11,
        Display_Off         = 0x28,
        Display_On          = 0x29,
        Column_Addr         = 0x2A,
        Page_Addr           = 0x2B,
        Write_Memory        = 0x2C,
        /* Memory_Access_Ctrl  = 0x36, */
        /* Set_Pixel_Format    = 0x3A, */
        /* RGB_Interface_Ctrl  = 0xB0, */
        /* Power_Ctrl_1        = 0xC0, */
        /* VCOM_Ctrl_1         = 0xC5 */
    };

    /* === Classes === */
    /** 
     * @class Controller
     * @brief Display controller for the GC9A01A display controller.
     * @tparam bus_controller The type of the used bus controller.
     * @tparam gpio The Pin class used for the bus communication.
     */
    template<class bus_controller, class gpio>
    class Controller
    {
    public:
        /* === Constructor === */
        Controller() = delete;

        /**
         * @brief Constructor for display controller.
         * @tparam bus_controller The type of the used bus controller.
         * @tparam gpio The Pin class used for the bus communication.
         * @param bus_used The reference to the used bus peripheral.
         */
        Controller(bus_controller& bus_used, gpio& dx_used, gpio& cs_used, gpio& bl_used);

        /* === Methods === */
        /**
         * @brief Draw a buffer which contains a color value for
         * every pixel of the display.
         *
         * @param buffer_begin The begin iterator of the buffer.
         * @param buffer_end The end of the buffer
         * @return Returns true when the buffer was send successfully.
         */
        auto draw(const uint16_t *buffer_begin, const uint16_t* buffer_end) -> bool;

        /**
         * @brief Draw a buffer which contains only a black-white value for
         * every pixel of the display.
         *
         * @param buffer_begin The begin iterator of the buffer.
         * @param buffer_end The end of the buffer
         * @param color The foreground color for "white" pixels.
         * @param background The background color for "black" pixels.
         * @return Returns true when the buffer was send successfully.
         */
        auto draw(const uint8_t *buffer_begin, const uint8_t* buffer_end, uint32_t color, uint32_t background) -> bool;

        /**
         * @brief Draw a buffer which contains only a black-white value for
         * every pixel of the display.
         *
         * This function can be given a call hook, for calling i.e. the
         * yield() function.
         *
         * @param buffer_begin The begin iterator of the buffer.
         * @param buffer_end The end of the buffer
         * @param color The foreground color for "white" pixels.
         * @param background The background color for "black" pixels.
         * @param hook Additional hook to function which gets called after each pixel transfer.
         * @return Returns true when the buffer was send successfully.
         */
        auto draw(const uint8_t *buffer_begin, const uint8_t* buffer_end, uint32_t color, uint32_t background, void (*hook)()) -> bool;

        /**
         * @brief Draw a single pixel on the display.
         *
         * @param x The X coordinate of the pixel.
         * @param y The Y coordinate of the pixel.
         * @param color The color of the pixel.
         * @return Returns true when the pixel was send successfully.
         */
        auto draw_pixel(uint32_t x, uint32_t y, uint32_t color) -> bool;

        /**
         * @brief Initialize the Display.
         * The configuration assumes the configuration of the
         * display as it is used on the Waveshare 1.28" round LCD board.
         *
         * @details The command is quite long and there is no documentation from
         * the manufacturer to explain the commands. The commands are
         * directly taken from the Waveshare example code.
         *
         * @return Returns true when the display was initialized successfully.
         */
        auto initialize_custom() -> bool;

        /**
         * @brief Turn the display off.
         *
         * @tparam bus_controller The type of the used bus controller.
         * @tparam gpio The Pin class used for the bus communication.
         * @return Returns True when command was send successfully.
         */
        auto off() -> bool;

        /**
         * @brief Turn the display on.
         *
         * @return Returns True when command was send successfully.
         */
        auto on() -> bool;

        /**
         * @brief Reset the Display.
         *
         * @return Returns True when command was send successfully.
         */
        auto reset() -> bool;

        /**
         * @brief Go out of sleep mode.
         *
         * @return Returns True when command was send successfully.
         */
        auto wake_up() -> bool;

    private:
        /* === Methods === */
        /**
         * @brief Send a command and 1 payload byte to the display.
         *
         * @param cmd The command code to send to the display.
         * @param byte0 The payload byte for the command.
         * @return Returns True when command was send successfully.
         */
        auto send_command(Command cmd, uint8_t byte0) -> bool;

        /**
         * @brief Send a command and 1 payload word to the display.
         *
         * @param cmd The command code to send to the display.
         * @param word0 The payload word for the command.
         * @return Returns True when command was send successfully.
         */
        auto send_command(Command cmd, uint32_t word0) -> bool;

        /**
         * @brief Send a command and 2 payload bytes to the display.
         *
         * @param cmd The command code to send to the display.
         * @param byte0 The 1st payload byte for the command.
         * @param byte1 The 2nd payload byte for the command.
         * @return Returns True when command was send successfully.
         */
        auto send_command(Command cmd, uint8_t byte0, uint8_t byte1) -> bool;

        /**
         * @brief Send a command and 2 payload words to the display.
         *
         * @param cmd The command code to send to the display.
         * @param word0 The 1st payload word for the command.
         * @param word1 The 2nd payload word for the command.
         * @return Returns True when command was send successfully.
         */
        auto send_command(Command cmd, uint32_t word0, uint32_t word1) -> bool;

        /**
         * @brief Send a command byte to the display.
         *
         * @param cmd The command code to send to the display.
         * @return Returns True when command was send successfully.
         */
        auto send_command_byte(Command cmd) -> bool;

        /**
         * @brief Send a data byte to the display.
         *
         * @param data The data byte to send to the display.
         * @return Returns True when command was send successfully.
         */
        auto send_data_byte(uint8_t data) -> bool;

        /**
         * @brief Send a register byte to the display without checking
         * whether the register is valid.
         *
         * @param reg The register code to send to the display.
         * @return Returns True when command was send successfully.
         */
        auto send_register_byte(uint8_t reg) -> bool;

        /* === Properties === */
        bus_controller mybus; /**< The bus controller used for communication. */
        gpio* dx_pin;   /**< The data pin used for communication. */
        gpio* cs_pin;   /**< The chip select pin used for communication. */
        gpio* bl_pin;   /**< The backlight pin used for communication. */
    };

    /* === Functions === */
    /**
     * @brief Setup the SPI bus object.
     * 
     * @tparam spi_bus The SPI bus instance used
     * @param bus_used The SPI bus object which will be used by the display controller.
     */
    void setup_spi_bus(spi::Controller& bus_used)
    {
        bus_used.set_clock_timing(Level::High, Edge::Rising);
        bus_used.set_use_hardware_chip_select(false);
        bus_used.enable();
    };
};

#endif
