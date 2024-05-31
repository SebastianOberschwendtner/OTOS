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
 * @attention The specifications of the FAT file system are taken from the
 * official Microsoft FAT Specification.
 *
 * <h2><center>&copy; COPYRIGHT(c) 2004 Microsoft Corporation</center></h2>
 */

#ifndef FAT32_H_
#define FAT32_H_

/* === Includes === */
#include <algorithm>
#include <array>
#include <misc/types.h>

namespace fat32
{
    /* === Enums for byte positions */
    enum MBR_Pos : uint16_t
    {
        MAGIC_NUMBER = 0x1FE,
        PART1_TYPE = 0x1C2,     // char
        PART1_LBA_BEGIN = 0x1C6 // long
    };

    // EFI
    enum EFI_Pos : uint16_t
    {
        TABLE_LBA_BEGIN = 0x48, // long
        PART_LBA_BEGIN = 0x20   // long
    };

    // BPB
    enum BPB_Pos : uint16_t
    {
        BYTES_PER_SECTOR = 0x0B,   // short
        SEC_PER_CLUSTER = 0x0D,    // char
        RESERVED_SEC = 0x0E,       // short
        NUMBER_OF_FAT = 0x10,      // byte
        ROOT_ENT_CNT = 0x11,       // short
        TOT_SECTORS_16 = 0x13,     // short
        FAT_SIZE_16 = 0x16,        // short
        TOT_SECTORS_32 = 0x20,     // long
        FAT_SIZE_32 = 0x24,        // long
        ROOT_DIR_CLUSTER = 0x2C,   // long
        FAT16_VOLUME_LABEL = 0x2B, /* 11 bytes */
        FAT32_VOLUME_LABEL = 0x47  /* 11 bytes */
    };

    /* Directory/File Property positions */
    enum DIR_Entry : uint16_t
    {
        Attributes = 0x0B,
        Creation_Time = 0x0E,
        Creation_Date = 0x10,
        Access_Date = 0x12,
        Write_Time = 0x16,
        Write_Date = 0x18,
        First_Cluster_H = 0x14,
        First_Cluster_L = 0x1A,
        Filesize = 0x1C
    };

    /* Directory/File Attribute Bits */
    enum Attribute : uint8_t
    {
        Read_Only = 0x01,
        Hidden = 0x02,
        System = 0x04,
        Volume_ID = 0x08,
        is_Directory = 0x10,
        Archive = 0x20,
    };

    // Date and time Bit positions
    enum Date_Pos : uint8_t
    {
        Day = 0,
        Month = 5,
        Year = 9
    };
    enum Time_Pos : uint8_t
    {
        Seconds = 0,
        Minutes = 5,
        Hours = 11
    };

    /* FAT Codes */
    enum FAT_Code : uint32_t
    {
        Empty = 0x00000000,
        End_of_File = 0xFFFFFFFF,
        Bad_Sector = 0xFFFFFFF8
    };

    /* === Classes === */
    struct Partition
    {
        /* === Methods === */
        /**
         * @brief Get the byte position of a cluster within a FAT sector.
         *
         * @param cluster The number of the current cluster.
         * @return The byte position of the cluster.
         */
        auto get_FAT_position(const uint32_t cluster) const -> uint16_t;

        /**
         * @brief Get the FAT sector for a specific cluster.
         *
         * @param cluster The current cluster to get the FAT entry for.
         * @param FAT The index of the FAT. Should be [1..2], defaults to 1.
         * @return The sector of the cluster according to the FAT.
         */
        auto get_FAT_sector(const uint32_t cluster, const uint8_t FAT = 1) const -> uint32_t;

        /**
         * @brief Compute the LBA begin address of a specific cluster
         * Cluster numbering begins at 2!
         *
         * Cluster number 0 is an exception of FAT16.
         *
         * @param cluster The cluster of which the address should be computed.
         * @return LBA begin address of cluster.
         */
        auto get_LBA_of_cluster(const uint32_t cluster) const -> uint32_t;

        /**
         * @brief Compute the LBA begin address of the root cluster.
         * @return LBA begin address of root cluster.
         */
        auto get_LBA_of_root_cluster() const -> uint32_t;

        /**
         * @brief Read the FAT entry of the loaded Sector and return the content.
         * @param FAT_buffer Pointer to the data buffer.
         * @param position The position inside the FAT.
         * @return The content of the FAT at the given position.
         */
        auto read_FAT(const uint8_t *FAT_buffer, const uint16_t position) const -> uint32_t;

        /**
         * @brief Write the FAT entry of the loaded Sector.
         * @param FAT_buffer Pointer to the data buffer.
         * @param position The position inside the FAT.
         * @param data The data to write to the FAT.
         */
        void write_FAT(uint8_t *const FAT_buffer, const uint16_t position, const uint32_t data) const;

        /* === Properties === */
        uint32_t FAT_Begin{0};              /**< The block where the FAT begins. */
        uint32_t FAT_Size{0};               /**< The size of the FAT in blocks. */
        uint32_t First_Data_Sector{0};      /**< The first sector of the data area. */
        uint32_t Root_Directory_Cluster{0}; /**< The cluster of the root directory. */
        uint32_t Sectors_per_Cluster{0};    /**< The number of sectors per cluster. */
        bool is_fat16{false};               /**< True when the FAT is a FAT16. */
        std::array<char, 12> name{0};       /**< The name of the volume. */
    };

    struct Filehandler
    {
        /* === Struct to hold current read count === */
        struct Counter
        {
            uint16_t byte{0};    /**< The current byte position. */
            uint32_t cluster{0}; /**< The current cluster. */
            uint8_t sector{0};   /**< The current sector. */
        };

        /* === File/Directory Properties === */
        uint32_t id{0};                           /**< The id of the file. */
        uint8_t attributes{0};                    /**< The attributes of the file. */
        uint32_t size{0};                         /**< [bytes] The size of the file. */
        uint32_t directory_cluster{0};            /**< The cluster of the directory. */
        uint32_t start_cluster{0};                /**< The cluster of the first data block. */
        std::array<char, 12> name{0};             /**< The name of the file. */
        std::array<uint8_t, 512> block_buffer{0}; /**< The buffer for the file data. */
        Counter current{};                        /**< For keeping track of the current write/read position. */

        /* === Methods === */
        /**
         * @brief Check whether filehandler contains a directory.
         *
         * @return Returns True when filehandler represents a directory.
         */
        bool is_directory();

        /**
         * @brief Check whether filehandler contains a file.
         *
         * @return Returns True when filehandler represents a file.
         */
        bool is_file();

        /**
         * @brief Read the file/directory properties from the
         * block buffer after reading the correct sector from
         * the memory.
         *
         * @attention The file ID has to be set beforehand!
         */
        void update_properties_from_buffer();
    };

    /* === Functions === */
    /**
     * @brief Check whether the boot sector contains
     * a master boot record (MBR) as partition table.
     *
     * @param block The pointer to the block data buffer.
     * @return Returns True when the drive has a supported MBR.
     */
    auto boot_is_MBR(const uint8_t *block) -> bool;

    /**
     * @brief Check whether the boot sector contains
     * a EFI as partition table.
     *
     * @param block The pointer to the block data buffer.
     * @return Returns True when the drive has a supported EFI.
     */
    auto boot_is_EFI(const uint8_t *block) -> bool;

    /**
     * @brief Check whether the block contains the magic numer (0xAA55)
     * at the end. Only boot sectors who follow this convention are
     * supported and valid!
     *
     * @param block The pointer to the block data buffer.
     * @return Returns True when the Magic Number is present and the boot sector is valid
     */
    auto boot_sector_valid(const uint8_t *block) -> bool;

    /**
     * @brief Get the block address of the beginning of
     * the partition table.
     *
     * @param block The pointer to the block data buffer.
     * @return The block address of the next block to read.
     */
    auto get_partition_begin(const uint8_t *block) -> uint32_t;

    /**
     * @brief Read a byte from the provided block data buffer.
     *
     * @param block The pointer to the block data buffer.
     * @param position The byte number to read from the block. Should be [0..511].
     * @return Returns the byte in the buffer at the specified position.
     */
    auto read_byte(const uint8_t *block, const uint16_t position) -> uint8_t;

    /**
     * @brief Read a long from the provided block data buffer.
     *
     * @param block The pointer to the block data buffer.
     * @param position The byte number to read from the block. Should be [0..508].
     * @return Returns the long in the buffer at the specified position.
     */
    auto read_long(const uint8_t *block, const uint16_t position) -> uint32_t;

    /**
     * @brief Read an short from the provided block data buffer.
     *
     * @param block The pointer to the block data buffer.
     * @param position The byte number to read from the block. Should be [0..510].
     * @return Returns the short in the buffer at the specified position.
     */
    auto read_short(const uint8_t *block, const uint16_t position) -> uint16_t;

    /**
     * @brief Write a byte to the block data buffer.
     *
     * @param block The pointer to the block data buffer.
     * @param position The byte position where the data should be written to.
     * @param data The data byte.
     */
    void write_byte(uint8_t *const block, const uint16_t position, const uint8_t data);

    /**
     * @brief Write a long to the block data buffer.
     *
     * @param block The pointer to the block data buffer.
     * @param position The byte position where the data should be written to.
     * @param data The data long.
     */
    void write_long(uint8_t *const block, const uint16_t position, const uint32_t data);

    /**
     * @brief Write an short to the block data buffer.
     *
     * @param block The pointer to the block data buffer.
     * @param position The byte position where the data should be written to.
     * @param data The data short.
     */
    void write_short(uint8_t *const block, const uint16_t position, const uint16_t data);

    /* === Specialized Groups === */
    namespace EFI
    {
        /**
         * @brief Get the block address of the beginning of
         * the EFI file system.
         *
         * @param block The pointer to the block data buffer.
         * @return The block address of the next block to read.
         * @todo Add parameter to select one of the four possible partitions in the MBR.
         */
        auto get_partition_begin(const uint8_t *block) -> uint32_t;

        /**
         * @brief Get the block address of the beginning of
         * the EFI partition table.
         *
         * @param block The pointer to the block data buffer.
         * @return The block address of the next block to read.
         */
        auto get_table_begin(const uint8_t *block) -> uint32_t;
    }; // namespace EFI
    namespace BPB
    {
        /**
         * @brief Initialize a partition from the BPB block.
         * The BPB block has to be available in the given
         * block buffer.
         *
         * @param block Block buffer which contains the BPB sector.
         * @param partition_begin The block address where the BPB block was read from.
         * @return Returns the initialized partition block
         */
        auto initialize_partition(const uint8_t *block, const uint32_t partition_begin) -> Partition;

        /**
         * @brief Read the number of bytes per sector
         * as defined in the Bios Parameter Block (BPB).
         *
         * @param block The pointer to the block data buffer.
         * @return The number of bytes per sector.
         */
        auto get_bytes_per_sector(const uint8_t *block) -> uint16_t;

        /**
         * @brief Read the number of sectors the FAT occupies
         * as defined in the Bios Parameter Block (BPB).
         *
         * @param block The pointer to the block data buffer.
         * @return The number of sectors of FAT.
         */
        auto get_FAT_size(const uint8_t *block) -> uint32_t;

        /**
         * @brief Read the number of file allocation tables (FATs)
         * as defined in the Bios Parameter Block (BPB).
         *
         * @param block The pointer to the block data buffer.
         * @return The number of FATs.
         */
        auto get_number_of_FAT(const uint8_t *block) -> uint8_t;

        /**
         * @brief Read the number of reserved sectors
         * as defined in the Bios Parameter Block (BPB).
         *
         * @param block The pointer to the block data buffer.
         * @return The number of reserved sectors.
         */
        auto get_reserved_sectors(const uint8_t *block) -> uint8_t;

        /**
         * @brief Get the root directory cluster from the BPB.
         * 
         * @param block  The pointer to the block data buffer.
         * @return uint32_t The root directory cluster.
         */
        auto get_root_directory_cluster(const uint8_t *block) -> uint32_t;

        /**
         * @brief Read the number of sectors per cluster
         * as defined in the Bios Parameter Block (BPB).
         *
         * @param block The pointer to the block data buffer.
         * @return The number of sectors per cluster.
         */
        auto get_sectors_per_cluster(const uint8_t *block) -> uint8_t;

        /**
         * @brief Read the number of total sectors the data occupies
         * as defined in the Bios Parameter Block (BPB).
         *
         * @param block The pointer to the block data buffer.
         * @return The number of sectors of the data.
         */
        auto get_total_sector_size(const uint8_t *block) -> uint32_t;

        /**
         * @brief Read the number of sectors the root directory occupies
         * as defined in the Bios Parameter Block (BPB).
         *
         * @param block The pointer to the block data buffer.
         * @return The number of sectors of the root directory.
         */
        auto root_directory_sectors(const uint8_t *block) -> uint32_t;
    }; // namespace BPB
};    // namespace fat32

#endif