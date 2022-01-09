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
 * @file    test_volumes.cpp
 * @author  SO
 * @version v2.7.1
 * @date    04-January-2022
 * @brief   Unit tests for testing the Volume interface.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>

/** === Test List ===
 * Missing Functions:
 * ✓ Read the End sector of a file after getting the filehandler
 * ✓ Update the filesize in the directory entry of the file
 * ✓ Write current file content to memory
 * ✓ interface to get the system time for creating files/directories
 * ✓ Make entry in directory for (mkdir/mkfile)
*/

// === Fixtures ===
struct Mock_Memory
{
};
Mock::Callable<bool> read_single_block;
Mock::Callable<bool> write_single_block;

namespace Drive
{
    bool read_single_block(Mock_Memory& memory, const unsigned long* buffer, const unsigned long block)
    {
        ::read_single_block.add_call(static_cast<int>(block));
        return true; 
    };
    bool write_single_block(Mock_Memory& memory, const unsigned long* buffer, const unsigned long block)
    {
        ::write_single_block.add_call(static_cast<int>(block));
        return true; 
    };
}

#include "volumes.h"
#include "volumes.cpp"
template class FAT32::Volume<Mock_Memory>;

// === Tests ===
void setUp(void) {
    // set stuff up here
    ::read_single_block.reset();
    ::write_single_block.reset();
};

void tearDown(void) {
    // clean stuff up here
};

/// @brief Test the constructor
void test_constructor(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
};

/// @brief Test reading cluster
void test_read_cluster(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file{};

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.First_Data_Sector = 0x12;

    // Test reading a cluster
    TEST_ASSERT_TRUE( UUT.read_cluster(file, 2) );
    TEST_ASSERT_EQUAL( 2, file.current.cluster );
    TEST_ASSERT_EQUAL( 1, file.current.sector );
    ::read_single_block.assert_called_once_with( 0x12 );
};

/// @brief Test writing the current sector of a file
void test_write_current_sector(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file{};

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.First_Data_Sector = 0x12;
    file.current.sector = 1;
    file.current.cluster = 2;

    // Test reading a cluster
    TEST_ASSERT_TRUE( UUT.write_current_sector(file) );
    ::write_single_block.assert_called_once_with( 0x12 );
};

/// @brief Test reading the FAT and getting the next sector of a cluster
void test_get_FAT_entry(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file{};

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.First_Data_Sector = 0x12;
    UUT.partition.Sectors_per_Cluster = 0x40;
    UUT.partition.FAT_Begin = 0x64;
    UUT.FAT[8] = 0x56;
    file.current.sector = 1;
    file.current.cluster = 2;

    // Test reading the next sector when no new cluster is required
    auto response = UUT.read_FAT_entry(2);
    TEST_ASSERT_TRUE( response );
    TEST_ASSERT_EQUAL( 0x56, response.value() );
    ::read_single_block.assert_called_once_with(0x64);

    // Test reading the next sector when no new cluster is required
    // and the FAT containing the entry was alread read
    setUp();
    UUT.FAT[12] = 0x57;
    response = UUT.read_FAT_entry(3);
    TEST_ASSERT_TRUE( response );
    TEST_ASSERT_EQUAL( 0x57, response.value() );
    TEST_ASSERT_EQUAL(0, ::read_single_block.call_count);
};

/// @brief Test reading the next sector of a cluster
void test_read_next_sector(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file{};

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.First_Data_Sector = 0x12;
    UUT.partition.Sectors_per_Cluster = 0x40;
    UUT.partition.FAT_Begin = 0x64;
    UUT.FAT[8] = 0x04;
    file.current.sector = 1;
    file.current.cluster = 2;

    // Test reading the next sector when no new cluster is required
    TEST_ASSERT_TRUE( UUT.read_next_sector_of_cluster(file) );
    ::read_single_block.assert_called_once_with(0x13);
    TEST_ASSERT_EQUAL( 2, file.current.cluster);
    TEST_ASSERT_EQUAL( 2, file.current.sector);

    // Test reading the next sector when a new cluster is required
    setUp();
    file.current.sector = UUT.partition.Sectors_per_Cluster;
    file.current.cluster = 2;
    TEST_ASSERT_TRUE( UUT.read_next_sector_of_cluster(file) );
    ::read_single_block.assert_called_last_with(0x12 + 2*0x40);

    // Test reading the next sector when end of file is reached -> FAT32
    setUp();
    UUT.partition.is_fat16 = false;
    UUT.FAT[4] = 0xFF;
    UUT.FAT[5] = 0xFF;
    UUT.FAT[6] = 0x00;
    UUT.FAT[7] = 0x00;
    UUT.FAT[8] = 0xFF;
    UUT.FAT[9] = 0xFF;
    UUT.FAT[10] = 0xFF;
    UUT.FAT[11] = 0xFF;
    file.current.sector = UUT.partition.Sectors_per_Cluster;
    file.current.cluster = 2;
    TEST_ASSERT_FALSE( UUT.read_next_sector_of_cluster(file) );
    TEST_ASSERT_EQUAL(Error::Code::End_of_File_Reached, UUT.error );

    // Test reading the next sector when end of file is reached -> FAT16
    setUp();
    UUT.partition.is_fat16 = true;
    UUT.FAT[4] = 0xFF;
    UUT.FAT[5] = 0xFF;
    UUT.FAT[6] = 0x00;
    UUT.FAT[7] = 0x00;
    UUT.FAT[8] = 0xFF;
    UUT.FAT[9] = 0xFF;
    UUT.FAT[10] = 0xFF;
    UUT.FAT[11] = 0xFF;
    file.current.sector = UUT.partition.Sectors_per_Cluster;
    file.current.cluster = 2;
    TEST_ASSERT_FALSE( UUT.read_next_sector_of_cluster(file) );
    TEST_ASSERT_EQUAL(Error::Code::End_of_File_Reached, UUT.error );

    // Test reading the next sector when a bad is reached -> FAT32
    setUp();
    UUT.partition.is_fat16 = false;
    UUT.FAT[4] = 0xF8;
    UUT.FAT[5] = 0xFF;
    UUT.FAT[6] = 0x00;
    UUT.FAT[7] = 0x00;
    UUT.FAT[8] = 0xF8;
    UUT.FAT[9] = 0xFF;
    UUT.FAT[10] = 0xFF;
    UUT.FAT[11] = 0xFF;
    file.current.sector = UUT.partition.Sectors_per_Cluster;
    file.current.cluster = 2;
    TEST_ASSERT_FALSE( UUT.read_next_sector_of_cluster(file) );
    TEST_ASSERT_EQUAL(Error::Code::Bad_Sector, UUT.error );

    // Test reading the next sector when a bad is reached -> FAT16
    setUp();
    UUT.partition.is_fat16 = true;
    UUT.FAT[4] = 0xF8;
    UUT.FAT[5] = 0xFF;
    UUT.FAT[6] = 0x00;
    UUT.FAT[7] = 0x00;
    UUT.FAT[8] = 0xF8;
    UUT.FAT[9] = 0xFF;
    UUT.FAT[10] = 0xFF;
    UUT.FAT[11] = 0xFF;
    file.current.sector = UUT.partition.Sectors_per_Cluster;
    file.current.cluster = 2;
    TEST_ASSERT_FALSE( UUT.read_next_sector_of_cluster(file) );
    TEST_ASSERT_EQUAL(Error::Code::Bad_Sector, UUT.error );

    // Test reading the next sector when the FAT is corrupted-> FAT32
    setUp();
    UUT.partition.is_fat16 = false;
    UUT.FAT[4] = 0xF8;
    UUT.FAT[5] = 0xFF;
    UUT.FAT[6] = 0x00;
    UUT.FAT[7] = 0x00;
    UUT.FAT[8] = 0x00;
    UUT.FAT[9] = 0x00;
    UUT.FAT[10] = 0x00;
    UUT.FAT[11] = 0x00;
    file.current.sector = UUT.partition.Sectors_per_Cluster;
    file.current.cluster = 2;
    TEST_ASSERT_FALSE( UUT.read_next_sector_of_cluster(file) );
    TEST_ASSERT_EQUAL(Error::Code::FAT_Corrupted, UUT.error );

    // Test reading the next sector when the FAT is corrupted-> FAT16
    setUp();
    UUT.partition.is_fat16 = true;
    UUT.FAT[4] = 0x00;
    UUT.FAT[5] = 0x00;
    UUT.FAT[6] = 0x00;
    UUT.FAT[7] = 0x00;
    UUT.FAT[8] = 0x00;
    UUT.FAT[9] = 0x00;
    UUT.FAT[10] = 0x00;
    UUT.FAT[11] = 0x00;
    file.current.sector = UUT.partition.Sectors_per_Cluster;
    file.current.cluster = 2;
    TEST_ASSERT_FALSE( UUT.read_next_sector_of_cluster(file) );
    TEST_ASSERT_EQUAL(Error::Code::FAT_Corrupted, UUT.error );
};

/// @brief Test reading a file/directory using its ID
void test_get_file_with_id(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file{};
    file.start_cluster = 2;
    file.attributes = FAT32::Attribute::is_Directory;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.First_Data_Sector = 0x12;
    UUT.partition.Sectors_per_Cluster = 0x40;

    // Test successful read
    TEST_ASSERT_TRUE( UUT.get_file(file, 1) );
    TEST_ASSERT_EQUAL( 1, file.id);
    ::read_single_block.assert_called_once_with( 0x12 );
    TEST_ASSERT_EQUAL( Error::Code::None, UUT.error);

    // Test successful read when entry is not in current sector
    setUp();
    file.start_cluster = 2;
    file.attributes = FAT32::Attribute::is_Directory;
    TEST_ASSERT_TRUE( UUT.get_file(file, 17) );
    TEST_ASSERT_EQUAL( 17, file.id );
    TEST_ASSERT_EQUAL( 2, ::read_single_block.call_count );
    ::read_single_block.assert_called_last_with( 0x13 );
    TEST_ASSERT_EQUAL( Error::Code::None, UUT.error );

    // Test read when the provided file handle is not a directory
    setUp();
    file.start_cluster = 2;
    file.attributes = 0;
    TEST_ASSERT_FALSE( UUT.get_file(file, 1) );
    TEST_ASSERT_EQUAL( 0, ::read_single_block.call_count);
    TEST_ASSERT_EQUAL( Error::Code::Not_a_Directory, UUT.error);
};

/// @brief Test reading the root directory of a mounted volume
void test_read_root(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file{};
    file.block_buffer[0] = 'V';
    file.block_buffer[1] = 'o';
    file.block_buffer[2] = 'l';
    file.block_buffer[3] = 'u';
    file.block_buffer[4] = 'm';
    file.block_buffer[5] = 'e';
    file.block_buffer[6] = ' ';
    file.block_buffer[7] = '1';
    file.block_buffer[8] = ' ';
    file.block_buffer[9] = ' ';
    file.block_buffer[10] = ' ';


    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.Root_Directory_Cluster = 0x02;
    UUT.partition.First_Data_Sector = 0x12;
    UUT.partition.Sectors_per_Cluster = 0x40;

    // Test successful read
    TEST_ASSERT_TRUE( UUT.read_root(file) );
    TEST_ASSERT_TRUE( file.is_directory() );
    ::read_single_block.assert_called_once_with( 0x02 );
};

/// @brief Test mounting a volume
void test_mount(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.FAT[11] = 0x02;
    UUT.FAT[13] = 0x40;
    UUT.FAT[450] = 0x06;
    UUT.FAT[457] = 0x12;
    UUT.FAT[456] = 0x13;
    UUT.FAT[455] = 0x14;
    UUT.FAT[454] = 0x15;
    UUT.FAT[510] = 0x55;
    UUT.FAT[511] = 0xAA;

    // Test successful read
    TEST_ASSERT_TRUE( UUT.mount() );
    TEST_ASSERT_EQUAL(2, ::read_single_block.call_count);
    ::read_single_block.assert_called_last_with(0x12131415);
    TEST_ASSERT_EQUAL( Error::Code::None, UUT.error);
};

/// @brief Test setting the state of a cluster in the FAT
void test_set_cluster(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.Sectors_per_Cluster = 0x40;
    UUT.partition.FAT_Begin = 0x12;
    UUT.partition.FAT_Size = 0x88;

    // Test setting the cluster state
    TEST_ASSERT_TRUE( UUT.write_FAT_entry(8, 0x14) );
    TEST_ASSERT_EQUAL(0x14, UUT.FAT[32] );
    ::read_single_block.assert_called_once_with( 0x12 );
    TEST_ASSERT_EQUAL( 2, ::write_single_block.call_count );
    ::write_single_block.assert_called_last_with( 0x12 + 0x88 );
};

/// @brief Test getting the id of the next empty file/directory
void test_get_empty_file_id(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler directory;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.Sectors_per_Cluster = 0x40;
    UUT.partition.FAT_Begin = 0x12;
    UUT.partition.FAT_Size = 0x88;

    // Only read the id when filehandler is directory
    auto id = UUT.get_empty_id(directory);
    TEST_ASSERT_FALSE( id );

    // Read the id when the directory is valid, first entry is empty
    directory.attributes = FAT32::Attribute::is_Directory;
    id = UUT.get_empty_id(directory);
    TEST_ASSERT_TRUE( id );
    TEST_ASSERT_EQUAL( 0, id.value() );

    // Read the id when the directory is valid, first entry is deleted
    directory.attributes = FAT32::Attribute::is_Directory;
    directory.block_buffer[0] = 0xE5;
    id = UUT.get_empty_id(directory);
    TEST_ASSERT_TRUE( id );
    TEST_ASSERT_EQUAL( 0, id.value() );

    // Read the id when the directory is valid, third entry is empty
    directory.attributes = FAT32::Attribute::is_Directory;
    directory.block_buffer[0] = 'F';
    directory.block_buffer[32] = 'F';
    directory.block_buffer[64] = 0x00;
    id = UUT.get_empty_id(directory);
    TEST_ASSERT_TRUE( id );
    TEST_ASSERT_EQUAL( 2, id.value() );

    // Read the id when the directory is valid, third entry is deleted
    directory.attributes = FAT32::Attribute::is_Directory;
    directory.block_buffer[0] = 'F';
    directory.block_buffer[32] = 'F';
    directory.block_buffer[64] = 0xE5;
    id = UUT.get_empty_id(directory);
    TEST_ASSERT_TRUE( id );
    TEST_ASSERT_EQUAL( 2, id.value() );
};

/// @brief Get the next empty cluster which can be allocated
void test_get_next_empty_cluster(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.FAT_Size = 0x40;
    UUT.FAT.fill(0x11);
    UUT.FAT[8] = 0x00;
    UUT.FAT[9] = 0x00;
    UUT.FAT[10] = 0x00;
    UUT.FAT[11] = 0x00;

    // Test reading next empty cluster is 2
    auto empty_cluster = UUT.get_next_empty_cluster();
    TEST_ASSERT_TRUE( empty_cluster );
    TEST_ASSERT_EQUAL( 2, empty_cluster.value() );

    // Test when no space is left
    UUT.FAT.fill(0x11);
    empty_cluster = UUT.get_next_empty_cluster();
    TEST_ASSERT_FALSE( empty_cluster );
    TEST_ASSERT_EQUAL( Error::Code::No_Memory_Left, UUT.error );
};

/// @brief Test getting the fileid of a file by name
void test_get_fileid(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler directory;
    directory.block_buffer[32] = 'T';
    directory.block_buffer[33] = 'e';
    directory.block_buffer[34] = 's';
    directory.block_buffer[35] = 't';
    directory.block_buffer[36] = ' ';
    directory.block_buffer[37] = ' ';
    directory.block_buffer[38] = ' ';
    directory.block_buffer[39] = ' ';
    directory.block_buffer[40] = 't';
    directory.block_buffer[41] = 'x';
    directory.block_buffer[42] = 't';

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    std::array<char, 12> filename = {"Test    txt"};

    // Test getting the file id, when filehandle is not a directory
    auto id = UUT.get_fileid(directory, filename);
    TEST_ASSERT_FALSE( id );
    TEST_ASSERT_EQUAL( Error::Code::Not_a_Directory, UUT.error );

    // Test getting the file id
    directory.attributes = FAT32::Attribute::is_Directory;
    id = UUT.get_fileid(directory, filename);
    TEST_ASSERT_TRUE( id );
    TEST_ASSERT_EQUAL( 1, id.value() );
};

/// @brief Read the last sector of a file after loading it
void test_read_last_sector(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file;
    file.start_cluster = 0x04;
    file.size = 120;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.Sectors_per_Cluster = 0x40;
    UUT.partition.First_Data_Sector = 0x02;

    // Test reading the last sector when no new cluster is required
    TEST_ASSERT_TRUE( UUT.read_last_sector_of_file(file) );
    TEST_ASSERT_EQUAL( 0x04, file.current.cluster);
    TEST_ASSERT_EQUAL( 0x01, file.current.sector);
    TEST_ASSERT_EQUAL( 120, file.current.byte);
    ::read_single_block.assert_called_once_with( 0x02 + 2*0x40 );
    
    // Test reading the last sector when an new sector is required
    setUp();
    file.size = 600;
    TEST_ASSERT_TRUE( UUT.read_last_sector_of_file(file) );
    TEST_ASSERT_EQUAL( 0x04, file.current.cluster);
    TEST_ASSERT_EQUAL( 0x02, file.current.sector);
    TEST_ASSERT_EQUAL( 88, file.current.byte);
    ::read_single_block.assert_called_once_with( 0x02 + 2*0x40 + 1 );
    
    // Test reading the last sector when an new cluster is required
    setUp();
    file.size = 0x8001;
    UUT.FAT[16] = 0x05;
    TEST_ASSERT_TRUE( UUT.read_last_sector_of_file(file) );
    TEST_ASSERT_EQUAL( 0x05, file.current.cluster);
    TEST_ASSERT_EQUAL( 0x01, file.current.sector);
    TEST_ASSERT_EQUAL( 1, file.current.byte);
    TEST_ASSERT_EQUAL( 2, ::read_single_block.call_count);
    ::read_single_block.assert_called_last_with( 0x02 + 3*0x40 );
};

/// @brief Update the filesize of a file in the directory
void test_update_filesize(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file;
    file.directory_cluster = 0x02;
    file.start_cluster = 0x04;
    file.size = 120;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.Sectors_per_Cluster = 0x40;
    UUT.partition.First_Data_Sector = 0x02;

    // Test writing the file size when fileid requires no new sector or cluster
    file.id = 2;
    TEST_ASSERT_TRUE( UUT.write_filesize_to_directory(file) );
    TEST_ASSERT_EQUAL( 0x78, file.block_buffer[2*32 + FAT32::Filesize]);
    ::read_single_block.assert_called_once_with( 0x02 + 0*0x40 );
    ::write_single_block.assert_called_once_with( 0x02 + 0*0x40 );

    // Test writing the file size when fileid requires a new sector
    setUp();
    file.id = 19;
    TEST_ASSERT_TRUE( UUT.write_filesize_to_directory(file) );
    TEST_ASSERT_EQUAL( 0x78, file.block_buffer[3*32 + FAT32::Filesize]);
    TEST_ASSERT_EQUAL( 2, ::read_single_block.call_count);
    ::read_single_block.assert_called_last_with( 0x02 + 0*0x40 + 1);
    ::write_single_block.assert_called_once_with( 0x02 + 0*0x40 + 1);
};

/// @brief Test writing the current file buffer to memory
void test_write_file_content(void)
{
    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler file;
    file.directory_cluster = 0x02;
    file.start_cluster = 0x04;
    file.size = 120;
    file.current.cluster = 0x02;
    file.current.sector = 1;
    file.current.byte = 120;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.Sectors_per_Cluster = 0x40;
    UUT.partition.First_Data_Sector = 0x02;
    UUT.partition.FAT_Size = 0x02;

    // Test writing the file content when no new sector or cluster is required
    TEST_ASSERT_TRUE( UUT.write_file_to_memory(file) );
    ::write_single_block.assert_called_once_with( 0x02 + 0*0x40 );

    // Test writing the file content when sector is full after the write
    setUp();
    file.size = 512;
    file.current.byte = 512;
    TEST_ASSERT_TRUE( UUT.write_file_to_memory(file) );
    TEST_ASSERT_EQUAL(2, file.current.sector);
    TEST_ASSERT_EQUAL(0, file.current.byte);
    ::write_single_block.assert_called_once_with( 0x02 + 0*0x40 );

    // Test writing the file content when cluster is full after the write
    setUp();
    file.size = 64 * 512;
    file.current.sector = 64;
    file.current.byte = 512;
    UUT.FAT.fill(0xFF);
    UUT.FAT[20] = 0x00;
    UUT.FAT[21] = 0x00;
    UUT.FAT[22] = 0x00;
    UUT.FAT[23] = 0x00;
    UUT.FAT[24] = 0x00;
    UUT.FAT[25] = 0x00;
    UUT.FAT[26] = 0x00;
    UUT.FAT[27] = 0x00;
    TEST_ASSERT_TRUE( UUT.write_file_to_memory(file) );
    TEST_ASSERT_EQUAL(1, file.current.sector);
    TEST_ASSERT_EQUAL(0x05, file.current.cluster);
    TEST_ASSERT_EQUAL(0, file.current.byte);
};

/// @brief Test making a file entry in a directory
void test_make_file_entry(void)
{
    // Provide test date
    std::tm date_time = {4, 15, 20, 23, 1, 94};
    time_t file_time = std::mktime(&date_time);

    // Setup Test
    setUp();
    Mock_Memory memory;
    FAT32::Filehandler directory;
    directory.current.cluster = 0x02;
    directory.current.sector = 1;

    // Create volume
    FAT32::Volume<Mock_Memory> UUT(memory);
    UUT.partition.Sectors_per_Cluster = 0x40;
    UUT.partition.First_Data_Sector = 0x02;
    UUT.partition.FAT_Size = 0x02;

    // Test making an entry in the current directory
    unsigned long id = 2, start_cluster = 0x01020304;
    unsigned char attributes = FAT32::Attribute::Read_Only;
    std::array<char, 12> name = {"Test    txt"};
    TEST_ASSERT_TRUE( UUT.make_directory_entry(directory, id, start_cluster, name, attributes, file_time));
    ::write_single_block.assert_called_once_with(0x02 + 0*0x40);
    TEST_ASSERT_EQUAL( 'T', directory.block_buffer[id*32 + 0]);
    TEST_ASSERT_EQUAL( 'e', directory.block_buffer[id*32 + 1]);
    TEST_ASSERT_EQUAL( 's', directory.block_buffer[id*32 + 2]);
    TEST_ASSERT_EQUAL( 't', directory.block_buffer[id*32 + 3]);
    TEST_ASSERT_EQUAL( ' ', directory.block_buffer[id*32 + 4]);
    TEST_ASSERT_EQUAL( ' ', directory.block_buffer[id*32 + 5]);
    TEST_ASSERT_EQUAL( ' ', directory.block_buffer[id*32 + 6]);
    TEST_ASSERT_EQUAL( ' ', directory.block_buffer[id*32 + 7]);
    TEST_ASSERT_EQUAL( 't', directory.block_buffer[id*32 + 8]);
    TEST_ASSERT_EQUAL( 'x', directory.block_buffer[id*32 + 9]);
    TEST_ASSERT_EQUAL( 't', directory.block_buffer[id*32 + 10]);
    TEST_ASSERT_EQUAL( 0x04, directory.block_buffer[id*32 + FAT32::DIR_Entry::First_Cluster_L]);
    TEST_ASSERT_EQUAL( 0x03, directory.block_buffer[id*32 + FAT32::DIR_Entry::First_Cluster_L + 1]);
    TEST_ASSERT_EQUAL( 0x02, directory.block_buffer[id*32 + FAT32::DIR_Entry::First_Cluster_H]);
    TEST_ASSERT_EQUAL( 0x01, directory.block_buffer[id*32 + FAT32::DIR_Entry::First_Cluster_H + 1]);
    TEST_ASSERT_EQUAL( FAT32::Attribute::Read_Only, directory.block_buffer[id*32 + FAT32::DIR_Entry::Attributes]);
    TEST_ASSERT_EQUAL( 0xE2, directory.block_buffer[id*32 + FAT32::DIR_Entry::Creation_Time]);
    TEST_ASSERT_EQUAL( 0xA1, directory.block_buffer[id*32 + FAT32::DIR_Entry::Creation_Time + 1]);
    TEST_ASSERT_EQUAL( 0xE2, directory.block_buffer[id*32 + FAT32::DIR_Entry::Write_Time]);
    TEST_ASSERT_EQUAL( 0xA1, directory.block_buffer[id*32 + FAT32::DIR_Entry::Write_Time + 1]);
    TEST_ASSERT_EQUAL( 0x57, directory.block_buffer[id*32 + FAT32::DIR_Entry::Creation_Date]);
    TEST_ASSERT_EQUAL( 0x1C, directory.block_buffer[id*32 + FAT32::DIR_Entry::Creation_Date + 1]);
    TEST_ASSERT_EQUAL( 0x57, directory.block_buffer[id*32 + FAT32::DIR_Entry::Write_Date]);
    TEST_ASSERT_EQUAL( 0x1C, directory.block_buffer[id*32 + FAT32::DIR_Entry::Write_Date + 1]);
    TEST_ASSERT_EQUAL( 0x57, directory.block_buffer[id*32 + FAT32::DIR_Entry::Access_Date]);
    TEST_ASSERT_EQUAL( 0x1C, directory.block_buffer[id*32 + FAT32::DIR_Entry::Access_Date + 1]);
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    test_constructor();
    test_read_cluster();
    test_write_current_sector();
    test_get_FAT_entry();
    test_read_next_sector();
    test_get_file_with_id();
    test_read_root();
    test_mount();
    test_set_cluster();
    test_get_empty_file_id();
    test_get_next_empty_cluster();
    test_get_fileid();
    test_read_last_sector();
    test_update_filesize();
    test_write_file_content();
    test_make_file_entry();
    UNITY_END();
    return EXIT_SUCCESS;
};