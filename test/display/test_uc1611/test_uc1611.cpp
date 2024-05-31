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
/**
 ==============================================================================
 * @file    test_uc1611.cpp
 * @author  SO
 * @version v2.7.3
 * @date    26-Dezember-2021
 * @brief   Unit tests to test the UC1611S display controller.
 ==============================================================================
 */

/* === Includes === */
#include <unity.h>
#include <mock.h>

/** === Test List ===
 */

/* === Fixtures === */

/* Mock the bus driver */
struct Bus_Mock { };
Mock::Callable<bool> send_byte;
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_array_leader;

struct GPIO_Mock 
{
    Mock::Callable<bool> set_high;
    Mock::Callable<bool> set_low;
};

namespace bus {
    bool send_byte(Bus_Mock& bus, uint8_t data)
    {
       return ::send_byte(data);
    };
    bool send_bytes(Bus_Mock& bus, uint8_t byte0, uint8_t byte1)
    {
       return ::send_byte((byte0<<8) | byte1);
    };
    bool send_word(Bus_Mock& bus, uint32_t data)
    {
       return ::send_word(data);
    };
    bool send_array_leader(Bus_Mock& bus, const uint8_t byte, const uint8_t* data, const uint8_t n_bytes)
    { 
        return ::send_array_leader(byte, data, n_bytes); 
    };
};

/* === UUT === */
#include "display/uc1611.h"
#include "display/uc1611.cpp"
template class uc1611::Controller<Bus_Mock, GPIO_Mock>;

void setUp(){
    /* set stuff up here */
    ::send_byte.reset();
    ::send_word.reset();
    ::send_array_leader.reset();
};

void tearDown(){
    /* clean stuff up here */
};

/* === Define Tests === */
/** 
 * @brief Test the constructor of the display controller
 */
void test_constructor()
{
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);

    /* perform testing */
    CS.set_high.assert_called_once();
};

/** 
 * @brief Test adjusting the temperature compensation
 */
void test_temperature_compensation()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.set_temperature_compensation(uc1611::TC::_0_10_per_degC) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = static_cast<int>(uc1611::Command::Temperature_Compensation) 
        | static_cast<int>(uc1611::TC::_0_10_per_degC);
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test setting the contrast
 */
void test_setting_contrast()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.set_contrast(124) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = (static_cast<int>(uc1611::Command::Set_Potentiometer) << 8) 
        | 124U;
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test setting the line rate
 */
void test_line_rate()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.set_line_rate(3) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = static_cast<int>(uc1611::Command::Set_Line_Rate)
        | 3U;
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test enabling the display in black-white mode
 */
void test_enable_bw()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.enable_bw() );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = static_cast<int>(uc1611::Command::Set_Display_Enable)
        | 1U;
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test setting the display pattern when in black-white mode
 */
void test_set_display_pattern()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.show_pattern(1) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = static_cast<int>(uc1611::Command::Set_Display_Pattern)
        | (1 << 1) | 1;
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test setting COM end
 */
void test_set_COM_end()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.set_COM_end(127) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = ( static_cast<int>(uc1611::Command::Set_COM_End) << 8 )
        | 127;
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test setting partial display start
 */
void test_set_partial_display_start()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.set_partial_start(12) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = ( static_cast<int>(uc1611::Command::Set_Partial_Display_Start) << 8 )
        | 12;
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test setting partial display end
 */
void test_set_partial_display_end()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.set_partial_end(124) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = ( static_cast<int>(uc1611::Command::Set_Partial_Display_End) << 8 )
        | 124;
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test setting mirror options
 */
void test_set_mirrored()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.set_mirrored(true, true) );
    CS.set_low.assert_called_once();
    DX.set_low.assert_called_once();
    int32_t Expected = ( static_cast<int>(uc1611::Command::Set_LCD_Mapping_Ctrl) << 8 )
        | 0b110;
    ::send_byte.assert_called_once_with( Expected );
};

/** 
 * @brief Test drawing a display buffer
 */
void test_draw()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object and buffer */
    uc1611::Controller UUT(bus, DX, CS);
    CS.set_high.reset();
    std::array<uint8_t, 240*128/8> buffer{0};
    buffer.fill(13);

    /* Test the command */
    TEST_ASSERT_TRUE( UUT.draw(buffer.begin(), buffer.end()) );
    CS.set_low.assert_called_once();
    DX.set_high.assert_called_once();
    ::send_byte.assert_called_last_with( 13 );
    TEST_ASSERT_EQUAL( buffer.size(), ::send_byte.call_count);
};

/** 
 * === Run Tests ===
 */
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_constructor);
    RUN_TEST(test_temperature_compensation);
    RUN_TEST(test_setting_contrast);
    RUN_TEST(test_line_rate);
    RUN_TEST(test_enable_bw);
    RUN_TEST(test_set_display_pattern);
    RUN_TEST(test_set_COM_end);
    RUN_TEST(test_set_partial_display_start);
    RUN_TEST(test_set_partial_display_end);
    RUN_TEST(test_set_mirrored);
    RUN_TEST(test_draw);
    return UNITY_END();
};
