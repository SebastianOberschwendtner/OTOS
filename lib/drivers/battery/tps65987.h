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

#ifndef TPS65987_H_
#define TPS65987_H_

// === includes ===
// #include <stdint.h>
#include <array>
#include "drivers.h"
#include "misc/bits.h"

// === Command codes ===
namespace TPS65987
{

    // === Constants ===
    constexpr uint8_t i2c_address = 0x40;

    // === Bits ===
    constexpr uint8_t PlugDetails_0 = (1 << 0);
    constexpr uint8_t PlugDetails_1 = (1 << 1);
    constexpr uint8_t CCPullUp_0 = (1 << 2);
    constexpr uint8_t CCPullUp_1 = (1 << 3);
    constexpr uint8_t PortType_0 = (1 << 4);
    constexpr uint8_t PortType_1 = (1 << 5);
    constexpr uint8_t PresentRole = (1 << 6);

    // === Registers ===
    struct reg_t
    {
        uint8_t address;
        uint8_t length;
    };

    namespace Register
    {
        constexpr reg_t Mode = {0x03, 4};
        constexpr reg_t Cmd1 = {0x08, 4};
        constexpr reg_t Data1 = {0x09, 64};
        constexpr reg_t Status = {0x1A, 8};
        constexpr reg_t Power_Path_Status = {0x26, 8};
        constexpr reg_t Global_SysConfig = {0x27, 14};
        constexpr reg_t Port_Config = {0x28, 8};
        constexpr reg_t Port_Ctrl = {0x29, 4};
        constexpr reg_t TX_Source_Cap = {0x32, 64};
        constexpr reg_t TX_Sink_Cap = {0x33, 57};
        constexpr reg_t Active_PDO = {0x34, 6};
        constexpr reg_t Active_RDO = {0x35, 4};
        constexpr reg_t PD_Status = {0x40, 4};
    }; // namespace Register

    // === Modes ===
    enum class Mode : uint8_t
    {
        BOOT = 0,
        PTCH,
        APP,
        OTHER
    };

    // === Power contracts ===
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
    private:
        uint32_t data{0}; // The raw data of the PDO

    public:
        // ** Enum with PDO types ***
        enum Type
        {
            Fixed_Supply = 0,
            Battery = 1,
            Variable_Supply = 2,
            APDO = 3 // Augmented Power Data Object
        };

        // *** Constructors ***
        PDO() = default;
        explicit PDO(uint32_t data) : data(data) {}
        PDO(const PDO &other) = default;
        PDO(PDO &&other) = default;
        PDO &operator=(const PDO &other) = default;
        PDO &operator=(PDO &&other) = default;
        PDO &operator=(uint32_t data)
        {
            this->data = data;
            return *this;
        }
        ~PDO() = default;

        // *** Methods ***
        [[nodiscard]] uint32_t get_data() const;
        [[nodiscard]] uint16_t voltage() const;
        [[nodiscard]] uint16_t current() const;
        [[nodiscard]] Type type() const;
        void set_voltage(uint16_t voltage);
        void set_current(uint16_t current);
    };

    /**
     * @brief Status bit field for non-interrupt events.
     * @details Read-only register.
     */
    class Status : public reg_t
    {
    private:
        std::array<uint8_t, Register::Status.length> data{0};
    
    public:
        Status() : reg_t{Register::Status} {}

        // *** Get all the options ***
        [[nodiscard]] auto& get_data() { return this->data; }
        [[nodiscard]] inline bool PlugPresent() const { return (this->data.at(0) & (1 << 0)) != 0; }
        [[nodiscard]] inline uint8_t ConnState() const { return bits::get(this->data.at(0), {0b111, 1}); }
        [[nodiscard]] inline bool PortRole() const { return (this->data.at(0) & (1 << 5)) != 0; }
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
    private:
        std::array<uint8_t, Register::Global_SysConfig.length> data{0};

    public:
        GlobalConfiguration() : reg_t{Register::Global_SysConfig} {}

        // *** Get all the options ***
        [[nodiscard]] auto& get_data() { return this->data; }
        [[nodiscard]] inline uint8_t PP1_CABLEconfig() const { return bits::get(this->data.at(0), {0b11, 0}); }
        [[nodiscard]] inline uint8_t PP2_CABLEconfig() const { return bits::get(this->data.at(0), {0b11, 2}); }
        [[nodiscard]] inline uint8_t PP1config() const { return bits::get(this->data.at(2), {0b111, 0}); }
        [[nodiscard]] inline uint8_t PP2config() const { return bits::get(this->data.at(2), {0b111, 3}); }

        // *** Set all the options ***
        inline void set_PP1_CABLEconfig(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b11, 0, value}); }
        inline void set_PP2_CABLEconfig(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b11, 2, value}); }
        inline void set_PP1config(const uint8_t value) { this->data[2] = bits::set(this->data.at(2), {0b111, 0, value}); }
        inline void set_PP2config(const uint8_t value) { this->data[2] = bits::set(this->data.at(2), {0b111, 3, value}); }
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
    private:
        std::array<uint8_t, Register::Port_Config.length> data{0};
    
    public:
        PortConfiguration() : reg_t{Register::Port_Config} {}

        // *** Get all the options ***
        [[nodiscard]] auto& get_data() { return this->data; }
        [[nodiscard]] inline uint8_t TypeCStateMachine() const { return bits::get(this->data.at(0), {0b11, 0}); }
        [[nodiscard]] inline uint8_t ReceptacleType() const { return bits::get(this->data.at(0), {0b111, 3}); }
        [[nodiscard]] inline uint8_t VCONNsupported() const { return bits::get(this->data.at(1), {0b11, 3}); }

        // *** Set all the options ***
        inline void set_TypeCStateMachine(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b11, 0, value}); }
        inline void set_ReceptacleType(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b111, 3, value}); }
        inline void set_VCONNsupported(const uint8_t value) { this->data[1] = bits::set(this->data.at(1), {0b11, 3, value}); }

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
    private:
        std::array<uint8_t, Register::Port_Ctrl.length> data{0};

    public:
        PortControl() : reg_t{Register::Port_Ctrl} {}

        // *** Get all the options ***
        [[nodiscard]] auto& get_data() { return this->data; }
        [[nodiscard]] inline uint8_t TypeCCurrent() const { return bits::get(this->data.at(0), {0b11, 0}); }

        // *** Set all the options ***
        inline void set_TypeCCurrent(const uint8_t value) { this->data[0] = bits::set(this->data.at(0), {0b11, 0, value}); }
    };

    using Bus::Data_t;
    template <class bus_controller>
    class Controller
    {
    private:
        // *** properties ***
        bus_controller mybus;
        Data_t i2c_data = {0};
        std::array<char, 6> buffer_cmd = {0x08, 0x04, 0, 0, 0, 0}; // NOLINT
        Mode mode_active = Mode::BOOT;
        mutable char cmd_active[4] = {0}; // NOLINT
        Contract contract_active = {0, 0, 0, 0};

    public:
        std::array<uint8_t, 66> buffer_data = {0}; // NOLINT
        // *** Constructor ***
        /**
         * @brief Constructor for PD controller.
         * @param bus_used The reference to the used bus peripheral.
         */
        explicit Controller(bus_controller bus_used) : mybus{bus_used} {}

        // Properties

        // *** Methods ***
        bool initialize();
        auto get_mode() const -> Mode { return this->mode_active; }
        auto get_active_command() const -> char * { return &this->cmd_active[0]; }
        auto get_active_contract() const -> Contract { return this->contract_active; }
        bool read_register(reg_t reg);
        bool write_register(reg_t reg);
        bool read_active_command();
        bool write_command(const char *cmd);
        bool read_mode();
        bool read_PD_status();
        auto read_status() -> std::optional<uint32_t>;
        auto read_active_pdo() -> std::optional<PDO>;
        auto read_TX_sink_pdo(uint8_t pdo_number) -> std::optional<PDO>;

        /**
         * @brief Read a register from the TPS65987 and directly
         *        copy the data to the register class.
         * 
         * @tparam Treg The register class.
         * @param reg The register object to copy the data to.
         * @return Returns true if the register is read successfully.
         */
        template <class Treg>
        bool read(Treg &reg)
        {
            // Read the register
            if (!this->read_register({reg.address, reg.length}))
                return false;

            /* Copy the data to the register in reverse order
             * because the i2c controller reads the data in reverse order.
             */
            std::copy(
                this->buffer_data.rend() - reg.length - 1,
                this->buffer_data.rend() - 1,
                reg.get_data().begin()
            );

            return true;
        }

        /**
         * @brief Write a register to the TPS65987.
         * 
         * @tparam Treg The register class.
         * @param reg The register object to write.
         * @return Returns true if the register is written successfully.
         */
        template <class Treg>
        bool write(Treg &reg)
        {
            // Copy the data to the buffer
            std::copy(
                reg.get_data().cbegin(),
                reg.get_data().cend(),
                this->buffer_data.begin() + 2
            );

            // Write the register
            return this->write_register({reg.address, reg.length});
        }
    };
}; // namespace TPS65987
#endif
