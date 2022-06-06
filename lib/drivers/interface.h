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

#ifndef INTERFACE_H_
#define INTERFACE_H_

// === Includes ===
#include "error_codes.h"
#include <optional>
#include <cstdint>

// === Common identifier for specialized IOs ===
enum class IO : unsigned char
{
    SYSTEM_ = 0,
    TIM_1,
    TIM_2,
    TIM_3,
    TIM_4,
    TIM_5,
    TIM_6,
    TIM_7,
    TIM_8,
    TIM_9,
    TIM_10,
    TIM_11,
    TIM_12,
    TIM_13,
    TIM_14,
    I2C_1,
    I2C_2,
    I2C_3,
    SPI_1,
    SPI_2,
    SPI_3,
    SPI_4,
    SPI_5,
    SPI_6,
    USART_1,
    USART_2,
    USART_3,
    USART_4,
    USART_5,
    USART_6,
    USART_7,
    USART_8,
    CAN_1,
    CAN_2,
    OTG_FS_,
    OTG_HS_,
    ETH_,
    FSMC_,
    SDIO_,
    DCMI_,
    EVENTOUT_
};

// Common enums
enum class Edge: bool
{
    Falling = false,
    Rising = true
};

enum class Level: bool
{
    Low = false,
    High = true
};

// === Common base class for every driver ===
namespace Driver
{
    class Base
    {
    private:
        // *** Properties ***
        Error::Code error{Error::Code::None};
        unsigned int timeout{0};
        unsigned int called{0};

    public:
        // *** Constructor ***
        Base() = default;
        Base(const IO IO_instance) : instance{IO_instance} {};

        // *** Methods ***
        void set_error(const Error::Code err) { this->error = err; };
        void set_timeout(const unsigned int call_count) { this->timeout = call_count; };
        void reset_timeout(void) { this->called = 0; };
        bool timed_out(void) { return (++this->called > this->timeout); };
        Error::Code get_error(void) const { return this->error; };

        // *** Properties ***
        IO instance{IO::SYSTEM_};
    };
};

// === Define the Interfaces ===

// => GPIO Interface
namespace GPIO
{
    /**
     * @brief Set the alternate function of a GPIO pin.
     * 
     * @tparam IO The GPIO pin class which implements the alternate function.
     * @param Pin The instance of the pin class.
     * @param IO_Controller The desired alternate function of the pin.
     */
    template <class IO>
    void assign(IO &Pin, Driver::Base &IO_Controller)
    {
        Pin.set_alternate_function(IO_Controller.instance);
    }
};

// => Bus Communication Interface
namespace Bus
{
    // === Enums ===
    // Bus state
    enum class State: unsigned char
    {
        Init = 1, Idle, Busy, Error
    };

    // Data type to handle different byte formats
    union Data_t
    {
        unsigned long value;
        unsigned int word[2];
        unsigned char byte[4];

        // Overload the cast operator to int(), mainly for testing but maybe it can be usefull in general
        operator int() { return static_cast<unsigned int>(this->value); };
    };

    /* === Bus interface === */
    /**
     * @brief Set the address of the target for the next communication.
     * The actual implementation depends on whether the bus actually
     * uses addresses.
     * @param address The address of the target.
     */
    template <class bus_controller>
    void change_address(bus_controller &bus, const unsigned char address)
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
    bool send_byte(bus_controller &bus, const unsigned char byte)
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
        const unsigned char first_byte,
        const unsigned char second_byte)
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
        const unsigned char first_byte,
        const unsigned char second_byte,
        const unsigned char third_byte)
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
    bool send_word(bus_controller &bus, const unsigned int word)
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
    bool send_array(bus_controller &bus, const unsigned char *data, const unsigned char n_bytes)
    {
        return bus.send_array(data, n_bytes);
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
    bool send_array_leader(bus_controller &bus, const unsigned char byte, const unsigned char *data, const unsigned char n_bytes)
    {
        return bus.send_array_leader(byte, data, n_bytes);
    };

    /**
     * @brief Read a word from a bus target
     * @param reg Register address of target to get the data from
     * @return Returns True and Value when the word was read successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    std::optional<unsigned int> read_word(bus_controller &bus, const unsigned char reg)
    {
        if (bus.read_data(reg, 2))
            return bus.get_rx_data().word[0];
        return {};
    };

    /**
     * @brief Read an array with n bytes from a bus target
     * The highest byte in the output array is the first received byte!
     * @param reg Register address of target to get the data from
     * @param dest Address of the array where to save the responce
     * @param n_bytes How many bytes should be read
     * @return Returns True when the bytes were read successfully, False otherwise.
     * @details blocking function
     */
    template <class bus_controller>
    bool read_array(bus_controller &bus, const unsigned char reg, unsigned char *dest, const unsigned char n_bytes)
    {
        return bus.read_array(reg, dest, n_bytes);
    };
};

// => SD Card Interface
namespace SD
{
    /*
    * The following functions are just wrapper for the SDIO controller class
    * You can provide your own specialized overloads when you want to use an SPI bus
    * to communicate with an sd card.
    */
    template<class sdio>
    bool send_command_no_response(sdio& card, const unsigned char command, const unsigned long arguments)
    {
        return card.send_command_no_response(command, arguments);
    };
    template<class sdio>
    std::optional<unsigned long> send_command_R1_response(sdio& card, const unsigned char command, const unsigned long arguments)
    {
        return card.send_command_R1_response(command, arguments);
    };
    template<class sdio>
    std::optional<unsigned long> send_command_R2_response(sdio& card, const unsigned char command, const unsigned long arguments)
    {
        return card.send_command_R2_response(command, arguments);
    };
    template<class sdio>
    std::optional<unsigned long> send_command_R3_response(sdio& card, const unsigned char command, const unsigned long arguments)
    {
        return card.send_command_R3_response(command, arguments);
    };
    template<class sdio>
    std::optional<unsigned long> send_command_R6_response(sdio& card, const unsigned char command, const unsigned long arguments)
    {
        return card.send_command_R6_response(command, arguments);
    };
    template<class sdio>
    std::optional<unsigned long> send_command_R7_response(sdio& card, const unsigned char command, const unsigned long arguments)
    {
        return card.send_command_R1_response(command, arguments);
    };
    template<class sdio>
    bool read_single_block(sdio& card, const unsigned long* buffer_begin, const unsigned long* buffer_end)
    {
        return card.read_single_block(buffer_begin, buffer_end);
    };
    template<class sdio>
    bool write_single_block(sdio& card, const unsigned long* buffer_begin, const unsigned long* buffer_end)
    {
        return card.write_single_block(buffer_begin, buffer_end);
    };
};

// => Timer Interface
namespace Timer
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
     * @return unsigned int: The current count of the timer.
     */
    template <class timer>
    unsigned int get_count(timer &my_timer) { return my_timer.get_count(); };
};
#endif
