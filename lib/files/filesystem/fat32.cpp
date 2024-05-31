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
 * @file    fat32.cpp
 * @author  SO
 * @version v2.7.0
 * @date    03-January-2022
 * @brief   Interface for FAT32 filesystems. Also supports FAT16.
 ==============================================================================
 */

/* === Includes === */
#include "filesystem/fat32.h"

namespace fat32
{
    /* === Methods === */
    auto Partition::get_FAT_position(const uint32_t cluster) const -> uint16_t
    {
        const uint32_t FAT_offset = this->is_fat16 ? cluster * 2 : cluster * 4;
        return static_cast<uint16_t>(FAT_offset % 512);
    };

    auto Partition::get_FAT_sector(const uint32_t cluster, const uint8_t FAT) const -> uint32_t
    {
        const uint32_t FAT_offset = this->is_fat16 ? cluster * 2 : cluster * 4;
        return this->FAT_Begin + (FAT_offset / 512) + (this->FAT_Size * (FAT - 1));
    };

    auto Partition::get_LBA_of_cluster(const uint32_t cluster) const -> uint32_t
    {
        if (cluster >= 2)
            return this->First_Data_Sector + (cluster - 2) * this->Sectors_per_Cluster;
        else
            return this->FAT_Begin + (2 * this->FAT_Size); /* Root directory of FAT16 */
    };

    auto Partition::get_LBA_of_root_cluster(void) const -> uint32_t
    {
        if (this->is_fat16)
            return this->FAT_Begin + 2 * this->FAT_Size;
        else
            return this->Root_Directory_Cluster;
    };

    auto Partition::read_FAT(const uint8_t *FAT_buffer, const uint16_t position) const -> uint32_t
    {
        if (this->is_fat16)
            return static_cast<uint32_t>(read_short(FAT_buffer, position));
        else
            return read_long(FAT_buffer, position);
    };

    void Partition::write_FAT(
        uint8_t *const FAT_buffer,
        const uint16_t position,
        const uint32_t data) const
    {
        if (this->is_fat16)
            write_short(FAT_buffer, position, static_cast<uint16_t>(data));
        else
            write_long(FAT_buffer, position, data);
    };

    auto Filehandler::is_directory() -> bool
    {
        return this->attributes & Attribute::is_Directory;
    };

    auto Filehandler::is_file() -> bool
    {
        return not this->is_directory();
    };

    void Filehandler::update_properties_from_buffer()
    {
        /*
         * The fileid is a consecutive number over all sectors of the directory. To get the specific entry within one sector,
         * the remainder of the entries per sector has to be used to access the information in the buffer.
         */
        const uint16_t entry_offset = static_cast<uint16_t>(this->id % (512 / 32));
        const uint16_t pos_offset = entry_offset * 32;

        /* Read the name */
        std::copy(
            this->block_buffer.begin() + pos_offset,
            this->block_buffer.begin() + pos_offset + 11,
            this->name.begin());

        /* Read the attribute byte */
        this->attributes = read_byte(this->block_buffer.begin(), pos_offset + DIR_Entry::Attributes);

        /* Save current cluster as directory cluster */
        this->directory_cluster = this->current.cluster;

        /* Read the start cluster of the file */
        this->start_cluster = static_cast<uint32_t>(read_short(this->block_buffer.begin(), pos_offset + DIR_Entry::First_Cluster_L));

        /* FAT32 uses additional bytes (have to be all 0 for FAT16 drives according to specs from Microsoft!) */
        this->start_cluster |= (static_cast<uint32_t>(read_short(this->block_buffer.begin(), pos_offset + DIR_Entry::First_Cluster_H)) << 16);

        /* Read the file size */
        this->size = read_long(this->block_buffer.begin(), pos_offset + DIR_Entry::Filesize);
    };

    /* === Functions === */
    auto boot_is_EFI(const uint8_t *block) -> bool
    {
        return read_byte(block, PART1_TYPE) == 0xEE;
    };

    auto boot_is_MBR(const uint8_t *block) -> bool
    {
        const uint8_t type = read_byte(block, PART1_TYPE);
        return (type == 0x04) | (type == 0x14)    /* FAT16 < 32 MiB */
               | (type == 0x06) | (type == 0x16)  /* FAT16 > 32 MiB */
               | (type == 0x0C) | (type == 0x1C)  /* FAT32 with LBA */
               | (type == 0x0E) | (type == 0x1E); /* FAT16 > 32 MiB with LBA */
    };

    auto boot_sector_valid(const uint8_t *block) -> bool
    {
        return read_short(block, MAGIC_NUMBER) == 0xAA55;
    };

    auto get_partition_begin(const uint8_t *block) -> uint32_t
    {
        return read_long(block, PART1_LBA_BEGIN);
    };

    auto read_byte(const uint8_t *block, const uint16_t position) -> uint8_t
    {
        return *(block + position);
    };

    auto read_long(const uint8_t *block, const uint16_t position) -> uint32_t
    {
        const uint8_t byte0 = block[position];
        const uint8_t byte1 = *(block + position + 1);
        const uint8_t byte2 = *(block + position + 2);
        const uint8_t byte3 = *(block + position + 3);
        /* Assemble the output value, the 0 is for unit testing on sytems where long is 64bit (looking at you linux!) */
        return 0U | (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
    };

    auto read_short(const uint8_t *block, const uint16_t position) -> uint16_t
    {
        const uint8_t byte0 = *(block + position);
        const uint8_t byte1 = *(block + position + 1);
        return (byte1 << 8) | byte0;
    };

    void write_byte(
        uint8_t *const block,
        const uint16_t position,
        const uint8_t data)
    {
        *(block + position) = data;
    };

    void write_long(
        uint8_t *const block,
        const uint16_t position,
        const uint32_t data)
    {
        *(block + position) = static_cast<uint8_t>(data & 0xFF);
        *(block + position + 1) = static_cast<uint8_t>((data >> 8) & 0xFF);
        *(block + position + 2) = static_cast<uint8_t>((data >> 16) & 0xFF);
        *(block + position + 3) = static_cast<uint8_t>((data >> 24) & 0xFF);
    };

    void write_short(
        uint8_t *const block,
        const uint16_t position,
        const uint16_t data)
    {
        *(block + position) = static_cast<uint8_t>(data & 0xFF);
        *(block + position + 1) = static_cast<uint8_t>((data >> 8) & 0xFF);
    };

    auto EFI::get_partition_begin(const uint8_t *block) -> uint32_t
    {
        return read_long(block, PART_LBA_BEGIN);
    };

    auto EFI::get_table_begin(const uint8_t *block) -> uint32_t
    {
        return read_long(block, TABLE_LBA_BEGIN);
    };

    auto BPB::initialize_partition(
        const uint8_t *block,
        const uint32_t partition_begin) -> Partition
    {
        /* Read needed values from the BPB */
        const uint32_t root_dir_sectors = root_directory_sectors(block);
        const uint32_t FAT_size = get_FAT_size(block);
        const uint32_t total_sectors = get_total_sector_size(block);
        const uint8_t reserved_sectors = get_reserved_sectors(block);
        const uint8_t number_FAT = get_number_of_FAT(block);
        const uint32_t sector_per_cluster = get_sectors_per_cluster(block);

        /* Calculate dependent parameters */
        const uint32_t data_sectors = total_sectors - (reserved_sectors + number_FAT * FAT_size) + root_dir_sectors;
        const uint32_t cluster_count = data_sectors / sector_per_cluster;
        const uint32_t lba_begin = partition_begin + reserved_sectors;
        const uint32_t first_data_sector = lba_begin + (2 * FAT_size) + root_dir_sectors;

        /* Check whether FAT16 is used */
        const bool is_fat16 = cluster_count < 65525 ? true : false;
        const uint32_t root_dir_cluster = is_fat16 ? 0 : get_root_directory_cluster(block);

        /* Get the volume label */
        const uint16_t label_position = is_fat16 ? FAT16_VOLUME_LABEL : FAT32_VOLUME_LABEL;
        std::array<char, 12> label{0};
        std::copy(
            block + label_position,
            block + label_position + 11,
            label.begin());

        /* Construct the volume with the derived values */
        return Partition{
            lba_begin, FAT_size, first_data_sector, root_dir_cluster, sector_per_cluster, is_fat16, label};
    };

    auto BPB::get_bytes_per_sector(const uint8_t *block) -> uint16_t
    {
        return read_short(block, BYTES_PER_SECTOR);
    };

    auto BPB::get_FAT_size(const uint8_t *block) -> uint32_t
    {
        /* check whether filesystem is FAT16 */
        const uint16_t fat16_size = read_short(block, FAT_SIZE_16);
        if (fat16_size != 0)
            return static_cast<uint32_t>(fat16_size);

        /* File system is FAT32 */
        return read_long(block, FAT_SIZE_32);
    };

    auto BPB::get_number_of_FAT(const uint8_t *block) -> uint8_t
    {
        return read_byte(block, NUMBER_OF_FAT);
    };

    auto BPB::get_reserved_sectors(const uint8_t *block) -> uint8_t
    {
        return read_byte(block, RESERVED_SEC);
    };

    auto BPB::get_root_directory_cluster(const uint8_t *block) -> uint32_t
    {
        return read_long(block, ROOT_DIR_CLUSTER);
    };

    auto BPB::get_sectors_per_cluster(const uint8_t *block) -> uint8_t
    {
        return read_byte(block, SEC_PER_CLUSTER);
    };

    auto BPB::get_total_sector_size(const uint8_t *block) -> uint32_t
    {
        /* check whether filesystem is FAT16 */
        const uint16_t fat16_size = read_short(block, TOT_SECTORS_16);
        if (fat16_size != 0)
            return static_cast<uint32_t>(fat16_size);

        /* File system is FAT32 */
        return read_long(block, TOT_SECTORS_32);
    };

    auto BPB::root_directory_sectors(const uint8_t *block) -> uint32_t
    {
        const uint16_t bytes_per_sector = get_bytes_per_sector(block);
        const uint16_t root_entry_count = read_short(block, ROOT_ENT_CNT);
        return ((root_entry_count * 32) + bytes_per_sector - 1) / bytes_per_sector;
    };
}; // namespace fat32