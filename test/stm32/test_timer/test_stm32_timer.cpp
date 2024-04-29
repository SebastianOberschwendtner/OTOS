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
 * @file    test_timer_stm32.cpp
 * @author  SO
 * @version v4.2.0
 * @date    31-October-2021
 * @brief   Unit tests for testing the timer driver for stm32 microcontrollers.
 ==============================================================================
 */

// ****** Includes ******
#include <unity.h>
#include <mock.h>
#include "stm32/timer_stm32.h"
#include "stm32/gpio_stm32.h"

/** === Test List ===
 * ✓ Timer instance can be created:
 *      ✓ clock for peripheral gets enabled
 * ✓ Timer count can be read
 * ✓ Timer can be started
 * ✓ Timer can be stopped
 * ✓ The top value (reload value) can be set
 * ✓ The compare match value can be set
 * ✓ The timer prescaler can be set to match a certain rate
 * ▢ The timer mode can be read
 * ▢ The mode of the timer channels can be set:
 *      ✓ PWM Output Mode
 *      ▢ Input Capture Mode
 * (✓) I/O Pins can be assigned to the timer
*/

// === Mocks ===
extern Mock::Callable<bool> CMSIS_NVIC_EnableIRQ;
extern Mock::Callable<bool> CMSIS_NVIC_SetPriority;
extern Mock::Callable<uint32_t> CMSIS_SysTick_Config;

// === Tests ===
void setUp()
{
    // set stuff up here
    TIM1->registers_to_default();
    RCC->registers_to_default();
};

void tearDown(){
    // clean stuff up here
};

/// @brief Test the initialization of the controller
void test_init()
{
    // Create timer
    Timer::Timer UUT(IO::TIM_1);

    // perform tests
    TEST_ASSERT_BIT_HIGH(0, RCC->APB2ENR);
    TEST_ASSERT_EQUAL(0xFFFF, TIM1->ARR);
};

/// @brief Test the SysTick configuration
void test_configure_SysTick()
{
    // Reset mocked function
    CMSIS_SysTick_Config.reset();   

    // Configure SysTick
    Timer::SysTick_Configure();

    // Test whether function was called with the correct parameters
    unsigned int Expected = F_CPU / 1000;
    CMSIS_SysTick_Config.assert_called_once_with(Expected);
    TEST_ASSERT_EQUAL(2, CMSIS_NVIC_SetPriority.call_count);
};

/// @brief Test the reading of the counter value
void test_get_count()
{
    // Set stuff up
    setUp();

    // Create timer
    Timer::Timer UUT(IO::TIM_1);

    // First the count should be 0
    TEST_ASSERT_EQUAL(0, UUT.get_count());

    // Increase the actual counter and test whether result is correct
    TIM1->CNT++;
    TEST_ASSERT_EQUAL(1, UUT.get_count());
};

/// @brief Test enabling and disabling the timer
void test_enable_disable()
{
    // Set stuff up
    setUp();

    // Create timer
    Timer::Timer UUT(IO::TIM_1);

    // Test whether timer is disabled by default
    TEST_ASSERT_FALSE(UUT.is_running());

    // Enable timer
    UUT.start();

    // Test whether timer is enabled
    TEST_ASSERT_BIT_HIGH(0, TIM1->CR1);
    TEST_ASSERT_TRUE(UUT.is_running());

    // Disable timer
    UUT.stop();

    // Test whether timer is disabled
    TEST_ASSERT_BIT_LOW(0, TIM1->CR1);
    TEST_ASSERT_FALSE(UUT.is_running());
};

/// @brief Test setting the counter frequency
void test_set_tick_frequency()
{
    using namespace OTOS::literals;
    // Set stuff up
    setUp();

    // Create timer
    Timer::Timer UUT(IO::TIM_1);
    Timer::Timer UUT2(IO::TIM_2);

    // Set frequency
    UUT.set_tick_frequency(1_kHz);
    UUT2.set_tick_frequency(5_kHz);

    // Test whether the correct value was written to the register
    uint32_t expected = F_APB2 / 1000 - 1;
    TEST_ASSERT_EQUAL(expected, TIM1->PSC);
    expected = F_APB1 / 5000 - 1;
    TEST_ASSERT_EQUAL(expected, TIM2->PSC);
};

/// @brief Test setting the top value
void test_set_top_value()
{
    using namespace std::literals::chrono_literals;
    using namespace  OTOS::literals;

    // Set stuff up
    setUp();

    // Create timer
    Timer::Timer UUT(IO::TIM_1);
    UUT.set_tick_frequency(1_kHz);

    // Set top value
    UUT.set_top_value(100);
    TEST_ASSERT_EQUAL(100, TIM1->ARR);

    // Set top value as a period
    UUT.set_period(1s);
    TEST_ASSERT_EQUAL(1000, TIM1->ARR);
    UUT.set_period(1ms);
    TEST_ASSERT_EQUAL(1, TIM1->ARR);
};

/// @brief Test setting the compare mode
void test_set_pwm_mode()
{
    // Set stuff up
    setUp();

    // Create timer
    Timer::Timer UUT(IO::TIM_1);

    // Set compare mode
    auto ch1 = UUT.get_channel(1);
    ch1.set_mode(Timer::Mode::PWM);
    // UUT.set_channel(1, Timer::Mode::PWM);
    TEST_ASSERT_BITS_LOW(0b11, TIM1->CCMR1);
    TEST_ASSERT_BITS((0b111<<4), (0b110<<4), TIM1->CCMR1);
    UUT.set_channel(2, Timer::Mode::PWM);
    TEST_ASSERT_BITS_LOW(0b11 << 8, TIM1->CCMR1);
    TEST_ASSERT_BITS((0b111<<12), (0b110<<12), TIM1->CCMR1);
    UUT.set_channel(3, Timer::Mode::PWM);
    TEST_ASSERT_BITS_LOW(0b11, TIM1->CCMR2);
    TEST_ASSERT_BITS((0b111<<4), (0b110<<4), TIM1->CCMR2);
    UUT.set_channel(4, Timer::Mode::PWM);
    TEST_ASSERT_BITS_LOW(0b11 << 8, TIM1->CCMR2);
    TEST_ASSERT_BITS((0b111<<12), (0b110<<12), TIM1->CCMR2);
};

/// @brief Test enabling and disabling the timer channels
void test_enable_disable_channel()
{
    // Set stuff up
    setUp();

    // Create timer
    Timer::Timer UUT(IO::TIM_1);

    // Enable output compare channels
    UUT.enable_channel(1);
    TEST_ASSERT_BIT_HIGH(TIM_CCER_CC1E_Pos, TIM1->CCER);
    UUT.enable_channel(2);
    TEST_ASSERT_BIT_HIGH(TIM_CCER_CC2E_Pos, TIM1->CCER);
    UUT.enable_channel(3);
    TEST_ASSERT_BIT_HIGH(TIM_CCER_CC3E_Pos, TIM1->CCER);
    UUT.enable_channel(4);
    TEST_ASSERT_BIT_HIGH(TIM_CCER_CC4E_Pos, TIM1->CCER);

    // Disable output compare channels
    UUT.disable_channel(1);
    TEST_ASSERT_BIT_LOW(TIM_CCER_CC1E_Pos, TIM1->CCER);
    UUT.disable_channel(2);
    TEST_ASSERT_BIT_LOW(TIM_CCER_CC2E_Pos, TIM1->CCER);
    UUT.disable_channel(3);
    TEST_ASSERT_BIT_LOW(TIM_CCER_CC3E_Pos, TIM1->CCER);
    UUT.disable_channel(4);
    TEST_ASSERT_BIT_LOW(TIM_CCER_CC4E_Pos, TIM1->CCER);

    // Enable channels via the channel object
    auto ch1 = UUT.get_channel(1);
    ch1.enable();
    TEST_ASSERT_BIT_HIGH(TIM_CCER_CC1E_Pos, TIM1->CCER);
    ch1.disable();
    TEST_ASSERT_BIT_LOW(TIM_CCER_CC1E_Pos, TIM1->CCER);
};

/// @brief Test setting the compare values
void test_set_compare_value()
{
    using namespace OTOS::literals;
    using namespace std::literals::chrono_literals;
    // Set stuff up
    setUp();

    // Create timer
    Timer::Timer UUT(IO::TIM_1);

    // Set compare values as ticks
    auto ch1 = UUT.get_channel(1);
    ch1.set_compare_value(100U);
    TEST_ASSERT_EQUAL(100, TIM1->CCR1);

    // Set compare values as a percentage
    UUT.set_top_value(1000);
    ch1.set_duty_cycle(0.5f);
    TEST_ASSERT_EQUAL(500, TIM1->CCR1);
    ch1.set_duty_cycle(0.0f);
    TEST_ASSERT_EQUAL(0, TIM1->CCR1);
    ch1.set_duty_cycle(1.0f);
    TEST_ASSERT_EQUAL(1000, TIM1->CCR1);
    ch1.set_duty_cycle(1.1f);
    TEST_ASSERT_EQUAL(1000, TIM1->CCR1);
    ch1.set_duty_cycle(-0.1f);
    TEST_ASSERT_EQUAL(1000, TIM1->CCR1);

    // Set compare values as a duration
    UUT.set_tick_frequency(1_kHz);
    ch1.set_pulse_width(50ms);
    TEST_ASSERT_EQUAL(50, TIM1->CCR1);
    UUT.set_tick_frequency(1_Hz);
    ch1.set_pulse_width(1min);
    TEST_ASSERT_EQUAL(60, TIM1->CCR1);
};

/// @brief Test assigning I/O pins to the timer
void test_assign_pins()
{
    // Set stuff up
    setUp();
    GPIOA->registers_to_default();

    // Create timer and a pin
    Timer::Timer timer(IO::TIM_1);
    GPIO::PIN pin{GPIO::Port::A, 0};

    // Assign pins
    GPIO::assign(pin, timer);

    // Test whether the pin was assigned correctly
    TEST_ASSERT_EQUAL(0b0010, GPIOA->MODER);
    TEST_ASSERT_EQUAL(0b0001, GPIOA->AFR[0]);
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_configure_SysTick);
    RUN_TEST(test_get_count);
    RUN_TEST(test_enable_disable);
    RUN_TEST(test_set_tick_frequency);
    RUN_TEST(test_set_top_value);
    RUN_TEST(test_set_pwm_mode);
    RUN_TEST(test_enable_disable_channel);
    RUN_TEST(test_set_compare_value);
    RUN_TEST(test_assign_pins);
    return UNITY_END();
};