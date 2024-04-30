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
/**
 ==============================================================================
 * @file    tps65987.cpp
 * @author  SO
 * @version v5.0.0
 * @date    14-November-2021
 * @brief   Driver for the TPS65987DDH(K) USB-3 PD controller.
 ==============================================================================
 */

/* === Includes === */
#include "tps65987.h"

/* Provide template instantiations with allowed bus controllers */
template class tps65987::Controller<i2c::Controller>;

namespace tps65987
{
    /* === Methods === */
    auto PDO::get_data() const -> uint32_t
    {
        return this->data;
    }

    auto PDO::current() const -> uint16_t
    {
        return (this->data & 0x3FF) * 10;
    }

    auto PDO::voltage() const -> uint16_t
    {
        /* Get the bit position of the voltage depending on the PDO type */
        const uint8_t bit_pos = this->type() == Fixed_Supply ? 10 : 20;

        /* Return the converted voltage in [mV] */
        return ((this->data >> bit_pos) & 0x3FF) * 50;
    }

    auto PDO::type() const -> PDO::Type
    {
        return static_cast<Type>((this->data >> 30) & 0b11);
    }

    void PDO::set_current(const uint16_t current)
    {
        /* Calculate the current in 10mA steps */
        const uint16_t I_set = current / 10;

        /* Clear the current bits */
        this->data &= ~0x3FF;

        /* Set the voltage bits */
        this->data |= (I_set & 0x3FF);
    }

    void PDO::set_voltage(const uint16_t voltage)
    {
        /* Calculate the voltage in 50mV steps */
        const uint16_t volt = voltage / 50;

        /* Clear the voltage bits */
        this->data &= ~(0x3FF << 10);

        /* Set the voltage bits */
        this->data |= (volt & 0x3FF) << 10;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::initialize() -> bool
    {
        /* set the i2c address */
        bus::change_address(this->mybus, i2c_address);
        this->mybus.set_timeout(65000);

        /* First read the current mode of the IC */
        if (!this->read_mode())
            return false;

        /* When in Patch mode, exit this mode */
        if (this->get_mode() == Mode::PTCH)
        {
            /* Write command */
            if (!this->write_command("PTCc"))
                return false;
        }

        return true;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::read_active_command() -> bool
    {
        /* Read the data */
        if (!this->read_register(registers::Cmd1))
            return false;

        /* Convert the data */
        this->cmd_active[3] = this->buffer_data[1];
        this->cmd_active[2] = this->buffer_data[2];
        this->cmd_active[1] = this->buffer_data[3];
        this->cmd_active[0] = this->buffer_data[4];
        return true;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::read_active_pdo() -> std::optional<PDO>
    {
        /* Read the active PDO register */
        if (!this->read_register(registers::Active_PDO))
            return {};

        /* Convert the received data */
        uint32_t pdo = this->buffer_data[6];
        pdo |= (this->buffer_data[5] << 8);
        pdo |= (this->buffer_data[4] << 16);
        pdo |= (this->buffer_data[3] << 24);

        /* Return the PDO */
        return PDO(pdo);
    }

    template <class bus_controller>
    auto Controller<bus_controller>::read_mode() -> bool
    {
        /* Read the mode from the controller */
        if (!this->read_register(registers::Mode))
            return false;

        /* Decode the mode */
        switch (this->buffer_data[4])
        {
            case 'B':
                this->mode_active = Mode::BOOT;
                break;
            case 'P':
                this->mode_active = Mode::PTCH;
                break;
            case 'A':
                this->mode_active = Mode::APP;
                break;
            default:
                this->mode_active = Mode::OTHER;
                break;
        }
        return true;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::read_PD_status() -> bool
    {
        /* Read the status register */
        if (!this->read_register(registers::PD_Status))
            return false;

        /* Convert the received data */
        this->i2c_data.byte[3] = this->buffer_data[1];
        this->i2c_data.byte[2] = this->buffer_data[2];
        this->i2c_data.byte[1] = this->buffer_data[3];
        this->i2c_data.byte[0] = this->buffer_data[4];

        /* Check plug type */
        std::uint8_t temp = this->i2c_data.byte[0];
        if ((temp & PlugDetails_0) != 0)
            this->contract_active.USB_type = 2;
        else
            this->contract_active.USB_type = 3;

        /* Active role */
        if ((temp & PresentRole) != 0)
            this->contract_active.role = 1;
        else
            this->contract_active.role = 0;

        /* Check CC Mode */
        temp = (temp & (CCPullUp_0 | CCPullUp_1)) >> 2;
        switch (temp)
        {
            /* USB Default power */
            case 1:
                this->contract_active.voltage = 5000;
                this->contract_active.current = 900;
                break;

            /* USB power increased to 1.5A */
            case 2:
                this->contract_active.voltage = 5000;
                this->contract_active.current = 1500;
                break;

            /* USB power increased to 3.0A */
            case 3:
                this->contract_active.voltage = 5000;
                this->contract_active.current = 3000;
                break;

            /* No cable or contract */
            default:
                this->contract_active.voltage = 0;
                this->contract_active.current = 0;
                break;
        }
        return true;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::read_register(const reg_t reg) -> bool
    {
        /* Set the target address */
        /* this->mybus.set_target_address(i2c_address); */

        /* Send the data buffer to the register */
        this->buffer_data[0] = reg.address;
        return bus::read_array(this->mybus, reg.address, &this->buffer_data[1], reg.length + 1);
    }

    template <class bus_controller>
    auto Controller<bus_controller>::read_status() -> std::optional<uint32_t>
    {
        /* Read the status register */
        if (!this->read_register(registers::Status))
            return {};

        /* Convert the received data */
        uint32_t status = this->buffer_data[8];
        status |= (this->buffer_data[7] << 8);
        status |= (this->buffer_data[6] << 16);
        status |= (this->buffer_data[5] << 24);

        /* Return the status */
        return status;
    }

    template <class bus_controller>
    auto Controller<bus_controller>::read_TX_sink_pdo(
        const std::uint8_t pdo_number) -> std::optional<PDO>
    {
        /* Read the TX sink PDO capabilities register */
        if (!this->read_register(registers::TX_Sink_Cap))
            return {};

        /* Check whether the PDO number is valid */
        auto valid_pdos = this->buffer_data[57] & 0b111;
        if (pdo_number > valid_pdos)
            return {};

        /* Convert the received data */
        uint32_t pdo = this->buffer_data.at(56 - pdo_number * 4);
        pdo |= (this->buffer_data.at(55 - pdo_number * 4) << 8);
        pdo |= (this->buffer_data.at(54 - pdo_number * 4) << 16);
        pdo |= (this->buffer_data.at(53 - pdo_number * 4) << 24);

        return PDO(pdo);
    }

    template <class bus_controller>
    void Controller<bus_controller>::register_TX_source_capability(const Capability &cap)
    {
        /* Get the current number of valid PDOs in the buffer */
        uint8_t valid_pdos = this->buffer_data[2] & 0b111;

        /* Reset the number of valid PDOs if there are already 7 valid PDOs */
        if (valid_pdos >= 7)
            valid_pdos = 0;

        /* Update the power path configuration */
        const uint8_t bitshift = 2 * valid_pdos;
        this->buffer_data[6] = bits::set(this->buffer_data[6], {0b11, bitshift, cap.second});

        /* Update the number of valid PDOs */
        //* @todo Add setting the advertise bit when registering more than one PDO */
        this->buffer_data[2] = (this->buffer_data[2] & 0b11111000) | ++valid_pdos;

        /* Update the PDO in the register */
        this->buffer_data.at(valid_pdos * 4 + 6) = bits::get(cap.first.get_data(), {0xFF, 0});
        this->buffer_data.at(valid_pdos * 4 + 7) = bits::get(cap.first.get_data(), {0xFF, 8});
        this->buffer_data.at(valid_pdos * 4 + 8) = bits::get(cap.first.get_data(), {0xFF, 16});
        this->buffer_data.at(valid_pdos * 4 + 9) = bits::get(cap.first.get_data(), {0xFF, 24});
    }

    template <class bus_controller>
    auto Controller<bus_controller>::write_command(const char *cmd) -> bool
    {
        /* Assemble the cmd array */
        this->buffer_cmd[0] = registers::Cmd1.address;
        this->buffer_cmd[1] = registers::Cmd1.length;
        for (uint8_t nByte = 0; nByte < 4; nByte++)
            this->buffer_cmd.at(nByte + 2) = *(cmd + nByte); /* NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) */

        /* Send the array */
        auto *dest = reinterpret_cast<std::uint8_t *>(&this->buffer_cmd[0]); /* NOLINT */
        return bus::send_array(this->mybus, dest, registers::Cmd1.length + 2);
    }

    template <class bus_controller>
    auto Controller<bus_controller>::write_register(const reg_t reg) -> bool
    {
        /* Set the target address */
        /* this->mybus.set_target_address(i2c_address); */

        /* Send the data buffer to the register */
        this->buffer_data[0] = reg.address;
        this->buffer_data[1] = reg.length;
        return bus::send_array(this->mybus, &this->buffer_data[0], reg.length + 2);
    }
}; // namespace tps65987