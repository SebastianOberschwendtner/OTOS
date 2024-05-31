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

#ifndef SDHC_H_
#define SDHC_H_

/* === Includes === */
#include <array>
#include <optional>

/* === Needed Interfaces === */
#include <interface.h>
#include <task.h>

namespace sdhc
{
    /* === Helper === */
    /* SD Check Pattern */
    constexpr uint8_t CHECK_PATTERN = 0b10101;
    /*
     * The standard block length, since SDHC cards only support
     * 512 bytes it is the default value.
     *  -> Should not be changed for SDSC/SDHC cards
     */
    constexpr uint32_t BLOCKLENGTH = 512;

    /* Create a buffer for n blocks, 1 block has 512 bytes -> 128 longs */
    template <uint32_t n_blocks>
    constexpr auto create_block_buffer()
    {
        std::array<uint32_t, n_blocks * 128> buffer{0};
        return buffer;
    };

    /* SD Commands */
    template <uint8_t number>
    constexpr auto CMD() -> uint8_t { return number; };

    template <uint8_t number>
    constexpr auto ACMD() -> uint8_t { return number; };

    /* SD Command bits */
    enum CMD8 : uint32_t
    {
        Voltage_0 = (1 << 8) // Voltage Range 2.7V - 3.0V
    };

    enum ACMD41 : uint32_t
    {
        HCS = (1U << 30), // Host Capacity Support
        XPC = (1U << 28)  // Power Control (0: 0.36W; 1: 0.54W)
    };

    /* Response bits */
    enum R1 : uint32_t
    {
        APP_CMD = (1U << 5),      // Card will accept ACMD as next command
        ERROR = (1U << 19),       // Generic error bit
        ILLEGAL_CMD = (1U << 22), // Illegal command
        READY_4_DATA = (1U << 8)  // Card processed old data and is ready to receive new data
    };

    enum R3 : uint32_t
    {
        NOT_BUSY = (1U << 31), // Card indicates whether initialization is completed
        CCS = (1U << 30)       // Card Capacity Status
    };

    /* SD Register bits */
    enum OCR : uint32_t
    {
        _3_0V = (1U << 17)
    };

    /* === Enums === */
    /* Card states */
    enum class State : uint32_t
    {
        Identification = 0,
        StandBy,
        Transfering,
        Sending,
        Receiving,
        Programming,
        Disconnected
    };

    /* === Classes === */
    /**
     * @brief Interface to the SDHC bus controller.
     *
     * This interface is used to communicate with the SDHC bus controller.
     * It provides the necessary methods to send commands and read/write data.
     */
    struct interface
    {
        virtual ~interface(){};
        virtual auto send_command_no_response(const uint8_t command, const uint32_t argument) -> bool = 0;
        virtual auto send_command_R1_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> = 0;
        virtual auto send_command_R2_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> = 0;
        virtual auto send_command_R3_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> = 0;
        virtual auto send_command_R6_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> = 0;
        virtual auto send_command_R7_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> = 0;
        virtual auto read_single_block(const uint32_t *buffer_begin, const uint32_t *buffer_end) -> bool = 0;
        virtual auto write_single_block(const uint32_t *buffer_begin, const uint32_t *buffer_end) -> bool = 0;
    };

    /**
     * @brief Class template to provide the bus implementation
     */
    template <class bus>
    struct interface_impl final : public interface
    {
        /* Member is the pointer to the bus controller */
        bus *const pimpl;

        /* === Constructor === */
        interface_impl() = delete;
        interface_impl(bus &bus_used) : pimpl(&bus_used){};

        /* Provide the implementations */
        auto send_command_no_response(const uint8_t command, const uint32_t argument) -> bool final
        {
            return sdio::send_command_no_response(*this->pimpl, command, argument);
        };
        auto send_command_R1_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> final
        {
            return sdio::send_command_R1_response(*this->pimpl, command, argument);
        };
        auto send_command_R2_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> final
        {
            return sdio::send_command_R2_response(*this->pimpl, command, argument);
        };
        auto send_command_R3_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> final
        {
            return sdio::send_command_R3_response(*this->pimpl, command, argument);
        };
        auto send_command_R6_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> final
        {
            return sdio::send_command_R6_response(*this->pimpl, command, argument);
        };
        auto send_command_R7_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t> final
        {
            return sdio::send_command_R7_response(*this->pimpl, command, argument);
        };
        auto read_single_block(const uint32_t *buffer_begin, const uint32_t *buffer_end) -> bool final
        {
            return sdio::read_single_block(*this->pimpl, buffer_begin, buffer_end);
        };
        auto write_single_block(const uint32_t *buffer_begin, const uint32_t *buffer_end) -> bool final
        {
            return sdio::write_single_block(*this->pimpl, buffer_begin, buffer_end);
        };
    };

    /**
     * @brief Class representing a SDHC card.
     *
     * This class provides the necessary methods to communicate with a SDHC card.
     */
    class Card
    {
      public:
        /* === Constructor === */
        Card() = delete;
        Card(interface &bus_used) : mybus{&bus_used} {};

        /* === Setters === */
        /**
         * @brief Set the supply voltage range to 2.7-3.6V.
         * -> Needed for card identification, other ranges are reserved.
         *
         * @return Returns True when the card responded and accepted the voltage range.
         */
        auto set_supply_voltage() -> bool;

        /**
         * @brief Change the bus width for the communication to
         * 4bits.
         *
         * This is only possible when the used SDIO peripheral
         * does support that!
         *
         * After issuing this change, you should wait 10ms for
         * the card to change its mode.
         * Also the change has to be forwared to the SDIO
         * peripheral.
         *
         * @return Returns True when the card switched to a 4bit bus.
         */
        auto set_bus_width_4bits() -> bool;

        /* === Getters === */
        /**
         * @brief Read the RCA (Relative Card Address) of the connected
         * Card. This command concludes the identification phase.
         *
         * @return Returns True when the RCA was read successfully.
         */
        auto get_RCA() -> bool;

        /**
         * @brief Check whether the card is a **SDSC** card.
         *
         * @return Returns True when the card is a **SDSC** card.
         * If false the card is a **SDHC** card.
         */
        auto is_SDSC() -> bool const { return this->type_sdsc; };

        /* === Methods === */
        /**
         * @brief Eject the card from the bus.
         *
         * This command is used to eject the card from the bus.
         * The card is not usable after this command.
         */
        void eject();

        /**
         * @brief Tell the card to start its initialization procedure.
         * This command returns False until the card finished the procedure.
         *
         * Check the card state to catch non-responding cards.
         *
         * @return Returns True when the card is initialized.
         */
        auto initialize_card() -> bool;

        /**
         * @brief Read a single block from the Card.
         *
         * @param buffer_begin The begin iterator of the receive buffer.
         * @param block The block address to read from the card.
         * @return Returns True when block was read successfully.
         */
        auto read_single_block(const uint32_t *buffer_begin, uint32_t block) -> bool;

        /**
         * @brief Reset the SDHC card.
         *
         * @return Returns True when the reset was sent to the card.
         */
        auto reset() -> bool;

        /**
         * @brief Select a card using its RCA.
         *
         * @return Returns True when the card is selected.
         */
        auto select() -> bool;

        /**
         * @brief Write a single block from the Card.
         *
         * @param buffer_begin The begin iterator of the data to send.
         * @param block The block address to write to the card.
         * @return Returns True when block was written successfully.
         */
        auto write_single_block(const uint32_t *buffer_begin, uint32_t block) -> bool;

        /* === Properties === */
        bool type_sdsc{true};               /**< The type of the card, true for SDSC and false for SDHC. */
        State state{State::Identification}; /**< The current state of the card. */
        uint16_t RCA{0};                    /**< The relative card address of the card. */

      private:
        /* === Properties === */
        interface *mybus; /**< The pointer to the bus controller implementing the bus interface. */
    };

    /**
     * @brief Class representing a SDHC service.
     * This class provides the necessary methods to communicate with a SDHC card.
     * The service is the "glue" between the volume and the car bus controller.
     *
     * @tparam Bus_Controller The bus controller class which is used for communication.
     */
    template <class Bus_Controller>
    class service
    {
      public:
        /* === Constructor === */
        service() : service{400'000} {};
        service(const uint32_t frequency) : bus{frequency}, bus_impl{bus}, card{bus_impl} {};

        /* === Methods === */
        auto initialize(OTOS::TimedTask &mytask) -> bool
        {
            if (not this->card.reset())
                return false;
            mytask.wait_ms(100);

            if (not this->card.set_supply_voltage())
                return false;

            while (not this->card.initialize_card())
            {
                mytask.wait_ms(100);
            }
            this->card.get_RCA();
            this->card.select();
            return true;
        }

        /* === Properties === */
        Card card;          /**< The used SDHC card. */
        Bus_Controller bus; /**< The used bus controller. */

      private:
        /* === Properties === */
        interface_impl<Bus_Controller> bus_impl; /**< The implementation of the bus controller interface. */
    };
}; // namespace sdhc
#endif // SDHC_H_