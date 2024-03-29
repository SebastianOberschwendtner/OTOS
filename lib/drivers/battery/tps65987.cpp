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
/**
 ******************************************************************************
 * @file    tps65987.cpp
 * @author  SO
 * @version v4.1.0
 * @date    14-November-2021
 * @brief   Driver for the TPS65987DDH(K) USB-3 PD controller.
 ******************************************************************************
 */

// === Includes ===
#include "tps65987.h"

// Provide template instantiations with allowed bus controllers
template class TPS65987::Controller<I2C::Controller>;

// === Functions ===

/**
 * @brief Get the raw value of the PDO.
 *
 * @return The raw PDO in bits 0-31.
 */
uint32_t TPS65987::PDO::get_data() const
{
    return this->data;
}
/**
 * @brief Returns the maximum current indicated by the PDO.
 *
 * @return The maximum current in [mA].
 */
uint16_t TPS65987::PDO::current() const
{
    return (this->data & 0x3FF) * 10;
}

/**
 * @brief Returns the fixed voltage indicated by the PDO.
 *
 * @return The voltage in [mV].
 */
uint16_t TPS65987::PDO::voltage() const
{
    // Get the bit position of the voltage depending on the PDO type
    const uint8_t bit_pos = this->type() == Fixed_Supply ? 10 : 20;

    // Return the converted voltage in [mV]
    return ((this->data >> bit_pos) & 0x3FF) * 50;
}

/**
 * @brief Check the type of PDO object.
 *
 * @return Returns the type of the object.
 */
TPS65987::PDO::Type TPS65987::PDO::type() const
{
    return static_cast<Type>((this->data >> 30) & 0b11);
}

/**
 * @brief Set the voltage of the PDO.
 *
 * @param voltage The voltage in [mV].
 */
void TPS65987::PDO::set_voltage(const uint16_t voltage)
{
    // Calculate the voltage in 50mV steps
    const uint16_t volt = voltage / 50;

    // Clear the voltage bits
    this->data &= ~(0x3FF << 10);

    // Set the voltage bits
    this->data |= (volt & 0x3FF) << 10;
}

/**
 * @brief Set the current of the PDO.
 *
 * @param current The current in [mA].
 */
void TPS65987::PDO::set_current(const uint16_t current)
{
    // Calculate the current in 10mA steps
    const uint16_t I_set = current / 10;

    // Clear the current bits
    this->data &= ~0x3FF;

    // Set the voltage bits
    this->data |= (I_set & 0x3FF);
}

/**
 * @brief Initialize the PD IC based on its current mode.
 * @return Returns True when the IC was initialized successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::initialize()
{
    // set the i2c address
    Bus::change_address(this->mybus, TPS65987::i2c_address);
    this->mybus.set_timeout(65000);

    // First read the current mode of the IC
    if (!this->read_mode())
        return false;

    // When in Patch mode, exit this mode
    if (this->get_mode() == Mode::PTCH)
    {
        // Write command
        if (!this->write_command("PTCc"))
            return false;
    }

    return true;
}

/**
 * @brief Read a register of the PD controller.
 * The function takes the specific length of the register into account.
 * @param reg The register to read from.
 * @return Returns True when the register was read successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::read_register(const reg_t reg)
{
    // Set the target address
    // this->mybus.set_target_address(i2c_address);

    // Send the data buffer to the register
    this->buffer_data[0] = reg.address;
    return Bus::read_array(this->mybus, reg.address, &this->buffer_data[1], reg.length + 1);
}

/**
 * @brief Write a register of the PD controller.
 * The function takes the specific length of the register into account.
 * @param reg The register to write to.
 * @return Returns True when the register was written successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::write_register(const reg_t reg)
{
    // Set the target address
    // this->mybus.set_target_address(i2c_address);

    // Send the data buffer to the register
    this->buffer_data[0] = reg.address;
    this->buffer_data[1] = reg.length;
    return Bus::send_array(this->mybus, &this->buffer_data[0], reg.length + 2);
}

/**
 * @brief Read the currently active command and update the
 * active command property.
 * @return Returns True when the command register was read successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::read_active_command()
{
    // Read the data
    if (!this->read_register(Register::Cmd1))
        return false;

    // Convert the data
    this->cmd_active[3] = this->buffer_data[1];
    this->cmd_active[2] = this->buffer_data[2];
    this->cmd_active[1] = this->buffer_data[3];
    this->cmd_active[0] = this->buffer_data[4];
    return true;
}

/**
 * @brief Write a command to the command register
 * The Data1 register has to be written first, when the
 * command expects data!
 * @param cmd The command string => has to have 4 characters.
 * @return Returns True when the command was send successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::write_command(const char *cmd)
{
    // Assemble the cmd array
    this->buffer_cmd[0] = Register::Cmd1.address;
    this->buffer_cmd[1] = Register::Cmd1.length;
    for (uint8_t nByte = 0; nByte < 4; nByte++)
        this->buffer_cmd.at(nByte + 2) = *(cmd + nByte); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // Send the array
    auto *dest = reinterpret_cast<std::uint8_t *>(&this->buffer_cmd[0]); // NOLINT
    return Bus::send_array(this->mybus, dest, Register::Cmd1.length + 2);
}

/**
 * @brief Read the active mode of the controller.
 * @return Returns True when the mode was read successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::read_mode()
{
    // Read the mode from the controller
    if (!this->read_register(Register::Mode))
        return false;

    // Decode the mode
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

/**
 * @brief Read the current PD status from the PD Controller.
 * @return Returns True when the status was read successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::read_PD_status()
{
    // Read the status register
    if (!this->read_register(Register::PD_Status))
        return false;

    // Convert the received data
    this->i2c_data.byte[3] = this->buffer_data[1];
    this->i2c_data.byte[2] = this->buffer_data[2];
    this->i2c_data.byte[1] = this->buffer_data[3];
    this->i2c_data.byte[0] = this->buffer_data[4];

    // Check plug type
    std::uint8_t temp = this->i2c_data.byte[0];
    if ((temp & PlugDetails_0) != 0)
        this->contract_active.USB_type = 2;
    else
        this->contract_active.USB_type = 3;

    // Active role
    if ((temp & PresentRole) != 0)
        this->contract_active.role = 1;
    else
        this->contract_active.role = 0;

    // Check CC Mode
    temp = (temp & (CCPullUp_0 | CCPullUp_1)) >> 2;
    switch (temp)
    {
    // USB Default power
    case 1:
        this->contract_active.voltage = 5000;
        this->contract_active.current = 900;
        break;

    // USB power increased to 1.5A
    case 2:
        this->contract_active.voltage = 5000;
        this->contract_active.current = 1500;
        break;

    // USB power increased to 3.0A
    case 3:
        this->contract_active.voltage = 5000;
        this->contract_active.current = 3000;
        break;

    // No cable or contract
    default:
        this->contract_active.voltage = 0;
        this->contract_active.current = 0;
        break;
    }
    return true;
}

/**
 * @brief Read the status register [0x1A] from the PD Controller.
 * @return Returns True and the value of the lower 4 bytes of the status when read successfully.
 */
template <class bus_controller>
std::optional<uint32_t> TPS65987::Controller<bus_controller>::read_status()
{
    // Read the status register
    if (!this->read_register(Register::Status))
        return {};

    // Convert the received data
    uint32_t status = this->buffer_data[8];
    status |= (this->buffer_data[7] << 8);
    status |= (this->buffer_data[6] << 16);
    status |= (this->buffer_data[5] << 24);

    // Return the status
    return status;
}

/**
 * @brief Read the active PDO from the PD Controller.
 *
 * @tparam bus_controller The used bus for the communication.
 * @return True and the active PDO when read successfully.
 */
template <class bus_controller>
std::optional<TPS65987::PDO> TPS65987::Controller<bus_controller>::read_active_pdo()
{
    // Read the active PDO register
    if (!this->read_register(Register::Active_PDO))
        return {};

    // Convert the received data
    uint32_t pdo = this->buffer_data[6];
    pdo |= (this->buffer_data[5] << 8);
    pdo |= (this->buffer_data[4] << 16);
    pdo |= (this->buffer_data[3] << 24);

    // Return the PDO
    return PDO(pdo);
}

/**
 * @brief Read a TX source PDO from the PD Controller.
 *
 * @tparam bus_controller The used bus for the communication.
 * @param pdo_number The PDO number to read.
 * @return Returns True and the PDO when read successfully and when the PDO number is valid.
 */
template <class bus_controller>
std::optional<TPS65987::PDO> TPS65987::Controller<bus_controller>::read_TX_sink_pdo(
    const std::uint8_t pdo_number)
{
    // Read the TX sink PDO capabilities register
    if (!this->read_register(Register::TX_Sink_Cap))
        return {};

    // Check whether the PDO number is valid
    auto valid_pdos = this->buffer_data[57] & 0b111;
    if (pdo_number > valid_pdos)
        return {};

    // Convert the received data
    uint32_t pdo = this->buffer_data.at(56 - pdo_number * 4);
    pdo |= (this->buffer_data.at(55 - pdo_number * 4) << 8);
    pdo |= (this->buffer_data.at(54 - pdo_number * 4) << 16);
    pdo |= (this->buffer_data.at(53 - pdo_number * 4) << 24);

    return PDO(pdo);
}

/**
 * @brief Update the tx buffer with the given capability.
 * 
 * A capability is a PDO with the corresponding power path configuration.
 * Make sure to delete the tx buffer before calling this function!
 *
 * @tparam bus_controller The used bus for the communication.
 * @param cap The capability to update the tx buffer with.
 */
template <class bus_controller>
void TPS65987::Controller<bus_controller>::register_TX_source_capability(const Capability &cap)
{
    // Get the current number of valid PDOs in the buffer
    uint8_t valid_pdos = this->buffer_data[2] & 0b111;

    // Reset the number of valid PDOs if there are already 7 valid PDOs
    if (valid_pdos >= 7)
        valid_pdos = 0;

    // Update the power path configuration
    const uint8_t bitshift = 2 * valid_pdos;
    this->buffer_data[6] = bits::set(this->buffer_data[6], {0b11, bitshift, cap.second});

    // Update the number of valid PDOs
    /// @todo Add setting the advertise bit when registering more than one PDO
    this->buffer_data[2] = (this->buffer_data[2] & 0b11111000) | ++valid_pdos;

    // Update the PDO in the register
    this->buffer_data.at(valid_pdos * 4 + 6) = bits::get(cap.first.get_data(), {0xFF, 0});
    this->buffer_data.at(valid_pdos * 4 + 7) = bits::get(cap.first.get_data(), {0xFF, 8});
    this->buffer_data.at(valid_pdos * 4 + 8) = bits::get(cap.first.get_data(), {0xFF, 16});
    this->buffer_data.at(valid_pdos * 4 + 9) = bits::get(cap.first.get_data(), {0xFF, 24});
}