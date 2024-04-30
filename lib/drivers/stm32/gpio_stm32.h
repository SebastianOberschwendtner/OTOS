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

/* === Includes === */
#include "interface.h"
#include "vendors.h"

/* === Get the available peripherals === */
#include "peripherals_stm32.h"

namespace gpio
{
    /* === Enums === */
    enum class Port : uint8_t
    {
        A = 0, B, C, D, E, F, G, H, I, J, K
    };
    enum class Mode : uint8_t
    {
        Input = 0, Output, AF_Mode, Analog
    };
    enum class Output : bool
    {
        Push_Pull = false,
        Open_Drain = true
    };
    enum class Speed : uint8_t
    {
        Low = 0, Medium, High, Very_High
    };
    enum class Pull : uint8_t
    {
        No_Pull = 0, Pull_Up, Pull_Down
    };
    enum class Edge : uint8_t
    {
        Rising = 1, Falling = 2, Both = 3
    };

    /* === Classes === */
    class Pin
    {
      public:
        /* === Factory === */
        /**
         * @brief Create a Pin object with specified output type.
         * 
         * @tparam port_used The Port the pin belongs to
         * @param Pin The pin number of the pin in the port
         * @param mode The mode of the pin. Default is Input.
         * @return The created Pin object.
         */
        template<Port port_used>
        static auto create(std::uint8_t pin, Mode mode = Mode::Input) -> Pin;

        /* === Constructors === */
        Pin() = delete;
        Pin(const Pin &) = default;
        Pin(Pin &&) = default;
        auto operator=(const Pin &) -> Pin & = default;
        auto operator=(Pin &&) -> Pin & = default;

        /**
         * @brief Constructor for PIN object without specified output type.
         * @param Port The Port the pin belongs to
         * @param Pin The pin number of the pin in the port
         */

        /* === Setters === */
        /**
         * @brief Enable the desired alternate function of the pin.
         * This function does also have side effects when:
         * - Setting the AF to I2C -> The pin is automatically set to open drain.
         * 
         * @attention This function does not check whether the alternate
         * function actually ois valid for the pin!
         * This version works best for STM32F4 devices since
         * they assign the same alternate function code for each pin.
         * The STM32L0 devices have a bit more complex mapping and are not
         * fully supported by this function!
         * -> Use the other version of this function for STM32L0 devices if
         * you are unsure whether your use case is covered by this overload.
         * 
         * @param function The alternate function of the pin.
         * @return Returns a reference to the pin object.
         */
        auto set_alternate_function(stm32::Peripheral function) -> Pin &;

        /**
         * @brief Set the alternate function of the pin by
         * providing the alternate function code. This function
         * does not have other side effects.
         * 
         * @param af_code The alternate function code of the pin.
         * @return Returns a reference to the pin object.
         */
        auto set_alternate_function(uint8_t af_code) -> Pin &;
        
        /**
         * @brief Set the GPIO pin high. Atomic access, sets the pin
         * always high.
         * @return Returns a reference to the pin object.
         */
        auto set_high() -> Pin &;

        /**
         * @brief Set the GPIO pin low. Atomic access, sets the pin
         * always low.
         * @return Returns a reference to the pin object.
         */
        auto set_low() -> Pin &;

        /**
         * @brief Set the output mode of the GPIO pin.
         * @param NewMode The new mode of the pin.
         * @return Returns a reference to the pin object.
         */
        auto set_mode(Mode NewMode) -> Pin &;

        /**
         * @brief Set the output type of the GPIO pin.
         * @param NewType The new output type of the pin.
         * @return Returns a reference to the pin object.
         */
        auto set_output_type(Output NewType) -> Pin &;

        /**
         * @brief Set the pull type of the GPIO pin.
         * @param NewPull The new pull state of the pin.
         * @return Returns a reference to the pin object.
         */
        auto set_pull(Pull NewPull) -> Pin &;

        /**
         * @brief Set the output speed of the GPIO pin.
         * @param NewSpeed The new speed of the pin.
         * @return Returns a reference to the pin object.
         */
        auto set_speed(Speed NewSpeed) -> Pin &;

        /**
         * @brief Set the logic output state of the GPIO pin.
         * @param NewState The new output state of the pin.
         * @return Returns a reference to the pin object.
         */
        auto set_state(bool NewState) -> Pin &;

        /**
         * @brief Toggle the logic state of a GPIO output pin. Atomic access,
         * toggles the pin state.
         * @return Returns a reference to the pin object.
         */
        auto toggle() -> Pin &;

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
        /* === Constructors === */
        /**
         * @brief Construct a new Pin object.
         * @note This constructor does not enable the peripheral clock. It is
         * intended to be used by the factory method.
         * 
         * @param port_address The address of the GPIO port.
         * @param port_id The port id of the pin.
         * @param pin The pin number of the pin in the port.
         * @param mode The mode of the pin to set.
         */
        Pin(std::uintptr_t port_address, Port port_id, std::uint8_t pin, Mode mode);

        /* === Methods === */
        /**
         * @brief Get the code for the alternate function for the AFR register.
         * @param function The desired alternate function of a pin.
         * @return The AF code of the alternate function to put into the AF register.
         */
        auto get_af_code(stm32::Peripheral function) const -> char;

        /* === Properties === */
        volatile GPIO_TypeDef *port; /**< The underlying GPIO port of the pin. */
        uint8_t pin;     /**< The pin number of the pin in the port. */
        Port port_id;               /**< The port id of the pin. */
        bool state_old = false;          /**< The old state of the pin for software edge detection. */
        bool edge_rising = false;        /**< Whether the software edge detection detected a rising edge. */
        bool edge_falling = false;       /**< Whether the software edge detection detected a falling edge. */
    };
}; // namespace gpio

#endif