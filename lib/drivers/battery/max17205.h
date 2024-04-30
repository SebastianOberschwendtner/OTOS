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

#ifndef MAX17205_H_
#define MAX17205_H_

/* === includes === */
#include "drivers.h"
#include <optional>
#include <ratio>
#include <misc/bits.h>
#include <misc/types.h>
#include <misc/units.h>

/* === Command codes === */
namespace max17205
{

    /* === Constants === */
    constexpr uint8_t i2c_address_low = 0x6C;  /* For accessing registers 0x000 - 0x0FF */
    constexpr uint8_t i2c_address_high = 0x16; /* For accessing registers 0x180 - 0x1FF */
    constexpr uint8_t R_sense_mOhm = 5;        /* [mΩ] The value of the current sense resistor */

    /* === Define units and literal operators === */
    using percent = OTOS::Unit<std::ratio<1U, 256U>, uint16_t>;             /* [%], Resolution is 1/256 [%] */
    using mAh = OTOS::Unit<std::ratio<5U, R_sense_mOhm>, uint16_t>;         /* [mAh], Resolution is 5 / R_sense [uVh/mOhm]  */
    using mA = OTOS::Unit<std::ratio<1000, 640LL * R_sense_mOhm>, int16_t>; /* [mA], Resolution is 1000 / (640 * R_sense) [uV/mOhm] */
    using mV = OTOS::Unit<std::ratio<10U, 128U>, uint16_t>;                 /* [mV], Resolution is 10/128 [mV] = 78.125 [uV] */
    using degC = OTOS::Unit<std::ratio<1U, 256U>, int16_t>;                 /* [°C], Resolution is 1/256 [°C] */
    using Ohm = OTOS::Unit<std::ratio<1U, 4096U>, uint16_t>;                /* [Ohm], Resolution is 1/4096 [Ohm] */
    using seconds = OTOS::Unit<std::ratio<5625U, 1000U>, uint32_t>;         /* [s], Resolution is 5625/1000 [s] = 5.625 [s] */

    /* === Bits === */
    /* *** nPackCfg *** */
    constexpr uint16_t BALCFG_0 = (1 << 5);
    constexpr uint16_t BALCFG_1 = (1 << 6);
    constexpr uint16_t BALCFG_2 = (1 << 7);
    constexpr uint16_t ChEn = (1 << 10);
    constexpr uint16_t TdEn = (1 << 11);

    namespace registers /* Register addresses */
    {
        /* === Registers === */
        constexpr uint16_t SAlrtTh = 0x003;
        constexpr uint16_t Cap_Remaining = 0x005;
        constexpr uint16_t SOC = 0x006;
        constexpr uint16_t TTE = 0x011;
        constexpr uint16_t Config = 0x01D;
        constexpr uint16_t TTF = 0x020;
        constexpr uint16_t Current = 0x00A;
        constexpr uint16_t Avg_Current = 0x00B;
        constexpr uint16_t PackCfg = 0x0BD;
        constexpr uint16_t Avg_Cell_4 = 0x0D1;
        constexpr uint16_t Avg_Cell_3 = 0x0D2;
        constexpr uint16_t Avg_Cell_2 = 0x0D3;
        constexpr uint16_t Avg_Cell_1 = 0x0D4;
        constexpr uint16_t Cell_4 = 0x0D5;
        constexpr uint16_t Cell_3 = 0x0D6;
        constexpr uint16_t Cell_2 = 0x0D7;
        constexpr uint16_t Cell_1 = 0x0D8;
        constexpr uint16_t Batt_Register = 0x0DA;
        constexpr uint16_t nConfig = 0x1B0;
        constexpr uint16_t nPackCfg = 0x1B5;
    }; // namespace registers

    /**
     * @brief Base class for all registers
     *
     * @tparam register_type The type of the register value
     */
    template <typename register_type>
    struct RegisterBase
    {
        /* === Constructors === */
        RegisterBase() = delete;
        /**
         * @brief Construct a new Register Base object
         *
         * @param address The address of the register
         */
        RegisterBase(uint16_t address) : address(address), value(0){};

        /* === Properties === */
        const uint16_t address;
        register_type value;
    };

    class SAlrtTh : public RegisterBase<uint16_t>
    {
      public:
        /* === Constructors === */
        SAlrtTh() : RegisterBase(registers::SAlrtTh){};

        /* === Setters === */
        void set_SMIN(const uint8_t soc) { this->value = bits::set(this->value, {0xFF, 0, soc}); }
        void set_SMAX(const uint8_t soc) { this->value = bits::set(this->value, {0xFF, 8, soc}); }

        /* === Getters === */
        [[nodiscard]] auto SMIN() const -> uint8_t { return static_cast<uint8_t>(bits::get(value, {0xFF, 0})); }
        [[nodiscard]] auto SMAX() const -> uint8_t { return static_cast<uint8_t>(bits::get(value, {0xFF, 8})); }
    };

    class Config : public RegisterBase<uint16_t>
    {
      public:
        /* === Constructors === */
        Config() : RegisterBase(registers::Config){};

        /* === Setters === */
        void set_Aen(const bool flag) { this->value = bits::set(this->value, {1, 2, flag}); }
        void set_ALRTp(const bool flag) { this->value = bits::set(this->value, {1, 11, flag}); }

        /* === Getters === */
        [[nodiscard]] auto Aen() const -> bool { return (value & (1 << 2)) > 0; }
        [[nodiscard]] auto ALRTp() const -> bool { return (value & (1 << 11)) > 0; }
    };

    class PackCfg : public RegisterBase<uint16_t>
    {
      public:
        /* === Constructors === */
        PackCfg() : RegisterBase(registers::PackCfg){};

        /* === Setters === */
        void set_NCELLS(const uint8_t cells) { value = bits::set(this->value, {0b1111, 0, cells}); }
        void set_BALCFG(const uint8_t config) { value = bits::set(this->value, {0b111, 5, config}); }

        /* === Getters === */
        [[nodiscard]] auto NCELLS() const -> uint8_t { return static_cast<uint8_t>(bits::get(value, {0b1111, 0})); }
        [[nodiscard]] auto BALCFG() const -> uint8_t { return static_cast<uint8_t>(bits::get(value, {0b111, 5})); }
        [[nodiscard]] auto CxEn() const -> bool { return (value & (1 << 8)) > 0; }
        [[nodiscard]] auto BtEn() const -> bool { return (value & (1 << 9)) > 0; }
        [[nodiscard]] auto ChEn() const -> bool { return (value & (1 << 10)) > 0; }
        [[nodiscard]] auto TdEn() const -> bool { return (value & (1 << 11)) > 0; }
        [[nodiscard]] auto A1En() const -> bool { return (value & (1 << 12)) > 0; }
        [[nodiscard]] auto A2En() const -> bool { return (value & (1 << 13)) > 0; }
        [[nodiscard]] auto FGT() const -> bool { return (value & (1 << 15)) > 0; }
    };

    using bus::Data_t;
    template <class bus_controller>
    class Controller
    {
      public:
        /* === Constructors === */
        /**
         * @brief Constructor for balancer controller.
         * @param bus_used The reference to the used bus peripheral.
         */
        Controller() = delete;
        explicit Controller(bus_controller bus_used)
            : mybus{bus_used} {};

        /* === Setters === */
        /* === Getters === */
        /**
         * @brief Get the age of the battery pack.
         * @return Returns the age as a percentage of the original capacity.
         */
        [[nodiscard]] auto get_age() const -> percent;

        /**
         * @brief Get the current of the battery pack.
         * @return Returns the current in [mA].
         */
        [[nodiscard]] auto get_battery_current() const -> mA;

        /**
         * @brief Get the voltage of the battery pack.
         * @return Returns the voltage in [mV].
         */
        [[nodiscard]] auto get_battery_voltage() const -> mV;

        /**
         * @brief Get the voltage of a specific cell.
         * @param cell The number of the cell (1 or 2)
         * @return Returns the voltage in [mV].
         */
        [[nodiscard]] auto get_cell_voltage(uint8_t cell) const -> mV;

        /**
         * @brief Get the internal series resistance of the battery pack.
         * @return Returns the resistance in [mΩ].
         */
        [[nodiscard]] auto get_ESR() const -> Ohm;

        /**
         * @brief Get the number of cycles of the battery pack.
         * @return Returns the number of cycles.
         */
        [[nodiscard]] auto get_number_cycles() const -> uint16_t;

        /**
         * @brief Get the remaining capacity of the battery pack.
         * @return Returns the capacity in [mAh].
         */
        [[nodiscard]] auto get_remaining_capacity() const -> mAh;

        /**
         * @brief Get the SOC of the battery pack.
         * @return Returns the SOC in [%].
         */
        [[nodiscard]] auto get_SOC() const -> percent;

        /**
         * @brief Get the temperature of the battery pack.
         * @return Returns the voltage in [mV].
         */
        [[nodiscard]] auto get_temperature() const -> degC;

        /**
         * @brief Get the total capacity of the battery pack.
         * @return Returns the capacity in [mAh].
         */
        [[nodiscard]] auto get_total_capacity() const -> mAh;


        /**
         * @brief Get the time until the battery pack is empty.
         * @return Returns the time in [s].
         */
        [[nodiscard]] auto get_TTE() const -> seconds;

        /**
         * @brief Get the time until the battery pack is fully charged.
         * @return Returns the time in [s].
         */
        [[nodiscard]] auto get_TTF() const -> seconds;

        /* === Methods === */
        /**
        * @brief Initialize the balancer for the application.
        * The chip does not require any initialization.
        * @return Returns True when the balancer was initialized correctly.
        */
        auto initialize() -> bool;

        /**
         * @brief Read the pack voltage from the balancer and convert to mV.
         * @return Returns True when the pack voltage was read successfully.
         */
        auto read_battery_voltage() -> bool;

        /**
         * @brief Read the pack current from the balancer and convert to mA.
         * @return Returns True when the pack current was read successfully.
         */
        auto read_battery_current() -> bool;

        /**
         * @brief Read the average pack current from the balancer and convert to mA.
         * @return Returns True when the pack current was read successfully.
         */
        auto read_battery_current_avg() -> bool;

        /**
         * @brief Read the cell voltages of the balancer
         * @return Returns true when the cell voltages are updated successfully.
         */
        auto read_cell_voltage() -> bool;

        /**
         * @brief Read the avg cell voltages of the balancer
         * @return Returns true when the cell voltages are updated successfully.
         */
        auto read_cell_voltage_avg() -> bool;

        /**
         * @brief Read the remaining capacity from the balancer and convert to mAh.
         * @return Returns true when the capacity is updated successfully.
         */
        auto read_remaining_capacity() -> bool;

        /**
         * @brief Read the SOC from the balancer and convert to %.
         * @return Returns true when the soc is updated successfully.
         */
        auto read_soc() -> bool;

        /**
         * @brief Read the time until the battery is empty and convert to [s].
         * @return Returns true when the TTE is updated successfully.
         */
        auto read_TTE() -> bool;

        /**
         * @brief Read the time until the battery is full and convert to [s].
         * @return Returns true when the TTF is updated successfully.
         */
        auto read_TTF() -> bool;

        /**
         * @brief Read a register of the balancer.
         * The balancer sends LSB first. The data is stored already
         * sorted to LSB last in the i2c_data struct.
         * @param reg The register to read from.
         * @return Returns true and the value when the register was read successfully.
         */
        [[nodiscard]] auto read_register(uint16_t reg) -> std::optional<uint16_t>;

        /**
         * @brief Read a register and set the value of the register
         *
         * @tparam T The underlying data type of the register.
         * @param target The register to set the value of
         * @return Returns true if the read was successful
         */
        template <typename T>
        auto read(RegisterBase<T> &target) -> bool
        {
            /* Read the register */
            auto result = read_register(target.address);
            /* Check if the read was successful */
            if (!result)
                return false;
            /* Set the value of the register */
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
        template <typename T>
        auto write(RegisterBase<T> &target) -> bool
        {
            /* Read the register */
            return write_register(target.address, target.value);
        }

        /**
         * @brief Write a register of the balancer.
         * The balancer expects LSB first.
         * @param reg The register to write to.
         * @param data The data for the register
         * @return Returns True when the data was sent successfully.
         */
        auto write_register(uint16_t reg, uint16_t data) -> bool;

      private:
        /* === Properties === */
        bus_controller mybus;              /**< The bus controller used for communication */
        Data_t i2c_data{0};                /**< The data buffer for the I2C communication */
        mV voltage_battery{0};             /**< [mV] The battery voltage */
        mA current_battery{0};             /**< [mA] The battery current */
        std::array<mV, 2> voltage_cell{0}; /**< [mV] The cell voltages */
        std::array<mAh, 2> capacity{0};    /**< [mAh] The cell capacities */
        percent soc{0};                    /**< [%] The state of charge */
        uint16_t cycles{0};                /**< The number of charge cycles */
        percent age{0};                    /**< [%] The age of the battery */
        Ohm esr{0};                        /**< [Ohm] The equivalent series resistance */
        degC temperature{0};               /**< [°C] The temperature of the battery */
        seconds time2empty{0};             /**< [s] The time to empty */
        seconds time2full{0};              /**< [s] The time to full */
    };
};     // namespace MAX17205
#endif // MAX17205_H_
