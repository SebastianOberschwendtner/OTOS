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

#ifndef VOLUMES_H_
#define VOLUMES_H_

/* === Includes === */
#include <ctime>
#include <optional>
#include "filesystem/fat32.h"
#include "memory/sdhc.h"

namespace drive
{
    /* === Functions to interface with memory === */
    template <class Memory>
    bool read_single_block(Memory &memory, const uint32_t *buffer, const uint32_t block)
    {
        return memory.read_single_block(buffer, block);
    };
    template <class Memory>
    bool write_single_block(Memory &memory, const uint32_t *buffer, const uint32_t block)
    {
        return memory.write_single_block(buffer, block);
    };
}; // namespace drive

namespace fat32
{
    /**
     * @brief Class representing a FAT32 volume.
     *
     * @tparam Memory The memory used for the volume.
     */
    template <class Memory>
    class Volume
    {
      public:
        /* === Constructors === */
        Volume() = delete;

        /**
         * @brief Construct a new Volume object
         *
         * @param memory_used The object reference to the used memory.
         */
        Volume(Memory &memory_used);

        /* === Methods === */
        /**
         * @brief Get the next empty fileid which fits in the current directory.
         * The directory has to be read from memory.
         *
         * The directory clusters are not extended when directory is full!
         *
         * @param directory Filehandler to use for memory access.
         * @return Returns the fileid of the next empty entry, or False when no id was found.
         */
        auto get_empty_id(Filehandler &directory) -> std::optional<uint32_t>;

        /**
         * @brief Read the entry of the given directory and store the entry in
         * the given filehandle.
         *
         * @param file The filehandler to use for data access.
         * @param id The ID of the entry to read from the directory.
         * @return Returns True when the ID was found and the entry read successfully.
         */
        auto get_file(Filehandler &file, uint32_t id) -> bool;

        /**
         * @brief Read the current directory and get the fileid
         * of the file/directory specified by its name.
         *
         * @param directory Filehandler of directory to use for memory access.
         * @param filename The name of the file to search for.
         * @return Returns the fileid of the file when it was found, returns False when
         * file was not found.
         */
        auto get_fileid(Filehandler &directory, const std::array<char, 12> filename) -> std::optional<uint32_t>;

        /**
         * @brief Read the FAT and get the next cluster which
         * is empty and can be allocated.
         *
         * @return Returns the number of the next empty cluster, False when no cluster is empty.
         */
        auto get_next_empty_cluster() -> std::optional<uint32_t>;

        /**
         * @brief Create a entry in the provided open directory.
         * The loaded sector has to contain the ID of the file!
         *
         * @param directory Filehandler with loaded directory sector which contains the entry ID.
         * @param id The file id to make the entry for.
         * @param start_cluster The start cluster of the new file.
         * @param name The name of the file. ALL UPPER CASE!
         * @param attributes The file attributes. For File ARCHIVE is important.
         * @param time The current time of file creation.
         * @return Returns True when the entry was written to memory.
         */
        auto make_directory_entry(
            Filehandler &directory,
            uint32_t id,
            uint32_t start_cluster,
            const std::array<char, 12> name,
            uint8_t attributes,
            std::time_t time) -> bool;

        /**
         * @brief Mount the volume using the assigned memory interface.
         * Reads the boot sector of the memory and derives the filesystem
         * from that.
         *
         * When the filesystem is valid, the partition is initialized accordingly.
         *
         * @return Returns True when the memory contains a recognized filesystem.
         */
        auto mount() -> bool;

        /**
         * @brief Read the first sector of a specific cluster from memory.
         *
         * @param file The filehandler to use for data access.
         * @param cluster The cluster number to be read.
         * @return Returns True when the first sector of the specified cluster was read.
         */
        auto read_cluster(Filehandler &file, uint32_t cluster) -> bool;

        /**
         * @brief Get the next cluster of a current cluster, reading the FAT.
         * The internal FAT buffer is used for data transfer.
         *
         * @param cluster The number of the cluster to get the next sector.
         * @return The entry of in the FAT of the current cluster.
         */
        auto read_FAT_entry(uint32_t cluster) -> std::optional<uint32_t>;

        /**
         * @brief Read the last sector of an opened file.
         *
         * @param file The filehandle to the opened file to be used for memory access.
         * @return Returns True when the end of file was found and the data was read.
         */
        auto read_last_sector_of_file(Filehandler &file) -> bool;

        /**
         * @brief Read the next sector of a specific cluster from memory
         * until the end of the file is reached.
         *
         * @param file The filehandler to use for data access.
         * @return Returns True when the next sector was found and could be read.
         */
        auto read_next_sector_of_cluster(Filehandler &file) -> bool;

        /**
         * @brief Read the root directory of the file system
         * and determine the name of the volume.
         *
         * @param file The filehandler to use for data access.
         * @return Returns True when the root directory was read successfully.
         */
        auto read_root(Filehandler &file) -> bool;

        /**
         * @brief Write the current sector of a specific cluster to memory.
         *
         * @param file The filehandler to use for data access.
         * @return Returns True when the sector of the specified cluster was written.
         */
        auto write_current_sector(Filehandler &file) -> bool;

        /**
         * @brief Set the state of a cluster writing to the FAT.
         *
         * @param cluster The number of the cluster to be update.
         * @param FAT_Entry The new state of the cluster.
         * @return Returns True when the FAT was properly updated.
         */
        auto write_FAT_entry(uint32_t cluster, uint32_t FAT_Entry) -> bool;

        /**
         * @brief Update the filesize of a file in the directory
         * entry of the file.
         *
         * The file buffer has to be written to the memory!
         * The file buffer is used for the directory access, so any
         * data which is not written to the memory is lost.
         *
         * @param file The filehandle of the file of which the directory entry should be updated.
         * @return Returns True when the directory entry was updated successfully.
         */
        auto write_filesize_to_directory(Filehandler &file) -> bool;

        /**
         * @brief Write the current file buffer to memory.
         * When the current sector of cluster is full after
         * the write, new memory is allocated.
         *
         * The filesize is not updated in the directory! This has
         * to be done after writing the content!
         *
         * @param file The filehandle to the file which will be written to memory.
         * @return Returns True when the content was written successfully.
         */
        auto write_file_to_memory(Filehandler &file) -> bool;

        /* === Properties === */
        Memory memory;                        /**< The memory used for the volume. */
        std::array<uint8_t, 512> FAT{0};      /**< The FAT buffer. */
        Partition partition{};                /**< The partition of the volume. */
        error::Code error{error::Code::None}; /**< The error code of the volume. */

      private:
        /* === Properties === */
        uint32_t FAT_sector_in_buffer{0xFFFFFFFF}; /**< The current FAT sector in the buffer. */
    };
}; // namespace fat32

#endif // VOLUMES_H_
