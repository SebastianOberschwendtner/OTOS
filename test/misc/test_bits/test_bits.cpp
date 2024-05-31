/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2023 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    test_bits.cpp
 * @author  SO
 * @version v4.1.0
 * @date    30-April-2023
 * @brief   Unit tests for testing the bit manipulation functions.
 ==============================================================================
 */

// === Includes ===
#include <unity.h>
#include <mock.h>

// Include the UUT
#include "misc/bits.h"

void setUp()
{
    // set stuff up here
}

void tearDown()
{
    // clean stuff up here
}

// === Tests ===
/// @brief Test getting a bitset from a byte
void test_get_bitset_from_byte()
{
    // Create the a byte to read from
    const uint8_t byte = 0b10101010;

    // Read the subset
    const auto result = bits::get(byte, {0b111, 5});

    // Check the result
    TEST_ASSERT_EQUAL(0b101, result);
}

/// @brief Test writing a bitset to a byte
void test_write_bitset_to_byte()
{
    // Create the a byte to read from
    const uint8_t byte = 0b10101010;
    const uint8_t data = 0b110;

    // Write the subset
    auto result = bits::set(byte, {0b111, 5, data});

    // Check the result
    TEST_ASSERT_EQUAL(0b11001010, result);
}

/// === Run Tests ===
int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_get_bitset_from_byte);
    RUN_TEST(test_write_bitset_to_byte);
    return UNITY_END();
}
