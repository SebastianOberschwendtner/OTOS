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
 * @file    test_file.cpp
 * @author  SO
 * @version v2.8.0
 * @date    08-January-2022
 * @brief   Unit tests for testing the File interface.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>
#include "file.h"

/** === Test List ===
 * Missing Functions:
*/

// === Fixtures ===
struct Mock_Volume
{
    // === Provide Data Injection Point ===
    FAT32::Filehandler file_arg{};
    FAT32::Filehandler file_return{};
    std::optional<unsigned long> id_return{0};
    std::optional<unsigned long> cluster_return{0};

    // === Track calls ===
    Mock::Callable<bool> call_get_fileid;
    Mock::Callable<bool> call_get_file;
    Mock::Callable<bool> call_read_last_sector_of_file;
    Mock::Callable<bool> call_read_root;
    Mock::Callable<bool> call_read_cluster;
    Mock::Callable<bool> call_read_next_sector_of_cluster;

    // === Provide the expected interface ===
    std::optional<unsigned long> get_fileid(FAT32::Filehandler& directory, const std::array<char, 12> filename)
    {
        call_get_fileid.add_call(0);
        file_arg.name = filename;
        return id_return;
    };

    bool get_file(FAT32::Filehandler& file, const unsigned char id)
    {
        file = file_return;
        return call_get_file(id);
    };

    bool read_last_sector_of_file(FAT32::Filehandler& file)
    {
        return call_read_last_sector_of_file(file.id);
    };

    bool read_root(FAT32::Filehandler& file)
    {
        return call_read_root(0);
    };

    bool read_cluster(FAT32::Filehandler& file, const unsigned long cluster)
    {
        return call_read_cluster(cluster);
    };

    bool read_next_sector_of_cluster(FAT32::Filehandler& file)
    {
        return call_read_next_sector_of_cluster(0);
    };
};
Mock_Volume volume;

// === UUT ===
#include "file.cpp"
template class FAT32::File<Mock_Volume>;

// === Tests ===
void setUp(void) {
    // set stuff up here
    volume = {};
};

void tearDown(void) {
    // clean stuff up here
};

/// @brief Test the constructor
void test_constructor(void)
{
    // Setup Test
    setUp();
    FAT32::Filehandler filehandle{};
    filehandle.size = 0x12;

    // Construct a file
    FAT32::File file{filehandle, volume};

    // Test side effects
    TEST_ASSERT_EQUAL(0x12, file.size() );
    TEST_ASSERT_EQUAL(Files::State::Closed, file.state);
};

/// @brief Test opening a file using the volume
void test_open_file(void)
{
    // Setup Test
    setUp();
    volume.id_return = 3;
    volume.file_return.id = 3;
    volume.file_return.size = 12;
    volume.file_return.start_cluster = 4;

    // open a file
    auto file = FAT32::open(volume, "0:/Test.txt");

    // Test opening a file which exists
    TEST_ASSERT_EQUAL_STRING("TEST    TXT", volume.file_arg.name.begin());
    TEST_ASSERT_EQUAL(12, file.size() );
    TEST_ASSERT_EQUAL(Files::State::Open, file.state );
    volume.call_get_file.assert_called_once_with(3);
    volume.call_get_fileid.assert_called_once();
    volume.call_read_cluster.assert_called_once_with(4);
    volume.call_read_root.assert_called_once();

    // Test opening a file which does not exist
    setUp();
    volume.id_return = {};
    file = FAT32::open(volume, "0:/NoTest.tx");
    TEST_ASSERT_EQUAL_STRING("NOTEST  TX ", volume.file_arg.name.begin());
    TEST_ASSERT_EQUAL(Files::State::Not_Found, file.state );
    TEST_ASSERT_EQUAL( 0, volume.call_get_file.call_count);
    volume.call_get_fileid.assert_called_once();
    volume.call_read_root.assert_called_once();
};

/// @brief Test reading data from the file
void test_read_file(void)
{
    // Setup data
    setUp();
    FAT32::Filehandler dummy;
    dummy.block_buffer[0] = 5;
    dummy.block_buffer[1] = 6;
    dummy.size = 2;
    FAT32::File file(dummy, volume);
    
    // Read from file when data is valid
    TEST_ASSERT_EQUAL( 0, file.tell() );
    TEST_ASSERT_EQUAL( 5, file.read() );
    TEST_ASSERT_EQUAL( 1, file.tell() );
    TEST_ASSERT_EQUAL( 6, file.read() );
    TEST_ASSERT_EQUAL( 2, file.tell() );

    // Additional reads should only return 0
    // because the filesize only indicates 1 byte
    TEST_ASSERT_EQUAL( 0, file.read() );
    TEST_ASSERT_EQUAL( 2, file.tell() );
};

/// @brief Test reading data from the file when a new sector has to be read
void test_read_file_and_sector(void)
{
    // Setup data
    setUp();
    FAT32::Filehandler dummy;
    dummy.block_buffer[0] = 5;
    dummy.block_buffer[1] = 6;
    dummy.size = 600;
    FAT32::File file(dummy, volume);
    
    // Read from file when data is valid
    for(unsigned int count = 0; count<512; count++)
        file.read();
    TEST_ASSERT_EQUAL( 512, file.tell() );

    // Now a new sector has to be read
    TEST_ASSERT_EQUAL(5, file.read() );
    TEST_ASSERT_EQUAL( 513, file.tell() );
    volume.call_read_next_sector_of_cluster.assert_called_once();
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    test_constructor();
    test_open_file();
    test_read_file();
    test_read_file_and_sector();
    UNITY_END();
    return EXIT_SUCCESS;
};