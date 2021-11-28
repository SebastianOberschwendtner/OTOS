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
 * @file    test_display.cpp
 * @author  SO
 * @version v1.4.0
 * @date    14-November-2021
 * @brief   Unit tests to test the display driver.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include "display/ssd1306.h"


/** === Test List ===
 * ✓ display driver initializes hardware correctly
 * ✓ display can be turned on
 * ✓ display can be tuned off
 * ▢ buffer content can be sent to display
 */

// === Fixtures ===

// Mock the i2c driver
class I2C_Mock : public Mock::Peripheral
{
public:
    // *** Constructor
    I2C_Mock(){};
    Mock::Callable<bool> set_target_address;
    Mock::Callable<bool> send_array;
    Mock::Callable<bool> send_array_leader;
};

namespace Mock {
// *** Variables to track calls
Callable<bool> send_byte;
Callable<bool> send_word;
};

namespace Bus {

    // bool send_word(I2C_Mock* bus, unsigned int word);
    bool send_word(I2C_Mock* bus, unsigned int word)
    {
        return Mock::send_word(word);
    };
};
// #include "display/ssd1306.cpp"
// template class SSD1306::Controller<I2C_Mock>;


void setUp(void){
    // set stuff up here
};

void tearDown(void){
    // clean stuff up here
};

// === Define Tests ===
void test_init(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    SSD1306::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.initialize());
    i2c.set_target_address.assert_called_once_with(SSD1306::i2c_address);
    TEST_ASSERT_TRUE(Mock::send_word.call_count > 0);
};

/// @brief Test whether the display can be turned on and off
void test_on_and_off(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    SSD1306::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.on());
    Mock::send_word.assert_called_last_with(
        static_cast<unsigned char>(SSD1306::Command::display_on));

    TEST_ASSERT_TRUE(UUT.off());
    Mock::send_word.assert_called_last_with(
        static_cast<unsigned char>(SSD1306::Command::display_off));
};

/// @brief Test whether a canvas buffer can be send.
void test_draw_buffer(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;
    unsigned char buffer[128 * 32 / 8];

    // create the controller object
    SSD1306::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.draw(buffer));
    TEST_ASSERT_EQUAL(4, i2c.send_array_leader.call_count);
    i2c.send_array_leader.assert_called_last_with(0x40);
};

/// === Run Tests ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    test_init();
    test_on_and_off();
    test_draw_buffer();
    UNITY_END();
    return 0;
};
