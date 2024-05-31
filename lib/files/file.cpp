/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    file.cpp
 * @author  SO
 * @version v3.4.0
 * @date    09-January-2022
 * @brief   Interface for files.
 ==============================================================================
 */

/* === Includes === */
#include "file.h"

/* Provide valid template instantiations */
template class fat32::File<fat32::Volume<sdhc::Card>>;

namespace fat32
{
    /* === Methods === */
    template <class Volume_t>
    auto File<Volume_t>::close() -> bool
    {
        /* Flush the block buffer to the card */
        this->flush();

        /* Set file state to closed */
        this->state = files::State::Closed;

        return true;
    };

    template <class Volume_t>
    void File<Volume_t>::flush()
    {
        this->volume->write_file_to_memory(this->handle);
        this->volume->write_filesize_to_directory(this->handle);
    };

    template <class Volume_t>
    auto File<Volume_t>::put(const char byte) -> bool
    {
        /* Only when file is not read-only */
        if (
            (this->state == files::State::Read_only) or
            (this->state == files::State::Closed))
            return false;

        /* Set file state to changed */
        this->state = files::State::Changed;

        /* Write the byte to the current buffer position */
        this->handle.block_buffer[this->handle.current.byte] = byte;

        /* Update position counter */
        this->handle.current.byte++;
        this->handle.size++;
        this->access_position++;

        /* When block buffer is full flush it to the card */
        if (this->handle.current.byte == 512)
        {
            this->volume->write_file_to_memory(this->handle);
            this->volume->write_filesize_to_directory(this->handle);
            this->handle.current.byte = 0;
        }

        return true;
    };

    template <class Volume_t>
    auto File<Volume_t>::read() -> uint8_t
    {
        /* Check whether the end of file is already reached */
        if (this->tell() == this->size())
            return 0;

        /* Check whether end of sector was reached */
        if (this->handle.current.byte == 512)
        {
            volume->read_next_sector_of_cluster(this->handle);
            this->handle.current.byte = 0;
        }

        /* return the data at the current block position */
        this->access_position++;
        return this->handle.block_buffer[this->handle.current.byte++];
    };

    template <class Volume_t>
    auto File<Volume_t>::size() const -> uint32_t
    {
        return this->handle.size;
    };

    template <class Volume_t>
    auto File<Volume_t>::tell() const -> uint32_t
    {
        return this->access_position;
    };

    template <class Volume_t>
    auto File<Volume_t>::write(
        const char *begin,
        const std::size_t len) -> bool
    {
        /* Only when file is not read-only */
        if (
            (this->state == files::State::Read_only) or
            (this->state == files::State::Closed))
            return false;

        /* Set file state to changed */
        this->state = files::State::Changed;

        /* iterate through the data */
        auto ptr = begin;
        for (std::size_t iByte = 0; iByte < len; iByte++)
        {
            /* Write the byte to the current buffer position */
            this->handle.block_buffer[this->handle.current.byte] = *ptr++;

            /* Update position counter */
            this->handle.current.byte++;
            this->handle.size++;
            this->access_position++;

            /* When block buffer is full flush it to the card */
            if (this->handle.current.byte == 512)
            {
                this->flush();
                this->handle.current.byte = 0;
            }
        }

        return true;
    };
}; // namespace fat32
