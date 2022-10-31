/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @version v3.4.0
 * @date    31-October-2022
 * @brief   This example shows how to use the ILI9341 display driver.
 ==============================================================================
 */
// *** Variables ***
Graphics::Buffer_BW<240, 320> display_buffer;

// === Example Task ===
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
    // Timed Task
    OTOS::Timed_Task task(OTOS::get_time_ms);

    // IO Pins
    GPIO::PIN DX(GPIO::Port::D, 13, GPIO::Mode::Output);
    GPIO::PIN CS(GPIO::Port::C, 2, GPIO::Mode::Output);
    GPIO::PIN SCK(GPIO::Port::F, 7, GPIO::Mode::Output);
    GPIO::PIN MOSI(GPIO::Port::F, 9, GPIO::Mode::Output);
    GPIO::PIN Led_Green(GPIO::Port::G, 13, GPIO::Mode::Output);

    // SPI
    SPI::Controller<IO::SPI_5> spi_display(8'000'000);
    GPIO::assign(SCK, spi_display);
    GPIO::assign(MOSI, spi_display);

    // Display controller
    ILI9341::setup_spi_bus(spi_display);
    ILI9341::Controller display(spi_display, DX, CS);

    // Canvas
    Graphics::Canvas_BW canvas(display_buffer.data.data(), display_buffer.width_px, display_buffer.height_px);
    canvas.set_font(Font::_24px::DelugiaPLMono);
    canvas << "hello world!";

    // Initialize display
    display.wake_up();
    task.wait_ms(100);
    display.initialize();
    display.on();

    // Start drawing the display buffer
    while (true)
    {
        Led_Green.set_high();
        display.draw(
            display_buffer.data.cbegin(),
            display_buffer.data.cend(),
            ILI9341::RGB_16bit<255, 255, 255>(),
            0);
        Led_Green.set_low();
        OTOS::Task::yield();
    }
};
