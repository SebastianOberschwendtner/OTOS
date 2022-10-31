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
 * @file    using_sd_card.cpp
 * @author  SO
 * @version v3.4.0
 * @date    30-October-2022
 * @brief   This example shows how to setup a SD card, open a file and then
            write to it.
 ==============================================================================
 */

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
    // Create timed task
    OTOS::Timed_Task task(OTOS::get_time_ms);

    // Create SD IO Pins
    GPIO::PIN CLK(GPIO::Port::C, 12, GPIO::Mode::Output);
    GPIO::PIN CMD(GPIO::Port::D, 2, GPIO::Mode::Output);
    GPIO::PIN D0(GPIO::Port::C, 8, GPIO::Mode::Output);
    GPIO::PIN CD(GPIO::Port::C, 11, GPIO::Mode::Output);
    CMD.set_output_type(GPIO::Output::Open_Drain);
    D0.set_output_type(GPIO::Output::Open_Drain);
    CD.set_output_type(GPIO::Output::Open_Drain);
    CMD.set_pull(GPIO::Pull::Pull_Up);
    D0.set_pull(GPIO::Pull::Pull_Up);
    CD.set_pull(GPIO::Pull::Pull_Up);

    // Create SD Card  
    SDHC::service<SD::Controller> sd_service(400'000);
    GPIO::assign(CLK, sd_service.bus);
    GPIO::assign(CMD, sd_service.bus);
    GPIO::assign(D0, sd_service.bus);
    GPIO::assign(CD, sd_service.bus);


    // init Card
    sd_service.initialize(task);

    // mount volume
    FAT32::Volume volume(sd_service.card);
    volume.mount();

    // Read root 
    FAT32::Filehandler root;
    volume.read_root(root);

    // create test_file if it does not exist
    auto test_file = FAT32::open(volume, "0:/TEST.DAT");
    if (test_file.state == Files::State::Not_Found)
        test_file = FAT32::open(volume, "0:/TEST.DAT", Files::out);

    // Write to the file
    test_file << "Hello World!" << "\n";

    // close file again
    test_file.close();

    while (1)
    {
        OTOS::Task::yield();
    }
};

