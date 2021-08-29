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
 * @file    test_gpio_stm32.c
 * @author  SO
 * @version v1.0.5
 * @date    16-March-2021
 * @brief   Unit tests for the gpio drivers of stm32 controllers.
 ******************************************************************************
 */

// ****** Includes ******
#include <unity.h>
#include <stm32/gpio_stm32.h>

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// ****** Define Tests ******
void test_init_pin(void)
{
    // Assume the AHB1ENR register was already written to
    RCC->AHB1ENR = 0b1000;

    // Create GPIO object
    GPIO::PIN UUT(GPIO::PORTA, GPIO::PIN0);

    // Check whether clock for port was enabled and already
    // set bit was not deleted.
    TEST_ASSERT_EQUAL(0b1001, RCC->AHB1ENR);
};

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init_pin();
    UNITY_END();
    return 0;
};