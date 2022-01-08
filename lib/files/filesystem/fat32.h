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
 * @attention The specifications of the FAT file system are taken from the
 * official Microsoft FAT Specification.
 * 
 * <h2><center>&copy; COPYRIGHT(c) 2004 Microsoft Corporation</center></h2>
 */

#ifndef FAT32_H_
#define FAT32_H_

// === Includes ===
#include <array>
#include <algorithm>

namespace FAT32
{
    // === Enums for byte positions
    enum MBR_Pos: unsigned short
    {
        MAGIC_NUMBER    = 0x1FE,
        PART1_TYPE      = 0x1C2,    //char
        PART1_LBA_BEGIN = 0x1C6     //long
    };

    //EFI
    enum EFI_Pos: unsigned short
    {
        TABLE_LBA_BEGIN = 0x48,     //long
        PART_LBA_BEGIN  = 0x20      //long
    };

    //BPB
    enum BPB_Pos: unsigned short
    {
        BYTES_PER_SECTOR    = 0x0B, //short
        SEC_PER_CLUSTER     = 0x0D, //char
        RESERVED_SEC        = 0x0E, //short
        NUMBER_OF_FAT       = 0x10, //byte
        ROOT_ENT_CNT        = 0x11, //short
        TOT_SECTORS_16      = 0x13, //short
        FAT_SIZE_16         = 0x16, //short
        TOT_SECTORS_32      = 0x20, //long
        FAT_SIZE_32         = 0x24, //long
        ROOT_DIR_CLUSTER    = 0x2C, //long
        FAT16_VOLUME_LABEL  = 0x2B, // 11 bytes
        FAT32_VOLUME_LABEL  = 0x47  // 11 bytes
    };

    // Directory/File Property positions
    enum DIR_Entry: unsigned short
    {
        Attributes      = 0x0B,
        Creation_Time   = 0x0E,
        Creation_Date   = 0x10,
        Access_Date     = 0x12,
        Write_Time      = 0x16,
        Write_Date      = 0x18,
        First_Cluster_H = 0x14,
        First_Cluster_L = 0x1A,
        Filesize        = 0x1C
    };

    // Directory/File Attribute Bits
    enum Attribute: unsigned char
    {
        Read_Only    = 0x01,
        Hidden       = 0x02,
        System       = 0x04,
        Volume_ID    = 0x08,
        is_Directory = 0x10,
        Archive      = 0x20,
    };

    //Date and time Bit positions
    enum Date_Pos: unsigned char
    {
        Day = 0, Month = 5, Year = 9
    };
    enum Time_Pos: unsigned char
    {
        Seconds = 0, Minutes = 5, Hours = 11
    };

    // FAT Codes
    enum FAT_Code: unsigned long
    {
        Empty       = 0x00000000,
        End_of_File = 0xFFFFFFFF,
        Bad_Sector  = 0xFFFFFFF8
    };

    // === Classes ===
    struct Partition
    {
        // *** Properties ***
        unsigned long FAT_Begin{0};
        unsigned long FAT_Size{0};
        unsigned long First_Data_Sector{0};
        unsigned long Root_Directory_Cluster{0};
        unsigned long Sectors_per_Cluster{0};
        bool is_fat16{false};
        std::array<char, 12> name{0};

        // *** Methods ***
        unsigned long get_LBA_of_cluster(const unsigned long cluster) const;
        unsigned long get_LBA_of_root_cluster(void) const;
        unsigned long get_FAT_sector(const unsigned long cluster, const unsigned char FAT = 1) const;
        unsigned short get_FAT_position(const unsigned long cluster) const;
        unsigned long read_FAT(const unsigned char* FAT_buffer, const unsigned short position) const;
        void write_FAT(unsigned char* const FAT_buffer, const unsigned short position, const unsigned long data) const;
    };

    struct Filehandler
    {
        // *** Struct to hold current read count ***
        struct Counter
        {
            unsigned short byte{0};
            unsigned long cluster{0};
            unsigned char sector{0};
        };

        // *** File/Directory Properties ***
        unsigned long id{0};
        unsigned char attributes{0};
        unsigned long size{0};
        unsigned long directory_cluster{0};
        unsigned long start_cluster{0};
        std::array<char, 12> name{0};
        std::array<unsigned char,512> block_buffer{0};

        // *** For keeping track of the current location in the drive.
        Counter current{};

        // *** Methods ***
        void update_properties_from_buffer(void);
        bool is_directory(void);
        bool is_file(void);
    };

    // === Functions ===
    unsigned char read_byte(const unsigned char* block, const unsigned short position);
    unsigned short read_short(const unsigned char* block, const unsigned short position);
    unsigned long read_long(const unsigned char* block, const unsigned short position);
    void write_byte(unsigned char* const block, const unsigned short position, const unsigned char data);
    void write_short(unsigned char* const block, const unsigned short position, const unsigned short data);
    void write_long(unsigned char* const block, const unsigned short position, const unsigned long data);
    bool boot_sector_valid(const unsigned char* block);
    bool boot_is_MBR(const unsigned char* block);
    bool boot_is_EFI(const unsigned char* block);
    unsigned long get_partition_begin(const unsigned char* block);

    // === Specialized Groups ===
    namespace EFI
    {
        unsigned long get_table_begin(const unsigned char* block);
        unsigned long get_partition_begin(const unsigned char* block);
    }
    namespace BPB
    {
        unsigned short get_bytes_per_sector(const unsigned char* block);
        unsigned char get_number_of_FAT(const unsigned char* block);
        unsigned long root_directory_sectors(const unsigned char* block);
        unsigned long get_FAT_size(const unsigned char* block);
        unsigned long get_total_sector_size(const unsigned char* block);
        unsigned char get_reserved_sectors(const unsigned char* block);
        unsigned char get_sectors_per_cluster(const unsigned char* block);
        unsigned long get_root_directory_cluster(const unsigned char* block);
        Partition initialize_partition(const unsigned char* block, const unsigned long partition_begin);
    };
};

#endif