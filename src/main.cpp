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
 * @version v2.0.0
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
    GPIO::PIN LED3(GPIO::Port::G, 13, GPIO::Mode::Output);

    while (1)
    {
        LED3.toggle();
        OTOS::Task::yield();
    }
};

/**
 * @brief Example task which blinks LED4 of the STM32F429-Discovery board.
 * @details Example Task
 */
void Blink_LED4(void)
{
    GPIO::PIN LED4(GPIO::Port::G, 14, GPIO::Mode::Output);

    while (1)
    {
        LED4.toggle();
        OTOS::Task::yield();
    }
};

/**
 * @brief Example task which uses a i2c controller
 * @details Example Task
 */
void Task_I2C(void)
{
    // GPIO::PIN Led_Green(GPIO::PORTG, GPIO::PIN13, GPIO::OUTPUT);
    // GPIO::PIN Led_Red(GPIO::PORTG, GPIO::PIN14, GPIO::OUTPUT);
    // Led_Green.setLow();
    // Led_Red.setLow();

    GPIO::PIN OUT(GPIO::Port::A, 5, GPIO::Mode::Output);

    I2C::Controller i2c(IO::I2C_1, 100000);
    GPIO::PIN SCL(GPIO::Port::B, 8);
    GPIO::PIN SDA(GPIO::Port::B, 9);
    GPIO::assign(SCL, i2c);
    GPIO::assign(SDA, i2c);
    i2c.enable();

    OTOS::Task::yield();

    // Set address
    i2c.set_target_address(0x78);

    // Send 2 bytes
    // Bus::send_bytes(i2c, 0x00, 0xAF);
    OUT.set_high();

    while (1)
    {
        OTOS::Task::yield();
    }
};

/**
 * @brief Example task which uses a spi controller
 * @details Example Task
 */
void Task_SPI(void)
{
    GPIO::PIN DX(GPIO::Port::D, 13, GPIO::Mode::Output);
    GPIO::PIN CS(GPIO::Port::C,  2, GPIO::Mode::Output);
    GPIO::PIN SCK(GPIO::Port::F, 7, GPIO::Mode::Output);
    GPIO::PIN MOSI(GPIO::Port::F, 9, GPIO::Mode::Input);
    
    CS.set_high();

    SPI::Controller<IO::SPI_5> spi(1'000'000);
    spi.set_use_hardware_chip_select(false);
    GPIO::assign(SCK, spi);
    GPIO::assign(MOSI, spi);
    spi.enable();

    DX.set_low();
    CS.set_low();
    Bus::send_byte(spi, 0x01);
    CS.set_high();
    OTOS::Task::yield();

    DX.set_low();
    CS.set_low();
    Bus::send_byte(spi, 0x11);
    CS.set_high();
    OTOS::Task::yield();

    while (1)
    {
        OTOS::Task::yield();
    }
};

// Create the kernel object
OTOS::Kernel OS;

// *** Main ***
int main(void)
{

    // Configure Systick timer for interrupts every 1 ms
    Timer::SysTick_Configure();

    // Schedule Threads
    OS.schedule_thread<256>(&Blink_LED3, OTOS::Priority::Normal, 10);
    OS.schedule_thread<256>(&Blink_LED4, OTOS::Priority::High, 2);
    OS.schedule_thread<256>(&Task_I2C, OTOS::Priority::Normal);
    OS.schedule_thread<256>(&Task_SPI, OTOS::Priority::Normal, 10);

    // Start the task execution
    OS.start();

    // Never reached
    return 0;
};

/** 
 * @brief Provide a Interrupt handler for the systick timer,
 * which gets called every 1 ms.
 */
extern "C" void SysTick_Handler(void)
{
    OS.count_time_ms();
    OS.update_schedule();
};