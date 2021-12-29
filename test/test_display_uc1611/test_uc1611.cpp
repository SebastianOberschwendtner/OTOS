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
 * @file    test_uc1611.cpp
 * @author  SO
 * @version v2.3.0
 * @date    26-Dezember-2021
 * @brief   Unit tests to test the UC1611S display controller.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>

/** === Test List ===
 */

// === Fixtures ===

// Mock the bus driver
struct Bus_Mock { };
Mock::Callable<bool> send_byte;
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_array_leader;

struct GPIO_Mock 
{
    Mock::Callable<bool> set_high;
    Mock::Callable<bool> set_low;
};

namespace Bus {
    bool send_byte(Bus_Mock& bus, unsigned char data)
    {
       return ::send_byte(data);
    };
    bool send_bytes(Bus_Mock& bus, unsigned char byte0, unsigned char byte1)
    {
       return ::send_byte((byte0<<8) | byte1);
    };
    bool send_word(Bus_Mock& bus, unsigned int data)
    {
       return ::send_word(data);
    };
    bool send_array_leader(Bus_Mock& bus, const unsigned char byte, const unsigned char* data, const unsigned char n_bytes)
    { 
        return ::send_array_leader(byte, data, n_bytes); 
    };
};

// === UUT ===
#include "display/uc1611.h"
#include "display/uc1611.cpp"
template class UC1611::Controller<Bus_Mock, GPIO_Mock>;

void setUp(void){
    // set stuff up here
    ::send_byte.reset();
    ::send_word.reset();
    ::send_array_leader.reset();
};

void tearDown(void){
    // clean stuff up here
};

// === Define Tests ===
/// @brief Test the constructor of the display controller
void test_constructor(void)
{
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);

    // perform testing
    CS.set_high.assert_called_once();
};

/// @brief Test adjusting the temperature compensation
void test_temperature_compensation(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.set_temperature_compensation(UC1611::TC::_0_10_per_degC) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = static_cast<int>(UC1611::Command::Temperature_Compensation) 
        | static_cast<int>(UC1611::TC::_0_10_per_degC);
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test setting the contrast
void test_setting_contrast(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.set_contrast(124) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = (static_cast<int>(UC1611::Command::Set_Potentiometer) << 8) 
        | 124U;
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test setting the line rate
void test_line_rate(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.set_line_rate(3) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = static_cast<int>(UC1611::Command::Set_Line_Rate)
        | 3U;
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test enabling the display in black-white mode
void test_enable_bw(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.enable_bw() );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = static_cast<int>(UC1611::Command::Set_Display_Enable)
        | 1U;
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test setting the display pattern when in black-white mode
void test_set_display_pattern(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.show_pattern(1) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = static_cast<int>(UC1611::Command::Set_Display_Pattern)
        | (1 << 1) | 1;
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test setting COM end
void test_set_COM_end(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.set_COM_end(127) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = ( static_cast<int>(UC1611::Command::Set_COM_End) << 8 )
        | 127;
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test setting partial display start
void test_set_partial_display_start(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.set_partial_start(12) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = ( static_cast<int>(UC1611::Command::Set_Partial_Display_Start) << 8 )
        | 12;
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test setting partial display end
void test_set_partial_display_end(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.set_partial_end(124) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = ( static_cast<int>(UC1611::Command::Set_Partial_Display_End) << 8 )
        | 124;
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test setting mirror options
void test_set_mirrored(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    // Test the command
    TEST_ASSERT_TRUE( UUT.set_mirrored(true, true) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int Expected = ( static_cast<int>(UC1611::Command::Set_LCD_Mapping_Ctrl) << 8 )
        | 0b110;
    ::send_byte.assert_called_once_with( Expected );
};

/// @brief Test drawing a display buffer
void test_draw(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    // create the controller object and buffer
    UC1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();
    std::array<unsigned char, 240*128/8> buffer{0};
    buffer.fill(13);

    // Test the command
    TEST_ASSERT_TRUE( UUT.draw(buffer.begin(), buffer.end()) );
    CS.set_low.assert_called_once();
    DX.set_high.assert_called_once();
    ::send_byte.assert_called_last_with( 13 );
    TEST_ASSERT_EQUAL( buffer.size(), ::send_byte.call_count);
};

/// === Run Tests ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    test_constructor();
    test_temperature_compensation();
    test_setting_contrast();
    test_line_rate();
    test_enable_bw();
    test_set_display_pattern();
    test_set_COM_end();
    test_set_partial_display_start();
    test_set_partial_display_end();
    test_set_mirrored();
    test_draw();
    UNITY_END();
    return EXIT_SUCCESS;
};
