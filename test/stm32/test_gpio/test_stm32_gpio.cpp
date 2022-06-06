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
 * @file    test_gpio_stm32.cpp
 * @author  SO
 * @version v2.7.3
 * @date    16-March-2021
 * @brief   Unit tests for the gpio drivers of stm32 controllers.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include "stm32/gpio_stm32.h"

// === Mocks ===
extern Mock::Callable<bool> CMSIS_NVIC_EnableIRQ;

// === Fixtures ===
void setUp(void) {
    // set stuff up here
    RCC->registers_to_default();
    GPIOA->registers_to_default();
    EXTI->registers_to_default();
};

void tearDown(void) {
    // clean stuff up here
};

// === Define Tests ===

/// @brief Test initialization of pin.
void test_init_pin(void)
{
    setUp();
    // Assume the AHB1ENR register was already written to
    RCC->AHB1ENR = 0b1000;

    // Create GPIO object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // Check whether clock for port was enabled and already
    // set bit was not deleted.
    TEST_ASSERT_EQUAL(0b1001, RCC->AHB1ENR);
};

/// @brief Test initialization of pin with specifying the output mode.
void test_init_pin_with_mode(void)
{
    setUp();
    // Assume the AHB1ENR register was already written to
    RCC->AHB1ENR = 0b1000;

    // Create GPIO objects
    // GPIO::PIN OUTPUT(GPIO::Port::A, 0, GPIO::Mode::Output);
    GPIO::PIN AF_MODE(GPIO::Port::A, 1, GPIO::Mode::AF_Mode);

    // Check whether clock for port was enabled and already
    // set bit was not deleted.
    TEST_ASSERT_EQUAL(0b1001, RCC->AHB1ENR);
    // TEST_ASSERT_EQUAL(0b1001, GPIOA->MODER);
};

/// @brief Test the mode setting.
void test_set_mode(void)
{
    setUp();
    // Assume other outputs are already set
    GPIOA->MODER = 0b1100;

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // Perform testing
    TEST_ASSERT_EQUAL(0b1100, GPIOA->MODER); // Output modes have not changed
    UUT.set_mode(GPIO::Mode::Output);
    TEST_ASSERT_EQUAL(0b1101, GPIOA->MODER); // Only output mode of pin has changed
    UUT.set_mode(GPIO::Mode::AF_Mode);
    TEST_ASSERT_EQUAL(0b1110, GPIOA->MODER); // Only output mode of pin has changed
};

/// @brief Test the output type setting.
void test_set_type(void)
{
    setUp();
    // Assume other outputs are already set
    GPIOA->OTYPER = 0b1100;

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // Perform testing
    TEST_ASSERT_EQUAL(0b1100, GPIOA->OTYPER);
    UUT.set_output_type(GPIO::Output::Push_Pull);
    TEST_ASSERT_EQUAL(0b1100, GPIOA->OTYPER);
    UUT.set_output_type(GPIO::Output::Open_Drain);
    TEST_ASSERT_EQUAL(0b1101, GPIOA->OTYPER);
};

/// @brief Test the output speed setting
void test_set_speed(void)
{
    setUp();
    // Assume other outputs are already set
    GPIOA->OSPEEDR = 0b1100;

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // perform testing
    TEST_ASSERT_EQUAL(0b1100, GPIOA->OSPEEDR);
    UUT.set_speed(GPIO::Speed::Low);
    TEST_ASSERT_EQUAL(0b1100, GPIOA->OSPEEDR);
    UUT.set_speed(GPIO::Speed::Medium);
    TEST_ASSERT_EQUAL(0b1101, GPIOA->OSPEEDR);
    UUT.set_speed(GPIO::Speed::High);
    TEST_ASSERT_EQUAL(0b1110, GPIOA->OSPEEDR);
    UUT.set_speed(GPIO::Speed::Very_High);
    TEST_ASSERT_EQUAL(0b1111, GPIOA->OSPEEDR);
};

/// @brief Test the push or pull output setting.
void test_set_pull(void)
{
    setUp();
    // Assume other outputs are already set
    GPIOA->PUPDR = 0b1100;

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // perform testing
    TEST_ASSERT_EQUAL(0b1100, GPIOA->PUPDR);
    UUT.set_pull(GPIO::Pull::No_Pull);
    TEST_ASSERT_EQUAL(0b1100, GPIOA->PUPDR);
    UUT.set_pull(GPIO::Pull::Pull_Up);
    TEST_ASSERT_EQUAL(0b1101, GPIOA->PUPDR);
    UUT.set_pull(GPIO::Pull::Pull_Down);
    TEST_ASSERT_EQUAL(0b1110, GPIOA->PUPDR);
};

/// @brief Test setting the output high.
void test_set_high(void)
{
    setUp();

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    //Perform testing
    TEST_ASSERT_EQUAL(0b0000, GPIOA->BSRR);
    UUT.set_high();
    TEST_ASSERT_EQUAL(0b0001, GPIOA->BSRR);
};

/// @brief Test setting the output low.
void test_set_low(void)
{
    setUp();

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    //Perform testing
    TEST_ASSERT_EQUAL(0b0000, GPIOA->BSRR);
    UUT.set_low();
    TEST_ASSERT_EQUAL((1<<16), GPIOA->BSRR);
};

/// @brief Test the setting of the output state.
void test_set_state(void)
{
    setUp();

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // perform testing
    TEST_ASSERT_EQUAL(0b0000, GPIOA->BSRR);
    UUT.set_state(true);
    TEST_ASSERT_EQUAL(0b0001, GPIOA->BSRR);
    GPIOA->registers_to_default();
    UUT.set_state(false);
    TEST_ASSERT_EQUAL((1<<16), GPIOA->BSRR);
};

/// @brief Test the output toggle function.
void test_toggle(void)
{
    setUp();
    // Assume other outputs are already set
    GPIOA->ODR = 0b1100;

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // perform testing
    TEST_ASSERT_EQUAL(0b1100, GPIOA->ODR);
    UUT.toggle();
    TEST_ASSERT_EQUAL(0b1101, GPIOA->ODR);
    UUT.toggle();
    TEST_ASSERT_EQUAL(0b1100, GPIOA->ODR);
};

/// @brief Test reading the input value.
void test_get(void)
{
    setUp();
    // Assume other outputs are already set
    GPIOA->IDR = 0b1100;

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // perform testing
    TEST_ASSERT_FALSE(UUT.get_state());
    GPIOA->IDR = 0b1101;
    TEST_ASSERT_TRUE(UUT.get_state());
};

/// @brief Test setting an alternate function in the low register
void test_alternate_function_low(void)
{
    setUp();
    // Assume other AFs are already set
    GPIOA->AFR[0] = 0b1101;
    GPIOA->MODER = 0b0011;
    GPIOA->OTYPER = 0b0001;

    // Create Pin object
    GPIO::PIN UUT(GPIO::Port::A, 1);
    UUT.set_alternate_function(IO::TIM_8);

    // perform testing
    TEST_ASSERT_EQUAL(0b00111101, GPIOA->AFR[0]);
    TEST_ASSERT_EQUAL(0b1011, GPIOA->MODER);
    TEST_ASSERT_EQUAL(0b0001, GPIOA->OTYPER);

    // AF specific options
    UUT.set_alternate_function(IO::I2C_1);

    // perform testing
    TEST_ASSERT_EQUAL(0b01001101, GPIOA->AFR[0]);
    TEST_ASSERT_EQUAL(0b0011, GPIOA->OTYPER);
};

/// @brief Test setting an alternate function in the low register
void test_alternate_function_high(void)
{
    setUp();
    // Assume other AFs are already set
    GPIOA->AFR[1] = 0b1101;

    // Create Pin object
    GPIO::PIN UUT(GPIO::Port::A, 9);
    UUT.set_alternate_function(IO::ETH_);

    // perform testing
    TEST_ASSERT_EQUAL(0b10111101, GPIOA->AFR[1]);
};

/// @brief Test the reading of rising and falling edges
void test_edges(void)
{
    setUp();

    // Create PIN object
    GPIO::PIN UUT(GPIO::Port::A, 0);

    // Perform testing
    UUT.read_edge();
    TEST_ASSERT_FALSE(UUT.rising_edge());
    TEST_ASSERT_FALSE(UUT.falling_edge());

    // Simulate rising edge
    GPIOA->IDR = 1;
    UUT.read_edge();
    TEST_ASSERT_TRUE(UUT.rising_edge());
    TEST_ASSERT_FALSE(UUT.falling_edge());

    // Simulate no edge
    GPIOA->IDR = 1;
    UUT.read_edge();
    TEST_ASSERT_FALSE(UUT.rising_edge());
    TEST_ASSERT_FALSE(UUT.falling_edge());

    // Simulate falling edge
    GPIOA->IDR = 0;
    UUT.read_edge();
    TEST_ASSERT_FALSE(UUT.rising_edge());
    TEST_ASSERT_TRUE(UUT.falling_edge());

    // Simulate no edge
    GPIOA->IDR = 0;
    UUT.read_edge();
    TEST_ASSERT_FALSE(UUT.rising_edge());
    TEST_ASSERT_FALSE(UUT.falling_edge());
};

/// @brief test enabling interrupts
void test_enable_interrupt(void)
{
    setUp();

    // Create PIN object
    GPIO::PIN PA0(GPIO::Port::A, 0);
    GPIO::PIN PA2(GPIO::Port::A, 2);
    GPIO::PIN PC5(GPIO::Port::C, 5);

    // Perform testing
    // PA0
    TEST_ASSERT_TRUE(PA0.enable_interrupt(GPIO::Edge::Rising));
    TEST_ASSERT_EQUAL(0b01, EXTI->IMR);
    TEST_ASSERT_EQUAL(0b00, EXTI->EMR);
    TEST_ASSERT_EQUAL(0b01, EXTI->RTSR);
    TEST_ASSERT_EQUAL(0b00, EXTI->FTSR);
    TEST_ASSERT_BIT_HIGH(14, RCC->APB2ENR);
    TEST_ASSERT_EQUAL(0x00, SYSCFG->EXTICR[0]);
    CMSIS_NVIC_EnableIRQ.assert_called_once_with((int)EXTI0_IRQn);

    // PA2
    TEST_ASSERT_TRUE(PA2.enable_interrupt(GPIO::Edge::Falling));
    TEST_ASSERT_EQUAL(0b101, EXTI->IMR);
    TEST_ASSERT_EQUAL(0b000, EXTI->EMR);
    TEST_ASSERT_EQUAL(0b001, EXTI->RTSR);
    TEST_ASSERT_EQUAL(0b100, EXTI->FTSR);
    TEST_ASSERT_BIT_HIGH(14, RCC->APB2ENR);
    TEST_ASSERT_EQUAL(0x00, SYSCFG->EXTICR[0]);
    TEST_ASSERT_EQUAL(0x00, SYSCFG->EXTICR[2]);
    CMSIS_NVIC_EnableIRQ.assert_called_once_with((int)EXTI2_IRQn);

    // PC5
    TEST_ASSERT_TRUE(PC5.enable_interrupt(GPIO::Edge::Both));
    TEST_ASSERT_EQUAL(0b100101, EXTI->IMR);
    TEST_ASSERT_EQUAL(0b000000, EXTI->EMR);
    TEST_ASSERT_EQUAL(0b100001, EXTI->RTSR);
    TEST_ASSERT_EQUAL(0b100100, EXTI->FTSR);
    TEST_ASSERT_BIT_HIGH(14, RCC->APB2ENR);
    TEST_ASSERT_EQUAL(0x00, SYSCFG->EXTICR[0]);
    TEST_ASSERT_EQUAL(0x00, SYSCFG->EXTICR[0]);
    TEST_ASSERT_EQUAL((2 << 4), SYSCFG->EXTICR[1]);
    CMSIS_NVIC_EnableIRQ.assert_called_once_with((int)EXTI9_5_IRQn);
};

/// @brief Test reseting a pending interrupt
void test_reset_pending_interrupt(void)
{
    setUp();

    // Create PIN object
    GPIO::PIN PA0(GPIO::Port::A, 0);
    GPIO::PIN PA2(GPIO::Port::A, 2);
    PA0.enable_interrupt(GPIO::Edge::Rising);
    PA2.enable_interrupt(GPIO::Edge::Both);

    // Perform testing => The interrupts are cleared by writing 1 to
    // the pending register
    EXTI->PR = 0b000;
    PA0.reset_pending_interrupt();
    TEST_ASSERT_EQUAL(0b001, EXTI->PR);

    PA2.reset_pending_interrupt();
    TEST_ASSERT_EQUAL(0b101, EXTI->PR);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_pin);
    RUN_TEST(test_init_pin_with_mode);
    RUN_TEST(test_set_mode);
    RUN_TEST(test_set_type);
    RUN_TEST(test_set_speed);
    RUN_TEST(test_set_pull);
    RUN_TEST(test_set_high);
    RUN_TEST(test_set_low);
    RUN_TEST(test_set_state);
    RUN_TEST(test_toggle);
    RUN_TEST(test_get);
    RUN_TEST(test_alternate_function_low);
    RUN_TEST(test_alternate_function_high);
    RUN_TEST(test_edges);
    RUN_TEST(test_enable_interrupt);
    RUN_TEST(test_reset_pending_interrupt);
    return UNITY_END();
};