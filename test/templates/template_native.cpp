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
 * @file    template_native.c
 * @author  SO
 * @version v1.0.0
 * @date    16-March-2021
 * @brief   Unit tests template to be run on host.
 ******************************************************************************
 */

// ****** Includes ******
#include <unity.h>

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// ****** Define Tests ******
void test_name(void)
{
    TEST_ASSERT(true);
};

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_name();
    UNITY_END();
    return 0;
};