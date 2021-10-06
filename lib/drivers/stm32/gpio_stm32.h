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

    // === Classes ===
    class PIN final: public PIN_Base 
    {
    private:
        // properties
        volatile GPIO_TypeDef*  thisPort;
        const PinNumber         thisPin;
        const PinPort           PortID;
        bool                    state_old    = false;
        bool                    edge_rising  = false;
        bool                    edge_falling = false;

        // methods
        unsigned char   get_af_code(const GPIO::Alternate function) const;

    public:

        // Constructor
        PIN(const PinPort Port, const PinNumber Pin);
        PIN(const PinPort Port, const PinNumber Pin, const Mode PinMode);

        // Methods
        void setMode                (const Mode NewMode)        final;
        void setType                (const Type NewType)        final;
        void setSpeed               (const Speed NewSpeed)      final;
        void setPull                (const Pull NewPull)        final;
        void set_alternate_function (const Alternate function)  final;
        void set                    (const bool NewState)       final;
        void setHigh                (void)                      final;
        void setLow                 (void)                      final;
        void toggle                 (void)                      final;
        bool get                    (void) const                final;
        void read_edge              (void)                      final;
        bool rising_edge            (void) const                final;
        bool falling_edge           (void) const                final;
        bool enable_interrupt       (const Edge NewEdge) const  final;
    };
};

#endif