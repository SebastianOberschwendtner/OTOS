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
 ==============================================================================
 * @file    main.c
 * @author  SO
 * @version v1.0.0
 * @date    09-March-2021
 * @brief   Main function for the OTOS. Mainly used to demonstrate how the OTOS
 *          functions work and should be used.
 ==============================================================================
 */

// *** Includes ***
#include "main.h"

// *** Variables ***

// *** Functions ***

/**
 * @brief Example task which blinks LED3 of the STM32F429-Discovery board.
 * @details Example Task
 */
void Blink_LED3(void)
{
    unsigned long counter = 0;
    GPIO::PIN<GPIO::PORTG, GPIO::PIN13> LED3(GPIO::OUTPUT);

    LED3.setHigh();
    while(1)
    {
        counter++;

        if(counter == 100000)
        {
            counter = 0;
            LED3.toggle();
        }
        OTOS::Task::yield();
    }
};

/**
 * @brief Example task which blinks LED4 of the STM32F429-Discovery board.
 * @details Example Task
 */
void Blink_LED4(void)
{
    unsigned long counter = 0;
    GPIO::PIN<GPIO::PORTG, GPIO::PIN14> LED4(GPIO::OUTPUT);

    LED4.setHigh();
    while(1)
    {
        counter++;

        if(counter == 50000)
        {
            counter = 0;
            LED4.toggle();
        }
        OTOS::Task::yield();
    }
};

// *** Main ***
int main(void)
{
    // Create the kernel object
    OTOS::Kernel OS;

    // Schedule Threads
    OS.scheduleThread(&Blink_LED3, OTOS::Check::StackSize<256>(), OTOS::PrioNormal);
    OS.scheduleThread(&Blink_LED4, OTOS::Check::StackSize<256>(), OTOS::PrioNormal);

    // Start the task execution
    OS.start();

    // Never reached
    return 0;
};