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
 * @file    test_stm32_sdio.cpp
 * @author  SO
 * @version v5.0.0
 * @date    31-December-2021
 * @brief   Unit tests for testing the core functions for configuring stm32 microcontrollers.
 ==============================================================================
 */

/* === Includes === */
#include "stm32/core_stm32.h"
#include <array>
#include <mock.h>
#include <unity.h>

/** === Test List ===
 */

/* === Tests === */
void setUp()
{
    /* set stuff up here */
    RCC->registers_to_default();
    FLASH->registers_to_default();
};

void tearDown(){
    /* clean stuff up here */
};

/** 
 * @brief Test configuring the PLL
 */
void test_configure_PLL()
{
    /* Get the correct M value for the HSI clock */
    const uint32_t M = stm_core::pll::get_M_HSI();
    TEST_ASSERT_EQUAL(8, M);

    /* Get the correct M value for the HSI clock */
    const uint32_t N = stm_core::pll::get_N_HSI();
    TEST_ASSERT_EQUAL(120, N);

    /* Get the correct Q value for the HSI clock */
    const uint32_t Q = stm_core::pll::get_Q_HSI();
    TEST_ASSERT_EQUAL(5, Q);

    /* Get the correct P value for the HSI clock */
    uint32_t P = stm_core::pll::get_P_HSI<120>();
    TEST_ASSERT_EQUAL(0, P);
    P = stm_core::pll::get_P_HSI<60>();
    TEST_ASSERT_EQUAL(1, P);
    P = stm_core::pll::get_P_HSI<40>();
    TEST_ASSERT_EQUAL(2, P);
    P = stm_core::pll::get_P_HSI<30>();
    TEST_ASSERT_EQUAL(3, P);
};

/** 
 * @brief Test the configuration of the flash controller
 */
void test_flash_control()
{
    setUp();

    /* Test setting the flash wait states for the normal voltage range */
    stm_core::Flash::configure<stm_core::VCC::_2_7V_to_3_6V, 16>();
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_0WS, FLASH->ACR);
    stm_core::Flash::configure<stm_core::VCC::_2_7V_to_3_6V, 30>();
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_0WS, FLASH->ACR);
    stm_core::Flash::configure<stm_core::VCC::_2_7V_to_3_6V, 60>();
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_1WS, FLASH->ACR);
    stm_core::Flash::configure<stm_core::VCC::_2_7V_to_3_6V, 90>();
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_2WS, FLASH->ACR);
    stm_core::Flash::configure<stm_core::VCC::_2_7V_to_3_6V, 120>();
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_3WS, FLASH->ACR);
    stm_core::Flash::configure<stm_core::VCC::_2_7V_to_3_6V, 150>();
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_4WS, FLASH->ACR);
    stm_core::Flash::configure<stm_core::VCC::_2_7V_to_3_6V, 168>();
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_5WS, FLASH->ACR);
};

/** 
 * @brief Test getting the prescaler values
 */
void test_get_prescaler_values()
{
    /* Test the APB1 prescaler */
    uint32_t Result = stm_core::get_APB1_prescaler<16, 16>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE1_DIV1, Result);
    Result = stm_core::get_APB1_prescaler<16, 8>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE1_DIV2, Result);
    Result = stm_core::get_APB1_prescaler<16, 4>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE1_DIV4, Result);
    Result = stm_core::get_APB1_prescaler<16, 2>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE1_DIV8, Result);
    Result = stm_core::get_APB1_prescaler<16, 1>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE1_DIV16, Result);
    Result = stm_core::get_APB1_prescaler<16, 32>();
    TEST_ASSERT_EQUAL(1, Result);

    /* Test the APB2 prescaler */
    Result = stm_core::get_APB2_prescaler<16, 16>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE2_DIV1, Result);
    Result = stm_core::get_APB2_prescaler<16, 8>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE2_DIV2, Result);
    Result = stm_core::get_APB2_prescaler<16, 4>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE2_DIV4, Result);
    Result = stm_core::get_APB2_prescaler<16, 2>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE2_DIV8, Result);
    Result = stm_core::get_APB2_prescaler<16, 1>();
    TEST_ASSERT_EQUAL(RCC_CFGR_PPRE2_DIV16, Result);
    Result = stm_core::get_APB2_prescaler<16, 32>();
    TEST_ASSERT_EQUAL(1, Result);
};

/** 
 * @brief Test setting the system clock to the PLL
 */
void test_switch_system_clock()
{
    setUp();

    /* For valid clock switching is is assumed, that every clock is stable! */
    RCC->CR |= RCC_CR_PLLSAIRDY | RCC_CR_PLLI2SRDY | RCC_CR_PLLRDY | RCC_CR_HSERDY | RCC_CR_HSIRDY;

    /* Test switching to the HSI clock with 16 MHz */
    RCC->CFGR = RCC_CFGR_SWS_HSI;
    stm_core::switch_system_clock<stm_core::Clock::HSI, 16, 16, 16>();
    TEST_ASSERT_BIT_HIGH(RCC_CR_HSION_Pos, RCC->CR);
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_0WS, FLASH->ACR);
    TEST_ASSERT_EQUAL(0, RCC->CFGR);

    /* Test switching to the HSI clock with 16 MHz, with prescaled APB clocks */
    RCC->CFGR = RCC_CFGR_SWS_HSI;
    stm_core::switch_system_clock<stm_core::Clock::HSI, 16, 4, 8>();
    TEST_ASSERT_BIT_HIGH(RCC_CR_HSION_Pos, RCC->CR);
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_0WS, FLASH->ACR);
    TEST_ASSERT_BITS(RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_DIV4, RCC->CFGR);
    TEST_ASSERT_BITS(RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_DIV2, RCC->CFGR);

    /* Test switching to PLL with the HSI clock with 16 MHz, with prescaled APB clocks */
    RCC->CFGR = RCC_CFGR_SWS_PLL;
    stm_core::switch_system_clock<stm_core::Clock::PLL_HSI, 120, 30, 60>();
    TEST_ASSERT_BIT_HIGH(RCC_CR_HSION_Pos, RCC->CR);
    TEST_ASSERT_BIT_HIGH(RCC_CR_PLLON_Pos, RCC->CR);
    TEST_ASSERT_EQUAL(FLASH_ACR_LATENCY_3WS, FLASH->ACR);
    TEST_ASSERT_BITS(RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_DIV4, RCC->CFGR);
    TEST_ASSERT_BITS(RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_DIV2, RCC->CFGR);
    TEST_ASSERT_BITS(RCC_CFGR_SW_Msk, RCC_CFGR_SW_PLL, RCC->CFGR);
    uint32_t M = stm_core::pll::get_M_HSI();
    uint32_t N = stm_core::pll::get_N_HSI();
    uint32_t Q = stm_core::pll::get_Q_HSI();
    uint32_t P = stm_core::pll::get_P_HSI<120>();
    TEST_ASSERT_EQUAL((Q << RCC_PLLCFGR_PLLQ_Pos) | (P << RCC_PLLCFGR_PLLP_Pos) | (N << RCC_PLLCFGR_PLLN_Pos) | M, RCC->PLLCFGR);
    TEST_ASSERT_BIT_LOW(RCC_PLLCFGR_PLLSRC_Pos, RCC->CFGR);
};

/* === Main === */
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_configure_PLL);
    RUN_TEST(test_flash_control);
    RUN_TEST(test_get_prescaler_values);
    RUN_TEST(test_switch_system_clock);
    return UNITY_END();
};