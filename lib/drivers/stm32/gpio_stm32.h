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

#ifndef GPIO_STM32_H_
#define GPIO_STM32_H_

// === Includes ===
#include "vendors.h"
#include "interface.h"

// === Declarations ===

namespace GPIO {
    
    // === Enums ===
    enum class Port: unsigned char
    {
        A = 0, B, C, D, E, F, G, H, I, J, K 
    };
    enum class Mode: unsigned char
    {
        Input = 0, Output, AF_Mode, Analog
    };
    enum class Output: bool
    {
        Push_Pull = false, Open_Drain = true
    };
    enum class Speed: unsigned char
    {
        Low = 0, Medium, High, Very_High
    };
    enum class Pull: unsigned char
    {
        No_Pull = 0, Pull_Up, Pull_Down
    };
    enum class Edge: unsigned char
    {
        Rising = 1, Falling = 2, Both = 3
    };

    // namespace detail {
        // === Assert Functions ===
        // template<Port PinPort>
        // constexpr void assert_port_valid(void)
        // {
        //     static_assert()
        // };

        template<unsigned char PinNumber>
        constexpr unsigned char assert_pin_number_valid()
        {
            static_assert((PinNumber < 16) & (PinNumber >= 0), "Pin is not valid!");
            return PinNumber;
        };

        // === Helper functions ===
    // };

    // === Classes ===
    class PIN
    {
    private:
        // properties
        volatile GPIO_TypeDef*  thisPort;
        const unsigned char     thisPin;
        const Port              PortID;
        bool                    state_old    = false;
        bool                    edge_rising  = false;
        bool                    edge_falling = false;


        // methods
        unsigned char   get_af_code(const IO function) const;

    public:
        // Constructor
        PIN() = delete;
        PIN(const Port Port, const unsigned char Pin);
        PIN(const Port Port, const unsigned char Pin, const Mode PinMode);

        // Methods
        void set_mode               (const Mode NewMode);
        void set_output_type        (const Output NewType);
        void set_speed              (const Speed NewSpeed);
        void set_pull               (const Pull NewPull);
        void set_alternate_function (const IO function);
        void set_state              (const bool NewState);
        void set_high               (void);
        void set_low                (void);
        void toggle                 (void);
        bool get_state              (void) const;
        void read_edge              (void);
        bool rising_edge            (void) const;
        bool falling_edge           (void) const;
        bool enable_interrupt       (const Edge NewEdge) const;
        void reset_pending_interrupt(void) const;
    };
};

#endif