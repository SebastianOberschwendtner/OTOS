/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    test_file.cpp
 * @author  SO
 * @version v3.4.0
 * @date    08-January-2022
 * @brief   Unit tests for testing the File interface.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <algorithm>
#include <array>
#include <string_view>

// === UUT ===
#include "iostream.h"

/** === Test List ===
 */


// === Fixtures ===
struct Dummy_IO
{
    std::array<char, 80> char_buffer{0};
    std::size_t index{0};
    Mock::Callable<bool> call_put;
    Mock::Callable<bool> call_flush;
    Mock::Callable<bool> call_write;
    void put(char c) { call_put(c); char_buffer[index++] = c;};
    void flush(void) { call_flush(); };
    void write(const char* str, std::size_t len) { call_write(len);
        std::copy_n( str, len, char_buffer.begin()); };
};

void setUp(void)
{
    // set stuff up here
};

void tearDown(void){
    // clean stuff up here
};

// === Tests ===

/// @brief Test constructor of ostream class.
void test_ostream_constructor(void)
{
    // Create a dummy IO
    Dummy_IO io;
    
    // Create a new ostream object
    OTOS::ostream os{io};

    // Check if the object is created
    TEST_ASSERT_NOT_NULL(&os);
};

/// @brief Test the basic write method of the ostream class.
void test_ostream_basic(void)
{
    // Create a dummy IO
    Dummy_IO io;
    
    // Create a new ostream object
    OTOS::ostream os{io};

    // Write a character
    os.put('a');
    io.call_put.assert_called_once_with('a');

    // Write a string
    char c[]="This is sample text.";
    os.write(c, 5);
    io.call_write.assert_called_once_with(5);

    // flush buffer
    os.flush();
    io.call_flush.assert_called_once();
};

/// @brief Test the ostream output operator overloads
void test_ostream_output_overloads(void)
{
    // Create a dummy IO
    Dummy_IO io;
    
    // Create a new ostream object
    OTOS::ostream os{io};

    // Test writing null terminated strings
    char msg[] = "Null String.";
    os << msg;
    std::string_view result{io.char_buffer.data()};
    TEST_ASSERT_EQUAL(sizeof(msg)-1, io.call_put.call_count);
    TEST_ASSERT_EQUAL(0, result.compare(msg));
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_ostream_constructor);
    RUN_TEST(test_ostream_basic);
    RUN_TEST(test_ostream_output_overloads);
    return UNITY_END();
};
