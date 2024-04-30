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

#ifndef CORE_STM32_H_
#define CORE_STM32_H_

// === Includes ===
#include "vendors.h"
// #include <array>
// #include <algorithm>

namespace stm_core
{
    // === Enum for valid voltage ranges ===
    enum class VCC: uint8_t
    {
        _2_7V_to_3_6V = 0
    };

    // === Enum for System Clock Sources ===
    enum class Clock: uint8_t
    {
        HSI = 0, HSE, PLL_HSI, PLL_HSE
    };

    namespace pll
    {
        /**
         * @brief Get the M value when using the internal
         * clock HSI. F_HSI = 16 MHz.
         * 
         * F_VCO = F_HSI/M = 2 MHz => M = 8
         * @return Resturns the appropriate M value.
         */
        constexpr uint32_t get_M_HSI(void)
        {
            return 8;
        };

        /**
         * @brief Get the N value of the PLL
         * when using HSI.
         * 
         * VCO will be 240 MHz.
         */
        constexpr uint32_t get_N_HSI(void)
        {
            return 120;
        };

        /**
         * @brief Get the Q value of the PLL
         * when using HSI as clock.
         * 
         * => VCO is 240 MHz!
         */
        constexpr uint32_t get_Q_HSI(void)
        {
            return 5;
        };

        /**
         * @brief Get the P value of the PLL
         * when using HSI as clock.
         * 
         * => VCO is 240 MHz!
         * @tparam SysClock The desired system clock frequency in [MHz]
         */
        template<uint8_t SysClock>
        constexpr uint32_t get_P_HSI(void)
        {
            // Check whether the clock speed is valid
            static_assert( 
                (SysClock == 120) | (SysClock == 60) | (SysClock == 40) | (SysClock == 30)
                , "FCPU is not a valid value!");

            // Value is valid, get the P value
            constexpr uint8_t P = 240 / SysClock;

            // Return the P value bits as defined in the STM32 datasheet
            if constexpr ( P == 2)
                return 0;
            if constexpr ( P == 4)
                return 1;
            if constexpr ( P == 6)
                return 2;
            if constexpr ( P == 8)
                return 3;
        };

    }; // namespace pll

    struct Flash
    {
        /**
         * @brief Set the access control of the flash
         * according to the VCC voltage range and
         * the used system clock speed.
         * 
         * @tparam range The VCC voltage range.
         * @tparam SysClock The Clock speed of the system clock in [MHz].
         */
        template<VCC range, uint8_t SysClock>
        static void configure()
        {
            // Only normal voltage range is supported now
            static_assert(range == VCC::_2_7V_to_3_6V, "Voltage range for Flash control not supported yet!");
            // Check for valid CPU speed
#ifdef STM32F4
            static_assert(SysClock <= 168, "F_CPU is to high!");
#elif defined(STM32L0)
            static_assert(SysClock <= 32, "F_CPU is to high!");
#endif

            // Set the configuration for flash
#ifdef STM32F4
            if constexpr (SysClock <= 30)
                FLASH->ACR = FLASH_ACR_LATENCY_0WS;
            else if constexpr (SysClock <= 60)
                FLASH->ACR = FLASH_ACR_LATENCY_1WS;
            else if constexpr (SysClock <= 90)
                FLASH->ACR = FLASH_ACR_LATENCY_2WS;
            else if constexpr (SysClock <= 120)
                FLASH->ACR = FLASH_ACR_LATENCY_3WS;
            else if constexpr (SysClock <= 150)
                FLASH->ACR = FLASH_ACR_LATENCY_4WS;
            else if constexpr (SysClock <= 168)
                FLASH->ACR = FLASH_ACR_LATENCY_5WS;
            else
                FLASH->ACR = 0;
#elif defined(STM32L0)
            if constexpr (SysClock > 16)
                FLASH->ACR = FLASH_ACR_LATENCY;
#endif
        };
    };

    template<uint8_t f_cpu, uint8_t f_apb1>
    constexpr uint32_t get_APB1_prescaler()
    {
        constexpr uint8_t pre1 = f_cpu / f_apb1;
        if constexpr (pre1 == 1)
            return RCC_CFGR_PPRE1_DIV1;
        else if constexpr (pre1 == 2)
            return RCC_CFGR_PPRE1_DIV2;
        else if constexpr (pre1 == 4)
            return RCC_CFGR_PPRE1_DIV4;
        else if constexpr (pre1 == 8)
            return RCC_CFGR_PPRE1_DIV8;
        else if constexpr (pre1 == 16)
            return RCC_CFGR_PPRE1_DIV16;
        else
            return 1;
    };

    template<uint8_t f_cpu, uint8_t f_apb2>
    constexpr uint32_t get_APB2_prescaler()
    {
        constexpr uint8_t pre2 = f_cpu / f_apb2;
        if constexpr (pre2 == 1)
            return RCC_CFGR_PPRE2_DIV1;
        else if constexpr (pre2 == 2)
            return RCC_CFGR_PPRE2_DIV2;
        else if constexpr (pre2 == 4)
            return RCC_CFGR_PPRE2_DIV4;
        else if constexpr (pre2 == 8)
            return RCC_CFGR_PPRE2_DIV8;
        else if constexpr (pre2 == 16)
            return RCC_CFGR_PPRE2_DIV16;
        else
            return 1;
    };

    template<Clock source>
    constexpr uint32_t clock_enable()
    {
        if constexpr (source == Clock::HSI)
            return RCC_CR_HSION;
        else if constexpr (source == Clock::HSE)
            return RCC_CR_HSEON;
        else if constexpr (source == Clock::PLL_HSI)
            return RCC_CR_HSION | RCC_CR_PLLON;
        else if constexpr (source == Clock::PLL_HSE)
            return RCC_CR_HSEON | RCC_CR_PLLON;
        else 
            return 0;
    };

    template<Clock source>
    constexpr uint32_t clock_ready()
    {
        if constexpr (source == Clock::HSI)
            return RCC_CR_HSIRDY;
        else if constexpr (source == Clock::HSE)
            return RCC_CR_HSERDY;
        else if constexpr (source == Clock::PLL_HSI)
            return RCC_CR_HSIRDY | RCC_CR_PLLRDY;
        else if constexpr (source == Clock::PLL_HSE)
            return RCC_CR_HSERDY | RCC_CR_PLLRDY;
        else 
            return 0;
    };

    template<Clock source>
    constexpr uint32_t clock_source()
    {
        if constexpr (source == Clock::HSI)
            return RCC_CFGR_SW_HSI;
        else if constexpr (source == Clock::HSE)
            return RCC_CFGR_SW_HSE;
        else
            return RCC_CFGR_SW_PLL;
    };

    template<Clock source>
    constexpr uint32_t clock_source_status()
    {
        if constexpr (source == Clock::HSI)
            return RCC_CFGR_SWS_HSI;
        else if constexpr (source == Clock::HSE)
            return RCC_CFGR_SWS_HSE;
        else
            return RCC_CFGR_SWS_PLL;
    };

    /**
     * @brief Switch the system clock to the desired source
     * and set all prescaler to achieve the given clocks.
     * 
     * @tparam source The source for the system clock.
     * @tparam f_cpu The clock speed of the resulting system clock in [MHz].
     * @tparam f_apb1 The clock speed of the APB1 domain in [MHz].
     * @tparam f_apb2 The clock speed of the APB2 domain in [Mhz].
     */
    template<Clock source, uint8_t f_cpu, uint8_t f_apb1, uint8_t f_apb2>
    void switch_system_clock()
    {
        // Check clock limits
#if defined(STM32F4)
        static_assert(f_apb1 <= 42, "F_APB1 too high!");
        static_assert(f_apb2 <= 84, "F_APB2 too high!");
        static_assert(f_cpu <= 168, "F_CPU too high!");
#elif defined(STM32L0)
        static_assert(f_apb1 <= 32, "F_APB1 too high!");
        static_assert(f_apb2 <= 32, "F_APB2 too high!");
        static_assert(f_cpu <= 32, "F_CPU too high!");
#endif

        // Get the APB1 prescaler
        constexpr uint32_t APB1_Prescaler = get_APB1_prescaler<f_cpu, f_apb1>();
        static_assert(APB1_Prescaler != 1, "F_APB1 cannot be achieved with current clocks!");

        // Get the APB2 prescaler
        constexpr uint32_t APB2_Prescaler = get_APB2_prescaler<f_cpu, f_apb2>();
        static_assert(APB2_Prescaler != 1, "F_APB2 cannot be achieved with current clocks!");

#ifdef STM32F4
        // Set the Flash wait states
        Flash::configure<VCC::_2_7V_to_3_6V, f_cpu>();

        // Set PLL Parameters
        static_assert(source != Clock::PLL_HSE, "Configuring the PLL source with HSE is not yet supported!");
        if constexpr (source == Clock::PLL_HSI)
        {
            constexpr uint32_t M = stm_core::pll::get_M_HSI();
            constexpr uint32_t N = stm_core::pll::get_N_HSI();
            constexpr uint32_t Q = stm_core::pll::get_Q_HSI();
            constexpr uint32_t P = stm_core::pll::get_P_HSI<f_cpu>();
            RCC->PLLCFGR = (Q << RCC_PLLCFGR_PLLQ_Pos) | (P << RCC_PLLCFGR_PLLP_Pos) | (N << RCC_PLLCFGR_PLLN_Pos) | M;
        }
#elif defined(STM32L0)
        // Set the Flash wait states
        Flash::configure<VCC::_2_7V_to_3_6V, f_cpu>();

        // Set PLL Parameters
        static_assert(source != Clock::PLL_HSE, "Configuring the PLL source with HSE is not yet supported!");
        if constexpr (source == Clock::PLL_HSI)
        {
            constexpr uint32_t Mul = 0b0001; // Multiply by 4;
            constexpr uint32_t Div = 0b0001;   // Divide by 2;
            RCC->CFGR = (Mul << RCC_CFGR_PLLMUL_Pos) | (Div << RCC_CFGR_PLLDIV_Pos) | RCC_CFGR_PLLSRC_HSI;
        }
#endif

        // Enable the desired clocks
        RCC->CR |= clock_enable<source>();

        // Wait until the desired clock is active
        while(not (RCC->CR & clock_ready<source>()) );

        // Set the RCC configuration
        RCC->CFGR |=  APB2_Prescaler | APB1_Prescaler | clock_source<source>();

        // Wait for clock source to switch -> HSI is already valid at startup
        if constexpr (source != Clock::HSI)
            while(not (RCC->CFGR & clock_source_status<source>()) );
    };
}; // namespace stm_core

#endif
