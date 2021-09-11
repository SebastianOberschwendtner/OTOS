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
 * @version v1.0.5
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
    volatile unsigned long counter = 0;
    GPIO::PIN LED3(GPIO::PORTG, GPIO::PIN13, GPIO::OUTPUT);

    // LED3.setHigh();
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
    volatile unsigned long counter = 0;
    GPIO::PIN LED4(GPIO::PORTG, GPIO::PIN14, GPIO::OUTPUT);
    // GPIO::PIN LED4(GPIO::PORTA, GPIO::PIN5, GPIO::OUTPUT);

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

/**
 * @brief Example task which uses a i2c controller
 * @details Example Task
 */
void Task_I2C(void)
{
    GPIO::PIN Led_Green(GPIO::PORTG, GPIO::PIN13, GPIO::OUTPUT);
    GPIO::PIN Led_Red(GPIO::PORTG, GPIO::PIN14, GPIO::OUTPUT);
    Led_Green.setLow();
    Led_Red.setLow();

    I2C::Controller i2c(I2C::I2C_1, 100000);
    GPIO::PIN SCL(GPIO::PORTB, GPIO::PIN8);
    GPIO::PIN SDA(GPIO::PORTB, GPIO::PIN9);
    i2c.assign_pin(SCL);
    i2c.assign_pin(SDA);
    i2c.enable();

    OTOS::Task::yield();

    // Send address
    i2c.set_target_address(0x78);

    I2C::Data_t payload;

    payload.byte[1] = 0b00000000;
    payload.byte[0] = 0xA5;
    i2c.send_word(payload.word[0]);
    
    payload.byte[0] = 0xAF;
    i2c.send_word(payload.word[0]);
    
    while (1)
    {
        OTOS::Task::yield();
    }
};

// *** Main ***
int main(void)
{
    // Create the kernel object
    OTOS::Kernel OS;

    // Schedule Threads
    // OS.scheduleThread(&Blink_LED3, OTOS::Check::StackSize<256>(), OTOS::PrioNormal);
    // OS.scheduleThread(&Blink_LED4, OTOS::Check::StackSize<256>(), OTOS::PrioNormal);
    OS.scheduleThread(&Task_I2C,   OTOS::Check::StackSize<256>(), OTOS::PrioNormal);

    // Start the task execution
    OS.start();

    // Never reached
    return 0;
};