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
 * @file    sdhc.cpp
 * @author  SO
 * @version v2.7.0
 * @date    29-December-2021
 * @brief   Interface with SDHC (and SDSC) cards for file transfer.
 ==============================================================================
 */

/* === Includes === */
#include "memory/sdhc.h"

/* === Methods === */
namespace sdhc
{
    auto Card::set_supply_voltage() -> bool
    {
        auto response = this->mybus->send_command_R7_response(CMD<8>(), CMD8::Voltage_0 | CHECK_PATTERN);
        if (response && ((response.value() & 0xFF) == CHECK_PATTERN))
        {
            /* Card does respond */
            this->state = State::Identification;
            return true;
        }
        else
        {
            /* Card does not respond */
            this->state = State::Disconnected;
            return false;
        }
    };

    auto Card::get_RCA() -> bool
    {
        /* Tell cards to broadcast their RCA */
        auto response = this->mybus->send_command_R2_response(CMD<2>(), 0);

        /* Check for responding cards */
        if (!response)
            return false;

        /* Read the RCA of the connected card */
        response = this->mybus->send_command_R6_response(CMD<3>(), 0);

        /* => At this point32_tit is assumed that the card is responding! */
        /* Read the RCA */
        this->RCA = static_cast<uint16_t>(response.value() >> 16);

        /* Card is now in StandBy state */
        this->state = State::StandBy;
        return true;
    };

    auto Card::set_bus_width_4bits() -> bool
    {
        /* Goto subcommand mode */
        auto response = this->mybus->send_command_R1_response(CMD<55>(), 0);

        /* Check whether card respondend and accepted the subcommand */
        if (!(response.value_or(0) & R1::APP_CMD))
        {
            this->state = State::Disconnected;
            return false;
        }

        /* Sent command to change bus width */
        response = this->mybus->send_command_R1_response(CMD<6>(), 0b10);

        /* Check whether accepts the change */
        if (response.value() & R1::ERROR)
            return false;
        else
            return true;
    };

    void Card::eject()
    {
        /* When card is present eject it */
        if (this->state == State::Transfering)
        {
            this->mybus->send_command_no_response(CMD<15>(), this->RCA << 16);
            this->state = State::Disconnected;
        }
    };

    auto Card::initialize_card() -> bool
    {
        /* Goto subcommand mode */
        auto response = this->mybus->send_command_R1_response(CMD<55>(), 0);

        /* Check whether card respondend and accepted the subcommand */
        if (!(response.value_or(0) & R1::APP_CMD))
        {
            this->state = State::Disconnected;
            return false;
        }

        /* When card is in subcommand mode, start the initialization procedure */
        response = this->mybus->send_command_R3_response(ACMD<41>(), ACMD41::HCS | ACMD41::XPC | OCR::_3_0V);

        /* Check whether card is still busy */
        if (!(response.value() & R3::NOT_BUSY))
            return false;

        /* Check card type in response */
        if (response.value() & R3::CCS)
            this->type_sdsc = false; /* Card is a SDHC card */

        return true;
    };

    auto Card::read_single_block(const uint32_t *buffer_begin, const uint32_t block) -> bool
    {
        /*
         * Get the address:
         * -> SDSC use byte addressing
         * -> SDHC use block addressing
         */
        const uint32_t address_adjusted = this->is_SDSC() ? block * BLOCKLENGTH : block;

        /* Send command to start single block transfer */
        if (!this->mybus->send_command_R1_response(CMD<17>(), address_adjusted))
            return false;

        /* Receive the single block data, buffer holds 4 bytes per entry */
        if (!this->mybus->read_single_block(buffer_begin, buffer_begin + (BLOCKLENGTH / 4)))
            return false;

        /* Block was read successfully */
        return true;
    };

    auto Card::reset() -> bool
    {
        return this->mybus->send_command_no_response(CMD<0>(), 0);
    };

    auto Card::select() -> bool
    {
        /* Send command to select card with matching RCA */
        auto response = this->mybus->send_command_R1_response(CMD<7>(), this->RCA << 16);

        /* Check whether selecting was successfull */
        if (response.value() & R1::ERROR)
            return false;
        else
        {
            this->state = State::Transfering;
            return true;
        }
    };

    auto Card::write_single_block(const uint32_t *buffer_begin, const uint32_t block) -> bool
    {
        /*
         * Get the address:
         * -> SDSC use byte addressing
         * -> SDHC use block addressing
         */
        const uint32_t address_adjusted = this->is_SDSC() ? block * BLOCKLENGTH : block;

        /* Send command to start single block transfer */
        if (!this->mybus->send_command_R1_response(CMD<24>(), address_adjusted))
            return false;

        /* Write the single block data, BLOCKLENGTH need to be divided by 4 -> 4 bytes per long */
        if (!this->mybus->write_single_block(buffer_begin, buffer_begin + BLOCKLENGTH / 4))
            return false;

        /* Block was written successfully */
        return true;
    };
}; // namespace sdhc