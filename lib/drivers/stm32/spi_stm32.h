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

#ifndef SPI_STM32_H_
#define SPI_STM32_H_

// === Includes ===
#include <optional>
#include "vendors.h"

// === Needed Interfaces ===
#include "interface.h"

// === Declarations ===
namespace SPI
{
    // === Classes ===
    template <IO spi_instance>
    class Controller : public Driver::Base
    {
    private:
        // *** Properties ***
        SPI_TypeDef *const peripheral;
        Bus::Data_t rx_data{0};

        // *** Methods ***
        bool send_data_byte(const unsigned char data);
        std::optional<unsigned char> read_data_byte(void);

    public:
        // *** Constructor ***
        Controller() = delete;
        Controller(const unsigned long baudrate);

        // *** Methods ***
        void set_clock_timing(const Level idle, const Edge data_valid);
        void set_use_hardware_chip_select(const bool use_hardware);
        void set_target_address(const unsigned char address) { return; }; // To be compatible with the bus interface
        void set_data_to_16bit(void);
        void enable(void);
        void disable(void);
        bool last_transmit_finished(void) const;
        bool is_busy(void) const;
        bool send_data(const Bus::Data_t payload, const unsigned char n_bytes);
        bool send_array(const unsigned char *data, const unsigned char n_bytes);
        // bool send_array_leader(const unsigned char byte, const unsigned char *data, const unsigned char n_bytes);
        bool read_data(const unsigned char reg, unsigned char n_bytes);
        // bool read_array(const unsigned char reg, unsigned char *dest, const unsigned char n_bytes);
        bool read_array(unsigned char * const dest, const unsigned char n_bytes);
        Bus::Data_t get_rx_data(void) const;
        bool RX_data_valid       (void) const;
        // bool            transfer_finished   (void) const;

        // *** Templated Methods ***

        /**
         * @brief Create a dma stream object for the SPI controller.
         * This function should be called with a rvalue reference to a DMA Stream object.
         * 
         * @tparam DMA_Stream The type of the DMA Stream.
         * @param stream The rvalue reference to the DMA Stream object.
         * @param direction The direction of the DMA Stream.
         * @return Returns the DMA Stream object which is set up for the SPI controller.
         */
        template<class DMA_Stream>
        DMA_Stream create_dma_stream( DMA_Stream &&stream, const DMA::Direction direction ) const
        {
            this->peripheral->CR2 |= (direction == DMA::Direction::memory_to_peripheral) ? SPI_CR2_TXDMAEN : SPI_CR2_RXDMAEN;
            stream.assign_peripheral(this->peripheral->DR);
            stream.set_direction(direction);

            // When the SPI is in 16bit mode, the DMA has to transfer 2 bytes at once.
            if (this->peripheral->CR1 & SPI_CR1_DFF)
                stream.set_peripheral_size(DMA::Width::_16bit);

            return stream;
        };
    };
};

#endif
