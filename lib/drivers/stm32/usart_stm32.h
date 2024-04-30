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

#ifndef USART_STM32_H_
#define USART_STM32_H_

/* === Includes === */
#include "vendors.h"
#include <optional>

/* === Needed Interfaces === */
#include "interface.h"
#include "peripherals_stm32.h"

/* === Declarations === */
namespace usart
{
    /* === Enums === */
    enum class StopBits : uint8_t
    {
        _0_5 = 0b01,
        _1_0 = 0b00,
        _1_5 = 0b11,
        _2_0 = 0b10
    };

    /* === Classes === */
    class Controller : public driver::Base<stm32::Peripheral>
    {
      public:
        /* === Factory === */
        /**
         * @brief Create a USART controller object.
         *
         * @tparam usart The USART instance to use.
         * @param baudrate [bit/s] The baudrate to use.
         * @param data_bits How many data bits to use. Default is 8.
         * @param stop_bits How many stop bits to use. Default is 1.
         * @return Controller Returns a new USART controller object.
         */
        template <stm32::Peripheral usart>
        static auto create(
            const uint32_t baudrate,
            const uint8_t data_bits = 8,
            StopBits stop_bits = StopBits::_1_0) -> Controller;

        /* === Constructors === */
        Controller() = delete;
        Controller(const Controller &) = default;
        Controller(Controller &&) = default;
        auto operator=(const Controller &) -> Controller & = default;
        auto operator=(Controller &&) -> Controller & = default;

        /* === Setters === */
        /**
         * @brief Set the target address of the SPI controller.
         * @note This function is only present to be compatible with the bus interface
         * and does not have any effect on the USART controller.
         *
         * @param address The target address to set.
         * @return Controller& Returns a reference to the SPI controller object.
         */
        auto set_target_address(const uint8_t address) -> Controller & { return *this; };

        /* === Getters === */
        /**
         * @brief Check whether the last byte transfer is finished.
         *
         * @tparam usart_instance The USART peripheral to be used.
         * @return Returns True when the last transfer is finished, i.e.
         * the TX buffer is empty.
         */
        auto last_transmit_finished() const -> bool;

        /**
         * @brief Check whether the bus is busy.
         *
         * @return Returns True when the bus is busy.
         */
        auto is_busy() const -> bool;

        /* === Methods === */
        /**
         * @brief Enable the USART peripheral.
         */
        void enable();

        /**
         * @brief Disable the USART peripheral.
         */
        void disable();

        /**
         * @brief Send n bytes to an USART target
         * The highest byte in the payload is transmitted first!
         *
         * Be aware, that this functions exits when the last byte
         * is still beeing sent! So when you need to perform actions
         * after the last byte is sent, use the method `last_transmit_finished()`
         * to check whether the last byte is sent or not.
         *
         * Sets the following errors:
         * - USART_Timeout
         * - USART_BUS_Busy
         *
         * @param payload Data struct with maximum 4 bytes
         * @param n_bytes How many bytes should be sent
         * @return Returns True when the data was sent successfully, False otherwise.
         * @details blocking function
         */
        auto send_data(const bus::Data_t payload, const uint8_t n_bytes) -> bool;

        /**
         * @brief Send an array with n bytes to an USART target.
         * The first element in the array is transmitted first!
         *
         * Sets the following errors:
         * - USART_Timeout
         * - USART_BUS_Busy
         *
         * @param data Address of array which contains the data
         * @param n_bytes How many bytes should be sent
         * @return Returns True when the array was sent successfully, False otherwise.
         * @details blocking function
         */
        auto send_array(const uint8_t *data, const uint8_t n_bytes) -> bool;
        // bool send_array_leader(const uint8_t byte, const uint8_t *data, const uint8_t n_bytes);
        // bool read_data(const uint8_t reg, uint8_t n_bytes);
        // bool read_array(const uint8_t reg, uint8_t *dest, const uint8_t n_bytes);
        // Bus::Data_t get_rx_data(void) const;
        // bool            in_controller_mode  (void) const;
        // bool            start_sent          (void) const;
        // bool            address_sent        (void) const;
        // bool            ack_received        (void) const;
        // bool            RX_data_valid       (void) const;
        // bool            transfer_finished   (void) const;

      private:
        /* === Constructors === */
        Controller(
            stm32::Peripheral usart_id,
            std::uintptr_t usart_address,
            uint16_t prescaler,
            uint8_t data_bits,
            StopBits stop_bits);

        /* === Methods === */
        /**
         * @brief Send a byte onto the USART bus. Only when the TX buffer
         * is empty!
         * Sets the following errors:
         * - USART_Timeout
         *
         * @param data The byte to send
         * @return Returns True when the byte was send successfully.
         * @details blocking function
         */
        auto send_data_byte(const uint8_t data) -> bool;
        // auto read_data_byte() -> std::optional<uint8_t>;

        /* === Properties === */
        USART_TypeDef *peripheral; /**< Pointer to the used peripheral hardware. */
        bus::Data_t rx_data{0};    /**< Data received from the bus. */
    };
}; // namespace usart

#endif
