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

#ifndef TPS65987_H_
#define TPS65987_H_

/* === includes === */
#include "drivers.h"
#include <array>
#include <utility>
#include <misc/bits.h>
#include <misc/types.h>

/* === Command codes === */
namespace tps65987
{

    /* === Constants === */
    constexpr uint8_t i2c_address = 0x40;

    /* === Bits === */
    constexpr uint8_t PlugDetails_0 = (1 << 0);
    constexpr uint8_t PlugDetails_1 = (1 << 1);
    constexpr uint8_t CCPullUp_0 = (1 << 2);
    constexpr uint8_t CCPullUp_1 = (1 << 3);
    constexpr uint8_t PortType_0 = (1 << 4);
    constexpr uint8_t PortType_1 = (1 << 5);
    constexpr uint8_t PresentRole = (1 << 6);

    /* === Registers === */
    struct reg_t
    {
        uint8_t address;
        uint8_t length;
    };

    namespace registers
    {
        constexpr reg_t Mode = {0x03, 4};
        constexpr reg_t Cmd1 = {0x08, 4};
        constexpr reg_t Data1 = {0x09, 64};
        constexpr reg_t Status = {0x1A, 8};
        constexpr reg_t Power_Path_Status = {0x26, 8};
        constexpr reg_t Global_SysConfig = {0x27, 14};
        constexpr reg_t Port_Config = {0x28, 8};
        constexpr reg_t Port_Ctrl = {0x29, 4};
        constexpr reg_t RX_Source_Cap = {0x30, 29};
        constexpr reg_t RX_Sink_Cap = {0x31, 29};
        constexpr reg_t TX_Source_Cap = {0x32, 64};
        constexpr reg_t TX_Sink_Cap = {0x33, 57};
        constexpr reg_t Active_PDO = {0x34, 6};
        constexpr reg_t Active_RDO = {0x35, 4};
        constexpr reg_t Power_Status = {0x3F, 2};
        constexpr reg_t PD_Status = {0x40, 4};
    }; // namespace registers

    /* === Modes === */
    enum class Mode : uint8_t
    {
        BOOT = 0,
        PTCH,
        APP,
        OTHER
    };

    /* === Power contracts === */
    struct Contract
    {
        uint8_t role;
        uint8_t USB_type;
        uint16_t voltage;
        uint16_t current;
    };

    /**
     * @brief PDO (Power Data Object) for USB-PD.
     * The conversion are taken from the USB-PD specification.
     */
    class PDO
    {
      public:
        /* === Enum with PDO types === */
        enum Type
        {
            Fixed_Supply = 0,
            Battery = 1,
            Variable_Supply = 2,
            APDO = 3 /* Augmented Power Data Object */
        };

        /* === Constructors === */
        PDO() = default;
        explicit PDO(uint32_t data) : data(data) {}
        PDO(const PDO &other) = default;
        PDO(PDO &&other) = default;
        auto operator=(const PDO &other) -> PDO & = default;
        auto operator=(PDO &&other) -> PDO & = default;
        auto operator=(uint32_t data) -> PDO &
        {
            this->data = data;
            return *this;
        }
        ~PDO() = default;

        /* === Getters === */
        /**
         * @brief Get the raw value of the PDO.
         *
         * @return The raw PDO in bits 0-31.
         */
        [[nodiscard]] auto get_data() const -> uint32_t;

        /**
         * @brief Returns the maximum current indicated by the PDO.
         *
         * @return The maximum current in [mA].
         */
        [[nodiscard]] auto current() const -> uint16_t;

        /**
         * @brief Returns the fixed voltage indicated by the PDO.
         *
         * @return The voltage in [mV].
         */
        [[nodiscard]] auto voltage() const -> uint16_t;

        /**
         * @brief Check the type of PDO object.
         *
         * @return Returns the type of the object.
         */
        [[nodiscard]] auto type() const -> Type;

        /* === Setters === */
        /**
         * @brief Set the current of the PDO.
         *
         * @param current The current in [mA].
         */
        void set_current(uint16_t current);

        /**
         * @brief Set the voltage of the PDO.
         *
         * @param voltage The voltage in [mV].
         */
        void set_voltage(uint16_t voltage);

      private:
        uint32_t data{0}; /**< The raw data of the PDO */
    };

    /* Type to pair the PDO with the power path selection
     * for setting the source and sink capabilities
     */
    using Capability = std::pair<tps65987::PDO, uint8_t>;

    /**
     * @brief Status bit field for non-interrupt events.
     * @details Read-only register.
     */
    class Status : public reg_t
    {
      public:
        Status() : reg_t{registers::Status} {}

        /* === Get all the options === */
        [[nodiscard]] auto &get_data() { return this->data; }
        [[nodiscard]] auto PlugPresent() const -> bool{ return (this->data.at(0) & (1 << 0)) != 0; }
        [[nodiscard]] auto ConnState() const -> uint8_t { return bits::get(this->data.at(0), {0b111, 1}); }
        [[nodiscard]] auto PortRole() const -> bool { return (this->data.at(0) & (1 << 5)) != 0; }
        [[nodiscard]] auto VbusStatus() const -> uint8_t { return bits::get(this->data.at(2), {0b11, 4}); }

      private:
        std::array<uint8_t, registers::Status.length> data{0};
    };

    /**
     * @brief Status bit field for power path.
     * @details Read-only register.
     */
    class PowerPathStatus : public reg_t
    {
      public:
        PowerPathStatus() : reg_t{registers::Power_Path_Status} {}

        /* === Get all the options === */
        [[nodiscard]] auto &get_data() { return this->data; }
        [[nodiscard]] auto PP1_CABLEswitch() const -> uint8_t { return bits::get(this->data.at(0), {0b11, 0}); }
        [[nodiscard]] auto PP2_CABLEswitch() const -> uint8_t { return bits::get(this->data.at(0), {0b11, 2}); }
        [[nodiscard]] auto PP1switch() const -> uint8_t { return bits::get(this->data.at(0), {0b11, 6}); }
        [[nodiscard]] auto PP2switch() const -> uint8_t { return bits::get(this->data.at(1), {0b111, 1}); }

      private:
        std::array<uint8_t, registers::Power_Path_Status.length> data{0};
    };

    /**
     * @brief Configuration bits that define hardware that is common to all ports
     *  and in most cases will not change in normal operation or will not
     *  require immediate action if changed. Any modifications to this
     *  register will cause a port disconnect and reconnect with the new
     *  settings.
     * @details Initialized by Application Customization.
     */
    class GlobalConfiguration : public reg_t
    {
      public:
        GlobalConfiguration() : reg_t{registers::Global_SysConfig} {}

        /* === Set all the options === */
        inline void set_PP1_CABLEconfig(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b11, 0, value}); }
        inline void set_PP2_CABLEconfig(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b11, 2, value}); }
        inline void set_PP1config(const uint8_t value) { this->data[2] = bits::set(this->data.at(2), {0b111, 0, value}); }
        inline void set_PP2config(const uint8_t value) { this->data[2] = bits::set(this->data.at(2), {0b111, 3, value}); }

        /* === Get all the options === */
        [[nodiscard]] auto &get_data() { return this->data; }
        [[nodiscard]] auto PP1_CABLEconfig() const -> uint8_t { return bits::get(this->data.at(0), {0b11, 0}); }
        [[nodiscard]] auto PP2_CABLEconfig() const -> uint8_t { return bits::get(this->data.at(0), {0b11, 2}); }
        [[nodiscard]] auto PP1config() const -> uint8_t { return bits::get(this->data.at(2), {0b111, 0}); }
        [[nodiscard]] auto PP2config() const -> uint8_t { return bits::get(this->data.at(2), {0b111, 3}); }

      private:
        std::array<uint8_t, registers::Global_SysConfig.length> data{0};
    };

    /**
     * @brief Configuration bits that define hardware that is specific to a the
     *  respective port and in most cases will not change in normal
     *  operation or will not require immediate action if changed. Any
     *  modifications to this register will cause a port disconnect and
     *  reconnect with the new settings.
     * @details Initialized by Application Customization.
     */
    class PortConfiguration : public reg_t
    {
      public:
        PortConfiguration() : reg_t{registers::Port_Config} {}

        /* === Set all the options === */
        inline void set_TypeCStateMachine(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b11, 0, value}); }
        inline void set_ReceptacleType(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b111, 3, value}); }
        inline void set_VCONNsupported(const uint8_t value) { this->data[1] = bits::set(this->data.at(1), {0b11, 3, value}); }

        /* === Get all the options === */
        [[nodiscard]] auto &get_data() { return this->data; }
        [[nodiscard]] auto TypeCStateMachine() const -> uint8_t { return bits::get(this->data.at(0), {0b11, 0}); }
        [[nodiscard]] auto ReceptacleType() const -> uint8_t { return bits::get(this->data.at(0), {0b111, 3}); }
        [[nodiscard]] auto VCONNsupported() const -> uint8_t { return bits::get(this->data.at(1), {0b11, 3}); }

      private:
        std::array<uint8_t, registers::Port_Config.length> data{0};
    };

    /**
     * @brief Configuration bits affecting system policy. These bits may change
     *  during normal operation and are used for controlling the respective
     *  port. The PD Controller will not take immediate action upon writing.
     *  Changes made to this register will take effect the next time the
     *  appropriate policy is invoked.
     * @details Initialized by Application Customization.
     */
    class PortControl : public reg_t
    {
      public:
        PortControl() : reg_t{registers::Port_Ctrl} {}

        /* === Set all the options === */
        inline void set_TypeCCurrent(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b11, 0, value}); }
        inline void set_ChargerAdvertiseEnable(const uint8_t value) { this->data[3] = bits::set(this->data.at(3), {0b111, 2, value}); }
        inline void set_ChargerDetectEnable(const uint8_t value) { this->data[3] = bits::set(this->data.at(3), {0b11, 6, value}); }

        /* === Get all the options === */
        [[nodiscard]] auto &get_data() { return this->data; }
        [[nodiscard]] auto TypeCCurrent() const -> uint8_t { return bits::get(this->data.at(0), {0b11, 0}); }
        [[nodiscard]] auto ChargerAdvertiseEnable() const -> uint8_t { return bits::get(this->data.at(3), {0b111, 2}); }
        [[nodiscard]] auto ChargerDetectEnable() const -> uint8_t { return bits::get(this->data.at(3), {0b11, 6}); }

      private:
        std::array<uint8_t, registers::Port_Ctrl.length> data{0};
    };

    using bus::Data_t;
    template <class bus_controller>
    class Controller
    {
      public:
        /* === Constructor === */
        /**
         * @brief Constructor for PD controller.
         * @param bus_used The reference to the used bus peripheral.
         */
        explicit Controller(bus_controller bus_used) : mybus{bus_used} {}

        /* === Getters === */
        auto get_mode() const -> Mode { return this->mode_active; }
        auto get_active_command() const -> char * { return &this->cmd_active[0]; }
        auto get_active_contract() const -> Contract { return this->contract_active; }

        /* === Methods === */
        /**
         * @brief Initialize the PD IC based on its current mode.
         * @return Returns True when the IC was initialized successfully.
         */
        auto initialize() -> bool;

        /**
         * @brief Read a register from the TPS65987 and directly
         *        copy the data to the register class.
         *
         * @tparam Treg The register class.
         * @param reg The register object to copy the data to.
         * @return Returns true if the register is read successfully.
         */
        template <class Treg>
        auto read(Treg &reg) -> bool
        {
            /* Read the register */
            if (!this->read_register({reg.address, reg.length}))
                return false;

            /* Copy the data to the register in reverse order
             * because the i2c controller reads the data in reverse order.
             */
            std::copy(
                this->buffer_data.rend() - reg.length - 1,
                this->buffer_data.rend() - 1,
                reg.get_data().begin());

            return true;
        }

        /**
         * @brief Read the currently active command and update the
         * active command property.
         * @return Returns True when the command register was read successfully.
         */
        auto read_active_command() -> bool;

        /**
         * @brief Read the active PDO from the PD Controller.
         *
         * @return True and the active PDO when read successfully.
         */
        auto read_active_pdo() -> std::optional<PDO>;

        /**
         * @brief Read the active mode of the controller.
         * @return Returns True when the mode was read successfully.
         */
        auto read_mode() -> bool;

        /**
         * @brief Read the current PD status from the PD Controller.
         * @return Returns True when the status was read successfully.
         */
        auto read_PD_status() -> bool;

        /**
         * @brief Read a register of the PD controller.
         * The function takes the specific length of the register into account.
         * @param reg The register to read from.
         * @return Returns True when the register was read successfully.
         */
        auto read_register(reg_t reg) -> bool;

        /**
         * @brief Read the status register [0x1A] from the PD Controller.
         * @return Returns True and the value of the lower 4 bytes of the status when read successfully.
         */
        auto read_status() -> std::optional<uint32_t>;

        /**
         * @brief Read a TX source PDO from the PD Controller.
         *
         * @param pdo_number The PDO number to read.
         * @return Returns True and the PDO when read successfully and when the PDO number is valid.
         */
        auto read_TX_sink_pdo(uint8_t pdo_number) -> std::optional<PDO>;

        /**
         * @brief Update the tx buffer with the given capability.
         *
         * A capability is a PDO with the corresponding power path configuration.
         * Make sure to delete the tx buffer before calling this function!
         *
         * @param cap The capability to update the tx buffer with.
         */
        void register_TX_source_capability(const Capability &cap);

        /**
         * @brief Write a register to the TPS65987.
         *
         * @tparam Treg The register class.
         * @param reg The register object to write.
         * @return Returns true if the register is written successfully.
         */
        template <class Treg>
        auto write(Treg &reg) -> bool
        {
            /* Copy the data to the buffer */
            std::copy(
                reg.get_data().cbegin(),
                reg.get_data().cend(),
                this->buffer_data.begin() + 2);

            /* Write the register */
            return this->write_register({reg.address, reg.length});
        }

        /**
         * @brief Write a command to the command register
         * The Data1 register has to be written first, when the
         * command expects data!
         * @param cmd The command string => has to have 4 characters.
         * @return Returns True when the command was send successfully.
         */
        auto write_command(const char *cmd) -> bool;

        /**
         * @brief Write a register of the PD controller.
         * The function takes the specific length of the register into account.
         * @param reg The register to write to.
         * @return Returns True when the register was written successfully.
         */
        auto write_register(reg_t reg) -> bool;

        /* === Properties === */
        std::array<uint8_t, 66> buffer_data = {0}; /* NOLINT */

      private:
        /* === Properties === */
        bus_controller mybus; /**< The used bus controller. */
        Data_t i2c_data = {0}; /**< The data buffer for the i2c communication. */
        std::array<char, 6> buffer_cmd = {0x08, 0x04, 0, 0, 0, 0}; /* NOLINT */
        Mode mode_active = Mode::BOOT; /**< The active mode of the TPS65987. */
        mutable char cmd_active[4] = {0}; /* NOLINT */
        Contract contract_active = {0, 0, 0, 0}; /**< The active PDO contract. */
    };
};     // namespace TPS65987
#endif // TPS65987_H_
