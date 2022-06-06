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
 * @file    test_fat32.cpp
 * @author  SO
 * @version v2.7.3
 * @date    03-January-2022
 * @brief   Unit tests for testing the FAT32 shorterface.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>
#include "filesystem/fat32.h"

/** === Test List ===

*/

// === Tests ===
void setUp(void) {
    // set stuff up here
};

void tearDown(void) {
    // clean stuff up here
};

/// @brief Test reading data from the block buffer
void test_read_data(void)
{
    // Setup the block buffer
    std::array<unsigned char, 512> buffer;
    buffer[0] = 0x01;
    buffer[1] = 0x02;
    buffer[2] = 0x03;
    buffer[3] = 0x04;
    buffer[510] = 0x55;
    buffer[511] = 0xAA;

    // Read byte
    TEST_ASSERT_EQUAL( 0x01, FAT32::read_byte(buffer.begin(), 0) );
    TEST_ASSERT_EQUAL( 0x02, FAT32::read_byte(buffer.begin(), 1) );
    TEST_ASSERT_EQUAL( 0xAA, FAT32::read_byte(buffer.begin(), 511) );

    // Read short
    TEST_ASSERT_EQUAL( 0x0201, FAT32::read_short(buffer.begin(), 0) );
    TEST_ASSERT_EQUAL( 0xAA55, FAT32::read_short(buffer.begin(), 510) );

    // Read long
    TEST_ASSERT_EQUAL( 0x04030201, FAT32::read_long(buffer.begin(), 0) );
};

/// @brief Test writing data from the block buffer
void test_write_data(void)
{
    // Setup the block buffer
    std::array<unsigned char, 512> buffer{0};

    // Write byte
    FAT32::write_byte(buffer.begin(), 0, 0x01);
    TEST_ASSERT_EQUAL( 0x01, buffer[0] );
    FAT32::write_byte(buffer.begin(), 1, 0x02);
    TEST_ASSERT_EQUAL( 0x02, buffer[1] );
    FAT32::write_byte(buffer.begin(), 511, 0xAA);
    TEST_ASSERT_EQUAL( 0xAA, buffer[511] );

    // Write short
    buffer.fill(0);
    FAT32::write_short(buffer.begin(), 0, 0x0201);
    TEST_ASSERT_EQUAL( 0x01, buffer[0]);
    TEST_ASSERT_EQUAL( 0x02, buffer[1] );
    FAT32::write_short(buffer.begin(), 510, 0xAA55);
    TEST_ASSERT_EQUAL( 0x55, buffer[510]);
    TEST_ASSERT_EQUAL( 0xAA, buffer[511] );

    // Write long
    buffer.fill(0);
    FAT32::write_long(buffer.begin(), 0, 0x04030201);
    TEST_ASSERT_EQUAL( 0x01, buffer[0]);
    TEST_ASSERT_EQUAL( 0x02, buffer[1]);
    TEST_ASSERT_EQUAL( 0x03, buffer[2]);
    TEST_ASSERT_EQUAL( 0x04, buffer[3]);
};

/// @brief Test reading the bootsector
void test_read_bootsector(void)
{
    // Setup the block buffer
    std::array<unsigned char, 512> buffer{0};

    // Test reading the Magic number
    TEST_ASSERT_FALSE( FAT32::boot_sector_valid(buffer.begin()) );
    buffer[510] = 0x55;
    buffer[511] = 0xAA;
    TEST_ASSERT_TRUE( FAT32::boot_sector_valid(buffer.begin()) );

    // Test reading checking the partition table type
    TEST_ASSERT_FALSE( FAT32::boot_is_MBR(buffer.begin()) );
    buffer[450] = 0x04;
    TEST_ASSERT_TRUE( FAT32::boot_is_MBR(buffer.begin()) );
    buffer[450] = 0x14;
    TEST_ASSERT_TRUE( FAT32::boot_is_MBR(buffer.begin()) );
    buffer[450] = 0x06;
    TEST_ASSERT_TRUE( FAT32::boot_is_MBR(buffer.begin()) );
    buffer[450] = 0x16;
    TEST_ASSERT_TRUE( FAT32::boot_is_MBR(buffer.begin()) );
    buffer[450] = 0x0C;
    TEST_ASSERT_TRUE( FAT32::boot_is_MBR(buffer.begin()) );
    buffer[450] = 0x1C;
    TEST_ASSERT_TRUE( FAT32::boot_is_MBR(buffer.begin()) );
    buffer[450] = 0x0E;
    TEST_ASSERT_TRUE( FAT32::boot_is_MBR(buffer.begin()) );
    buffer[450] = 0x1E;
    TEST_ASSERT_TRUE( FAT32::boot_is_MBR(buffer.begin()) );

    // Test reading checking the partition table type
    TEST_ASSERT_FALSE( FAT32::boot_is_EFI(buffer.begin()) );
    buffer[450] = 0xEE;
    TEST_ASSERT_TRUE( FAT32::boot_is_EFI(buffer.begin()) );

    // Test reading the begin address of the partition table
    TEST_ASSERT_EQUAL( 0, FAT32::get_partition_begin(buffer.begin()) );
    buffer[457] = 0x12;
    buffer[456] = 0x13;
    buffer[455] = 0x14;
    buffer[454] = 0x15;
    TEST_ASSERT_EQUAL( 0x12131415, FAT32::get_partition_begin(buffer.begin()) );
};

/// @brief test reading the EFI partition table
void test_read_EFI(void)
{
    // Setup the block buffer
    std::array<unsigned char, 512> buffer{0};

    // Test reading the beginning of the table
    TEST_ASSERT_EQUAL( 0, FAT32::EFI::get_table_begin(buffer.begin()) );
    buffer[72] = 0x01;
    buffer[73] = 0x02;
    buffer[74] = 0x03;
    buffer[75] = 0x04;
    TEST_ASSERT_EQUAL( 0x04030201, FAT32::EFI::get_table_begin(buffer.begin()) );

    // Test reading the actual beginning of the file system
    TEST_ASSERT_EQUAL( 0, FAT32::EFI::get_partition_begin(buffer.begin()) );
    buffer[20] = 0x01;
    buffer[21] = 0x02;
    buffer[22] = 0x03;
    buffer[23] = 0x04;
    TEST_ASSERT_EQUAL( 0x04030201, FAT32::EFI::get_table_begin(buffer.begin()) );
};

/// @brief Test reading the bios parameter block (BPB)
void test_read_BPB(void)
{
    // Setup the block buffer
    std::array<unsigned char, 512> buffer{0};

    // Test reading the bytes per sector
    buffer[12] = 0x02;
    TEST_ASSERT_EQUAL( 512, FAT32::BPB::get_bytes_per_sector(buffer.begin()) );

    // Test reading the number of FATs
    buffer[16] = 0x02;
    TEST_ASSERT_EQUAL( 2, FAT32::BPB::get_number_of_FAT(buffer.begin()) );

    // Test reading the number of sectors occupied by the root directory
    buffer[17] = 0x41;
    buffer[18] = 0x00;
    TEST_ASSERT_EQUAL( 5, FAT32::BPB::root_directory_sectors(buffer.begin()) );

    // Test reading the count of sectors the FAT occupies
    buffer[22] = 0x00;
    buffer[36] = 0x32;
    TEST_ASSERT_EQUAL( 0x32, FAT32::BPB::get_FAT_size(buffer.begin()) );
    buffer[22] = 0x16;
    buffer[36] = 0x32;
    TEST_ASSERT_EQUAL( 0x16, FAT32::BPB::get_FAT_size(buffer.begin()) );

    // Test reading the count of sectors in the data region of the volume
    buffer[19] = 0x00;
    buffer[32] = 0x32;
    TEST_ASSERT_EQUAL( 0x32, FAT32::BPB::get_total_sector_size(buffer.begin()) );
    buffer[19] = 0x16;
    buffer[32] = 0x32;
    TEST_ASSERT_EQUAL( 0x16, FAT32::BPB::get_total_sector_size(buffer.begin()) );

    // Test reading the number of reserved sectors
    buffer[14] = 0x04;
    TEST_ASSERT_EQUAL( 0x04, FAT32::BPB::get_reserved_sectors(buffer.begin()) );

    // Test reading the number of sectors per cluster
    buffer[13] = 64;
    TEST_ASSERT_EQUAL( 64, FAT32::BPB::get_sectors_per_cluster(buffer.begin()) );

    // Test reading the address of the root directory cluster
    buffer[44] = 0x11;
    TEST_ASSERT_EQUAL( 0x11, FAT32::BPB::get_root_directory_cluster(buffer.begin()) );

};

/// @brief Test the initialization of a partition with the current BPB block
void test_partition(void)
{
    // Setup the block buffer for FAT32 file system
    std::array<unsigned char, 512> buffer{0};
    buffer[FAT32::BYTES_PER_SECTOR] = 0x02;
    buffer[FAT32::NUMBER_OF_FAT]    = 0x02;
    buffer[FAT32::ROOT_ENT_CNT]     = 0x41;
    buffer[FAT32::FAT_SIZE_16]      = 0x00;
    buffer[FAT32::FAT_SIZE_32]      = 0x32;
    buffer[FAT32::TOT_SECTORS_16]   = 0x00;
    buffer[FAT32::TOT_SECTORS_32]   = 0x32;
    buffer[FAT32::TOT_SECTORS_32 + 1]   = 0x32;
    buffer[FAT32::TOT_SECTORS_32 + 2]   = 0x32;
    buffer[FAT32::TOT_SECTORS_32 + 3]   = 0x32;
    buffer[FAT32::RESERVED_SEC]     = 0x01;
    buffer[FAT32::SEC_PER_CLUSTER]  = 0x40;
    buffer[FAT32::ROOT_DIR_CLUSTER] = 0x40;
    buffer[FAT32::FAT32_VOLUME_LABEL + 0] = 'F';
    buffer[FAT32::FAT32_VOLUME_LABEL + 1] = 'A';
    buffer[FAT32::FAT32_VOLUME_LABEL + 2] = 'T';
    buffer[FAT32::FAT32_VOLUME_LABEL + 3] = 'V';
    buffer[FAT32::FAT32_VOLUME_LABEL + 4] = 'o';
    buffer[FAT32::FAT32_VOLUME_LABEL + 5] = 'l';
    buffer[FAT32::FAT32_VOLUME_LABEL + 6] = 'u';
    buffer[FAT32::FAT32_VOLUME_LABEL + 7] = 'm';
    buffer[FAT32::FAT32_VOLUME_LABEL + 8] = 'e';
    buffer[FAT32::FAT32_VOLUME_LABEL + 9] = '3';
    buffer[FAT32::FAT32_VOLUME_LABEL + 10] = '2';

    // initialize a volume
    FAT32::Partition UUT = FAT32::BPB::initialize_partition(buffer.begin(), 0x01);

    // calculate expectations
    const unsigned long FAT_Begin = 0x01 + 0x01;
    const unsigned long FAT_Size = 0x32;
    const unsigned long First_Data_Sector = FAT_Begin + (2*FAT_Size) + FAT32::BPB::root_directory_sectors(buffer.begin());
    const unsigned long Root_Directory_Cluster = 0x40;
    const unsigned long Sectors_per_Cluster = 0x40;

    // test expectations
    TEST_ASSERT_EQUAL( FAT_Begin, UUT.FAT_Begin );
    TEST_ASSERT_EQUAL( FAT_Size, UUT.FAT_Size );
    TEST_ASSERT_EQUAL( First_Data_Sector, UUT.First_Data_Sector );
    TEST_ASSERT_EQUAL( Root_Directory_Cluster, UUT.Root_Directory_Cluster );
    TEST_ASSERT_EQUAL( Sectors_per_Cluster, UUT.Sectors_per_Cluster );
    TEST_ASSERT_FALSE( UUT.is_fat16 );
    TEST_ASSERT_EQUAL_STRING("FATVolume32", UUT.name.begin());

    // Test getting the LBA (Logical Block Addressing) address of a data cluster
    TEST_ASSERT_EQUAL( First_Data_Sector, UUT.get_LBA_of_cluster(2) );
    TEST_ASSERT_EQUAL( First_Data_Sector + Sectors_per_Cluster, UUT.get_LBA_of_cluster(3) );

    // Test getting the FAT sector of a specific cluster
    const unsigned long FAT_Sector = UUT.FAT_Begin;
    TEST_ASSERT_EQUAL(FAT_Sector, UUT.get_FAT_sector(2) );

    // Test getting the FAT byte position of a specific cluster
    const unsigned long FAT_Position = 4 % 512;
    TEST_ASSERT_EQUAL(0, UUT.get_FAT_position(0) );
    TEST_ASSERT_EQUAL(FAT_Position, UUT.get_FAT_position(1) );

    // Test reading a FAT at a specific position -> FAT32
    buffer[0] = 0x01;
    buffer[1] = 0x02;
    buffer[2] = 0x03;
    buffer[3] = 0x04;
    TEST_ASSERT_EQUAL(0x04030201, UUT.read_FAT(buffer.begin(), 0) );

    // Test reading a FAT at a specific position -> FAT16
    UUT.is_fat16 = true;
    buffer[0] = 0x01;
    buffer[1] = 0x02;
    buffer[2] = 0x03;
    buffer[3] = 0x04;
    TEST_ASSERT_EQUAL(0x00000201, UUT.read_FAT(buffer.begin(), 0) );

    // Test writing a FAT at a specific position -> FAT16
    UUT.write_FAT(buffer.begin(), 0, 0x1234);
    TEST_ASSERT_EQUAL( buffer[0], 0x34);
    TEST_ASSERT_EQUAL( buffer[1], 0x12);

    // Test writing a FAT at a specific position -> FAT32
    UUT.is_fat16 = false;
    UUT.write_FAT(buffer.begin(), 0, 0x12345678);
    TEST_ASSERT_EQUAL( buffer[0], 0x78);
    TEST_ASSERT_EQUAL( buffer[1], 0x56);
    TEST_ASSERT_EQUAL( buffer[2], 0x34);
    TEST_ASSERT_EQUAL( buffer[3], 0x12);

    // test getting the root directory cluster
    TEST_ASSERT_EQUAL( Root_Directory_Cluster, UUT.get_LBA_of_root_cluster() );
    UUT.is_fat16 = true;
    TEST_ASSERT_EQUAL( FAT_Begin + 2*FAT_Size, UUT.get_LBA_of_root_cluster() );
};

/// @brief Test reading file properties from a block buffer
void test_get_file_properties(void)
{
    // Create file
    FAT32::Filehandler file;

    // Fill the block buffer with valid data
    file.id = 1;
    file.current.cluster = 4;
    file.block_buffer[32] = 'T';
    file.block_buffer[33] = 'e';
    file.block_buffer[34] = 's';
    file.block_buffer[35] = 't';
    file.block_buffer[36] = ' ';
    file.block_buffer[37] = ' ';
    file.block_buffer[38] = ' ';
    file.block_buffer[39] = ' ';
    file.block_buffer[40] = 'T';
    file.block_buffer[41] = 'X';
    file.block_buffer[42] = 'T';
    file.block_buffer[43] = FAT32::is_Directory;
    file.block_buffer[44] = 0x00;
    file.block_buffer[45] = 0x00;
    file.block_buffer[46] = 0x00;
    file.block_buffer[47] = 0x00;
    file.block_buffer[48] = 0x00;
    file.block_buffer[49] = 0x00;
    file.block_buffer[50] = 0x00;
    file.block_buffer[51] = 0x00;
    file.block_buffer[52] = 0x00;
    file.block_buffer[53] = 0x00;
    file.block_buffer[54] = 0x00;
    file.block_buffer[55] = 0x00;
    file.block_buffer[56] = 0x00;
    file.block_buffer[57] = 0x00;
    file.block_buffer[58] = 0x12;
    file.block_buffer[59] = 0x00;
    file.block_buffer[60] = 0x01;
    file.block_buffer[61] = 0x02;
    file.block_buffer[62] = 0x03;
    file.block_buffer[63] = 0x04;

    // Test reading the properties from the buffer -> FAT16
    file.update_properties_from_buffer();
    TEST_ASSERT_EQUAL_STRING("Test    TXT", file.name.begin());
    TEST_ASSERT_EQUAL(FAT32::is_Directory, file.attributes);
    TEST_ASSERT_EQUAL(4, file.directory_cluster);
    TEST_ASSERT_EQUAL(0x0012, file.start_cluster);
    TEST_ASSERT_EQUAL(0x04030201, file.size);

    // Test reading the properties from the buffer -> FAT32
    file.block_buffer[52] = 0x01;
    file.update_properties_from_buffer();
    TEST_ASSERT_EQUAL_STRING("Test    TXT", file.name.begin());
    TEST_ASSERT_EQUAL(FAT32::is_Directory, file.attributes);
    TEST_ASSERT_EQUAL(4, file.directory_cluster);
    TEST_ASSERT_EQUAL(0x010012, file.start_cluster);
    TEST_ASSERT_EQUAL(0x04030201, file.size);
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_read_data);
    RUN_TEST(test_write_data);
    RUN_TEST(test_read_bootsector);
    RUN_TEST(test_read_EFI);
    RUN_TEST(test_read_BPB);
    RUN_TEST(test_partition);
    RUN_TEST(test_get_file_properties);
    return UNITY_END();
};