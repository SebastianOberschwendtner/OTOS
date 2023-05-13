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
 ******************************************************************************
 * @file    test_units.cpp
 * @author  SO
 * @version v4.2.0
 * @date    30-April-2023
 * @brief   Unit tests for testing the units of the OTOS project.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <ratio>
#include <array>

// Include the UUT
#include "misc/units.h"

void setUp()
{
    // set stuff up here
}

void tearDown()
{
    // clean stuff up here
}

// === Tests ===
/// @brief Test basic usage of units class
void test_units_usage()
{
    // Create a unit
    using Half = OTOS::Unit<std::ratio<1, 2>, int>;

    // Test default constructor
    Half half;
    TEST_ASSERT_EQUAL_INT(0, half);

    // Test assigning a value
    half = 10;
    TEST_ASSERT_EQUAL_INT(5, half);
    half = -20;
    TEST_ASSERT_EQUAL_INT(-10, half);

    // Test Copy constructor
    half = 20;
    const Half half2(half);
    TEST_ASSERT_EQUAL_INT(10, half2);

    // Get access to base type
    TEST_ASSERT_EQUAL_INT(10, half2.get());
}

/// @brief Test no rounding errors during conversion
void test_units_no_rounding_errors()
{
    // Create a unit
    using Volt = OTOS::Unit<std::ratio<10, 128>, uint16_t>;
    Volt volt;

    // Test assigning large value without rounding errors
    volt = 53760;
    TEST_ASSERT_EQUAL_INT(4200, volt);
}

/// @brief Test setting the converted value explicitly
void test_units_set_value()
{
    // Create a unit
    using Half = OTOS::Unit<std::ratio<1, 2>, int>;

    // Test constructor
    Half half{10};
    TEST_ASSERT_EQUAL_INT(10, half);

    // Test setting the value
    half.set_value(20);
    TEST_ASSERT_EQUAL_INT(20, half);
}

/// @brief Test casting from an integer
void test_units_casting()
{
    // Create a unit
    using Half = OTOS::Unit<std::ratio<1, 2>, int>;

    // Test casting to the unit
    Half half = static_cast<Half>(10);
    TEST_ASSERT_EQUAL_INT(10, half);
}

/// @brief Test using the unit in an array
void test_units_array()
{
    // Create a unit
    using Half = OTOS::Unit<std::ratio<1, 2>, int>;

    // Test using the unit in an array
    std::array<Half, 2> half_array{Half{10}, Half{20}};
    TEST_ASSERT_EQUAL_INT(10, half_array[0]);
    TEST_ASSERT_EQUAL_INT(20, half_array[1]);

    // Test using the unit in an 0 initialized array
    std::array<Half, 2> half_array2{};
    TEST_ASSERT_EQUAL_INT(0, half_array2[0]);

    // Test filling an array with a unit
    std::array<Half, 2> half_array3{};
    half_array3.fill(Half{10});
    TEST_ASSERT_EQUAL_INT(10, half_array3[0]);
}

/// === Run Tests ===
int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_units_usage);
    RUN_TEST(test_units_no_rounding_errors);
    RUN_TEST(test_units_set_value);
    RUN_TEST(test_units_casting);
    RUN_TEST(test_units_array);
    return UNITY_END();
}
