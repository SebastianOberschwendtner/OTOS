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

#ifndef ILI9341_H_
#define ILI9341_H_

/* === includes === */
/* #include "interface.h" */
#include "drivers.h"

/* === Command codes === */
namespace ili9341
{
    /* Color Expression */
    /* 16 Bit RGB */
    template <uint8_t Red, uint8_t Green, uint8_t Blue>
    constexpr auto RGB_16bit() -> uint32_t
    {
        uint32_t color = 0;
        /* Add Red */
        color |= (Red & 0b11111000) >> 3;
        /* Add Green */
        color |= (Green & 0b11111100) << 3;
        /* Add Blue */
        color |= (Blue & 0b11111000) << 8;
        return color;
    };

    /* commands */
    enum class Command : uint8_t
    {
        Reset = 0x01,
        Sleep_Out = 0x11,
        Display_Off = 0x28,
        Display_On = 0x29,
        Column_Addr = 0x2A,
        Page_Addr = 0x2B,
        Write_Memory = 0x2C,
        Memory_Access_Ctrl = 0x36,
        Set_Pixel_Format = 0x3A,
        RGB_Interface_Ctrl = 0xB0,
        Power_Ctrl_1 = 0xC0,
        VCOM_Ctrl_1 = 0xC5
    };

    /* === Classes === */
    /** 
     * @brief The display controller for the ILI9341 display.
     * @tparam bus_controller The type of the used bus controller.
     * @tparam gpio The Pin class used for the bus communication.
     */
    template <class bus_controller, class gpio>
    class Controller
    {
      public:
        /* === Constructor === */
        Controller() = delete;

        /**
         * @brief Constructor for display controller.
         * @param bus_used The reference to the used bus peripheral.
         */
        Controller(bus_controller &bus_used, gpio &dx_used, gpio &cs_used);

        /* === Setters === */
        /**
         * @brief Set the pixelformat for the communication
         * to 16 bits per pixel for all interface methods.
         *
         * @return Returns True when command was send successfully.
         */
        auto set_16bits_per_pixel() -> bool;

        /**
         * @brief Set the memory access control of the display.
         *
         *
         * @param configuration The configuration bits as byte.
         * @return Returns True when command was send successfully.
         */
        auto set_memory_access(uint8_t configuration) -> bool;

        /**
         * @brief Set the Power Control 1 register of the display.
         *
         * The datasheet describes VRH as:
         * "Set the GVDD level, which is a reference level for the
         * VCOM level and grayscale voltage level."
         *
         * @param VHR THE GVDD level to set.
         * @return Returns True when command was send successfully.
         */
        auto set_power_control_1(uint8_t VHR) -> bool;

        /**
         * @brief Set the VCOM Control 1 register of the display.
         *
         * The datasheet describes VMH and VML as:
         * "VMH: Set the VCOMH voltage."
         * "VML: Set the VCOML voltage."
         *
         * @param VHR THE GVDD level to set.
         * @return Returns True when command was send successfully.
         */
        auto set_VCOM_control_1(uint8_t VMH, uint8_t VML) -> bool;

        /* === Methods === */
        /**
         * @brief Draw a buffer which contains a color value for
         * every pixel of the display.
         *
         * @param buffer_begin The begin iterator of the buffer.
         * @param buffer_end The end of the buffer
         * @return Returns true when the buffer was send successfully.
         */
        auto draw(const uint16_t *buffer_begin, const uint16_t *buffer_end) -> bool;

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
        auto draw(const uint8_t *buffer_begin, const uint8_t *buffer_end, uint32_t color, uint32_t background) -> bool;

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
        auto draw(const uint8_t *buffer_begin, const uint8_t *buffer_end, uint32_t color, uint32_t background, void (*hook)()) -> bool;
        /* bool fill(const uint32_t color); */

        /**
         * @brief Initialize the Display.
         * The configuration assumes the configuration of the
         * display as it is used on the STM32F429 Discovery Board.
         *
         * The display should be waked up before calling this command.
         *
         * @return Returns true when the display was initialized successfully.
         */
        auto initialize() -> bool;

        /**
         * @brief Turn the display off.
         *
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
         * @brief Send a command and 2 payload bytes to the display.
         *
         * @param cmd The command code to send to the display.
         * @param byte0 The 1st payload byte for the command.
         * @param byte1 The 2nd payload byte for the command.
         * @return Returns True when command was send successfully.
         */
        auto send_command(Command cmd, uint8_t byte0, uint8_t byte1) -> bool;

        /**
         * @brief Send a command byte to the display.
         *
         * @param cmd The command code to send to the display.
         * @return Returns True when command was send successfully.
         */
        auto send_command_byte(Command cmd) -> bool;
        auto send_data_byte(uint8_t data) -> bool;

        /* === Properties === */
        bus_controller mybus; /**< The bus controller used for communication */
        gpio *dx_pin;   /**< The data/command pin */
        gpio *cs_pin;   /**< The chip select pin */
    };

    /* === Functions === */
    /**
     * @brief Setup the SPI bus object.
     *
     * @tparam spi_bus The SPI bus instance used
     * @param bus_used The SPI bus object which will be used by the display controller.
     */
    void setup_spi_bus(spi::Controller &bus_used)
    {
        bus_used.set_clock_timing(Level::Low, Edge::Rising);
        bus_used.set_use_hardware_chip_select(false);
        bus_used.enable();
    };
}; // namespace ili9341

#endif
