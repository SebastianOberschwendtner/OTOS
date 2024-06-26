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
 * @file    test_file.cpp
 * @author  SO
 * @version v3.4.0
 * @date    08-January-2022
 * @brief   Unit tests for testing the File interface.
 ==============================================================================
 */

/* === Includes === */
#include <unity.h>
#include <mock.h>
#include <array>
#include "file.h"

/** === Test List ===
 * Missing Functions:
 */

/* === Fixtures === */
struct Mock_Volume
{
    /* === Provide Data Injection Point32_t=== */
    fat32::Filehandler file_arg{};
    fat32::Filehandler file_return{};
    std::optional<uint32_t> id_return{0};
    std::optional<uint32_t> cluster_return{0};

    /* === Track calls === */
    Mock::Callable<bool> call_get_fileid;
    Mock::Callable<bool> call_get_file;
    Mock::Callable<bool> call_get_empty_id;
    Mock::Callable<bool> call_get_next_empty_cluster;
    Mock::Callable<bool> call_read_last_sector_of_file;
    Mock::Callable<bool> call_read_root;
    Mock::Callable<bool> call_read_cluster;
    Mock::Callable<bool> call_read_next_sector_of_cluster;
    Mock::Callable<bool> call_write_FAT_entry;
    Mock::Callable<bool> call_make_directory_entry;
    Mock::Callable<bool> call_write_filesize_to_directory;
    Mock::Callable<bool> call_write_file_to_memory;

        /* === Provide the expected interface === */
        std::optional<uint32_t>
        get_fileid(fat32::Filehandler &directory, const std::array<char, 12> filename)
    {
        call_get_fileid.add_call(0);
        file_arg.name = filename;
        return id_return;
    };

    bool get_file(fat32::Filehandler &file, const uint8_t id)
    {
        file = file_return;
        return call_get_file(id);
    };

    std::optional<uint32_t> get_empty_id(fat32::Filehandler &directory)
    {
        call_get_empty_id.add_call(0);
        return {6};
    };

    std::optional<uint32_t> get_next_empty_cluster()
    {
        call_get_next_empty_cluster.add_call(0);
        return {12};
    };

    bool read_last_sector_of_file(fat32::Filehandler &file)
    {
        return call_read_last_sector_of_file(file.id);
    };

    bool read_root(fat32::Filehandler &file)
    {
        return call_read_root(0);
    };

    bool read_cluster(fat32::Filehandler &file, const uint32_t cluster)
    {
        return call_read_cluster(cluster);
    };

    bool read_next_sector_of_cluster(fat32::Filehandler &file)
    {
        return call_read_next_sector_of_cluster(0);
    };

    bool write_FAT_entry(const uint32_t cluster, const uint32_t next_cluster)
    {
        return call_write_FAT_entry(next_cluster);
    };

    bool write_filesize_to_directory(fat32::Filehandler &file)
    {
        return call_write_filesize_to_directory(file.id);
    };

    bool write_file_to_memory(fat32::Filehandler &file)
    {
        return call_write_file_to_memory(file.id);
    };

    bool make_directory_entry(
        fat32::Filehandler &directory,
        const uint32_t id,
        const uint32_t cluster,
        const std::array<char, 12> filename,
        const uint8_t attributes,
        const std::time_t time)
    {
        return call_make_directory_entry(attributes);
    };
};
Mock_Volume volume;

/* === UUT === */
#include "file.cpp"
template class fat32::File<Mock_Volume>;

/* === Tests === */
void setUp()
{
    /* set stuff up here */
    volume = {};
};

void tearDown(){
    /* clean stuff up here */
};

/** 
 * @brief Test the constructor
 */
void test_constructor()
{
    /* Setup Test */
    setUp();
    fat32::Filehandler filehandle{};
    filehandle.size = 0x12;

    /* Construct a file */
    fat32::File file{filehandle, volume};

    /* Test side effects */
    TEST_ASSERT_EQUAL(0x12, file.size());
    TEST_ASSERT_EQUAL(files::State::Closed, file.state);
};

/** 
 * @brief Test opening a file using the volume
 */
void test_open_file()
{
    /* Setup Test */
    setUp();
    volume.id_return = 3;
    volume.file_return.id = 3;
    volume.file_return.size = 12;
    volume.file_return.start_cluster = 4;

    /* open a file */
    auto file = fat32::open(volume, "0:/Test.txt");

    /* Test opening a file which exists */
    TEST_ASSERT_EQUAL_STRING("TEST    TXT", volume.file_arg.name.begin());
    TEST_ASSERT_EQUAL(12, file.size());
    TEST_ASSERT_EQUAL(files::State::Read_only, file.state);
    volume.call_get_file.assert_called_once_with(3);
    volume.call_get_fileid.assert_called_once();
    volume.call_read_cluster.assert_called_once_with(4);
    volume.call_read_root.assert_called_once();

    /* Test opening a file in write mode */
    file = fat32::open(volume, "0:/Test.txt", files::Mode::out);
    TEST_ASSERT_EQUAL(files::State::Open, file.state);

    /* Test opening a file which does not exist */
    setUp();
    volume.id_return = {};
    file = fat32::open(volume, "0:/NoTest.tx");
    TEST_ASSERT_EQUAL_STRING("NOTEST  TX ", volume.file_arg.name.begin());
    TEST_ASSERT_EQUAL(files::State::Not_Found, file.state);
    TEST_ASSERT_EQUAL(0, volume.call_get_file.call_count);
    TEST_ASSERT_EQUAL(0, volume.call_get_empty_id.call_count);
    volume.call_get_fileid.assert_called_once();
    volume.call_read_root.assert_called_once();
};

/** 
 * @brief Test reading data from the file
 */
void test_read_file()
{
    /* Setup data */
    setUp();
    fat32::Filehandler dummy;
    dummy.block_buffer[0] = 5;
    dummy.block_buffer[1] = 6;
    dummy.size = 2;
    fat32::File file(dummy, volume);

    /* Read from file when data is valid */
    TEST_ASSERT_EQUAL(0, file.tell());
    TEST_ASSERT_EQUAL(5, file.read());
    TEST_ASSERT_EQUAL(1, file.tell());
    TEST_ASSERT_EQUAL(6, file.read());
    TEST_ASSERT_EQUAL(2, file.tell());

    /* Additional reads should only return 0 */
    /* because the filesize only indicates 1 byte */
    TEST_ASSERT_EQUAL(0, file.read());
    TEST_ASSERT_EQUAL(2, file.tell());
};

/** 
 * @brief Test reading data from the file when a new sector has to be read
 */
void test_read_file_and_sector()
{
    /* Setup data */
    setUp();
    fat32::Filehandler dummy;
    dummy.block_buffer[0] = 5;
    dummy.block_buffer[1] = 6;
    dummy.size = 600;
    fat32::File file(dummy, volume);

    /* Read from file when data is valid */
    for (uint32_t count = 0; count < 512; count++)
        file.read();
    TEST_ASSERT_EQUAL(512, file.tell());

    /* Now a new sector has to be read */
    TEST_ASSERT_EQUAL(5, file.read());
    TEST_ASSERT_EQUAL(513, file.tell());
    volume.call_read_next_sector_of_cluster.assert_called_once();
};

/** 
 * @brief Test creating files
 */
void test_create_file()
{
    /* Setup Test */
    setUp();
    volume.id_return = {};
    volume.file_return.id = 3;
    volume.file_return.size = 12;
    volume.file_return.start_cluster = 4;

    /* Test creating a file */
    auto file = fat32::open(volume, "0:/Test.txt", files::Mode::app);
    TEST_ASSERT_EQUAL_STRING("TEST    TXT", volume.file_arg.name.begin());
    TEST_ASSERT_EQUAL(files::State::Open, file.state);
    TEST_ASSERT_EQUAL(1, volume.call_get_file.call_count);
    TEST_ASSERT_EQUAL(2, volume.call_read_cluster.call_count);
    volume.call_get_fileid.assert_called_once();
    volume.call_read_root.assert_called_once();
    volume.call_get_empty_id.assert_called_once();
    volume.call_get_next_empty_cluster.assert_called_once();
    volume.call_write_FAT_entry.assert_called_once_with(0x0FFFFFFF);
    volume.call_make_directory_entry.assert_called_once_with(fat32::Attribute::Archive);
};

/** 
 * @brief Test writing data to a file
 */
void test_write_file()
{
    /* Setup Test */
    setUp();
    volume.id_return = {};
    volume.file_return.id = 3;
    volume.file_return.size = 0;
    volume.file_return.start_cluster = 4;

    /* create a file and write one byte to it */
    auto file = fat32::open(volume, "0:/Test.txt", files::Mode::app);
    TEST_ASSERT_TRUE(file.put(5));
    TEST_ASSERT_EQUAL(files::State::Changed, file.state);
    TEST_ASSERT_EQUAL(1, file.size());
    TEST_ASSERT_EQUAL(1, file.tell());
    TEST_ASSERT_EQUAL(0, volume.call_write_file_to_memory.call_count);
    TEST_ASSERT_EQUAL(0, volume.call_write_filesize_to_directory.call_count);

    /* write 511 more bytes */
    for (uint32_t count = 0; count < 511; count++)
        file.put(5);
    TEST_ASSERT_EQUAL(512, file.size());
    TEST_ASSERT_EQUAL(512, file.tell());
    TEST_ASSERT_EQUAL(1, volume.call_write_file_to_memory.call_count);
    TEST_ASSERT_EQUAL(1, volume.call_write_filesize_to_directory.call_count);

    /* Test writing to a read-only file */
    volume.id_return = 4;
    volume.file_return.size = 5;
    file = fat32::open(volume, "0:/Test.txt", files::Mode::in);
    TEST_ASSERT_FALSE(file.put(5));
    TEST_ASSERT_EQUAL(5, file.size());
    TEST_ASSERT_EQUAL(0, file.tell());

    /* Test writing to a closed file */
    file.state = files::State::Closed;
    TEST_ASSERT_FALSE(file.put(5));
    TEST_ASSERT_EQUAL(5, file.size());
    TEST_ASSERT_EQUAL(0, file.tell());
};

/** 
 * @brief Test closing a file
 */
void test_close_file()
{
    /* Setup Test */
    setUp();
    volume.id_return = {};
    volume.file_return.id = 3;
    volume.file_return.size = 0;
    volume.file_return.start_cluster = 4;

    /* create a file and write one byte to it */
    auto file = fat32::open(volume, "0:/Test.txt", files::Mode::app);
    file.put(5);

    /* close the file */
    TEST_ASSERT_TRUE(file.close());
    TEST_ASSERT_EQUAL(files::State::Closed, file.state);
    TEST_ASSERT_EQUAL(1, volume.call_write_file_to_memory.call_count);
    TEST_ASSERT_EQUAL(1, volume.call_write_filesize_to_directory.call_count);
};

/* === Main === */
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_constructor);
    RUN_TEST(test_open_file);
    RUN_TEST(test_read_file);
    RUN_TEST(test_read_file_and_sector);
    RUN_TEST(test_create_file);
    RUN_TEST(test_write_file);
    RUN_TEST(test_close_file);
    return UNITY_END();
};
