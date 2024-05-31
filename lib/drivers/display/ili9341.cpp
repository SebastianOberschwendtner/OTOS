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
/**
 ==============================================================================
 * @file    ili9341.cpp
 * @author  SO
 * @version v2.3.0
 * @date    23-Dezember-2021
 * @brief   Driver for the ILI9341 display controller.
 ==============================================================================
 */

/* === Includes === */
#include "ili9341.h"

/* Provide valid instantiations => use this as "concepts" for the display controller */
//* @todo This seems a bit much work, the template approach of the bus controllers is nice though... */
template class ili9341::Controller<spi::Controller, gpio::Pin>;

namespace ili9341
{
    /* === Functions === */
    template <class bus_controller, class gpio>
    Controller<bus_controller, gpio>::Controller(
        bus_controller &bus_used,
        gpio &dx_used,
        gpio &cs_used)
        : mybus{bus_used}, dx_pin{&dx_used}, cs_pin{&cs_used}
    {
        this->cs_pin->set_high();
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_16bits_per_pixel() -> bool
    {
        constexpr uint8_t format = (0b101 << 4) | (0b101);
        return this->send_command(Command::Set_Pixel_Format, format);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_memory_access(
        const uint8_t configuration) -> bool
    {
        return this->send_command(Command::Memory_Access_Ctrl, configuration);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_power_control_1(
        const uint8_t VHR) -> bool
    {
        return this->send_command(Command::Power_Ctrl_1, VHR);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::set_VCOM_control_1(
        const uint8_t VMH,
        const uint8_t VML) -> bool
    {
        return this->send_command(Command::VCOM_Ctrl_1, VMH, VML);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::draw(
        const uint16_t *buffer_begin,
        const uint16_t *buffer_end) -> bool
    {
        /* Start memory write */
        if (!this->send_command_byte(Command::Write_Memory))
            return false;

        /* Switch to data transfer */
        this->dx_pin->set_high();
        this->cs_pin->set_low();

        /* For every pixel in the buffer transmit its color value */
        for (const uint16_t *iter = buffer_begin; iter != buffer_end; iter++)
        {
            if (!bus::send_word(this->mybus, *iter))
                return false;
        }
        this->cs_pin->set_low();

        /* Transmit successful */
        return true;
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::draw(
        const uint8_t *buffer_begin,
        const uint8_t *buffer_end,
        const uint32_t color,
        const uint32_t background) -> bool
    {
        /* Start memory write */
        if (!this->send_command_byte(Command::Write_Memory))
            return false;

        /* Switch to data transfer */
        this->dx_pin->set_high();
        this->cs_pin->set_low();

        /* The BW buffer encodes 8 pixels per byte -> transform this to singular pixels */
        /* Split the buffer into pages which fit the width (240 pixels) of the display. */
        for (const uint8_t *page = buffer_begin; page != buffer_end; page += 240)
        {
            /* 8 Pixels per byte */
            for (uint8_t iPixel = 0; iPixel < 8; iPixel++)
            {
                /* Add one page of data */
                for (const uint8_t *iter = page; iter != (page + 240); iter++)
                {
                    /* Check whether pixel is black or white */
                    if (*iter & (1 << (iPixel)))
                    {
                        if (!bus::send_word(this->mybus, color))
                            return false;
                    }
                    else
                    {
                        if (!bus::send_word(this->mybus, background))
                            return false;
                    }
                }
            }
        }
        this->cs_pin->set_low();

        /* Transmit successful */
        return true;
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::draw(
        const uint8_t *buffer_begin,
        const uint8_t *buffer_end,
        const uint32_t color,
        const uint32_t background,
        void (*hook)()) -> bool
    {
        /* Start memory write */
        if (!this->send_command_byte(Command::Write_Memory))
            return false;

        /* Switch to data transfer */
        this->dx_pin->set_high();
        this->cs_pin->set_low();

        /* The BW buffer encodes 8 pixels per byte -> transform this to singular pixels */
        /* Split the buffer into pages which fit the width (240 pixels) of the display. */
        for (const uint8_t *page = buffer_begin; page != buffer_end; page += 240)
        {
            /* 8 Pixels per byte */
            for (uint8_t iPixel = 0; iPixel < 8; iPixel++)
            {
                /* Add one page of data */
                for (const uint8_t *iter = page; iter != (page + 240); iter++)
                {
                    /* Check whether pixel is black or white */
                    if (*iter & (1 << (iPixel)))
                    {
                        if (!bus::send_word(this->mybus, color))
                            return false;
                    }
                    else
                    {
                        if (!bus::send_word(this->mybus, background))
                            return false;
                    }
                }
                /* Call the hook function after pixel is transmitted */
                hook();
            }
        }
        this->cs_pin->set_low();

        /* Transmit successful */
        return true;
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::initialize() -> bool
    {
        /* Set Power Control */
        if (!this->set_power_control_1(0x03))
            return false;

        /* set VCOM control */
        if (!this->set_VCOM_control_1(0x01, 0x3C))
            return false;

        /* Inverse the Y Pixel mapping */
        if (!this->set_memory_access((1 << 7)))
            return false;

        /* Set RGB format to 16 bits per pixel */
        if (!this->set_16bits_per_pixel())
            return false;
        return true;
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::off() -> bool
    {
        return this->send_command_byte(Command::Display_Off);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::on() -> bool
    {
        return this->send_command_byte(Command::Display_On);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::reset() -> bool
    {
        return this->send_command_byte(Command::Reset);
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::wake_up() -> bool
    {
        return this->send_command_byte(Command::Sleep_Out);
    };

    /* template <class bus_controller, class gpio> */
    /* bool Controller<bus_controller, gpio>::fill( */
    /*     const uint32_t color */
    /* ) */
    /* { */
    /*     if(!this->send_command_byte(Command::Write_Memory)) */
    /*         return false; */
    /*     for(uint32_t i=0; i<(320*240); i++) */
    /*         if(!this->send_data_byte(color)) */
    /*             return false; */
    /*     return true; */
    /* }; */

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::send_command(
        const Command cmd,
        const uint8_t byte0) -> bool
    {
        /* Select Chip */
        this->cs_pin->set_low();
        /* Send the command and payload */
        /* Send the command */
        this->dx_pin->set_low();
        if (!bus::send_byte(this->mybus, static_cast<uint8_t>(cmd)))
            return false;
        /* Send the data */
        this->dx_pin->set_high();
        if (!bus::send_byte(this->mybus, byte0))
            return false;

        /* deselect chip */
        this->cs_pin->set_high();

        /* Return responce */
        return true;
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::send_command(
        const Command cmd,
        const uint8_t byte0,
        const uint8_t byte1) -> bool
    {
        /* Select Chip */
        this->cs_pin->set_low();
        /* Send the command and payload */
        /* Send the command */
        this->dx_pin->set_low();
        if (!bus::send_byte(this->mybus, static_cast<uint8_t>(cmd)))
            return false;
        /* Send the data */
        this->dx_pin->set_high();
        if (!bus::send_byte(this->mybus, byte0))
            return false;
        if (!bus::send_byte(this->mybus, byte1))
            return false;

        /* deselect chip */
        this->cs_pin->set_high();

        /* Return responce */
        return true;
    };

    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::send_command_byte(const Command cmd) -> bool
    {
        /* Indicate that next byte is a command */
        this->dx_pin->set_low();

        /* Select Chip */
        this->cs_pin->set_low();
        /* Send the byte */
        bool response = bus::send_byte(this->mybus, static_cast<uint8_t>(cmd));
        /* deselect chip */
        this->cs_pin->set_high();

        /* Return responce */
        return response;
    };

    /**
     * @brief Send a data byte to the display.
     *
     * @tparam bus_controller The type of the used bus controller.
     * @tparam gpio The Pin class used for the bus communication.
     * @param data The data byte to send to the display.
     * @return Returns True when command was send successfully.
     */
    template <class bus_controller, class gpio>
    auto Controller<bus_controller, gpio>::send_data_byte(const uint8_t data) -> bool
    {
        /* Indicate that next byte is data */
        this->dx_pin->set_high();

        /* Select Chip */
        this->cs_pin->set_low();
        /* Send the byte */
        bool response = bus::send_byte(this->mybus, data);
        /* deselect chip */
        this->cs_pin->set_high();

        /* Return response */
        return response;
    };
}; // namespace ili9341