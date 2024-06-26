/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    using_ili9341.cpp
 * @author  SO
 * @version v4.0.0
 * @date    31-October-2022
 * @brief   This example shows how to use the ILI9341 display driver.
 ==============================================================================
 */
#include "main.h" // => include not strictly necessary, but for intellisense to work

/* === Variables === */
graphics::Buffer_BW<240, 320> display_buffer;

/* === Example Task === */
/**
 * @brief Example task for using the ILI9341 display.
 * The following pins are used:
 * - PD13 -> DX
 * - PC2 -> CS
 * - PF7 -> SCK
 * - PF9 -> MOSI
 * - PG13 -> Green LED
 * 
 * @attention Error handling is not included in the example. Check
 * the individual controller functions for the meaning of their
 * return value.
 */
void Run_ILI9341_Example(void)
{
    /* Timed Task */
    OTOS::TimedTask task(OTOS::get_time_ms);

    /* IO Pins */
    auto DX = gpio::Pin::create<gpio::Port::D>(13, gpio::Mode::Output);
    auto CS = gpio::Pin::create<gpio::Port::C>(2, gpio::Mode::Output);
    auto SCK = gpio::Pin::create<gpio::Port::F>(7, gpio::Mode::Output);
    auto MOSI = gpio::Pin::create<gpio::Port::F>(9, gpio::Mode::Output);
    auto Led_Green = gpio::Pin::create<gpio::Port::G>(13, gpio::Mode::Output);

    /* SPI */
    auto spi_display = spi::Controller::create<stm32::Peripheral::SPI_5>(8'000'000);
    gpio::assign(SCK, spi_display);
    gpio::assign(MOSI, spi_display);

    /* Display controller */
    ili9341::setup_spi_bus(spi_display);
    ili9341::Controller display(spi_display, DX, CS);

    /* Canvas */
    graphics::Canvas_BW canvas(display_buffer.data.data(), display_buffer.width_px, display_buffer.height_px);
    canvas.set_font(font::_24px::DelugiaPLMono);

    /* Add strings */
    /** @attention The constant string stream does not yet support the
     * newline character. You you have to terminate each line with a
     * `OTOS::endl` instead. 
     * => std::string_view supports the newline character!*/
    canvas << "hello world!" << OTOS::endl;

    /* Add string_view */
    std::string_view sv = "string_view\n";
    canvas << sv;

    /* Add integer numbers */
    canvas << 42;

    /* Add floating point numbers */
    /* *** -> Coming soon *** */

    /* Initialize display */
    display.wake_up();
    task.wait_ms(100);
    display.initialize();
    display.on();

    /* Start drawing the display buffer */
    while (true)
    {
        /* Flash LED to indicate each display update */
        Led_Green.set_high();

        /* Draw the BW canvas with specified foreground and background color */
        display.draw(
            display_buffer.data.cbegin(),
            display_buffer.data.cend(),
            ili9341::RGB_16bit<255, 255, 255>(),
            0);
        Led_Green.set_low();

        /* Wait for next update */
        OTOS::Task::yield();
    }
};
