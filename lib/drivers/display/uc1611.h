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

#ifndef UC1611_H_
#define UC1611_H_

// === includes ===
// #include "interface.h"
#include "drivers.h"

// === Command codes ===
namespace UC1611
{
    // === Enums ===
    // commands
    enum class Command : unsigned char
    {
        Set_Column_Address_LSB      = 0b0000'0000,
        Set_Column_Address_MSB      = 0b0001'0000,
        Temperature_Compensation    = 0b0010'0100,
        Set_Panel_Loading           = 0b0010'1000,
        Set_Pump_Control            = 0b0010'1100,
        Set_Advanced_Program_Ctrl   = 0b0011'0000, // double-byte command
        Set_Scroll_Line_LSb         = 0b0100'0000,
        Set_Scroll_Line_MSB         = 0b0101'0000,
        Set_Page_Address_LSB        = 0b0110'0000,
        Set_Page_Address_MSB        = 0b0111'0000,
        Set_Potentiometer           = 0b1000'0001, // double-byte command
        // set_Isolation_Clock_Front         = 0,
        // set_Isolation_Clock_Back          = 0,
        Set_Partial_Display_Ctrl    = 0b1000'0100,
        Set_RAM_Address_Ctrl        = 0b1000'1000,
        Set_Fixed_Lines             = 0b1001'0000,
        Set_Line_Rate               = 0b1010'0000,
        Set_All_Pixel_On            = 0b1010'0100,
        Set_Inverse_Display         = 0b1010'0110,
        Set_Display_Enable          = 0b1010'1000,
        Set_LCD_Mapping_Ctrl        = 0b1100'0000, // double-byte command
        Set_nLine_Inversion         = 0b1100'1000, // double-byte command
        Set_Display_Pattern         = 0b1101'0000,
        Reset                       = 0b1110'0010,
        Nop                         = 0b1110'0010,
        Set_Test_Ctrl               = 0b1110'0100, // double-byte command
        Set_LCD_Bias_Ratio          = 0b1110'1000,
        Set_COM_End                 = 0b1111'0001,
        Set_Partial_Display_Start   = 0b1111'0010, // double-byte command?
        Set_Partial_Display_End     = 0b1111'0011, // double-byte command?
        Set_WP_Start_Column_Address = 0b1111'0100, // double-byte command?
        Set_WP_Start_Row_Address    = 0b1111'0101, // double-byte command?
        Set_WP_End_Column_Address   = 0b1111'0110, // double-byte command?
        Set_WP_End_Row_Address      = 0b1111'0111, // double-byte command?
        Window_Program_Mode         = 0b1111'1000,
        Set_MTP_Operation_Ctrl      = 0b1011'1000, // double-byte command?
        Set_MTP_Write_Mask          = 0b1011'1001, // double-byte command?
    };

    // Command Options
    enum class TC: unsigned char
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

    // === Classes ===
    template<class bus_controller, class gpio>
    class Controller
    {
    private:
        // *** properties ***
        bus_controller mybus;
        gpio* const dx_pin;
        gpio* const cs_pin;

        // *** methods ***
        bool send_command_byte(const unsigned char cmd);
        bool send_command_bytes(const unsigned char byte0, const unsigned char byte1);
        bool send_data_byte(const unsigned char data);

    public:
        // *** Constructor ***
        Controller() = delete;
        Controller(bus_controller& bus_used, gpio& dx_used, gpio& cs_used );

        // *** Methods ***
        bool set_temperature_compensation( const TC curve );
        bool set_contrast( const unsigned char value );
        bool set_line_rate( const unsigned char rate );
        bool enable_bw(void);
        bool show_pattern(const unsigned char pattern);
        bool set_COM_end(const unsigned char com_end);
        bool set_partial_start(const unsigned char start);
        bool set_partial_end(const unsigned char end);
        bool set_mirrored(const bool x_mirror, const bool y_mirror);
        bool draw(const unsigned char *buffer_begin, const unsigned char* buffer_end);
        bool draw(const unsigned char *buffer_begin, const unsigned char* buffer_end, void (*hook)());
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
};

#endif
