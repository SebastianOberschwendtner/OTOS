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
 * @file    using_sd_card.cpp
 * @author  SO
 * @version v4.0.0
 * @date    30-October-2022
 * @brief   This example shows how to setup a SD card, open a file and then
            write to it.
 ==============================================================================
 */
#include "main.h" // => include not strictly necessary, but for intellisense to work

/**
 * @brief Example task for using the filesystem with a SD Card.
 * The following pins are used:
 * - PC12 -> CLK
 * - PD2 -> CMD, Open Drain, Pull Up
 * - PC8 -> D0, Open Drain, Pull Up
 * - PC11 -> CD, Open Drain, Pull Up
 * 
 * @attention Error handling is not included in the example. Check
 * the individual controller functions for the meaning of their
 * return value.
 */
void Run_SD_Example(void)
{
    /* Create timed task */
    OTOS::TimedTask task(OTOS::get_time_ms);

    /* Create SD IO Pins */
    auto CLK = gpio::Pin::create<gpio::Port::C>(12, gpio::Mode::Output);
    auto CMD = gpio::Pin::create<gpio::Port::D>(2, gpio::Mode::Output)
        .set_output_type(gpio::Output::Open_Drain)
        .set_pull(gpio::Pull::Pull_Up);
    auto D0 = gpio::Pin::create<gpio::Port::C>(8, gpio::Mode::Output)
        .set_output_type(gpio::Output::Open_Drain)
        .set_pull(gpio::Pull::Pull_Up);
    auto CD = gpio::Pin::create<gpio::Port::C>(11, gpio::Mode::Output)
        .set_output_type(gpio::Output::Open_Drain)
        .set_pull(gpio::Pull::Pull_Up);

    /* Create SD Card   */
    sdhc::service<sdio::Controller> sd_service(400'000);
    gpio::assign(CLK, sd_service.bus);
    gpio::assign(CMD, sd_service.bus);
    gpio::assign(D0, sd_service.bus);
    gpio::assign(CD, sd_service.bus);


    /* init Card */
    sd_service.initialize(task);

    /* mount volume */
    fat32::Volume volume(sd_service.card);
    volume.mount();

    /* Read root  */
    fat32::Filehandler root;
    volume.read_root(root);

    /* create test_file if it does not exist */
    auto test_file = fat32::open(volume, "0:/TEST.DAT");
    if (test_file.state == files::State::Not_Found)
        test_file = fat32::open(volume, "0:/TEST.DAT", files::Mode::out);

    /* Write to the file */
    test_file << "Hello World!" << "\n";
    std::string_view test_string{"TestString"};
    test_file << test_string;

    /* close file again */
    test_file.close();

    while (1)
    {
        OTOS::Task::yield();
    }
};

