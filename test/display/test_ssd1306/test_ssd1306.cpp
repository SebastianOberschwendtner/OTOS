/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 -2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    test_display.cpp
 * @author  SO
 * @version v2.7.4
 * @date    14-November-2021
 * @brief   Unit tests to test the display driver.
 ==============================================================================
 */

/* === Includes === */
#include <unity.h>
#include <mock.h>

/** === Test List ===
 * ✓ display driver initializes hardware correctly
 * ✓ display can be turned on
 * ✓ display can be tuned off
 * ▢ buffer content can be sent to display
 */

/* === Fixtures === */

/* Mock the i2c driver */
struct I2C_Mock { };
Mock::Callable<bool> set_target_address;
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_array_leader;

namespace bus {
    void change_address(I2C_Mock& bus, const uint8_t address)
    {
        ::set_target_address(address);
        return;
    };
    bool send_word(I2C_Mock& bus, uint16_t word)
    {
       return ::send_word(word);
    };
    bool send_array_leader(I2C_Mock& bus, const uint8_t byte, uint8_t *const data, const uint8_t n_bytes)
    { 
        return ::send_array_leader(byte, data, n_bytes); 
    };
};

/* Include the UUT */
#include "display/ssd1306.h"
#include "display/ssd1306.cpp"
template class ssd1306::Controller<I2C_Mock>;

void setUp(){
    ::set_target_address.reset();
    ::send_word.reset();
    ::send_array_leader.reset();
};

void tearDown(){
    /* clean stuff up here */
};

/* === Define Tests === */
void test_init()
{
    /* Setup the mocked i2c driver */
    I2C_Mock i2c;

    /* create the controller object */
    ssd1306::Controller UUT(i2c);

    /* perform testing */
    TEST_ASSERT_TRUE(UUT.initialize());
    ::set_target_address.assert_called_once_with(ssd1306::i2c_address);
    TEST_ASSERT_TRUE(::send_word.call_count > 0);
};

/** 
 * @brief Test whether the display can be turned on and off
 */
void test_on_and_off()
{
    /* Setup the mocked i2c driver */
    I2C_Mock i2c;

    /* create the controller object */
    ssd1306::Controller UUT(i2c);

    /* perform testing */
    TEST_ASSERT_TRUE(UUT.on());
    ::send_word.assert_called_last_with(
        static_cast<uint8_t>(ssd1306::Command::display_on));

    TEST_ASSERT_TRUE(UUT.off());
    ::send_word.assert_called_last_with(
        static_cast<uint8_t>(ssd1306::Command::display_off));
};

/** 
 * @brief Test whether a canvas buffer can be send.
 */
void test_draw_buffer()
{
    /* Setup the mocked i2c driver */
    I2C_Mock i2c;
    uint8_t buffer[128 * 32 / 8];

    /* create the controller object */
    ssd1306::Controller UUT(i2c);

    /* perform testing */
    TEST_ASSERT_TRUE(UUT.draw(buffer));
    TEST_ASSERT_EQUAL(4, ::send_array_leader.call_count);
    ::send_array_leader.assert_called_last_with(0x40);
};

/** 
 * === Run Tests ===
 */
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_on_and_off);
    RUN_TEST(test_draw_buffer);
    return UNITY_END();
};
