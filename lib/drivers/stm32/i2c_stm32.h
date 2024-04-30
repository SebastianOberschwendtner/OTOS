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

#ifndef I2C_STM32_H_
#define I2C_STM32_H_

/* === Includes === */
#include "vendors.h"
#include <optional>

/* === Needed Interfaces === */
#include "interface.h"
#include "peripherals_stm32.h"
using Bus::Data_t;

/* === Declarations === */
namespace i2c
{
    /* === Classes === */
    class Controller : public Driver::Base<stm32::Peripheral>
    {
      public:
        /* === Builder === */
        template<stm32::Peripheral i2c_instance>
        static auto create(uint32_t frequency) -> Controller;
        
        /* === Constructors === */
        Controller() = delete;
        Controller(const Controller &) = default;
        Controller(Controller &&) = default;
        auto operator=(const Controller &) -> Controller & = default;
        auto operator=(Controller &&) -> Controller & = default;

        /* === Setters === */
        /**
         * @brief Set the address of the target for the next communication.
         * @param address The 7-bit address of the target.
         * @return Controller& Returns a reference to the controller object.
         */
        auto set_target_address(uint8_t address) -> Controller &;

        /* === Getters === */
        /**
         * @brief Check whether an acknowledge was received. When no communication is ongoing
         * on the bus this will always return true. Only when an nack was received this will
         * become false.
         * @return Returns true when an acknowledge was received in the current transaction.
         */
        auto ack_received() const -> bool;

        /**
         * @brief Check whether the target address was sent successfully on the bus.
         * @return Returns true when address transmission is complete.
         */
        auto address_sent() const -> bool;

        /**
         * @brief Detect wether the bus is currently busy.
         * @return Returns True when the bus is busy, False otherwise.
         */
        auto bus_busy() const -> bool;

        /**
         * @brief Return the most recent received data.
         * @return Union which contains the received data. The union can be used
         * to use certain parts of the read values.
         */
        auto get_rx_data() const -> Data_t;

        /**
         * @brief Return the currently active target address.
         * @return The 7-bit target address.
         */
        auto get_target_address() const -> uint8_t;

        /**
         * @brief Check whether the hardware peripheral is in controller mode.
         * @return Returns true when the peripheral is controller mode and false when
         * it is in target mode.
         */
        auto in_controller_mode() const -> bool;

        /**
         * @brief Check whether the RX data shift register contains valid data and
         * can be read.
         * @returns Returns True when new data can be read from the register.
         */
        auto RX_data_valid() const -> bool;

        /**
         * @brief Check whether the start condition was generated on the bus
         * by the controller hardware.
         * @return Returns true when start condition was generated.
         */
        auto start_sent() const -> bool;

        /**
         * @brief Check whether the transfer of the current byte is finished.
         * @returns Returns True when the transfer is finished.
         */
        auto transfer_finished() const -> bool;

        /**
         * @brief Check whether the TX data shift register is empty and
         * can receive new data.
         * @returns Returns True when new data can be written to the register.
         */
        auto TX_register_empty() const -> bool;

        /* === Methods === */
        /**
         * @brief Enable the hardware peripheral.
         */
        void enable();

        /**
         * @brief Enable the hardware peripheral.
         */
        void disable();

        /**
         * @brief Generate a start condition on the bus.
         */
        void generate_start() const;

        /**
         * @brief Generate a stop condition on the bus after the current byte transfer.
         * Also disables the ACK bit when in receiver mode.
         */
        void generate_stop() const;

        /**
         * @brief Read n bytes from an i2c target
         * The highest byte in the struct is the first received byte!
         * Sets the following errors:
         * - I2C_Timeout
         * - I2C_Data_ACK_Error
         * - I2C_BUS_Busy
         * @param reg Register address of target to get the data from
         * @param n_bytes How many bytes should be read
         * @return Returns True when the byte was read successfully, False otherwise.
         * @details blocking function
         */
        auto read_data(uint8_t reg, uint8_t n_bytes) -> bool;

        /**
         * @brief Read an array with n from an i2c target
         * The highest byte in the output array is the first received byte!
         * Sets the following errors:
         * - I2C_Timeout
         * - I2C_Data_ACK_Error
         * - I2C_BUS_Busy
         * @param reg Register address of target to get the data from
         * @param dest Address of the array where to save the responce
         * @param n_bytes How many bytes should be read
         * @return Returns True when the bytes were read successfully, False otherwise.
         * @details blocking function
         */
        auto read_array(uint8_t reg, uint8_t *dest, uint8_t n_bytes) -> bool;

        /**
         * @brief Send the set target address via the I2C bus.
         * Sets the following errors:
         * - I2C_Timeout
         * - I2C_Address_Error
         * @param read Set this to true when the address should indicate a data read.
         * @return Returns True when the address was sent successfully, False otherwise.
         * @details blocking function
         */
        auto send_address(bool read = false) -> bool;

        /**
         * @brief Send an array with n bytes to an i2c target.
         * The first element in the array is transmitted first!
         * Sets the following errors:
         * - I2C_Timeout
         * - I2C_Data_ACK_Error
         * - I2C_BUS_Busy
         * @param data Address of array which contains the data
         * @param n_bytes How many bytes should be sent
         * @return Returns True when the array was sent successfully, False otherwise.
         * @details blocking function
         */
        auto send_array(uint8_t *data, uint8_t n_bytes) -> bool;

        /**
         * @brief Send an array with n bytes to an i2c target.
         * Includes a leading byte in front of the array data.
         * The first element in the array is transmitted first!
         * Sets the following errors:
         * - I2C_Timeout
         * - I2C_Data_ACK_Error
         * - I2C_BUS_Busy
         * @param byte The leading byte in front of the array
         * @param data Address of array which contains the data
         * @param n_bytes How many bytes should be sent
         * @return Returns True when the array was sent successfully, False otherwise.
         * @details blocking function
         */
        auto send_array_leader(uint8_t byte, uint8_t *data, uint8_t n_bytes) -> bool;

        /**
         * @brief Send n bytes to an i2c target
         * The highest byte in the struct is transmitted first!
         * Sets the following errors:
         * - I2C_Timeout
         * - I2C_Data_ACK_Error
         * - I2C_BUS_Busy
         * @param payload Data struct with maximum 4 bytes
         * @param n_bytes How many bytes should be sent
         * @return Returns True when the byte was sent successfully, False otherwise.
         * @details blocking function
         */
        auto send_data(Data_t payload, uint8_t n_bytes) -> bool;

        /**
         * @brief Write the current target address to the output shift register.
         * @param read Set this to true when the address should indicate a data read.
         */
        void write_address(bool read = false);

      private:
        /* === Constructors === */
        /**
         * @brief constructor for i2c controller object.
         * @param instance The I2C hardware instance to be used as the controller hardware.
         * @param i2c_address The peripheral address of the used i2c hardware instance.
         * @param frequency The clock frequency for the i2c communication.
         */
        Controller(stm32::Peripheral i2c_instance, std::uintptr_t i2c_address, uint32_t frequency);

        /* === Methods === */
        /**
         * @brief Read a byte from the data register of the i2c peripheral
         * and wait until the data becomes valid. Does not send an address on the bus.
         * The data is returned as an optional type.
         * Sets the following errors:
         * - I2C_Data_ACK_Error
         * @return Returns True when the bytes were read successfully, False otherwise.
         * When True the return data contains valid data.
         * @details blocking function
         */
        auto read_data_byte() -> std::optional<uint8_t>;

        /**
         * @brief Read a byte from the input shift register.
         * Does not check whether the data is valid!
         * @return Returns the data in the receive register.
         */
        auto read_data_register() const -> uint8_t;

        /**
         * @brief Write a byte to the data register of the i2c peripheral
         * and wait until it is sent. Does not send an address on the bus.
         * Sets the following errors:
         * - I2C_Timeout
         * - I2C_Data_ACK_Error
         * @param data The byte to be sent
         * @return Returns True when the byte was sent successfully, False otherwise
         * @details blocking function
         */
        auto send_data_byte(uint8_t data) -> bool;

        /**
         * @brief Write a byte to the output shift register.
         * @param data Data for the ouput register
         */
        void write_data_register(uint8_t data);

        /* === Properties === */
        volatile I2C_TypeDef *peripheral; /**< Pointer to the used peripheral hardware. */
        uint8_t target{0};          /**< Target address of the I2C communication. */
        Data_t rx_data{0};                /**< Data received from the bus. */
    };
};

#endif