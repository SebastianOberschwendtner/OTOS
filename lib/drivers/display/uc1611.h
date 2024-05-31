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

#ifndef UC1611_H_
#define UC1611_H_

/* === includes === */
/* #include "interface.h" */
#include "drivers.h"

/* === Command codes === */
namespace uc1611
{
    /* === Enums === */
    /* commands */
    enum class Command : uint8_t
    {
        Set_Column_Address_LSB      = 0b0000'0000,
        Set_Column_Address_MSB      = 0b0001'0000,
        Temperature_Compensation    = 0b0010'0100,
        Set_Panel_Loading           = 0b0010'1000,
        Set_Pump_Control            = 0b0010'1100,
        Set_Advanced_Program_Ctrl   = 0b0011'0000, /* double-byte command */
        Set_Scroll_Line_LSb         = 0b0100'0000,
        Set_Scroll_Line_MSB         = 0b0101'0000,
        Set_Page_Address_LSB        = 0b0110'0000,
        Set_Page_Address_MSB        = 0b0111'0000,
        Set_Potentiometer           = 0b1000'0001, /* double-byte command */
        /* set_Isolation_Clock_Front         = 0, */
        /* set_Isolation_Clock_Back          = 0, */
        Set_Partial_Display_Ctrl    = 0b1000'0100,
        Set_RAM_Address_Ctrl        = 0b1000'1000,
        Set_Fixed_Lines             = 0b1001'0000,
        Set_Line_Rate               = 0b1010'0000,
        Set_All_Pixel_On            = 0b1010'0100,
        Set_Inverse_Display         = 0b1010'0110,
        Set_Display_Enable          = 0b1010'1000,
        Set_LCD_Mapping_Ctrl        = 0b1100'0000, /* double-byte command */
        Set_nLine_Inversion         = 0b1100'1000, /* double-byte command */
        Set_Display_Pattern         = 0b1101'0000,
        Reset                       = 0b1110'0010,
        Nop                         = 0b1110'0010,
        Set_Test_Ctrl               = 0b1110'0100, /* double-byte command */
        Set_LCD_Bias_Ratio          = 0b1110'1000,
        Set_COM_End                 = 0b1111'0001,
        Set_Partial_Display_Start   = 0b1111'0010, /* double-byte command? */
        Set_Partial_Display_End     = 0b1111'0011, /* double-byte command? */
        Set_WP_Start_Column_Address = 0b1111'0100, /* double-byte command? */
        Set_WP_Start_Row_Address    = 0b1111'0101, /* double-byte command? */
        Set_WP_End_Column_Address   = 0b1111'0110, /* double-byte command? */
        Set_WP_End_Row_Address      = 0b1111'0111, /* double-byte command? */
        Window_Program_Mode         = 0b1111'1000,
        Set_MTP_Operation_Ctrl      = 0b1011'1000, /* double-byte command? */
        Set_MTP_Write_Mask          = 0b1011'1001, /* double-byte command? */
    };

    /* Command Options */
    enum class TC: uint8_t
    {
        /* 
         * Choose temperature compensation curves
         * which adjusts -0.xx [%]/[C°]
         */
        _0_05_per_degC = 0b00,
        _0_10_per_degC = 0b01,
        _0_15_per_degC = 0b10,
        _0_00_per_degC = 0b11,
    };

    /* === Classes === */
    /**
     * @brief Controller for the UC1611 display controller.
     * 
     * @tparam bus_controller The bus controller type used for communication.
     * @tparam gpio The GPIO type used for the control pins.
     */
    template<class bus_controller, class gpio>
    class Controller
    {
    public:
        /* === Constructor === */
        Controller() = delete;

        /**
         * @brief Constructor for display controller.
         * 
         * @param bus_used The reference to the used bus peripheral.
         * @param dx_used The data/command pin.
         * @param cs_used The chip select pin.
         */
        Controller(bus_controller& bus_used, gpio& dx_used, gpio& cs_used );

        /* === Setters === */
        /**
         * @brief Select the end COM segment which is active, when
         * the display does not use all the available COM segments.
         *
         * @param com_end The end of the COM segment. Should be [0..159].
         * @return Returns True when the command was sent successfully.
         */
        auto set_COM_end(uint8_t com_end) -> bool;

        /**
         * @brief Control the contrast of the display.
         *
         * @tparam bus_controller The type of the used bus controller.
         * @tparam gpio The Pin class used for the bus communication.
         * @param value The contrast value to set.
         * @return Returns True when the command was sent successfully.
         */
        auto set_contrast(uint8_t value) -> bool;

        /**
         * @brief Control the line rate of the display.
         *
         * @param rate The configuration bits for the line rate. Should be [0..3].
         * @return Returns True when the command was sent successfully.
         */
        auto set_line_rate(uint8_t rate) -> bool;

        /**
         * @brief Select the mirror options fot the display.
         *
         * @param x_mirror Mirror the X direction.
         * @param y_mirror Mirror the Y direction.
         * @return Returns True when the command was sent successfully.
         */
        auto set_mirrored(bool x_mirror, bool y_mirror) -> bool;

        /**
         * @brief Select the end COM segment for the COM scan period.
         *
         * @param end The end of the COM segment. Should be [0..159].
         * @return Returns True when the command was sent successfully.
         */
        auto set_partial_end(uint8_t end) -> bool;

        /**
         * @brief Select the start COM segment for the COM scan period.
         *
         * @param start The start of the COM segment. Should be [0..159].
         * @return Returns True when the command was sent successfully.
         */
        auto set_partial_start(uint8_t start) -> bool;

        /**
         * @brief Set the temperature compensation curve the controller
         * uses.
         *
         * @tparam bus_controller The type of the used bus controller.
         * @tparam gpio The Pin class used for the bus communication.
         * @param curve One of the four available compensation curves.
         * @return Returns True when the command was sent successfully.
         */
        auto set_temperature_compensation(TC curve) -> bool;

        /* === Methods === */
        /**
         * @brief Send a complete display buffer to the display controller.
         *
         * @param buffer_begin The begin iterator of the buffer data.
         * @param buffer_end  The end iterator of the buffer data.
         * @return Returns True when the buffer data was sent successfully.
         */
        auto draw(const uint8_t *buffer_begin, const uint8_t* buffer_end) -> bool;

        /**
         * @brief Send a complete display buffer to the display controller.
         * In addition, you can give a call hook which gets called after each
         * byte transfer.
         *
         * @param buffer_begin The begin iterator of the buffer data.
         * @param buffer_end  The end iterator of the buffer data.
         * @param hook The function to be called after each byte transfer.
         * @return Returns True when the buffer data was sent successfully.
         */
        auto draw(const uint8_t *buffer_begin, const uint8_t* buffer_end, void (*hook)()) -> bool;

        /**
         * @brief Enable the display in black-white mode.
         *
         * @return Returns True when the display was enabled successfully.
         */
        auto enable_bw() -> bool;

        /**
         * @brief Select which pattern the display should show when in black-white mode.
         * In this mode the display controller can store 4 patterns individually.
         *
         * @param pattern The pattern to display. Should be [0..3].
         * @return Returns True when the command was sent successfully.
         */
        auto show_pattern(uint8_t pattern) -> bool;

    private:
        /* === Methods === */
        /**
         * @brief Send a command byte to the display controller.
         *
         * @param cmd The command byte to send.
         * @return Returns True when the byte was send successfully.
         */
        auto send_command_byte(uint8_t cmd) -> bool;

        /**
         * @brief Send a command byte to the display controller.
         *
         * @param cmd The command byte to send.
         * @return Returns True when the byte was send successfully.
         */
        auto send_command_bytes(uint8_t byte0, uint8_t byte1) -> bool;

        /**
         * @brief Send a data byte to the display controller.
         *
         * @tparam bus_controller The type of the used bus controller.
         * @tparam gpio The Pin class used for the bus communication.
         * @param data The data byte to send.
         * @return Returns True when the byte was send successfully.
         */
        auto send_data_byte(uint8_t data) -> bool;

        /* === Properties === */
        bus_controller mybus; /**< The bus controller used for communication */
        gpio* dx_pin;         /**< The data/command pin */
        gpio* cs_pin;         /**< The chip select pin */
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

    /**
     * @brief Configure the controller to be used for
     * the DOGXL 240-7 displays from Electronic Assembly.
     * 
     * @tparam spi_bus The SPI bus instance used
     * @param controller The reference to the used controller object.
     */
    template<class bus_controller, class gpio>
    void configure_DOGXL240(Controller<bus_controller, gpio>& controller)
    {
        controller.set_COM_end(127);
        controller.set_partial_start(0);
        controller.set_partial_end(127);
        controller.set_contrast(143);
        controller.set_mirrored(false, true);
        controller.set_line_rate(3);
        controller.set_temperature_compensation(TC::_0_10_per_degC);
        controller.enable_bw();
        controller.show_pattern(0);
    };
}; // namespace uc1611

#endif // UC1611_H_
