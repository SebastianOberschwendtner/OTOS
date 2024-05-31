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
 * @file    test_ili9341.cpp
 * @author  SO
 * @version v2.7.3
 * @date    23-Dezember-2021
 * @brief   Unit tests to test the ILI9341 display controller.
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
       return ::send_byte((byte0<<8) & byte1);
    };
    bool send_word(Bus_Mock& bus, uint16_t data)
    {
       return ::send_word(data);
    };
    bool send_array_leader(Bus_Mock& bus, const uint8_t byte, const uint8_t* data, const uint8_t n_bytes)
    { 
        return ::send_array_leader(byte, data, n_bytes); 
    };
};

/* === UUT === */
#include "display/ili9341.h"
#include "display/ili9341.cpp"
template class ili9341::Controller<Bus_Mock, GPIO_Mock>;

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
    ili9341::Controller UUT(bus, DX, CS);

    /* perform testing */
    CS.set_high.assert_called_once();
};

/** 
 * @brief Test turning the display on
 */
void test_on()
{
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    ili9341::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the side effects */
    TEST_ASSERT_TRUE( UUT.on() );
    DX.set_low.assert_called_once();
    CS.set_high.assert_called_once();
    CS.set_low.assert_called_once();
    ::send_byte.assert_called_once_with(
        static_cast<int>(ili9341::Command::Display_On)
        );
};

/** 
 * @brief Test turning the display on
 */
void test_off()
{
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    ili9341::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Test the side effects */
    TEST_ASSERT_TRUE( UUT.off() );
    DX.set_low.assert_called_once();
    CS.set_high.assert_called_once();
    CS.set_low.assert_called_once();
    ::send_byte.assert_called_once_with(
        static_cast<int>(ili9341::Command::Display_Off)
        );
};

/** 
 * @brief Test other single byte commands
 */
void test_single_byte_commands()
{
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    ili9341::Controller UUT(bus, DX, CS);

    /* Test reset */
    TEST_ASSERT_TRUE( UUT.reset() );
    ::send_byte.assert_called_last_with(
        static_cast<int>(ili9341::Command::Reset)
        );

    /* Test Sleep out */
    TEST_ASSERT_TRUE( UUT.wake_up() );
    ::send_byte.assert_called_last_with(
        static_cast<int>(ili9341::Command::Sleep_Out)
        );
};

/** 
 * @brief test sending command bytes with 1 data byte
 */
void test_command_one_data_byte()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    ili9341::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Send Power CTRL 1 */
    TEST_ASSERT_TRUE( UUT.set_power_control_1(0x0A) );
    DX.set_low.assert_called_once();
    DX.set_high.assert_called_once();
    CS.set_high.assert_called_once();
    CS.set_low.assert_called_once();
    ::send_byte.assert_called_last_with(0x0A);
    TEST_ASSERT_EQUAL(2, ::send_byte.call_count);

    /* *** Test other commands *** */
    /* Memory access */
    ::send_byte.reset();
    TEST_ASSERT_TRUE( UUT.set_memory_access(0x0b) );
    ::send_byte.assert_called_last_with(0x0b);
    TEST_ASSERT_EQUAL(2, ::send_byte.call_count);

    /* Pixel format access */
    ::send_byte.reset();
    TEST_ASSERT_TRUE( UUT.set_16bits_per_pixel() );
    ::send_byte.assert_called_last_with(0x55);
    TEST_ASSERT_EQUAL(2, ::send_byte.call_count);
};

/** 
 * @brief test sending command bytes with 2 data byte
 */
void test_command_two_data_byte()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    ili9341::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Send VCOM Control 1 */
    TEST_ASSERT_TRUE( UUT.set_VCOM_control_1(0x0A, 0x0B) );
    DX.set_low.assert_called_once();
    DX.set_high.assert_called_once();
    CS.set_high.assert_called_once();
    CS.set_low.assert_called_once();
    ::send_byte.assert_called_last_with(0x0b);
    TEST_ASSERT_EQUAL(3, ::send_byte.call_count);

    /* Test other commands */
};

// /**
//  * @brief Test filling the screen with one color
//  */
/* void test_fill_screen() */
/* { */
/*     setUp(); */
/*     /* Setup the mocked spi driver */
/*     Bus_Mock bus; */
/*     GPIO_Mock DX; */
/*     GPIO_Mock CS; */

/*     /* create the controller object */
/*     ILI9341::Controller UUT(bus, DX, CS); */
/*     CS.set_high.reset(); */

/*     /* Test the fill function */
/*     TEST_ASSERT_TRUE( UUT.fill(0xFF) ); */
/*     ::send_byte.assert_called_last_with(0xFF); */
/*     TEST_ASSERT_EQUAL(320*240, ::send_byte.call_count); */

/* }; */

/** 
 * @brief Test the color expressions
 */
void test_colors()
{
    /* Test default */
    auto Result = ili9341::RGB_16bit<0,0,0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000000000000000, Result);

    /* Test Red */
    Result = ili9341::RGB_16bit<8,0,0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000000000000001, Result);
    Result = ili9341::RGB_16bit<255,0,0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000000000011111, Result);

    /* Test Green */
    Result = ili9341::RGB_16bit<0,4,0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000000000100000, Result);
    Result = ili9341::RGB_16bit<0,255,0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000011111100000, Result);

    /* Test Blue */
    Result = ili9341::RGB_16bit<0,0,8>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000100000000000, Result);
    Result = ili9341::RGB_16bit<0,0,255>();
    TEST_ASSERT_BITS(0xFFFF, 0b1111100000000000, Result);
};

/** 
 * @brief Test drawing a buffer
 */
void test_draw_buffer()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    ili9341::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Create BW Canvas mock */
    std::array<uint16_t, 320*240> buffer{0};

    /* Test writing the buffer with all 0s */
    TEST_ASSERT_TRUE( UUT.draw(buffer.cbegin(), buffer.cend()) );
    ::send_byte.assert_called_once_with(static_cast<uint16_t>(ili9341::Command::Write_Memory));
    ::send_word.assert_called_last_with(0x0000);
    TEST_ASSERT_EQUAL(320*240, ::send_word.call_count);

    /* Test writing the buffer with all 0s */
    setUp();
    buffer.fill(0xFFFF);
    TEST_ASSERT_TRUE( UUT.draw(buffer.cbegin(), buffer.cend()) );
    ::send_byte.assert_called_once_with(static_cast<uint16_t>(ili9341::Command::Write_Memory));
    ::send_word.assert_called_last_with(0xFFFF);
    TEST_ASSERT_EQUAL(320*240, ::send_word.call_count);
};

/** 
 * @brief Test drawing a black-white buffer
 */
void test_draw_buffer_BW()
{
    setUp();
    /* Setup the mocked spi driver */
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;

    /* create the controller object */
    ili9341::Controller UUT(bus, DX, CS);
    CS.set_high.reset();

    /* Create BW Canvas mock */
    std::array<uint8_t, 320*240/8> buffer{0};

    /* Test writing the buffer with all 0s */
    TEST_ASSERT_TRUE( UUT.draw(buffer.cbegin(), buffer.cend(), 0xFFFF, 0x0000) );
    ::send_byte.assert_called_once_with(static_cast<uint16_t>(ili9341::Command::Write_Memory));
    ::send_word.assert_called_last_with(0x0000);
    TEST_ASSERT_EQUAL(320*240, ::send_word.call_count);

    /* Test writing the buffer with all 0s */
    setUp();
    buffer.fill(0xFF);
    TEST_ASSERT_TRUE( UUT.draw(buffer.cbegin(), buffer.cend(), 0xFFFF, 0x0000) );
    ::send_byte.assert_called_once_with(static_cast<uint16_t>(ili9341::Command::Write_Memory));
    ::send_word.assert_called_last_with(0xFFFF);
    TEST_ASSERT_EQUAL(320*240, ::send_word.call_count);

    /* Test writing the buffer when last bit is 0 */
    setUp();
    buffer.fill(0b0100'0000);
    TEST_ASSERT_TRUE( UUT.draw(buffer.cbegin(), buffer.cend(), 0xFFFF, 0x0000) );
    ::send_byte.assert_called_once_with(static_cast<uint16_t>(ili9341::Command::Write_Memory));
    ::send_word.assert_called_last_with(0x0000);
    TEST_ASSERT_EQUAL(320*240, ::send_word.call_count);

    /* Test writing the buffer when last bit is 1 */
    setUp();
    buffer.fill(0b1000'0000);
    TEST_ASSERT_TRUE( UUT.draw(buffer.cbegin(), buffer.cend(), 0xFFFF, 0x0000) );
    ::send_byte.assert_called_once_with(static_cast<uint16_t>(ili9341::Command::Write_Memory));
    ::send_word.assert_called_last_with(0xFFFF);
    TEST_ASSERT_EQUAL(320*240, ::send_word.call_count);
};

/** 
 * === Run Tests ===
 */
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_constructor);
    RUN_TEST(test_on);
    RUN_TEST(test_off);
    RUN_TEST(test_single_byte_commands);
    RUN_TEST(test_command_one_data_byte);
    RUN_TEST(test_command_two_data_byte);
    /* test_fill_screen(); */
    RUN_TEST(test_colors);
    RUN_TEST(test_draw_buffer);
    RUN_TEST(test_draw_buffer_BW);
    return UNITY_END();
};
