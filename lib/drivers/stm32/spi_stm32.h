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

#ifndef SPI_STM32_H_
#define SPI_STM32_H_

/* === Includes === */
#include "vendors.h"
#include <optional>

/* === Needed Interfaces === */
#include "interface.h"
#include "peripherals_stm32.h"

/* === Declarations === */
namespace spi
{
    /* === Classes === */
    class Controller : public driver::Base<stm32::Peripheral>
    {
      public:
        /* === Builder === */
        /**
         * @brief Create a SPI controller object.
         * This enables the peripheral clock and sets the baudrate.
         * 
         * @tparam spi_instance The peripheral instance to use.
         * @param baudrate [bit/s] The baudrate to set.
         * @return Controller The initialized SPI controller object.
         */
        template <stm32::Peripheral spi_instance>
        static auto create(uint32_t baudrate) -> Controller;

        /* === Constructors === */
        Controller() = delete;
        Controller(const Controller &other) = default;
        Controller(Controller &&other) = default;
        auto operator=(const Controller &other) -> Controller & = default;
        auto operator=(Controller &&other) -> Controller & = default;

        /* === Setters === */
        /**
         * @brief Set the timing parameters of the SPI clock SCK.
         *
         * @param idle The logic level of the clock when bus is idle.
         * @param data_valid The clock edge when the data on the bus is valid.
         * @return Controller& Returns a reference to the SPI controller object.
         */
        auto set_clock_timing(Level idle, Edge data_valid) -> Controller &;

        /**
         * @brief Set the SPI data width to 16 bits.
         * That means that the DR register is 16 bits wide.
         *
         * @attention This setting does not propagate to other drivers!
         * They all assume a 8 bit wide DR register.
         * @return Controller& Returns a reference to the SPI controller object.
         */
        auto set_data_to_16bit() -> Controller &;

        /**
         * @brief Select whether to use a hardware pin as the chip select.
         * The alternate function of the hardware pin has to be assigned to it separately!
         *
         * @param use_hardware Set to true when a hardware chip select should be used.
         * @return Controller& Returns a reference to the SPI controller object.
         */
        auto set_use_hardware_chip_select(bool use_hardware) -> Controller &;

        /**
         * @brief Set the target address of the SPI controller.
         * @note This function is only present to be compatible with the bus interface
         * and does not have any effect on the SPI controller.
         * 
         * @param address The target address to set.
         * @return Controller& Returns a reference to the SPI controller object.
         */
        auto set_target_address(const uint8_t address) -> Controller & { return *this; }; /* To be compatible with the bus interface */

        /* === Getters === */
        /**
         * @brief Check whether the TX data buffer is empty and can receive new data.
         *
         * @returns Returns True when new data can be written to the register.
         */
        auto last_transmit_finished() const -> bool;

        /**
         * @brief Check whether the SPI peripheral is busy.
         *
         * @returns Returns True when the peripheral is busy.
         */
        auto is_busy() const -> bool;

        /**
         * @brief Return the most recent received data.
         * 
         * @return Union which contains the received data. The union can be used
         * to use certain parts of the read values.
         */
        auto get_rx_data() const -> bus::Data_t;

        /**
         * @brief Check whether the RX buffer contains valid data.
         *
         * @return Returns True when the DR register contains valid data.
         */
        auto RX_data_valid() const -> bool;
        // auto transfer_finished() const -> bool;

        /* === Methods === */
        /**
         * @brief Enable the SPI peripheral
         */
        void enable();

        /**
         * @brief Disable the SPI peripheral
         */
        void disable();

        /**
         * @brief Send n bytes to a spi target
         * The highest byte in the payload is transmitted first!
         *
         * Sets the following errors:
         * - SPI_Timeout
         * - SPI_BUS_Busy
         *
         * @param payload Data struct with maximum 4 bytes
         * @param n_bytes How many bytes should be sent
         * @return Returns True when the data was sent successfully, False otherwise.
         * @details blocking function
         */
        auto send_data(bus::Data_t payload, uint8_t n_bytes) -> bool;

        /**
         * @brief Send an array with n bytes to a spi target.
         * The first element in the array is transmitted first!
         *
         * Sets the following errors:
         * - SPI_Timeout
         * - SPI_BUS_Busy
         *
         * @param data Address of array which contains the data
         * @param n_bytes How many bytes should be sent
         * @return Returns True when the array was sent successfully, False otherwise.
         * @details blocking function
         */
        auto send_array(const uint8_t *data, uint8_t n_bytes) -> bool;
        // bool send_array_leader(uint8_t byte, uint8_t *data, uint8_t n_bytes);

        /**
         * @brief Read n bytes from a spi target from an register.
         * The highest byte in the payload is received first!
         *
         * Sets the following errors:
         * - SPI_Timeout
         * - SPI_BUS_Busy
         *
         * @param payload Data struct with maximum 4 bytes
         * @param n_bytes How many bytes should be sent
         * @return Returns True when the data was sent successfully, False otherwise.
         * @details blocking function
         */
        auto read_data(uint8_t reg, uint8_t n_bytes) -> bool;
        // bool read_array(const uint8_t reg, uint8_t *dest, const uint8_t n_bytes);

        /**
         * @brief Read an array with n bytes from a spi target
         * without sending a register address.
         * The first element in the array is received first!
         *
         * Sets the following errors:
         * - SPI_Timeout
         * - SPI_BUS_Busy
         *
         * @param data Address of array where the data should be stored
         * @param n_bytes How many bytes should be read
         * @return Returns True when the array was read successfully, False otherwise.
         * @details blocking function
         */
        auto read_array(uint8_t *dest, uint8_t n_bytes) -> bool;

        /**
         * @brief Create a dma stream object for the SPI controller.
         * This function should be called with a rvalue reference to a DMA Stream object.
         *
         * @tparam DMA_Stream The type of the DMA Stream.
         * @param stream The rvalue reference to the DMA Stream object.
         * @param direction The direction of the DMA Stream.
         * @return Returns the DMA Stream object which is set up for the SPI controller.
         */
        template <class DMA_Stream>
        auto create_dma_stream(DMA_Stream &&stream, const dma::Direction direction) const -> DMA_Stream
        {
            this->peripheral->CR2 |= (direction == dma::Direction::memory_to_peripheral) ? SPI_CR2_TXDMAEN : SPI_CR2_RXDMAEN;
            stream.assign_peripheral(this->peripheral->DR);
            stream.set_direction(direction);

            // When the SPI is in 16bit mode, the DMA has to transfer 2 bytes at once.
            if (this->peripheral->CR1 & SPI_CR1_DFF)
                stream.set_peripheral_size(dma::Width::_16bit);

            return stream;
        };

      private:
        /* === Constructors === */
        /**
         * @brief Construct a new spi controller object
         * Only integer fractions of the CPU clock are possible for the baudrate!
         * @note This constructor does not enable the peripheral clock. It is
         * intended to be used by the builder method.
         *
         * @param spi_instance The peripheral instance to use.
         * @param spi_address The peripheral address.
         * @param prescaler The prescaler value to set.
         */
        Controller(stm32::Peripheral spi_instance, std::uintptr_t spi_address, uint8_t prescaler);

        /* === Methods === */
        /**
         * @brief Send a byte onto the SPI bus. Only when the TX buffer
         * is empty!
         * Sets the following errors:
         * - SPI_Timeout
         *
         * @param data The byte to send
         * @return Returns True when the byte was send successfully.
         * @details blocking function
         */
        auto send_data_byte(uint8_t data) -> bool;

        /**
         * @brief Read a byte from the SPI bus. Only when the RX buffer
         * is not empty!
         * Sets the following errors:
         * - SPI_Timeout
         *
         * @return Returns True and the received byte when the byte was read successfully.
         * @details blocking function
         */
        auto read_data_byte() -> std::optional<uint8_t>;

        /* === Properties === */
        SPI_TypeDef *peripheral; /**< Pointer to the underlying peripheral hardware address. */
        bus::Data_t rx_data{0};  /**< The last received data. */
    };
}; // namespace spi

#endif // SPI_STM32_H_
