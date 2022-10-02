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
 * @file    test_gc9a01a.cpp
 * @author  SO
 * @version v2.10.0
 * @date    25-September-2022
 * @brief   Unit tests to test the GC9A01A display controller. (Used by the round 1.28" display from Waveshare)
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>

/** === Test List ===
 */

// === Fixtures ===

// Mock the bus driver
struct Bus_Mock
{
};
Mock::Callable<bool> send_byte;
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_array_leader;

struct GPIO_Mock
{
    Mock::Callable<bool> set_high;
    Mock::Callable<bool> set_low;
};

namespace Bus
{
    bool send_byte(Bus_Mock &bus, unsigned char data)
    {
        return ::send_byte(data);
    };
    bool send_bytes(Bus_Mock &bus, unsigned char byte0, unsigned char byte1)
    {
        return ::send_byte((byte0 << 8) & byte1);
    };
    bool send_word(Bus_Mock &bus, unsigned int data)
    {
        return ::send_word(data);
    };
    bool send_array_leader(Bus_Mock &bus, const unsigned char byte, const unsigned char *data, const unsigned char n_bytes)
    {
        return ::send_array_leader(byte, data, n_bytes);
    };
};

// === UUT ===
#include "display/gc9a01a.h"
#include "display/gc9a01a.cpp"
template class GC9A01A::Controller<Bus_Mock, GPIO_Mock>;

void setUp(void)
{
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
    GPIO_Mock BL;

    // create the controller object
    GC9A01A::Controller UUT(bus, DX, CS, BL);

    // perform testing
    CS.set_high.assert_called_once();
};

/// @brief Test turning the display on
void test_on(void)
{
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;
    GPIO_Mock BL;

    // create the controller object
    GC9A01A::Controller UUT(bus, DX, CS, BL);
    CS.set_high.reset();

    // Test the side effects
    TEST_ASSERT_TRUE(UUT.on());
    DX.set_low.assert_called_once();
    CS.set_high.assert_called_once();
    CS.set_low.assert_called_once();
    ::send_byte.assert_called_once_with(
        static_cast<int>(GC9A01A::Command::Display_On));
};

/// @brief Test turning the display on
void test_off(void)
{
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;
    GPIO_Mock BL;

    // create the controller object
    GC9A01A::Controller UUT(bus, DX, CS, BL);
    CS.set_high.reset();

    // Test the side effects
    TEST_ASSERT_TRUE(UUT.off());
    DX.set_low.assert_called_once();
    CS.set_high.assert_called_once();
    CS.set_low.assert_called_once();
    ::send_byte.assert_called_once_with(
        static_cast<int>(GC9A01A::Command::Display_Off));
};

/// @brief Test other single byte commands
void test_single_byte_commands(void)
{
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;
    GPIO_Mock BL;

    // create the controller object
    GC9A01A::Controller UUT(bus, DX, CS, BL);

    // Test Sleep out
    TEST_ASSERT_TRUE(UUT.wake_up());
    ::send_byte.assert_called_last_with(
        static_cast<int>(GC9A01A::Command::Sleep_Out));
};

/// @brief Test the color expressions
void test_colors(void)
{
    // Test default
    auto Result = GC9A01A::RGB_16bit<0, 0, 0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000000000000000, Result);

    // Test Red
    Result = GC9A01A::RGB_16bit<8, 0, 0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000100000000000, Result);
    Result = GC9A01A::RGB_16bit<255, 0, 0>();
    TEST_ASSERT_BITS(0xFFFF, 0b1111100000000000, Result);

    // Test Green
    Result = GC9A01A::RGB_16bit<0, 4, 0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000000000100000, Result);
    Result = GC9A01A::RGB_16bit<0, 255, 0>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000011111100000, Result);

    // Test Blue
    Result = GC9A01A::RGB_16bit<0, 0, 8>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000000000000001, Result);
    Result = GC9A01A::RGB_16bit<0, 0, 255>();
    TEST_ASSERT_BITS(0xFFFF, 0b0000000000011111, Result);
};

/// @brief Test drawing a buffer
void test_draw_buffer(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;
    GPIO_Mock BL;

    // create the controller object
    GC9A01A::Controller UUT(bus, DX, CS, BL);
    CS.set_high.reset();

    // Create BW Canvas mock
    std::array<unsigned short, 240 * 240> buffer{0};

    // Test writing the buffer with all 0s
    TEST_ASSERT_TRUE(UUT.draw(buffer.cbegin(), buffer.cend()));
    ::send_byte.assert_called_once_with(static_cast<unsigned short>(GC9A01A::Command::Write_Memory));
    ::send_word.assert_called_last_with(0x0000);
    TEST_ASSERT_EQUAL(240 * 240, ::send_word.call_count);

    // Test writing the buffer with all 0s
    setUp();
    buffer.fill(0xFFFF);
    TEST_ASSERT_TRUE(UUT.draw(buffer.cbegin(), buffer.cend()));
    ::send_byte.assert_called_once_with(static_cast<unsigned short>(GC9A01A::Command::Write_Memory));
    ::send_word.assert_called_last_with(0xFFFF);
    TEST_ASSERT_EQUAL(240 * 240, ::send_word.call_count);
};

/// @brief Test drawing a black-white buffer
void test_draw_buffer_BW(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;
    GPIO_Mock BL;

    // create the controller object
    GC9A01A::Controller UUT(bus, DX, CS, BL);
    CS.set_high.reset();

    // Create BW Canvas mock
    std::array<unsigned char, 240 * 240 / 8> buffer{0};

    // Test writing the buffer with all 0s
    TEST_ASSERT_TRUE(UUT.draw(buffer.cbegin(), buffer.cend(), 0xFFFF, 0x0000));
    ::send_byte.assert_called_once_with(static_cast<unsigned short>(GC9A01A::Command::Write_Memory));
    ::send_word.assert_called_last_with(0x0000);
    TEST_ASSERT_EQUAL(240 * 240, ::send_word.call_count);

    // Test writing the buffer with all 0s
    setUp();
    buffer.fill(0xFF);
    TEST_ASSERT_TRUE(UUT.draw(buffer.cbegin(), buffer.cend(), 0xFFFF, 0x0000));
    ::send_byte.assert_called_once_with(static_cast<unsigned short>(GC9A01A::Command::Write_Memory));
    ::send_word.assert_called_last_with(0xFFFF);
    TEST_ASSERT_EQUAL(240 * 240, ::send_word.call_count);

    // Test writing the buffer when last bit is 0
    setUp();
    buffer.fill(0b0100'0000);
    TEST_ASSERT_TRUE(UUT.draw(buffer.cbegin(), buffer.cend(), 0xFFFF, 0x0000));
    ::send_byte.assert_called_once_with(static_cast<unsigned short>(GC9A01A::Command::Write_Memory));
    ::send_word.assert_called_last_with(0x0000);
    TEST_ASSERT_EQUAL(240 * 240, ::send_word.call_count);

    // Test writing the buffer when last bit is 1
    setUp();
    buffer.fill(0b1000'0000);
    TEST_ASSERT_TRUE(UUT.draw(buffer.cbegin(), buffer.cend(), 0xFFFF, 0x0000));
    ::send_byte.assert_called_once_with(static_cast<unsigned short>(GC9A01A::Command::Write_Memory));
    ::send_word.assert_called_last_with(0xFFFF);
    TEST_ASSERT_EQUAL(240 * 240, ::send_word.call_count);
};

/// @brief Test writing one pixel to the display
void test_draw_pixel(void)
{
    setUp();
    // Setup the mocked spi driver
    Bus_Mock bus;
    GPIO_Mock DX;
    GPIO_Mock CS;
    GPIO_Mock BL;

    // create the controller object
    GC9A01A::Controller UUT(bus, DX, CS, BL);
    CS.set_high.reset();

    // Test writing a pixel
    TEST_ASSERT_TRUE(UUT.draw_pixel(0, 0, 0xFFFF));
    ::send_byte.assert_called_last_with(static_cast<unsigned short>(GC9A01A::Command::Write_Memory));
    ::send_word.assert_called_last_with(0xFFFF);
};

/// === Run Tests ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_constructor);
    RUN_TEST(test_on);
    RUN_TEST(test_off);
    RUN_TEST(test_single_byte_commands);
    RUN_TEST(test_colors);
    RUN_TEST(test_draw_buffer);
    RUN_TEST(test_draw_buffer_BW);
    RUN_TEST(test_draw_pixel);
    return UNITY_END();
};
