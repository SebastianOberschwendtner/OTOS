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
 * @version v2.7.4
 * @date    14-November-2021
 * @brief   Driver for the TPS65987DDH(K) USB-3 PD controller.
 ******************************************************************************
 */

// === Includes ===
#include "tps65987.h"

// Provide template instanciations with allowed bus controllers
template class TPS65987::Controller<I2C::Controller>;

// === Functions ===

/**
 * @brief Initialize the PD IC based on its current mode.
 * @return Returns True when the IC was initialized successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::initialize(void)
{
    // set the i2c address
    Bus::change_address(this->mybus, TPS65987::i2c_address);

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
};

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
};

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
};

/**
 * @brief Read the currently active command and update the
 * active command property.
 * @return Returns True when the command register was read successfully.
 */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::read_active_command(void)
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
};

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
    for (unsigned char nByte = 0; nByte < 4; nByte++)
        this->buffer_cmd[nByte + 2] = *(cmd + nByte);

    // Send the array
    unsigned char *dest = reinterpret_cast<unsigned char *>(&this->buffer_cmd[0]);
    return Bus::send_array(this->mybus, dest, Register::Cmd1.length + 2);
};
/**
  * @brief Read the active mode of the controller.
  * @return Returns True when the mode was read successfully.
  */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::read_mode(void)
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
    };
    return true;
}

/**
  * @brief Read the current PD status from the PD Controller.
  * @return Returns True when the status was read successfully.
  */
template <class bus_controller>
bool TPS65987::Controller<bus_controller>::read_PD_status(void)
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
    unsigned char temp = this->i2c_data.byte[0];
    if (temp & PlugDetails_0)
        this->contract_active.USB_type = 2;
    else
        this->contract_active.USB_type = 3;

    // Active role
    if (temp & PresentRole)
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
    };
    return true;
};
