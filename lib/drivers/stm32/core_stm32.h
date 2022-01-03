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

#ifndef CORE_STM32_H_
#define CORE_STM32_H_

// === Includes ===
#include "vendors.h"
// #include <array>
// #include <algorithm>

namespace ST_Core
{
    // === Enum for valid voltage ranges ===
    enum class VCC: unsigned char
    {
        _2_7V_to_3_6V = 0
    };

    // === Enum for System Clock Sources ===
    enum class Clock: unsigned char
    {
        HSI = 0, HSE, PLL_HSI, PLL_HSE
    };

    namespace PLL
    {
        /**
         * @brief Get the M value when using the internal
         * clock HSI. F_HSI = 16 MHz.
         * 
         * F_VCO = F_HSI/M = 2 MHz => M = 8
         * @return Resturns the appropriate M value.
         */
        constexpr unsigned int get_M_HSI(void)
        {
            return 8;
        };

        /**
         * @brief Get the N value of the PLL
         * when using HSI.
         * 
         * VCO will be 240 MHz.
         */
        constexpr unsigned int get_N_HSI(void)
        {
            return 120;
        };

        /**
         * @brief Get the Q value of the PLL
         * when using HSI as clock.
         * 
         * => VCO is 240 MHz!
         */
        constexpr unsigned int get_Q_HSI(void)
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
        template<unsigned char SysClock>
        constexpr unsigned int get_P_HSI(void)
        {
            // Check whether the clock speed is valid
            static_assert( 
                (SysClock == 120) | (SysClock == 60) | (SysClock == 40) | (SysClock == 30)
                , "FCPU is not a valid value!");

            // Value is valid, get the P value
            constexpr unsigned char P = 240 / SysClock;

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

    };

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
        template<VCC range, unsigned char SysClock>
        static void configure()
        {
            // Only normal voltage range is supported now
            static_assert(range == VCC::_2_7V_to_3_6V, "Voltage range for Flash control not supported yet!");
            // Check for valid CPU speed
            static_assert(SysClock <= 168, "F_CPU is to high!");

            // Set the configuration for flash
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
        };
    };

    template<unsigned char f_cpu, unsigned char f_apb1>
    constexpr unsigned long get_APB1_prescaler()
    {
        constexpr unsigned char pre1 = f_cpu / f_apb1;
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

    template<unsigned char f_cpu, unsigned char f_apb2>
    constexpr unsigned long get_APB2_prescaler()
    {
        constexpr unsigned char pre2 = f_cpu / f_apb2;
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
    constexpr unsigned long clock_enable()
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
    constexpr unsigned long clock_ready()
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
    constexpr unsigned long clock_source()
    {
        if constexpr (source == Clock::HSI)
            return RCC_CFGR_SW_HSI;
        else if constexpr (source == Clock::HSE)
            return RCC_CFGR_SW_HSE;
        else
            return RCC_CFGR_SW_PLL;
    };

    template<Clock source>
    constexpr unsigned long clock_source_status()
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
    template<Clock source, unsigned char f_cpu, unsigned char f_apb1, unsigned char f_apb2>
    void switch_system_clock()
    {
        // Check clock limits
        static_assert(f_apb1 <= 42, "F_APB1 too high!");
        static_assert(f_apb2 <= 84, "F_APB2 too high!");
        static_assert(f_cpu <= 168, "F_CPU too high!");

        // Set the Flash wait states
        Flash::configure<VCC::_2_7V_to_3_6V, f_cpu>();

        // Get the APB1 prescaler
        constexpr unsigned long APB1_Prescaler = get_APB1_prescaler<f_cpu, f_apb1>();
        static_assert(APB1_Prescaler != 1, "F_APB1 cannot be achieved with current clocks!");

        // Get the APB2 prescaler
        constexpr unsigned long APB2_Prescaler = get_APB2_prescaler<f_cpu, f_apb2>();
        static_assert(APB2_Prescaler != 1, "F_APB2 cannot be achieved with current clocks!");

        // Set PLL Parameters
        static_assert(source != Clock::HSE, "Configuring the PLL source with HSE is not yet supported!");
        if constexpr (source == Clock::PLL_HSI)
        {
            constexpr unsigned int M = ST_Core::PLL::get_M_HSI();
            constexpr unsigned int N = ST_Core::PLL::get_N_HSI();
            constexpr unsigned int Q = ST_Core::PLL::get_Q_HSI();
            constexpr unsigned int P = ST_Core::PLL::get_P_HSI<120>();
            RCC->PLLCFGR = (Q << RCC_PLLCFGR_PLLQ_Pos) | (P << RCC_PLLCFGR_PLLP_Pos) | (N << RCC_PLLCFGR_PLLN_Pos) | M;
        }

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
};

#endif
