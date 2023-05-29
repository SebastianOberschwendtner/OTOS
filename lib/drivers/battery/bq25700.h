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

#ifndef BQ25700_H_
#define BQ25700_H_

// === includes ===
#include <cstdint>
#include "drivers.h"
#include <misc/bits.h>

// === Command codes ===
namespace BQ25700 {
    // Fixed device values
    constexpr uint8_t i2c_address     = 0x12;
    constexpr uint8_t manufacturer_id = 0x40;
    constexpr uint8_t device_id       = 0x79;

    // Option bits
    // *** Charge_Option_0 ***
    constexpr uint16_t EN_LWPWR             = (1<<15);
    constexpr uint16_t WDTMR_ADJ_1          = (1<<14);
    constexpr uint16_t WDTMR_ADJ_0          = (1<<13);
    constexpr uint16_t IDPM_AUTO_DISABLE    = (1<<12);
    constexpr uint16_t OTG_ON_CHRGOK        = (1<<11);
    constexpr uint16_t EN_OOA               = (1<<10);
    constexpr uint16_t PWM_FREQ             = (1<<9); // Set Switching frequency
    constexpr uint16_t EN_LEARN             = (1<<5);
    constexpr uint16_t IADPT_GAIN           = (1<<4); // IADPT amplifier ratio
    constexpr uint16_t IBAT_GAIN            = (1<<3); // IBAT amplifier ratio
    constexpr uint16_t EN_LDO               = (1<<2);
    constexpr uint16_t EN_IDPM              = (1<<1); // Enable IDPM regulation loop
    constexpr uint16_t CHRG_INHIBIT         = (1<<0);

    // *** Charge_Option_1 ***
    constexpr uint16_t EN_IBAT              = (1<<15); // Enable IBAT output buffer
    constexpr uint16_t EN_PSYS              = (1<<12); // Enable PSYS sensing circuit and output buffer
    constexpr uint16_t RSNS_RAC             = (1<<11); // Set input sense resistor RAC
    constexpr uint16_t RSNS_RSR             = (1<<10); // set charge sense resistor RSR
    constexpr uint16_t PSYS_RATIO           = (1<<9); // Ratio of PSYS output current vs. total input battery power
    constexpr uint16_t CMP_REF              = (1<<8); // Set independent comparator internal reference
    constexpr uint16_t CMP_POL              = (1<<7); // Set independent comparator polarity
    constexpr uint16_t FORCE_LATCHOFF       = (1<<3); // Force power path off
    constexpr uint16_t EN_SHIP_DCHRG        = (1<<1); // Discharge SRN for shipping mode
    constexpr uint16_t AUTO_WAKEUP_EN       = (1<<0);
    // *** Charge_Option_2 ***
    constexpr uint16_t EN_PKPWR_IDPM        = (1<<13); // Enable peak power mode triggered by input current overshoot
    constexpr uint16_t EN_PKPWR_VSYS        = (1<<12); // Enable peak power mode triggered by system voltage undershoot
    constexpr uint16_t PKPWR_OVLD_STAT      = (1<<11); // Indicator at the device is in overloading cycle
    constexpr uint16_t PKPWR_RELAX_STAT     = (1<<10); // Indicator at the device is in relaxation cycle
    constexpr uint16_t EN_EXTILIM           = (1<<7); // Enable ILIM_HIZ pin to set input current limit
    constexpr uint16_t EN_ICHG_IDCHG        = (1<<6);  // Set function of IBAT pin
    constexpr uint16_t Q2_OCP               = (1<<5); // Q2 OCP threshold by sensing Q2 VDS
    constexpr uint16_t ACX_OCP              = (1<<4); // Input current OCP threshold by sensing ACP-ACN
    constexpr uint16_t EN_ACOC              = (1<<3); // Enable input overcurrent protection by sensing the voltage across ACP-ACN
    constexpr uint16_t ACOC_VTH             = (1<<2); // Set Mosfet OCP threshold as percentage of IDPM
    constexpr uint16_t EN_BATOC             = (1<<1); // Enable battery discharge overcurrent protection by sensing the voltage across SRN-SRP
    constexpr uint16_t BATOC_VTH            = (1<<0); // Set battery discharge overcurrent threshold
    // *** Charge_Option_3 ***
    constexpr uint16_t EN_HIZ               = (1<<15); // Device HI-Z mode enable
    constexpr uint16_t RESET_REG            = (1<<14); // Reset registers
    constexpr uint16_t RESET_VINDPM         = (1<<13); // Reset VINDPM threshold
    constexpr uint16_t EN_OTG               = (1<<12); // Enable device OTG mode when EN_OTG pin is HIGH
    constexpr uint16_t EN_ICO_MODE          = (1<<11); // Enable ICO algorithm
    constexpr uint16_t BATFETOFF_HIZ        = (1<<1); // Control BATFET during HI-Z mode
    constexpr uint16_t PSYS_OTG_IDCHG       = (1<<0); // PSYS function during OTG mode
    // *** Prochot_Option_0 ***
    // *** Prochot_Option_1 ***
    // *** ADC_Option ***
    constexpr uint16_t ADC_CONV             = (1<<15); // Coonversion mode of ADC
    constexpr uint16_t ADC_START            = (1<<14); // Start ADC conversion
    constexpr uint16_t ADC_FULLSCALE        = (1<<13); // ADC input voltage range
    constexpr uint16_t EN_ADC_CMPIN         = (1<<7);
    constexpr uint16_t EN_ADC_VBUS          = (1<<6);
    constexpr uint16_t EN_ADC_PSYS          = (1<<5);
    constexpr uint16_t EN_ADC_IIN           = (1<<4);
    constexpr uint16_t EN_ADC_IDCHG         = (1<<3);
    constexpr uint16_t EN_ADC_ICHG          = (1<<2);
    constexpr uint16_t EN_ADC_VSYS          = (1<<1);
    constexpr uint16_t EN_ADC_VBAT          = (1<<0);
    // *** Charger_Status ***
    constexpr uint16_t AC_STAT              = (1<<15); // Input source status, same as CHRG_OK bit
    constexpr uint16_t ICO_DONE             = (1<<14); // ICO routine successfully executed
    constexpr uint16_t IN_VINDPM            = (1<<12);
    constexpr uint16_t IN_IINDPM            = (1<<11);
    constexpr uint16_t IN_FCHRG             = (1<<10); // Fast charge mode
    constexpr uint16_t IN_PCHRG             = (1<<9); // Pre-charge mode
    constexpr uint16_t IN_OTG               = (1<<8); // OTG mode
    constexpr uint16_t FAULT_ACOV           = (1<<7);
    constexpr uint16_t FAULT_BATOC          = (1<<6);
    constexpr uint16_t FAULT_ACOC           = (1<<5);
    constexpr uint16_t SYSOVP_STAT          = (1<<4); // SYSOVP status and clear
    constexpr uint16_t FAULT_LATCHOFF       = (1<<2);
    constexpr uint16_t FAULT_OTG_OVP        = (1<<1);
    constexpr uint16_t FAULT_OTG_UCP        = (1<<0);
    // *** Prochot_Status ***

    // command registers
    namespace Register
    {
        constexpr uint8_t Charge_Option_0     = 0x12; // R/W
        constexpr uint8_t Charge_Option_1     = 0x30; // R/W
        constexpr uint8_t Charge_Option_2     = 0x31; // R/W
        constexpr uint8_t Charge_Option_3     = 0x32; // R/W
        constexpr uint8_t Charge_Current      = 0x14; // R/W
        constexpr uint8_t Max_Charge_Voltage  = 0x15; // R/W
        constexpr uint8_t Prochot_Option_0    = 0x33; // R/W
        constexpr uint8_t Prochot_Option_1    = 0x34; // R/W
        constexpr uint8_t ADC_Option          = 0x35; // R/W
        constexpr uint8_t Charger_Status      = 0x20; // R
        constexpr uint8_t Prochot_Status      = 0x21; // R
        constexpr uint8_t Input_Limit_DPM     = 0x22; // R Used input current limit value
        constexpr uint8_t ADC_VBUS            = 0x23; // R ADC results of VBUS and PSYS
        constexpr uint8_t ADC_BAT_Current     = 0x24; // R ADC results of battery currents
        constexpr uint8_t ADC_Input_Current   = 0x25; // R ADC results of input current and CMPNI
        constexpr uint8_t ADC_SYS_Voltage     = 0x26; // R ADC results of VSYS and VBAT
        constexpr uint8_t OTG_Voltage         = 0x3B; // R/W
        constexpr uint8_t OTG_Current         = 0x3C; // R/W
        constexpr uint8_t Input_Voltage       = 0x3D; // R/W
        constexpr uint8_t Min_SYS_Voltage     = 0x3E; // R/W
        constexpr uint8_t Input_Limit_Host    = 0x3F; // R/W Set input current limit
        constexpr uint8_t Manufacturer_ID     = 0xFE; // R/W
        constexpr uint8_t Device_ID           = 0xFF; // R/W
    }; // namespace Register

    // Controller states
    enum class State: unsigned char
    {
        Init = 0, Idle, Charging, OTG, Error
    };


    // === Classes ===
    /**
     * @brief Register base class.
     */
    struct RegisterBase
    {
        const uint8_t address; 
        uint16_t value;
        RegisterBase() = delete;
        /**
         * @brief Construct a new Register Base object
         * 
         * @param address The address of the register
         */
        explicit RegisterBase(uint16_t address) : address(address), value(0) {};
    };

    /** @brief ChargeOption0 Register */
    class ChargeOption0 : public RegisterBase
    {
    public:
        ChargeOption0() : RegisterBase(Register::Charge_Option_0) {};
        /** @brief Enable Out of Audio Mode (f_switch > 25 kHz) */
        [[nodiscard]] bool EN_OOA() const { return (this->value & (1<<10)) > 0; }
        [[nodiscard]] uint8_t WDTMR_ADJ() const { return bits::get(this->value, {0b11, 13}); }
        void set_EN_OOA(bool bit) { this->value = bits::set(value, {0b1, 10, bit}); }
        void set_WDTMR_ADJ(uint8_t val) { this->value = bits::set(this->value, {0b11, 13, val}); }
    };

    /**
     * @brief Class for the charger controller.
     * @tparam bus_controller The bus controller class.
     */
    template<class bus_controller>
    class Controller
    {
    private:
        // *** properties ***
        bus_controller mybus;
        I2C::Data_t i2c_data{};
        State state = State::Init;
        uint16_t voltage_system = 0;
        uint16_t voltage_input  = 0;
        int16_t   current_input  = 0;
        uint16_t voltage_OTG    = 0;
        uint16_t current_OTG    = 0;
        uint16_t current_charge = 0;

        // *** methods ***
        // bool        send_command_byte(const unsigned char cmd);
        bool         read_register(uint8_t reg);

    public:
        // *** Constructor ***
        /**
         * @brief Constructor for charger controller.
         * @param bus_used The reference to the used bus peripheral.
         */
        explicit Controller(bus_controller &bus_used)
            : mybus{bus_used} {};

        // *** Properties

        // *** Methods ***
        [[nodiscard]] auto get_system_voltage() const -> uint16_t { return this->voltage_system; }
        [[nodiscard]] auto get_input_voltage() const -> uint16_t { return this->voltage_input; }
        [[nodiscard]] auto get_input_current() const -> int16_t { return this->current_input; }
        [[nodiscard]] auto get_OTG_voltage() const -> uint16_t { return this->voltage_OTG + 4480; }
        [[nodiscard]] auto get_OTG_current() const -> uint16_t { return this->current_OTG; }
        [[nodiscard]] auto get_charge_current() const -> uint16_t { return this->current_charge; }
        [[nodiscard]] auto get_state() const -> State { return this->state; }
        bool initialize();
        bool write_register(uint8_t reg, uint16_t data);
        bool set_charge_current(uint16_t current);
        bool set_OTG_voltage(uint16_t voltage);
        bool set_OTG_current(uint16_t current);
        bool enable_OTG(bool state);
        bool read(RegisterBase &reg);
        bool write(RegisterBase &reg);
    };
}; // namespace BQ25700

#endif
