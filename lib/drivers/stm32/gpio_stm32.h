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

#ifndef GPIO_STM32_H_
#define GPIO_STM32_H_

// === Includes ===
#include "interface.h"
#include "vendors.h"

// === Declarations ===

namespace GPIO
{

    // === Enums ===
    enum class Port : unsigned char
    {
        A = 0, B, C, D, E, F, G, H, I, J, K
    };
    enum class Mode : unsigned char
    {
        Input = 0, Output, AF_Mode, Analog
    };
    enum class Output : bool
    {
        Push_Pull = false,
        Open_Drain = true
    };
    enum class Speed : unsigned char
    {
        Low = 0, Medium, High, Very_High
    };
    enum class Pull : unsigned char
    {
        No_Pull = 0, Pull_Up, Pull_Down
    };
    enum class Edge : unsigned char
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

    template <unsigned char PinNumber>
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
      public:
        /* === Constructors === */
        // No default constructor
        PIN() = delete;

        /**
         * @brief Constructor for PIN object without specified output type.
         * @param Port The Port the pin belongs to
         * @param Pin The pin number of the pin in the port
         */
        PIN(Port Port, unsigned char Pin);

        /**
         * @brief Constructor for PIN object without specified output type.
         * @param Port The Port the pin belongs to
         * @param Pin The pin number of the pin in the port
         */
        PIN(Port Port, unsigned char Pin, Mode PinMode);

        /* === Setters === */
        /**
         * @brief Enable the desired alternate function of the pin.
         * This function does also have side effects when:
         * - Setting the AF to I2C -> The pins is automatically set to open drain.
         * 
         * @attention This version works best for STM32F4 devices since
         * they assign the same alternate function code for each pin.
         * The STM32L0 device have a bit more complex mapping and are not
         * fully supported by this function!
         * -> Use the other version of this function for STM32L0 devices.
         * 
         * @param function The alternate function of the pin.
         */
        void set_alternate_function(IO function);

        /**
         * @brief Set the alternate function of the pin by
         * providing the alternate function code. This function
         * does not have other side effects.
         * 
         * @param af_code The alternate function code of the pin.
         */
        void set_alternate_function(unsigned char af_code);
        
        /**
         * @brief Set the GPIO pin high. Atomic access, sets the pin
         * always high.
         */
        void set_high();

        /**
         * @brief Set the GPIO pin low. Atomic access, sets the pin
         * always low.
         */
        void set_low();

        /**
         * @brief Set the output mode of the GPIO pin.
         * @param NewMode The new mode of the pin.
         */
        void set_mode(Mode NewMode);

        /**
         * @brief Set the output type of the GPIO pin.
         * @param NewType The new output type of the pin.
         */
        void set_output_type(Output NewType);

        /**
         * @brief Set the pull type of the GPIO pin.
         * @param NewPull The new pull state of the pin.
         */
        void set_pull(Pull NewPull);

        /**
         * @brief Set the output speed of the GPIO pin.
         * @param NewSpeed The new speed of the pin.
         */
        void set_speed(Speed NewSpeed);

        /**
         * @brief Set the logic output state of the GPIO pin.
         * @param NewState The new output state of the pin.
         */
        void set_state(bool NewState);

        /**
         * @brief Toggle the logic state of a GPIO output pin. Atomic access,
         * toggles the pin state.
         */
        void toggle();

        /* === Getters === */
        /**
         * @brief Get the logical pin state of the GPIO pin.
         * @return The logical state of the pin.
         */
        auto get_state() const -> bool;

        /**
         * @brief Check whether the PIN got a falling edge
         * @attention GPIO::Pin::read_edge() must be called before this function!
         * 
         * @return Returns True when a falling edge was detected.
         */
        auto falling_edge() const -> bool;

        /**
         * @brief Check whether the PIN got a rising edge
         * @attention GPIO::Pin::read_edge() must be called before this function!
         * 
         * @return Returns True when a rising edge was detected.
         */
        auto rising_edge() const -> bool;

        /* === Methods === */
        /**
         * @brief Enable the edge triggered interrupt on the pin.
         * The interrupt is automatically enabled in the core.
         * @details Uses CMSIS driver to set the interrupts within
         * the ARM core.
         * 
         * @param NewEdge Which edges trigger the interrupt
         * @return Returns True when the interrupt was enabled
         * successfully, False otherwise.
         */
        auto enable_interrupt(Edge NewEdge) const -> bool;

        /**
         * @brief Read the current pin state and check whether
         * a logical change occurred.
         *
         * @note This functions is separate so that the program can
         * detect both rising and falling edges at once.
         */
        void read_edge();

        /**
         * @brief Reset a pending interrupt. This function is needed
         * for the STM32 devices because the flag is not automatically
         * reset when the interrupt handler is executed. This function
         * has to be called first in the corresponding interrupt handler!
         */
        void reset_pending_interrupt() const;

      private:
        /* === Properties === */
        volatile GPIO_TypeDef *thisPort; /**< The underlying GPIO port of the pin. */
        unsigned char thisPin;     /**< The pin number of the pin in the port. */
        Port PortID;               /**< The port id of the pin. */
        bool state_old = false;          /**< The old state of the pin for software edge detection. */
        bool edge_rising = false;        /**< Whether the software edge detection detected a rising edge. */
        bool edge_falling = false;       /**< Whether the software edge detection detected a falling edge. */

        /* === Methods === */
        /**
         * @brief Get the code for the alternate function for the AFR register.
         * @param function The desired alternate function of a pin.
         * @return The AF code of the alternate function to put into the AF register.
         */
        auto get_af_code(IO function) const -> unsigned char;
    };
};

#endif