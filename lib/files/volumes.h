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

#ifndef VOLUMES_H_
#define VOLUMES_H_

// === Includes ===
#include <optional>
#include <ctime>
#include "memory/sdhc.h"
#include "filesystem/fat32.h"

namespace Drive
{

    // === Functions to interface with memory ===
    template<class Memory>
    bool read_single_block(Memory& memory, const unsigned long* buffer, const unsigned long block)
    {
        return memory.read_single_block(buffer, block); 
    };
    template<class Memory>
    bool write_single_block(Memory& memory, const unsigned long* buffer, const unsigned long block)
    {
        return memory.write_single_block(buffer, block); 
    };
};

namespace FAT32
{
    template<class Memory>
    class Volume
    {
    private:
        unsigned long FAT_sector_in_buffer{0xFFFFFFFF};

    public:
        // *** Properties ***
        Memory memory;
        std::array<unsigned char, 512> FAT{0};
        Partition partition{};
        Error::Code error{Error::Code::None};

        // *** Constructor ***
        Volume() = delete;
        Volume(Memory& memory_used);

        // *** Methods ***
        bool read_cluster(Filehandler& file, const unsigned long cluster);
        std::optional<unsigned long> read_FAT_entry( const unsigned long cluster );
        bool write_FAT_entry(const unsigned long cluster, const unsigned long FAT_Entry);
        bool read_next_sector_of_cluster(Filehandler& file);
        bool write_current_sector(Filehandler& file);
        bool get_file(Filehandler& file, const unsigned long id);
        bool read_root(Filehandler& file);
        bool mount(void);
        std::optional<unsigned long> get_empty_id(Filehandler& directory);
        std::optional<unsigned long> get_next_empty_cluster(void);
        std::optional<unsigned long> get_fileid(Filehandler& directory, const std::array<char, 12> filename);
        bool read_last_sector_of_file(Filehandler& file);
        bool write_filesize_to_directory(Filehandler& file);
        bool write_file_to_memory(Filehandler& file);
        bool make_directory_entry(Filehandler& directory, const unsigned long id, const unsigned long start_cluster, const std::array<char, 12> name, const unsigned char attributes, const std::time_t time);
    };
};

#endif

