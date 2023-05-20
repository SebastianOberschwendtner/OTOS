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

#ifndef MAX17205_H_
#define MAX17205_H_

// === includes ===
#include <cstdint>
#include <ratio>
#include <optional>
#include <misc/units.h>
#include <misc/bits.h>
#include "drivers.h"

// === Command codes ===
namespace MAX17205 {

    // === Constants ===
    constexpr unsigned char i2c_address_low  = 0x6C; // For accessing registers 0x000 - 0x0FF
    constexpr unsigned char i2c_address_high = 0x16; // For accessing registers 0x180 - 0x1FF
    constexpr unsigned char R_sense_mOhm     = 5;   // [mΩ] The value of the current sense resistor

    // === Define units and literal operators ===
    using percent = OTOS::Unit<std::ratio<1U, 256U>, uint16_t>; // [%], Resolution is 1/256 [%]
    using mAh = OTOS::Unit<std::ratio<5U, R_sense_mOhm>, uint16_t>; // [mAh], Resolution is 5 / R_sense [uVh/mOhm] 
    using mA = OTOS::Unit<std::ratio<1000, 640LL * R_sense_mOhm>, int16_t>; // [mA], Resolution is 1000 / (640 * R_sense) [uV/mOhm]
    using mV = OTOS::Unit<std::ratio<10U, 128U>, uint16_t>; // [mV], Resolution is 10/128 [mV] = 78.125 [uV]
    using degC = OTOS::Unit<std::ratio<1U, 256U>, int16_t>; // [°C], Resolution is 1/256 [°C]
    using Ohm = OTOS::Unit<std::ratio<1U, 4096U>, uint16_t>; // [Ohm], Resolution is 1/4096 [Ohm]
    using seconds = OTOS::Unit<std::ratio<5625U, 1000U>, uint32_t>; // [s], Resolution is 5625/1000 [s] = 5.625 [s]

    // === Bits ===
    // *** nPackCfg ***
    constexpr uint16_t BALCFG_0        = (1 << 5);
    constexpr uint16_t BALCFG_1        = (1 << 6);
    constexpr uint16_t BALCFG_2        = (1 << 7);
    constexpr uint16_t ChEn            = (1 << 10);
    constexpr uint16_t TdEn            = (1 << 11);

    namespace Register // Register addresses
    {
        // === Registers ===
        constexpr uint16_t SAlrtTh         = 0x003;
        constexpr uint16_t Cap_Remaining   = 0x005;
        constexpr uint16_t SOC             = 0x006;
        constexpr uint16_t TTE             = 0x011;    
        constexpr uint16_t Config          = 0x01D;
        constexpr uint16_t TTF             = 0x020;    
        constexpr uint16_t Current         = 0x00A;
        constexpr uint16_t Avg_Current     = 0x00B;
        constexpr uint16_t PackCfg         = 0x0BD;
        constexpr uint16_t Avg_Cell_4      = 0x0D1;
        constexpr uint16_t Avg_Cell_3      = 0x0D2;
        constexpr uint16_t Avg_Cell_2      = 0x0D3;
        constexpr uint16_t Avg_Cell_1      = 0x0D4;
        constexpr uint16_t Cell_4          = 0x0D5;
        constexpr uint16_t Cell_3          = 0x0D6;
        constexpr uint16_t Cell_2          = 0x0D7;
        constexpr uint16_t Cell_1          = 0x0D8;
        constexpr uint16_t Batt_Register   = 0x0DA;
        constexpr uint16_t nConfig         = 0x1B0;
        constexpr uint16_t nPackCfg        = 0x1B5;
    }; // namespace Register

    /**
     * @brief Base class for all registers
     * 
     * @tparam register_type The type of the register value
     */
    template<typename register_type>
    struct RegisterBase
    {
        const uint16_t address;
        register_type value;
        RegisterBase() = delete;
        /**
         * @brief Construct a new Register Base object
         * 
         * @param address The address of the register
         */
        RegisterBase(uint16_t address) : address(address), value(0) {};
    };

    class SAlrtTh : public RegisterBase<uint16_t>
    {
    private:

    public:
        SAlrtTh() : RegisterBase(Register::SAlrtTh) {};
        [[nodiscard]] auto SMIN() const -> uint8_t { return static_cast<uint8_t>(bits::get(value, {0xFF, 0})); }
        [[nodiscard]] auto SMAX() const -> uint8_t { return static_cast<uint8_t>(bits::get(value, {0xFF, 8})); }
        void set_SMIN(const uint8_t soc) { this-> value = bits::set(this->value, {0xFF, 0, soc}); }
        void set_SMAX(const uint8_t soc) { this-> value = bits::set(this->value, {0xFF, 8, soc}); }
    };

    class Config : public RegisterBase<uint16_t>
    {
    private:

    public:
        Config() : RegisterBase(Register::Config) {};
        [[nodiscard]] auto Aen() const -> bool { return (value & (1 << 2)) > 0; }
        [[nodiscard]] auto ALRTp() const -> bool { return (value & (1 << 11)) > 0; }
        void set_Aen(const bool flag) { this->value = bits::set(this->value, {1, 2, flag}); }
        void set_ALRTp(const bool flag) { this->value = bits::set(this->value, {1, 11, flag}); }
    };

    class PackCfg : public RegisterBase<uint16_t>
    {
    private:

    public:
        PackCfg() : RegisterBase(Register::PackCfg) {};
        [[nodiscard]] auto NCELLS() const -> uint8_t { return static_cast<uint8_t>(bits::get(value, {0b1111, 0})); }
        [[nodiscard]] auto BALCFG() const -> uint8_t { return static_cast<uint8_t>(bits::get(value, {0b111, 5})); }
        [[nodiscard]] auto CxEn() const -> bool { return (value & (1 << 8)) > 0; }
        [[nodiscard]] auto BtEn() const -> bool { return (value & (1 << 9)) > 0; }
        [[nodiscard]] auto ChEn() const -> bool { return (value & (1 << 10)) > 0; }
        [[nodiscard]] auto TdEn() const -> bool { return (value & (1 << 11)) > 0; }
        [[nodiscard]] auto A1En() const -> bool { return (value & (1 << 12)) > 0; }
        [[nodiscard]] auto A2En() const -> bool { return (value & (1 << 13)) > 0; }
        [[nodiscard]] auto FGT() const -> bool { return (value & (1 << 15)) > 0; }
        void set_NCELLS(const uint8_t cells) { value = bits::set(this->value, {0b1111, 0, cells}); }
        void set_BALCFG(const uint8_t config) { value = bits::set(this->value, {0b111, 5, config}); }
    };

    using Bus::Data_t;
    template<class bus_controller>
    class Controller
    {
    private:
        // *** properties ***
        bus_controller mybus;
        Data_t i2c_data{0};
        mV voltage_battery{0};
        mA current_battery{0};
        std::array<mV, 2> voltage_cell{0};
        std::array<mAh, 2> capacity{0};
        percent soc{0};
        uint16_t cycles{0};
        percent age{0};
        Ohm esr{0};
        degC temperature{0};
        seconds time2empty{0};
        seconds time2full{0};

    public:
        // *** Constructor ***
        /**
         * @brief Constructor for balancer controller.
         * @param bus_used The reference to the used bus peripheral.
         */
        Controller() = delete;
        explicit Controller(bus_controller bus_used)
        : mybus{bus_used}
        {
        };

        // *** Methods ***
        auto initialize() -> bool;
        [[nodiscard]] auto get_battery_voltage() const -> mV;
        [[nodiscard]] auto get_battery_current() const -> mA;
        [[nodiscard]] auto get_cell_voltage(uint8_t cell) const -> mV;
        [[nodiscard]] auto get_total_capacity() const -> mAh;
        [[nodiscard]] auto get_remaining_capacity() const -> mAh;
        [[nodiscard]] auto get_number_cycles() const -> uint16_t;
        [[nodiscard]] auto get_age() const -> percent;
        [[nodiscard]] auto get_ESR() const -> Ohm;
        [[nodiscard]] auto get_temperature() const -> degC;
        [[nodiscard]] auto get_SOC() const -> percent;
        [[nodiscard]] auto get_TTE() const -> seconds;
        [[nodiscard]] auto get_TTF() const -> seconds;
        [[nodiscard]] auto read_register(uint16_t reg) -> std::optional<uint16_t>;
        bool write_register(uint16_t reg, uint16_t data);
        bool read_battery_voltage();
        bool read_battery_current();
        bool read_battery_current_avg();
        bool read_cell_voltage();
        bool read_cell_voltage_avg();
        bool read_remaining_capacity();
        bool read_soc();
        bool read_TTE();
        bool read_TTF();

        /**
         * @brief Read a register and set the value of the register
         * 
         * @tparam T The underlying data type of the register.
         * @param target The register to set the value of
         * @return Returns true if the read was successful
         */
        template<typename T>
        bool read(RegisterBase<T>& target)
        {
            // Read the register
            auto result = read_register(target.address);
            // Check if the read was successful
            if (!result)
                return false;
            // Set the value of the register
            target.value = result.value();
            return true;
        }

        /**
         * @brief Write a register
         * 
         * @tparam T The underlying data type of the register.
         * @param target The register to write
         * @return Returns true if the write was successful
         */
        template<typename T>
        bool write(RegisterBase<T>& target)
        {
            // Read the register
            return write_register(target.address, target.value);
        }
    };
}; // namespace MAX17205
#endif
