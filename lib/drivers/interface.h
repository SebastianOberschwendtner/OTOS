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

#ifndef INTERFACE_H_
#define INTERFACE_H_

/* === Includes === */
#include <array>
#include <cstdint>
#include <optional>
#include <misc/error_codes.h>

/* === Defines === */
/** For atomic driver functions which can be used within interrupts */
#define OTOS_ATOMIC [[gnu::always_inline]] inline

/* === Common enums === */
enum class Edge : bool
{
    Falling = false,
    Rising = true
};

enum class Level : bool
{
    Low = false,
    High = true
};

/* === Common base class for every driver === */
namespace driver
{
    template <typename Peripheral_t>
    class Base
    {
      public:
        /* === Constructor === */
        Base() = default;
        Base(const Peripheral_t IO_instance) : instance{IO_instance} {};

        /* === Methods === */
        void set_error(const error::Code err) { this->error = err; };
        void set_timeout(const uint32_t call_count) { this->timeout = call_count; };
        void reset_timeout() { this->called = 0; };
        auto timed_out() -> bool { return (++this->called > this->timeout); };
        auto get_error(void) const -> error::Code { return this->error; };

        /* === Properties === */
        Peripheral_t instance{};

      private:
        /* === Properties === */
        error::Code error{error::Code::None};
        uint32_t timeout{0};
        uint32_t called{0};
    };
};

// === Define the Interfaces ===

// => GPIO Interface
namespace gpio
{
    /**
     * @brief Set the alternate function of a GPIO pin.
     *
     * @tparam IO The GPIO pin class which implements the alternate function.
     * @param Pin The instance of the pin class.
     * @param IO_Controller The desired alternate function of the pin.
     */
    template <class IO, typename Peripheral_t>
    void assign(IO &Pin, driver::Base<Peripheral_t> &IO_Controller)
    {
        Pin.set_alternate_function(IO_Controller.instance);
    }
};

// => Bus Communication Interface
namespace bus
{
    // === Enums ===
    // Bus state
    enum class State : uint8_t
    {
        Init = 1,
        Idle,
        Busy,
        Error
    };

    // Data type to handle different byte formats
    union Data_t
    {
        uint32_t value;
        uint16_t word[2];
        uint8_t byte[4];

        // Overload the cast operator to int(), mainly for testing but maybe it can be useful in general
        operator int() { return static_cast<uint32_t>(this->value); };
    };

    /* === Bus interface === */
    /**
     * @brief Set the address of the target for the next communication.
     * The actual implementation depends on whether the bus actually
     * uses addresses.
     * @param address The address of the target.
     */
    template <class bus_controller>
    void change_address(bus_controller &bus, const uint8_t address)
    {
        bus.set_target_address(address);
        return;
    };

    /**
     * @brief Send a byte to a bus target
     * @param byte Byte to be sent
     * @return Returns True when the byte was sent successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    bool send_byte(bus_controller &bus, const uint8_t byte)
    {
        // set the payload data
        Data_t payload{};
        payload.byte[0] = byte;

        // send the data
        return bus.send_data(payload, 1);
    };

    template <class bus_controller>
    bool send_bytes(
        bus_controller &bus,
        const uint8_t first_byte,
        const uint8_t second_byte)
    {
        // set the payload data
        Data_t payload{};
        payload.byte[1] = first_byte;
        payload.byte[0] = second_byte;

        // send the data
        return bus.send_data(payload, 2);
    };

    template <class bus_controller>
    bool send_bytes(
        bus_controller &bus,
        const uint8_t first_byte,
        const uint8_t second_byte,
        const uint8_t third_byte)
    {
        // set the payload data
        Data_t payload{};
        payload.byte[2] = first_byte;
        payload.byte[1] = second_byte;
        payload.byte[0] = third_byte;

        // send the data
        return bus.send_data(payload, 3);
    };

    /**
     * @brief Send a word to a bus target
     * @param data Word (2 bytes) to be sent
     * @return Returns True when the byte was sent successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    bool send_word(bus_controller &bus, const uint16_t word)
    {
        // set the payload data
        Data_t payload{};
        payload.word[0] = word;

        // send the data
        return bus.send_data(payload, 2);
    };

    /**
     * @brief Send an array with n bytes to a bus target.
     * The first element in the array is transmitted first!
     * @param data Address of array which contains the data
     * @param n_bytes How many bytes should be sent
     * @return Returns True when the array was sent successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    bool send_array(bus_controller &bus, uint8_t *const data, const uint8_t n_bytes)
    {
        return bus.send_array(data, n_bytes);
    };

    /**
     * @brief Send an std::array to a bus target without sending a register address.
     * The highest byte in the output array is the first received byte!
     * @param dest Array which contains the data.
     * @return Returns True when the bytes were send successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller, size_t n_bytes>
    bool send_array(bus_controller &bus, std::array<uint8_t, n_bytes> &data)
    {
        return bus.send_array(data.data(), n_bytes);
    };

    /**
     * @brief Send an array with n bytes to a bus target.
     * Includes a leading byte in front of the array data.
     * The first element in the array is transmitted first!
     * @param byte The leading byte in front of the array
     * @param data Address of array which contains the data
     * @param n_bytes How many bytes should be sent
     * @return Returns True when the array was sent successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    bool send_array_leader(bus_controller &bus, const uint8_t byte, uint8_t *const data, const uint8_t n_bytes)
    {
        return bus.send_array_leader(byte, data, n_bytes);
    };

    /**
     * @brief Read a byte from a bus target without sending a register address.
     * @return Returns True and Value when the byte was read successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    std::optional<uint8_t> read_byte(bus_controller &bus)
    {
        if (bus.read_data(1))
            return bus.get_rx_data().byte[0];
        return {};
    };

    /**
     * @brief Read a word from a bus target
     * @param reg Register address of target to get the data from
     * @return Returns True and Value when the word was read successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    std::optional<uint16_t> read_word(bus_controller &bus, const uint8_t reg)
    {
        if (bus.read_data(reg, 2))
            return bus.get_rx_data().word[0];
        return {};
    };

    /**
     * @brief Read an array with n bytes from a bus target
     * The highest byte in the output array is the first received byte!
     * @param reg Register address of target to get the data from
     * @param dest Address of the array where to save the response
     * @param n_bytes How many bytes should be read
     * @return Returns True when the bytes were read successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    bool read_array(bus_controller &bus, const uint8_t reg, uint8_t *dest, const uint8_t n_bytes)
    {
        return bus.read_array(reg, dest, n_bytes);
    };

    /**
     * @brief Read an array with n bytes from a bus target without sending a register address.
     * The highest byte in the output array is the first received byte!
     * @param dest Address of the array where to save the response
     * @param n_bytes How many bytes should be read
     * @return Returns True when the bytes were read successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    bool read_array(bus_controller &bus, uint8_t *dest, const uint8_t n_bytes)
    {
        return bus.read_array(dest, n_bytes);
    };

    /**
     * @brief Read an array with n bytes from a bus target without sending a register address.
     * The highest byte in the output array is the first received byte!
     * @param dest Array where the data will be stored.
     * @return Returns True when the bytes were read successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller, size_t n_bytes>
    bool read_array(bus_controller &bus, std::array<uint8_t, n_bytes> &dest)
    {
        return bus.read_array(dest.data(), n_bytes);
    };
};

// => SD Card Interface
namespace sdio
{
    /*
     * The following functions are just wrapper for the SDIO controller class
     * You can provide your own specialized overloads when you want to use an SPI bus
     * to communicate with an sd card.
     */
    template <class sdio>
    bool send_command_no_response(sdio &card, const uint8_t command, const uint32_t arguments)
    {
        return card.send_command_no_response(command, arguments);
    };
    template <class sdio>
    std::optional<uint32_t> send_command_R1_response(sdio &card, const uint8_t command, const uint32_t arguments)
    {
        return card.send_command_R1_response(command, arguments);
    };
    template <class sdio>
    std::optional<uint32_t> send_command_R2_response(sdio &card, const uint8_t command, const uint32_t arguments)
    {
        return card.send_command_R2_response(command, arguments);
    };
    template <class sdio>
    std::optional<uint32_t> send_command_R3_response(sdio &card, const uint8_t command, const uint32_t arguments)
    {
        return card.send_command_R3_response(command, arguments);
    };
    template <class sdio>
    std::optional<uint32_t> send_command_R6_response(sdio &card, const uint8_t command, const uint32_t arguments)
    {
        return card.send_command_R6_response(command, arguments);
    };
    template <class sdio>
    std::optional<uint32_t> send_command_R7_response(sdio &card, const uint8_t command, const uint32_t arguments)
    {
        return card.send_command_R1_response(command, arguments);
    };
    template <class sdio>
    bool read_single_block(sdio &card, const uint32_t *buffer_begin, const uint32_t *buffer_end)
    {
        return card.read_single_block(buffer_begin, buffer_end);
    };
    template <class sdio>
    bool write_single_block(sdio &card, const uint32_t *buffer_begin, const uint32_t *buffer_end)
    {
        return card.write_single_block(buffer_begin, buffer_end);
    };
};

// => Timer Interface
namespace timer
{

    /**
     * @brief Start a timer.
     *
     * @tparam timer Timer controller class which implements behavior.
     * @param my_timer The instance of the timer class which should be started.
     */
    template <class timer>
    void start(timer &my_timer) { my_timer.start(); };

    /**
     * @brief Stop a timer.
     *
     * @tparam timer Timer controller class which implements behavior.
     * @param my_timer The instance of the timer class which should be stopped.
     */
    template <class timer>
    void stop(timer &my_timer) { my_timer.stop(); };

    /**
     * @brief Get the current count of the timer.
     *
     * @tparam timer Timer controller class which implements behavior.
     * @param my_timer The instance of the timer class where the count should be returned from.
     * @return uint32_t: The current count of the timer.
     */
    template <class timer>
    uint32_t get_count(timer &my_timer) { return my_timer.get_count(); };
};

namespace dma
{
    // === Enums ===
    enum class Direction
    {
        peripheral_to_memory = 0,
        memory_to_peripheral,
        memory_to_memory
    };
    enum class Width : uint8_t
    {
        _8bit = 0,
        _16bit = 1,
        _32bit = 2
    };
}; // namespace dma
#endif
