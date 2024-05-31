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
 * @file    volumes.cpp
 * @author  SO
 * @version v2.7.1
 * @date    04-January-2022
 * @brief   Interface for Volumes for file exchange.
 ==============================================================================
 */

/* === Includes === */
#include "volumes.h"

/* Provide valid template instantiations */
template class fat32::Volume<sdhc::Card>;

namespace fat32
{
    /* === Constructors === */
    template <class Memory>
    Volume<Memory>::Volume(Memory &memory_used)
        : memory{memory_used} {

          };

    /* === Methods === */
    template <class Memory>
    auto Volume<Memory>::read_cluster(Filehandler &file, const uint32_t cluster) -> bool
    {
        /* Set the current position in the filehandler */
        file.current.sector = 1;
        file.current.cluster = cluster;

        /* Read the correct block */
        const uint32_t block = this->partition.get_LBA_of_cluster(cluster);

        /* Convert the byte buffer to a 32-bit buffer for transfer with memory */
        const uint32_t *buffer = reinterpret_cast<uint32_t *>(file.block_buffer.begin());

        /* Read the new block from memory */
        return drive::read_single_block(this->memory, buffer, block);
    };

    template <class Memory>
    auto Volume<Memory>::read_FAT_entry(const uint32_t cluster) -> std::optional<uint32_t>
    {
        /* Read the FAT */
        const uint32_t block = this->partition.get_FAT_sector(cluster, 1);

        /* When sector is not already in buffer read it from memory */
        if (block != this->FAT_sector_in_buffer)
        {
            /* Convert the byte buffer to a 32-bit buffer for transfer with memory */
            const uint32_t *buffer = reinterpret_cast<uint32_t *>(this->FAT.begin());

            /* Read the memory */
            if (not drive::read_single_block(this->memory, buffer, block))
                return {};

            /* Update the current sector in the buffer */
            this->FAT_sector_in_buffer = block;
        }

        /* Read the FAT entry and return it. */
        const uint16_t position = this->partition.get_FAT_position(cluster);
        return this->partition.read_FAT(this->FAT.begin(), position);
    };

    template <class Memory>
    auto Volume<Memory>::write_FAT_entry(
        const uint32_t cluster,
        const uint32_t FAT_Entry) -> bool
    {
        /* get the LBA address of the block containing the FAT entry */
        uint32_t lba_address = this->partition.get_FAT_sector(cluster, 1);

        /* Convert the byte buffer to a 32-bit buffer for transfer with memory */
        const uint32_t *buffer = reinterpret_cast<uint32_t *>(this->FAT.begin());

        /* When sector is not already in buffer read it from memory */
        if (lba_address != this->FAT_sector_in_buffer)
        {
            /* Read the sector from memory */
            if (not drive::read_single_block(this->memory, buffer, lba_address))
                return false;

            /* Update the current sector in the buffer */
            this->FAT_sector_in_buffer = lba_address;
        }

        /* Write the new entry in the buffer */
        const uint16_t position = this->partition.get_FAT_position(cluster);
        this->partition.write_FAT(this->FAT.begin(), position, FAT_Entry);

        /* Write the new data to FAT 1 in the memory */
        if (not drive::write_single_block(this->memory, buffer, lba_address))
            return false;

        /* Write the new data to FAT 2 in the memory */
        lba_address = this->partition.get_FAT_sector(cluster, 2);
        return drive::write_single_block(this->memory, buffer, lba_address);
    };

    template <class Memory>
    auto Volume<Memory>::read_next_sector_of_cluster(Filehandler &file) -> bool
    {
        /* Convert the byte buffer to a 32-bit buffer for transfer with memory */
        const uint32_t *buffer = reinterpret_cast<uint32_t *>(file.block_buffer.begin());

        /* Check whether next sector can be read directly */
        if (file.current.sector != this->partition.Sectors_per_Cluster)
        {
            /* Get the next bock address */
            const uint32_t block = this->partition.get_LBA_of_cluster(file.current.cluster) + file.current.sector++;

            /* Read the next block from memory */
            return drive::read_single_block(this->memory, buffer, block);
        }
        else /* Next cluster has to be read */
        {
            /* Get the FAT entry of the current cluster */
            const auto FAT_Entry = this->read_FAT_entry(file.current.cluster);

            /* When FAT could not be read return false */
            if (not FAT_Entry)
                return false;
            else // Check the return code for valid content
            {
                /* Entry should not be zero */
                if (FAT_Entry.value() == 0)
                {
                    this->error = error::Code::FAT_Corrupted;
                    return false;
                }

                /* Get the return code and pad FAT16 entries with 1, so the same */
                /* decoding of the entry can be used for both FATs */
                const uint32_t code = this->partition.is_fat16 ? FAT_Entry.value() + 0xFFFF0000 : FAT_Entry.value();

                /* Decode the entry */
                if (code == FAT_Code::End_of_File)
                {
                    this->error = error::Code::End_of_File_Reached;
                    return false;
                }
                else if (code == FAT_Code::Bad_Sector)
                {
                    this->error = error::Code::Bad_Sector;
                    return false;
                }
            }

            /* Read the next cluster according to the FAT and return */
            return this->read_cluster(file, FAT_Entry.value());
        }
    };

    template <class Memory>
    auto Volume<Memory>::write_current_sector(Filehandler &file) -> bool
    {
        /* Calculate the block address */
        /* -> The current sector is decremented by 1, because the current sector count starts at 1 */
        const uint32_t block = this->partition.get_LBA_of_cluster(file.current.cluster) +
                               file.current.sector - 1;

        /* Convert the byte buffer to a 32-bit buffer for data transfer */
        const uint32_t *buffer = reinterpret_cast<uint32_t *>(file.block_buffer.begin());

        /* Write the block to memory */
        return drive::write_single_block(this->memory, buffer, block);
    };

    template <class Memory>
    auto Volume<Memory>::get_file(Filehandler &file, const uint32_t id) -> bool
    {
        /* Only when provided filehandle is a directory */
        if (file.is_file())
        {
            this->error = error::Code::Not_a_Directory;
            return false;
        }

        /* Set the ID of the files */
        file.id = id;

        /* Read the block from memory which contains the first file entry */
        if (not this->read_cluster(file, file.start_cluster))
            return false;

        /* Check in which sector the file id lies -> one sector contains 32 entries */
        const uint8_t sector_count = id / (512 / 32);

        /* Read the sectors */
        for (uint8_t count = 0; count < sector_count; count++)
        {
            /* Check whether next sector was found */
            if (not this->read_next_sector_of_cluster(file))
            {
                /* Sector with file id is not allocated -> ID does not exist */
                this->error = error::Code::File_ID_not_found;
                return false;
            }
        }

        /* When block was read, get the file properties */
        file.update_properties_from_buffer();
        return true;
    };

    template <class Memory>
    auto Volume<Memory>::read_root(Filehandler &file) -> bool
    {
        /* Read the block which contains the root cluster */
        const uint32_t block = this->partition.get_LBA_of_root_cluster();

        /* Convert the byte buffer to a 32-bit buffer for transfer with memory */
        const uint32_t *buffer = reinterpret_cast<uint32_t *>(file.block_buffer.begin());

        /* Read the block from memory */
        if (not drive::read_single_block(this->memory, buffer, block))
            return false;

        /* Set the current position in the filehandler */
        file.id = 0;
        file.current.sector = 1;
        file.current.cluster = this->partition.is_fat16 ? 0 : this->partition.Root_Directory_Cluster;
        file.directory_cluster = this->partition.is_fat16 ? 0 : this->partition.Root_Directory_Cluster;
        file.start_cluster = this->partition.is_fat16 ? 0 : this->partition.Root_Directory_Cluster;

        /* Current file buffer contains the root directory! */
        file.update_properties_from_buffer();

        /* Set the file also to containing a directory */
        file.attributes |= Attribute::is_Directory;

        return true;
    };

    template <class Memory>
    auto Volume<Memory>::mount() -> bool
    {
        /* Use the FAT buffer to read mount data */
        const uint32_t *buffer = reinterpret_cast<uint32_t *>(this->FAT.begin());

        /* Read first block */
        if (not drive::read_single_block(this->memory, buffer, 0))
            return false;

        /* check type */
        if (not boot_sector_valid(this->FAT.begin()))
        {
            /* boot section not recognized */
            this->error = error::Code::File_System_Invalid;
            return false;
        }

        /* Check type if partition table */
        if (not boot_is_MBR(this->FAT.begin()))
        {
            /* boot section not recognized */
            this->error = error::Code::File_System_Invalid;
            return false;
        }

        /* Boot section is valid and recognized */
        const uint32_t block_address = get_partition_begin(this->FAT.begin());
        if (not drive::read_single_block(this->memory, buffer, block_address))
            return false;
        this->partition = BPB::initialize_partition(this->FAT.begin(), block_address);
        return true;
    };

    template <class Memory>
    auto Volume<Memory>::get_empty_id(Filehandler &directory) -> std::optional<uint32_t>
    {
        /* Check whether directory was provided */
        if (directory.is_file())
            return {};

        /* The next empty file id */
        uint32_t empty_id = 0;

        /* Read the current sector and see whether there are empty entries */
        for (uint8_t entry_count = 0; entry_count < 16; entry_count++)
        {
            /* Current entry empty? */
            if ((directory.block_buffer[32 * entry_count] == 0x00) || (directory.block_buffer[32 * entry_count] == 0xE5))
                return static_cast<uint32_t>(entry_count);
        }

        /* The current sector does not contain an empty id, so keep reading next sectors */
        while (this->read_next_sector_of_cluster(directory))
        {
            /* One sector was read, so the possible id is incremented by 16 entries */
            empty_id += 16;

            /* Read the current sector and see whether there are empty entries */
            for (uint8_t entry_count = 0; entry_count < 16; entry_count++)
            {
                /* Current entry empty? */
                if ((directory.block_buffer[32 * entry_count] == 0x00) || (directory.block_buffer[32 * entry_count] == 0xE5))
                    return empty_id + entry_count;
            }
        }

        /* No empty id is found */
        return {};
    };

    template <class Memory>
    auto Volume<Memory>::get_next_empty_cluster() -> std::optional<uint32_t>
    {
        /* Limit the number of clusters according to the filesystem */
        const uint32_t max_cluster = this->partition.is_fat16 ? this->partition.FAT_Size * 512 / 16 : this->partition.FAT_Size * 512 / 32;

        /* Check FAT until cluster is empty, valid clusters start with number 2 */
        for (uint32_t empty_cluster = 2; empty_cluster <= max_cluster; empty_cluster++)
        {
            /* Check whether current cluster is empty */
            if (this->read_FAT_entry(empty_cluster).value_or(1) == 0U)
                return empty_cluster;
        }

        /* No empty cluster found -> drive is full */
        this->error = error::Code::No_Memory_Left;
        return {};
    };

    template <class Memory>
    auto Volume<Memory>::get_fileid(
        Filehandler &directory,
        const std::array<char, 12> filename) -> std::optional<uint32_t>
    {
        /* Check whether directory was provided */
        if (directory.is_file())
        {
            this->error = error::Code::Not_a_Directory;
            return {};
        }

        /* The file id */
        uint32_t file_id = 0;

        /* Check current sector for the name */
        for (uint8_t entry = 0; entry < (512 / 32); entry++)
        {
            if (std::equal(
                    filename.begin(),
                    filename.end() - 1,
                    directory.block_buffer.begin() + entry * 32))
            {
                return static_cast<uint32_t>(entry);
            }
        }

        /* The current sector does not contain the fileid, so keep reading next sectors */
        while (this->read_next_sector_of_cluster(directory))
        {
            /* One sector was read, so the possible id is incremented by 16 entries */
            file_id += 16;

            /* Read the current sector and see whether the file is found */
            for (uint8_t entry = 0; entry < (512 / 32); entry++)
            {
                if (std::equal(
                        filename.begin(),
                        filename.end() - 1,
                        directory.block_buffer.begin() + entry * 32))
                {
                    return file_id + entry;
                }
            }
        }

        /* File is not found */
        this->error = error::Code::File_ID_not_found;
        return {};
    };

    template <class Memory>
    auto Volume<Memory>::read_last_sector_of_file(Filehandler &file) -> bool
    {
        /* Get the number of clusters the file occupies */
        const uint32_t cluster_allocated = (file.size / 512) / this->partition.Sectors_per_Cluster;

        /* Read the next clusters until the cluster where the file ends was found */
        uint32_t cluster_offset = file.start_cluster;
        for (uint32_t count = 0; count < cluster_allocated; count++)
        {
            /* Read the FAT to determine the next cluster */
            auto next_cluster = this->read_FAT_entry(cluster_offset);

            /* Check whether entry is valid */
            if (next_cluster)
                cluster_offset = next_cluster.value();
            else
                return false;
        }

        /* Get the sector address of the cluster where the file ends */
        const uint32_t cluster_lba = this->partition.get_LBA_of_cluster(cluster_offset);

        /* Get the sector offset according to the size of the file */
        const uint32_t sector_offset = (file.size / 512) % this->partition.Sectors_per_Cluster;

        /* Convert the byte buffer to a 32-bit buffer for transfer with memory */
        const uint32_t *buffer = reinterpret_cast<uint32_t *>(file.block_buffer.begin());

        /* Read the new block from memory */
        if (not drive::read_single_block(this->memory, buffer, cluster_lba + sector_offset))
            return false;

        /* End of file was found, update file properties */
        file.current.cluster = cluster_offset;
        file.current.sector = sector_offset + 1;
        file.current.byte = static_cast<uint16_t>(
            file.size - ((cluster_allocated * this->partition.Sectors_per_Cluster + sector_offset) * 512));
        return true;
    };

    template <class Memory>
    auto Volume<Memory>::write_filesize_to_directory(Filehandler &file) -> bool
    {
        /* Get the offset of the file id when sector containing it is read */
        /* 16 entries per sector, one sectors has 32 bytes */
        const uint16_t entry_offset = (file.id % 16) * 32;

        /* Read the directory cluster of the file */
        if (not this->read_cluster(file, file.directory_cluster))
            return false;

        /* Read additional sectors if necessary */
        const uint32_t sector_offset = file.id / 16;
        for (uint32_t count = 0; count < sector_offset; count++)
        {
            /* Keep reading sectors */
            if (not this->read_next_sector_of_cluster(file))
                return false;
        }

        /* Update the entry */
        write_long(file.block_buffer.begin(), entry_offset + DIR_Entry::Filesize, file.size);

        /* Write entry to memory again */
        return this->write_current_sector(file);
    };

    template <class Memory>
    auto Volume<Memory>::write_file_to_memory(Filehandler &file) -> bool
    {
        /* Get the current location of the file in memory */
        const uint32_t cluster = file.current.cluster;
        const uint32_t sector = file.current.sector;

        /* Write the file buffer to the memory location */
        if (not this->write_current_sector(file))
            return false;

        /* When the current sector is full, update the filehandle */
        if (file.current.byte == 512)
        {
            /* Reset the byte counter */
            file.current.byte = 0;

            /* Check whether cluster is full */
            if (sector < this->partition.Sectors_per_Cluster)
                file.current.sector++;
            else /* When current cluster is full, allocate the next empty cluster  */
            {
                /* Get next empty cluster */
                auto next_cluster = this->get_next_empty_cluster();

                /* When next cluster was found, allocate it */
                if (not next_cluster)
                    return false;

                /* Set current cluster */
                if (not this->write_FAT_entry(cluster, next_cluster.value()))
                    return false;

                /* Set next cluster to end-of-file */
                const uint32_t entry = this->partition.is_fat16 ? 0xFFFF : 0xFFFFFFFF;
                if (not this->write_FAT_entry(next_cluster.value(), entry))
                    return false;

                /* Update the filehandle */
                file.current.cluster = next_cluster.value();
                file.current.sector = 1;
            }
        }
        return true;
    };

    template <class Memory>
    auto Volume<Memory>::make_directory_entry(
        Filehandler &directory,
        const uint32_t id,
        const uint32_t start_cluster,
        const std::array<char, 12> name,
        const uint8_t attributes,
        const std::time_t time) -> bool
    {
        /* Get the byte offset within the sector */
        const uint32_t offset = (id % 16) * 32;

        /* Write the Name to the entry */
        std::copy(
            name.begin(),
            name.end(),
            directory.block_buffer.begin() + offset);

        /* Set the start cluster of the entry */
        write_short(directory.block_buffer.begin(), DIR_Entry::First_Cluster_L + offset, start_cluster & 0xFFFF);
        if (this->partition.is_fat16)
            write_short(directory.block_buffer.begin(), DIR_Entry::First_Cluster_H + offset, (start_cluster >> 16) & 0U);
        else
            write_short(directory.block_buffer.begin(), DIR_Entry::First_Cluster_H + offset, (start_cluster >> 16) & 0xFFFF);

        /* Set the attributes */
        write_byte(directory.block_buffer.begin(), DIR_Entry::Attributes + offset, attributes);

        /* Convert the seconds since epoch to FAT format */
        std::tm *utc = std::localtime(&time);
        const uint16_t time_entry =
            (utc->tm_hour << Time_Pos::Hours) | (utc->tm_min << Time_Pos::Minutes) | (utc->tm_sec / 2);
        const uint16_t date_entry =
            ((utc->tm_year - 80) << Date_Pos::Year) | ((utc->tm_mon + 1) << Date_Pos::Month) | (utc->tm_mday);

        /* Write time */
        write_short(directory.block_buffer.begin(), offset + DIR_Entry::Creation_Time, time_entry);
        write_short(directory.block_buffer.begin(), offset + DIR_Entry::Write_Time, time_entry);

        /* Write date */
        write_short(directory.block_buffer.begin(), offset + DIR_Entry::Creation_Date, date_entry);
        write_short(directory.block_buffer.begin(), offset + DIR_Entry::Access_Date, date_entry);
        write_short(directory.block_buffer.begin(), offset + DIR_Entry::Write_Date, date_entry);

        // Write file size, 0 for empty file. When writing to file, the file size has to be updated!
        /* Otherwise OEMs will assume the file is still empty. */
        write_long(directory.block_buffer.begin(), offset + DIR_Entry::Filesize, 0U);

        /* After updating the entry, write the sector of the directory to memory */
        return this->write_current_sector(directory);
    };
}; // namespace fat32
