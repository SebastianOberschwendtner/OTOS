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

#ifndef FILE_H_
#define FILE_H_

/* === Includes === */
#include <cctype>
#include <cstring>
#include <ctime>
#include <optional>
#include <string_view>
#include <iostream.h>
#include "volumes.h"

namespace files
{
    /* === Enums === */
    /* File states */
    enum class State : uint8_t
    {
        Closed,
        Open,
        Read_only,
        Changed,
        Not_Found,
        Duplicate_File
    };

    /* Open Modes as defined in the C++ standard */
    typedef uint8_t openmode;
    namespace Mode
    {
        static constexpr openmode app = 0b00000001;
        /* static constexpr openmode binary = 0b00000010; */
        static constexpr openmode in = 0b00000100;
        static constexpr openmode out = 0b00001000;
        /* static constexpr openmode trunc = 0b00010000; */
        /* static constexpr openmode ate = 0b00100000; */
    }
}; // namespace files

namespace fat32
{
    /**
     * @brief Class to represent a file on a volume.
     *
     * @tparam Volume_t The volume class which is used for memory access.
     */
    template <class Volume_t>
    class File : public OTOS::iostream<File<Volume_t>>
    {
      public:
        /* === Constructors === */
        File() = delete;
        File(Filehandler &file, Volume_t &volume_used)
            : OTOS::iostream<File<Volume_t>>(*this), handle{file}, volume{&volume_used} {};
        File(Filehandler &file, Volume_t &volume_used, files::State file_state)
            : File{file, volume_used} { this->state = file_state; };
        File(File &&) = delete;
        File &operator=(File &) = delete;
        File &operator=(File &&other)
        {
            /* Copy everything except the pimpl pointer of the stream,
             * because the pointer is already set by the constructor
             * of the stream. */
            this->handle = other.handle;
            this->volume = other.volume;
            this->access_position = other.access_position;
            this->state = other.state;
            return *this;
        };
        ~File(){};

        /* === Methods === */
        /**
         * @brief Close the file and write data.
         *
         * @return Return True when the file was successfully written.
         */
        auto close() -> bool;

        /**
         * @brief Flush the file and write data.
         */
        void flush();

        /**
         * @brief Put a byte to the file. After
         * each access the byte counter is increased.
         *
         * @param data The data byte to write to the file.
         * @return Return True when the byte was successfully written.
         */
        auto put(char byte) -> bool;

        /**
         * @brief Read data from the file. After
         * each access the byte counter is increased.
         *
         * When the end of file is reached, the function
         * consecutively returns 0.
         *
         * @return The data byte at the current access position.
         */
        auto read() -> uint8_t;
        /* read_line(); */
        /* save(); */
        /* seek(); */

        /**
         * @brief Return the current size of the file
         * in bytes.
         *
         * @return Returns the file size in bytes.
         */
        auto size() const -> uint32_t;

        /**
         * @brief Get the internal access position pointer
         * for read and write access of the file.
         *
         * @return The current access position in bytes of the file.
         */
        auto tell() const -> uint32_t;

        /**
         * @brief Write data to the file using an iterator. After
         * each access the byte counter is increased.
         *
         * @param begin The begin iterator of the data to write.
         * @param len The number of bytes to write.
         * @return Return True when the data was successfully written.
         */
        auto write(const char *begin, std::size_t len) -> bool;
        /* write_line(); */

        /* === Properties === */
        using State = files::State;
        State state{State::Closed}; /**< State of the file. */

      private:
        /* === Properties === */
        fat32::Filehandler handle;   /**< Handle to current file. */
        Volume_t *volume;            /**< Pointer to the volume used. */
        uint32_t access_position{0}; /**< Current access position in the file. */
    };

    /* === Functions to interact with files === */
    /**
     * @brief Open a file on the volume.
     *
     * The volume has to be mounted and reacting.
     * The path to the file HAS to contain a file extension.
     * The exact path is not yet parsed -> everything is assumed to be in
     * the root directory. The volume or partition number is not yet parsed.
     *
     * Example Path: "0:/file.txt"
     *
     * @tparam Volume_t The volume type to use.
     * @param volume_used The mounted instance of the volume.
     * @param path_to_file The path to the file.
     * @param mode The mode to open the file in. The modes have the behavior as defined in the C++ standard.
     * @return File<Volume_t> Returns a file object indicating whether the file was found or not.
     */
    template <class Volume_t>
    auto open(Volume_t &volume_used, const char *path_to_file, const files::openmode mode = files::Mode::in) -> File<Volume_t>
    {
        /* Create a filehandle to use for data access */
        fat32::Filehandler ref{};
        ref.name.fill(' ');
        *ref.name.rbegin() = 0;

        /* get the root directory using the file handle */
        auto file_state = files::State::Not_Found;
        auto volume_responding = volume_used.read_root(ref);

        /* Get iterators of the provided path */
        const auto path_begin = path_to_file;
        const size_t path_size = std::strlen(path_to_file);
        const auto path_end = path_to_file + path_size;

        /* Get the position of the filename and extension */
        const auto ext_begin = std::find(path_begin, path_end, '.');
        const auto name_begin = std::find(
            std::reverse_iterator(path_end - 1),
            std::reverse_iterator(path_begin),
            '/');

        /* Copy the file extension to the name without the point */
        std::transform(
            ext_begin + 1,
            path_end,
            ref.name.begin() + 8,
            [](char character)
            { return std::toupper(character); });

        /* Copy the filename to the name */
        std::transform(
            name_begin.base(),
            ext_begin,
            ref.name.begin(),
            [](char character)
            { return std::toupper(character); });

        /* Only when volume is active -> it is assumed that volume keeps responding */
        if (volume_responding)
        {
            /* Read the file from memory */
            auto id = volume_used.get_fileid(ref, ref.name);

            /* When the file was not found but should be created */
            if (not id and (mode & (files::Mode::out | files::Mode::app)))
            {
                /* File not found -> create a new one */
                volume_used.read_cluster(ref, ref.start_cluster);

                /* Get empty file id */
                auto new_id = volume_used.get_empty_id(ref);

                /* Get next empty cluster and write its FAT entry */
                auto new_cluster = volume_used.get_next_empty_cluster();
                volume_used.write_FAT_entry(new_cluster.value(), 0x0FFFFFFF);

                /* Make the entry in the root directory */
                volume_used.make_directory_entry(ref, new_id.value(), new_cluster.value(), ref.name, fat32::Attribute::Archive, {0});
                id = new_id;
            }

            /* When file was found */
            if (id)
            {
                /* Load the file information and content */
                volume_used.get_file(ref, id.value());
                volume_used.read_cluster(ref, ref.start_cluster);

                /* Set file state to open or read-only depending on open mode */
                file_state = (mode & files::Mode::in) ? files::State::Read_only : files::State::Open;
            }
        }
        /* Create the file and return it */
        return File<Volume_t>{ref, volume_used, file_state};
    };
}; // namespace fat32

#endif
