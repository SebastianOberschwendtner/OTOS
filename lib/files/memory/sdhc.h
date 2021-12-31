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

#ifndef SDHC_H_
#define SDHC_H_

// === Includes ===
#include <optional>
#include <array>

// === Needed Interfaces ===
#include "interface.h"

// === Declarations ===
namespace SDHC
{
    // === helper ===
    // SD Check Pattern
    constexpr unsigned char CHECK_PATTERN = 0b10101;
    /*
    * The standard block length, since SDHC cards only support
    * 512 bytes it is the default value.
    *  -> Should not be changed for SDSC/SDHC cards
    */
   constexpr unsigned long BLOCKLENGTH = 512;

   // Create a buffer for n blocks, 1 block has 512 bytes -> 128 longs
   template<unsigned int n_blocks>
   constexpr auto create_block_buffer(void)
   {
        std::array<unsigned long, n_blocks*128> buffer{0};
        return buffer;
   };

    // SD Commands
    template<unsigned char number>
    constexpr unsigned char CMD() { return number; };

    template<unsigned char number>
    constexpr unsigned char ACMD() { return number; };

    //SD Command bits
    enum CMD8: unsigned long
    {
        Voltage_0 = (1<<8)   //Voltage Range 2.7V - 3.0V
    };

    enum ACMD41: unsigned long
    {
        HCS      = (1U<<30),  //Host Capacity Support
        XPC      = (1U<<28)   //Power Control (0: 0.36W; 1: 0.54W)
    };

    //Response bits
    enum R1: unsigned long
    {
        APP_CMD      = (1U<<5),   //Card will accept ACMD as next command
        ERROR        = (1U<<19),  //Generic error bit
        ILLEGAL_CMD  = (1U<<22),  //Illegal command
        READY_4_DATA = (1U<<8)    //Card processed old data and is ready to receive new data
    };

    enum R3: unsigned long
    {
        BUSY     = (1U<<31),  //Card indicates whether initialization is completed
        CCS      = (1U<<30)   //Card Capacity Status
    };

    //SD Register bits
    enum OCR: unsigned long
    {
        _3_0V   = (1U<<17)
    };

    // === Enums ===
    // Card states
    enum class State: unsigned long
    {
        Identification = 0,
        StandBy, Transfering, Sending, Receiving, Programming, Disconnected
    };

    // === Classes ===
    struct interface
    {
        virtual ~interface() {};
        virtual bool send_command_no_response(const unsigned char command, const unsigned long argument) = 0;
        virtual std::optional<unsigned long> send_command_R1_response(const unsigned char command, const unsigned long argument) = 0;
        virtual std::optional<unsigned long> send_command_R2_response(const unsigned char command, const unsigned long argument) = 0;
        virtual std::optional<unsigned long> send_command_R3_response(const unsigned char command, const unsigned long argument) = 0;
        virtual std::optional<unsigned long> send_command_R6_response(const unsigned char command, const unsigned long argument) = 0;
        virtual std::optional<unsigned long> send_command_R7_response(const unsigned char command, const unsigned long argument) = 0;
        virtual bool read_single_block(const unsigned long* buffer_begin, const unsigned long* buffer_end) = 0;
        virtual bool write_single_block(const unsigned long* buffer_begin, const unsigned long* buffer_end) = 0;
    };

    // Class template to provide the bus implementation
    template<class bus>
    struct interface_impl final: public interface
    {
        // Member is the pointer to the bus controller
        bus* const pimpl;

        // *** Constructor ***
        interface_impl() = delete;
        interface_impl(bus& bus_used): pimpl(&bus_used) {};

        // Provide the implementations
        bool send_command_no_response(const unsigned char command, const unsigned long argument) final
        {
            return SD::send_command_no_response(this->pimpl, command, argument);
        };
        std::optional<unsigned long> send_command_R1_response(const unsigned char command, const unsigned long argument) final
        {
            return SD::send_command_R1_response(this->pimpl, command, argument);
        };
        std::optional<unsigned long> send_command_R2_response(const unsigned char command, const unsigned long argument) final
        {
            return SD::send_command_R2_response(this->pimpl, command, argument);
        };
        std::optional<unsigned long> send_command_R3_response(const unsigned char command, const unsigned long argument) final
        {
            return SD::send_command_R3_response(this->pimpl, command, argument);
        };
        std::optional<unsigned long> send_command_R6_response(const unsigned char command, const unsigned long argument) final
        {
            return SD::send_command_R6_response(this->pimpl, command, argument);
        };
        std::optional<unsigned long> send_command_R7_response(const unsigned char command, const unsigned long argument) final
        {
            return SD::send_command_R7_response(this->pimpl, command, argument);
        };
        bool read_single_block(const unsigned long* buffer_begin, const unsigned long* buffer_end) final
        {
            return SD::read_single_block(this->pimpl, buffer_begin, buffer_end);
        };
        bool write_single_block(const unsigned long* buffer_begin, const unsigned long* buffer_end) final
        {
            return SD::write_single_block(this->pimpl, buffer_begin, buffer_end);
        };
    };

    class Card
    {
    private:
        // *** Properties ***
        interface* const mybus;

    public:

        // *** Constructor ***
        Card() = delete;
        Card(interface& bus_used): mybus{&bus_used} {};

        // *** Properties ***
        bool type_sdsc{true};
        State state{State::Identification};
        unsigned short RCA{0};

        // *** Methods ***
        bool is_SDSC(void) const {return this->type_sdsc; };
        bool reset(void);
        bool set_supply_voltage(void);
        bool initialize_card(void);
        bool get_RCA(void);
        bool select(void);
        bool set_bus_width_4bits(void);
        void eject(void);
        bool read_single_block(const unsigned long* buffer_begin, const unsigned long block);
        bool write_single_block(const unsigned long* buffer_begin, const unsigned long block);
    };
};
#endif