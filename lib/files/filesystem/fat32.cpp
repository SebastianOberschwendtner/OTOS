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
 * @file    fat32.cpp
 * @author  SO
 * @version v2.7.0
 * @date    03-January-2022
 * @brief   Interface for FAT32 filesystems. Also supports FAT16.
 ******************************************************************************
 */

// === Includes ===
#include "filesystem/fat32.h"

// === Class Methods ===

/**
 * @brief Compute the LBA begin address of a specific cluster
 * Cluster numbering begins at 2!
 * 
 * Cluster number 0 is an exception of FAT16.
 * 
 * @param cluster The cluster of which the address should be computed.
 * @return LBA begin address of cluster.
 */
unsigned long FAT32::Partition::get_LBA_of_cluster( const unsigned long cluster) const
{
    if (cluster >= 2) 
        return this->First_Data_Sector + (cluster - 2)*this->Sectors_per_Cluster;
    else
        return this->FAT_Begin + (2 * this->FAT_Size); // Root directory of FAT16
};

/**
 * @brief Compute the LBA begin address of the root cluster.
 * @return LBA begin address of root cluster.
 */
unsigned long FAT32::Partition::get_LBA_of_root_cluster( void ) const
{
    if (this->is_fat16)
        return this->FAT_Begin + 2*this->FAT_Size;
    else
        return this->Root_Directory_Cluster;
};

/**
 * @brief Get the FAT sector for a specific cluster.
 * 
 * @param cluster The current cluster to get the FAT entry for.
 * @param FAT The index of the FAT. Should be [1..2], defaults to 1.
 * @return The sector of the cluster according to the FAT.
 */
unsigned long FAT32::Partition::get_FAT_sector(const unsigned long cluster, const unsigned char FAT) const
{
    const unsigned long FAT_offset = this->is_fat16 ? cluster*2 : cluster*4;
    return this->FAT_Begin + (FAT_offset / 512) + (this->FAT_Size * (FAT-1));
};

/**
 * @brief Get the byte position of a cluster within a FAT sector.
 * 
 * @param cluster The number of the current cluster.
 * @return The byte position of the cluster.
 */
unsigned short FAT32::Partition::get_FAT_position( const unsigned long cluster) const
{
    const unsigned long FAT_offset = this->is_fat16 ? cluster*2 : cluster*4;
    return static_cast<unsigned short>(FAT_offset % 512);
};

/**
 * @brief Read the FAT entry of the loaded Sector and return the content.
 * @param FAT_buffer Poshorter to the data buffer.
 * @param position The position inside the FAT.
 * @return The content of the FAT at the given position.
 */
unsigned long FAT32::Partition::read_FAT(const unsigned char* FAT_buffer, const unsigned short position) const
{
    if (this->is_fat16)
        return static_cast<unsigned long>(read_short(FAT_buffer, position));
    else
        return read_long(FAT_buffer, position);
};

void FAT32::Partition::write_FAT(
    unsigned char* const FAT_buffer,
    const unsigned short position,
    const unsigned long data
    ) const
{
    if (this->is_fat16)
        write_short(FAT_buffer, position, static_cast<unsigned short>(data));
    else
        write_long(FAT_buffer, position, data);
};

/**
 * @brief Read the file/directory properties from the
 * block buffer after reading the correct sector from
 * the memory.
 * 
 * The file ID has to be set beforehand!
 */
void FAT32::Filehandler::update_properties_from_buffer(void)
{
    /*
    * The fileid is a consecutive number over all sectors of the directory. To get the specific entry within one sector,
    * the remainder of the entries per sector has to be used to access the information in the buffer.
    */
    const unsigned short entry_offset = static_cast<unsigned short>(this->id % (512 / 32));
    const unsigned short pos_offset = entry_offset * 32;

    // Read the name
    std::copy(
        this->block_buffer.begin() + pos_offset,
        this->block_buffer.begin() + pos_offset + 11,
        this->name.begin()
        );

    // Read the attribute byte
    this->attributes = read_byte(this->block_buffer.begin(), pos_offset + DIR_Entry::Attributes);

    // Save current cluster as directory cluster
    this->directory_cluster = this->current.cluster;

    // Read the start cluster of the file
    this->start_cluster = static_cast<unsigned long>(read_short(this->block_buffer.begin(), pos_offset + DIR_Entry::First_Cluster_L));

    // FAT32 uses additional bytes (have to be all 0 for FAT16 drives according to specs from Microsoft!)
    this->start_cluster |= (static_cast<unsigned long>(read_short(this->block_buffer.begin(), pos_offset + DIR_Entry::First_Cluster_H)) << 16);

    // Read the file size
    this->size = read_long(this->block_buffer.begin(), pos_offset + DIR_Entry::Filesize);
};

/**
 * @brief Check whether filehandler contains a directory.
 * 
 * @return Returns True when filehandler represents a directory.
 */
bool FAT32::Filehandler::is_directory(void)
{
    return this->attributes & Attribute::is_Directory;
};

/**
 * @brief Check whether filehandler contains a file.
 * 
 * @return Returns True when filehandler represents a file.
 */
bool FAT32::Filehandler::is_file(void)
{
    return not this->is_directory();
};

// === Functions ===
/**
 * @brief Read a byte from the provided block data buffer.
 * 
 * @param block The poshorter to the block data buffer.
 * @param position The byte number to read from the block. Should be [0..511].
 * @return Returns the byte in the buffer at the specified position.
 */
unsigned char FAT32::read_byte(const unsigned char* block, const unsigned short position)
{
    return *(block + position);
};

/**
 * @brief Read an short from the provided block data buffer.
 * 
 * @param block The poshorter to the block data buffer.
 * @param position The byte number to read from the block. Should be [0..510].
 * @return Returns the short in the buffer at the specified position.
 */
unsigned short FAT32::read_short(const unsigned char* block, const unsigned short position)
{
    const unsigned char byte0 = *(block + position);
    const unsigned char byte1 = *(block + position + 1);
    return (byte1 << 8) | byte0;
};

/**
 * @brief Read a long from the provided block data buffer.
 * 
 * @param block The poshorter to the block data buffer.
 * @param position The byte number to read from the block. Should be [0..508].
 * @return Returns the long in the buffer at the specified position.
 */
unsigned long FAT32::read_long(const unsigned char* block, const unsigned short position)
{
    const unsigned char byte0 = block[position];
    const unsigned char byte1 = *(block + position + 1);
    const unsigned char byte2 = *(block + position + 2);
    const unsigned char byte3 = *(block + position + 3);
    // Assemble the output value, the 0 is for unit testing on sytems where long is 64bit (looking at you linux!)
    return 0U | (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
};

/**
 * @brief Write a byte to the block data buffer.
 * 
 * @param block The poshorter to the block data buffer.
 * @param position The byte position where the data should be written to.
 * @param data The data byte.
 */
void FAT32::write_byte(
    unsigned char* const block,
    const unsigned short position,
    const unsigned char data
)
{
    *(block + position) = data;
};

/**
 * @brief Write an short to the block data buffer.
 * 
 * @param block The poshorter to the block data buffer.
 * @param position The byte position where the data should be written to.
 * @param data The data short.
 */
void FAT32::write_short(
    unsigned char* const block,
    const unsigned short position,
    const unsigned short data
)
{
    *(block + position) = static_cast<unsigned char>(data & 0xFF);
    *(block + position + 1) = static_cast<unsigned char>((data >> 8) & 0xFF);
};

/**
 * @brief Write a long to the block data buffer.
 * 
 * @param block The poshorter to the block data buffer.
 * @param position The byte position where the data should be written to.
 * @param data The data long.
 */
void FAT32::write_long(
    unsigned char* const block,
    const unsigned short position,
    const unsigned long data
)
{
    *(block + position) = static_cast<unsigned char>(data & 0xFF);
    *(block + position + 1) = static_cast<unsigned char>((data >> 8) & 0xFF);
    *(block + position + 2) = static_cast<unsigned char>((data >> 16) & 0xFF);
    *(block + position + 3) = static_cast<unsigned char>((data >> 24) & 0xFF);
};

/**
 * @brief Check whether the block contains the magic numer (0xAA55)
 * at the end. Only boot sectors who follow this convention are
 * supported and valid!
 * 
 * @param block The poshorter to the block data buffer.
 * @return Returns True when the Magic Number is present and the boot sector is valid
 */
bool FAT32::boot_sector_valid( const unsigned char* block)
{
    return read_short(block, MAGIC_NUMBER) == 0xAA55;
};

/**
 * @brief Check whether the boot sector contains
 * a master boot record (MBR) as partition table.
 * 
 * @param block The poshorter to the block data buffer.
 * @return Returns True when the drive has a supported MBR.
 */
bool FAT32::boot_is_MBR( const unsigned char* block)
{
    const unsigned char type = read_byte(block, PART1_TYPE);
    return (type == 0x04) | (type == 0x14)  // FAT16 < 32 MiB
        | (type == 0x06) | (type == 0x16)   // FAT16 > 32 MiB
        | (type == 0x0C) | (type == 0x1C)   // FAT32 with LBA
        | (type == 0x0E) | (type == 0x1E);  // FAT16 > 32 MiB with LBA
};

/**
 * @brief Check whether the boot sector contains
 * a EFI as partition table.
 * 
 * @param block The poshorter to the block data buffer.
 * @return Returns True when the drive has a supported EFI.
 */
bool FAT32::boot_is_EFI( const unsigned char* block)
{
    return read_byte(block, PART1_TYPE) == 0xEE;
};

/**
 * @brief Get the block address of the beginning of
 * the partition table.
 * 
 * @param block The poshorter to the block data buffer.
 * @return The block address of the next block to read.
 */
unsigned long FAT32::get_partition_begin( const unsigned char* block)
{
    return read_long(block, PART1_LBA_BEGIN);
};

/**
 * @brief Get the block address of the beginning of
 * the EFI partition table.
 * 
 * @param block The poshorter to the block data buffer.
 * @return The block address of the next block to read.
 */
unsigned long FAT32::EFI::get_table_begin( const unsigned char* block)
{
    return read_long(block, TABLE_LBA_BEGIN);
};

/**
 * @brief Get the block address of the beginning of
 * the EFI file system.
 * 
 * @param block The poshorter to the block data buffer.
 * @return The block address of the next block to read.
 * @todo Add parameter to select one of the four possible partitions in the MBR.
 */
unsigned long FAT32::EFI::get_partition_begin( const unsigned char* block)
{
    return read_long(block, PART_LBA_BEGIN);
};

/**
 * @brief Read the number of bytes per sector
 * as defined in the Bios Parameter Block (BPB).
 * 
 * @param block The poshorter to the block data buffer.
 * @return The number of bytes per sector.
 */
unsigned short FAT32::BPB::get_bytes_per_sector(const unsigned char* block)
{
    return read_short(block, BYTES_PER_SECTOR);
};

/**
 * @brief Read the number of file allocation tables (FATs)
 * as defined in the Bios Parameter Block (BPB).
 * 
 * @param block The poshorter to the block data buffer.
 * @return The number of FATs.
 */
unsigned char FAT32::BPB::get_number_of_FAT(const unsigned char* block)
{
    return read_byte(block, NUMBER_OF_FAT);
};

/**
 * @brief Read the number of sectors the root directory occupies
 * as defined in the Bios Parameter Block (BPB).
 * 
 * @param block The poshorter to the block data buffer.
 * @return The number of sectors of the root directory.
 */
unsigned long FAT32::BPB::root_directory_sectors(const unsigned char* block)
{
    const unsigned short bytes_per_sector = get_bytes_per_sector(block);
    const unsigned short root_entry_count = read_short(block, ROOT_ENT_CNT);
    return ((root_entry_count * 32) + bytes_per_sector - 1) / bytes_per_sector;
};

/**
 * @brief Read the number of sectors the FAT occupies
 * as defined in the Bios Parameter Block (BPB).
 * 
 * @param block The poshorter to the block data buffer.
 * @return The number of sectors of FAT.
 */
unsigned long FAT32::BPB::get_FAT_size(const unsigned char* block)
{
    // check whether filesystem is FAT16
    const unsigned short fat16_size = read_short(block, FAT_SIZE_16);
    if(fat16_size != 0)
        return static_cast<unsigned long>(fat16_size);

    // File system is FAT32
    return read_long(block, FAT_SIZE_32);
};


/**
 * @brief Read the number of total sectors the data occupies
 * as defined in the Bios Parameter Block (BPB).
 * 
 * @param block The poshorter to the block data buffer.
 * @return The number of sectors of the data.
 */
unsigned long FAT32::BPB::get_total_sector_size(const unsigned char* block)
{
    // check whether filesystem is FAT16
    const unsigned short fat16_size = read_short(block, TOT_SECTORS_16);
    if(fat16_size != 0)
        return static_cast<unsigned long>(fat16_size);

    // File system is FAT32
    return read_long(block, TOT_SECTORS_32);
};

unsigned char FAT32::BPB::get_reserved_sectors(const unsigned char* block)
{
    return read_byte(block, RESERVED_SEC);
};

unsigned char FAT32::BPB::get_sectors_per_cluster(const unsigned char* block)
{
    return read_byte(block, SEC_PER_CLUSTER);
};

unsigned long FAT32::BPB::get_root_directory_cluster(const unsigned char* block)
{
    return read_long(block, ROOT_DIR_CLUSTER);
};

/**
 * @brief Initialize a partition from the BPB block.
 * The BPB block has to be available in the given
 * block buffer.
 * 
 * @param block Block buffer which contains the BPB sector.
 * @param partition_begin The block address where the BPB block was read from.
 * @return Returns the initialized partition block
 */
FAT32::Partition FAT32::BPB::initialize_partition(
    const unsigned char* block,
    const unsigned long partition_begin
)
{
    // Read needed values from the BPB
    const unsigned long root_dir_sectors = root_directory_sectors(block);
    const unsigned long FAT_size = get_FAT_size(block);
    const unsigned long total_sectors = get_total_sector_size(block);
    const unsigned char reserved_sectors = get_reserved_sectors(block);
    const unsigned char number_FAT = get_number_of_FAT(block);
    const unsigned long sector_per_cluster = get_sectors_per_cluster(block);

    // Calculate dependent parameters
    const unsigned long data_sectors = total_sectors - ( reserved_sectors + number_FAT*FAT_size ) + root_dir_sectors;
    const unsigned long cluster_count = data_sectors / sector_per_cluster;
    const unsigned long lba_begin = partition_begin + reserved_sectors;
    const unsigned long first_data_sector = lba_begin + (2*FAT_size) + root_dir_sectors;

    // Check whether FAT16 is used
    const bool is_fat16 = cluster_count < 65525 ? true : false;
    const unsigned long root_dir_cluster = is_fat16 ? 0 : get_root_directory_cluster(block);

    // Get the volume label
    const unsigned short label_position = is_fat16 ? FAT16_VOLUME_LABEL : FAT32_VOLUME_LABEL;
    std::array<char, 12> label{0};
    std::copy(
        block + label_position,
        block + label_position + 11,
        label.begin()
    );

    // Construct the volume with the derived values
    return Partition{
        lba_begin,
        FAT_size,
        first_data_sector,
        root_dir_cluster,
        sector_per_cluster,
        is_fat16,
        label
        };
};