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
 * @version v2.7.3
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
    // GPIO::PIN LED3(GPIO::Port::G, 13, GPIO::Mode::Output);

    while (1)
    {
        // LED3.toggle();
        OTOS::Task::yield();
    }
};

/**
 * @brief Example task which blinks LED4 of the STM32F429-Discovery board.
 * @details Example Task
 */
void Blink_LED4(void)
{
    // GPIO::PIN LED4(GPIO::Port::G, 14, GPIO::Mode::Output);

    while (1)
    {
        // LED4.toggle();
        OTOS::Task::yield();
    }
};

/**
 * @brief Example task which uses a spi controller
 * @details Example Task
 */
Graphics::Buffer_BW<240,320> buffer;
Graphics::Canvas_BW frame(buffer.data.data(), buffer.width_px, buffer.height_px);
void GUI(void)
{
    OTOS::Timed_Task task(&OTOS::get_time_ms);

    GPIO::PIN DX(GPIO::Port::D, 13, GPIO::Mode::Output);
    GPIO::PIN CS(GPIO::Port::C,  2, GPIO::Mode::Output);
    GPIO::PIN SCK(GPIO::Port::F, 7, GPIO::Mode::Output);
    GPIO::PIN MOSI(GPIO::Port::F, 9, GPIO::Mode::Input);

    SPI::Controller<IO::SPI_5> spi(5'000'000);
    GPIO::assign(SCK, spi);
    GPIO::assign(MOSI, spi);

    frame.set_fontsize(Font::Size::Normal);
    frame.add_string("\nTeststring\n");
    frame.add_line_h({0, 40}, 200);
    frame.add_line_v({200,0}, 41);

    ILI9341::setup_spi_bus(spi);
    ILI9341::Controller display(spi, DX, CS);

    display.wake_up();
    task.wait_ms(100);
    display.initialize();
    display.on();

    display.draw(
        buffer.data.cbegin(),
        buffer.data.cend(),
        ILI9341::RGB_16bit<0xA2, 0xAD, 0x00>(),
        0x0000,
        OTOS::Task::yield
    );


    while (1)
    {
        display.draw(
            buffer.data.cbegin(),
            buffer.data.cend(),
            ILI9341::RGB_16bit<0xA2, 0xAD, 0x00>(),
            0x0000,
            OTOS::Task::yield
        );
        OTOS::Task::yield();
    }
};

// Example Task for file I/O
void File(void)
{
    // Create Task
    OTOS::Timed_Task task(&OTOS::get_time_ms);
    GPIO::PIN Red(GPIO::Port::G, 14, GPIO::Mode::Output);
    GPIO::PIN Green(GPIO::Port::G, 13, GPIO::Mode::Output);

    // Create SDIO pins
    GPIO::PIN CMD(GPIO::Port::D, 2);
    GPIO::PIN SCK(GPIO::Port::C,12);
    GPIO::PIN DAT0(GPIO::Port::C, 8);
    GPIO::PIN DAT3(GPIO::Port::C,11);
    CMD.set_pull(GPIO::Pull::Pull_Up);
    DAT0.set_pull(GPIO::Pull::Pull_Up);
    DAT3.set_pull(GPIO::Pull::Pull_Up);
    CMD.set_output_type(GPIO::Output::Open_Drain);
    DAT0.set_output_type(GPIO::Output::Open_Drain);
    DAT3.set_output_type(GPIO::Output::Open_Drain);

    CMD.set_alternate_function(IO::SDIO_);
    SCK.set_alternate_function(IO::SDIO_);
    DAT0.set_alternate_function(IO::SDIO_);
    DAT3.set_alternate_function(IO::SDIO_);

    // Create SDHC service, using to SDIO peripheral as bus interface
    SDHC::service<SD::Controller> drive;
    drive.initialize(task);

    // Create and mount the FAT32 volume
    FAT32::Volume volume{drive.card};
    volume.mount();

    // Open a file on the volume
    auto file = FAT32::open(volume, "0:/logo2.pbm");

    // Check whether file was found
    if (file.state == Files::State::Open)
        Green.set_high();
    else
        Red.set_high();

    char string[16] = {0};
    frame.set_cursor(0, 3);

    frame.add_string(volume.partition.name.begin());
    frame.newline();
    std::sprintf(string, "Size: %lu\n", file.size());
    frame.add_string(string);
    task.yield();

    for(unsigned char count = 0; count<11; count++)
        frame.add_char(file.read());
        
    unsigned char shift_counter = 8;
    unsigned char current_byte = file.read();
    for(unsigned int y=0; y<120; y++)
    {
        // if( shift_counter != 0)
        //     current_byte = file.read();
        // shift_counter = 8;
        for(unsigned int x = 0; x<120; x++)
        {
            auto pixel = (current_byte & (1<<(shift_counter-1))) ? Graphics::Color_BW::White : Graphics::Color_BW::Black;
            frame.draw_pixel(x+10, y+160, pixel);

            shift_counter--;
            if(shift_counter == 0)
            {
                current_byte = file.read();
                shift_counter = 8;
            }
        }
    }


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
    ST_Core::switch_system_clock<ST_Core::Clock::PLL_HSI, 120, 30, 60>();
    // Configure Systick timer for interrupts every 1 ms
    Timer::SysTick_Configure();

    // Schedule Threads
    OS.schedule_thread<256>(&GUI, OTOS::Priority::Normal);
    OS.schedule_thread<256>(&File, OTOS::Priority::Normal);

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