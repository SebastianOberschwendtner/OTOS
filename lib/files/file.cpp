/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    file.cpp
 * @author  SO
 * @version v3.4.0
 * @date    09-January-2022
 * @brief   Interface for files.
 ******************************************************************************
 */

// === Includes ===
#include "file.h"

// Provide valid template instanciations
template class FAT32::File<FAT32::Volume<SDHC::Card>>;

// === Class Methods ===

/**
 * @brief Return the current size of the file
 * in bytes.
 * 
 * @tparam Volume_t The volume class which is used for memory access.
 * @return Returns the file size in bytes.
 */
template<class Volume_t>
unsigned long FAT32::File<Volume_t>::size(void) const
{
    return this->handle.size;
};

/**
 * @brief Get the internal access position pointer
 * for read and write access of the file.
 * 
 * @tparam Volume_t The volume class which is used for memory access.
 * @return The current access position in bytes of the file.
 */
template<class Volume_t>
unsigned long FAT32::File<Volume_t>::tell(void) const
{
    return this->access_position;
};

/**
 * @brief Read data from the file. After
 * each access the byte counter is increased.
 * 
 * When the end of file is reached, the function
 * consecutively returns 0.
 * 
 * @tparam Volume_t The volume class which is used for memory access.
 * @return The data byte at the current access position.
 */
template<class Volume_t>
unsigned char FAT32::File<Volume_t>::read(void)
{
    // Check whether the end of file is already reached
    if (this->tell() == this->size())
        return 0;

    // Check whether end of sector was reached
    if (this->handle.current.byte == 512)
    {
        volume->read_next_sector_of_cluster(this->handle);
        this->handle.current.byte = 0;
    }

    // return the data at the current block position
    this->access_position++;
    return this->handle.block_buffer[this->handle.current.byte++];
};