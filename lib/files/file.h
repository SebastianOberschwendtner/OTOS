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

#ifndef FILE_H_
#define FILE_H_

// === Includes ===
#include <optional>
#include <ctime>
#include <cstring>
#include <string_view>
#include "volumes.h"
#include "iostream.h"

namespace Files
{
    // === Enums ===
    // File states
    enum class State : unsigned char
    {
        Closed,
        Open,
        Read_only,
        Changed,
        Not_Found,
        Duplicate_File
    };

    // Open Modes as defined in the C++ standard
    typedef unsigned char openmode;
    namespace Mode
    {
        static constexpr openmode app = 0b00000001;
        // static constexpr openmode binary = 0b00000010;
        static constexpr openmode in = 0b00000100;
        static constexpr openmode out = 0b00001000;
        // static constexpr openmode trunc = 0b00010000;
        // static constexpr openmode ate = 0b00100000;
    }
};

namespace FAT32
{
    template <class Volume_t>
    class File : public OTOS::iostream<File<Volume_t>>
    {
    private:
        FAT32::Filehandler handle;
        Volume_t *volume;
        unsigned long access_position{0};

    public:
        // *** Properties ***
        using State = Files::State;
        State state{State::Closed};

        // *** Constructors ***
        File() = delete;
        File(Filehandler &file, Volume_t &volume_used)
            : OTOS::iostream<File<Volume_t>>(*this)
            , handle{file}
            , volume{&volume_used} {};
        File(Filehandler &file, Volume_t &volume_used, Files::State file_state)
            : File{file, volume_used} { this->state = file_state; };
        File(File&&) = delete;
        File &operator=(File&) = delete;
        File &operator=(File&& other)
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
        ~File() {};

        // *** Methods ***
        unsigned long size(void) const;
        unsigned long tell(void) const;
        unsigned char read(void);
        // seek();
        bool put(const char byte);
        bool write(const char* begin, const std::size_t len);
        void flush(void);
        // write_line();
        // read_line();
        // save();
        bool close(void);
    };

    // === Functions to interact with files ===

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
    File<Volume_t> open(Volume_t &volume_used, const char *path_to_file, const Files::openmode mode = Files::Mode::in)
    {
        // Create a filehandle to use for data access
        FAT32::Filehandler ref{};
        ref.name.fill(' ');
        *ref.name.rbegin() = 0;

        // get the root directory using the file handle
        auto file_state = Files::State::Not_Found;
        auto volume_responding = volume_used.read_root(ref);

        // Get iterators of the provided path
        const auto path_begin = path_to_file;
        const size_t path_size = std::strlen(path_to_file);
        const auto path_end = path_to_file + path_size;

        // Get the position of the filename and extension
        const auto ext_begin = std::find(path_begin, path_end, '.');
        const auto name_begin = std::find(
            std::reverse_iterator(path_end - 1),
            std::reverse_iterator(path_begin),
            '/');

        // Copy the file extension to the name without the point
        std::transform(
            ext_begin + 1,
            path_end,
            ref.name.begin() + 8,
            [](char character)
            { return std::toupper(character); });

        // Copy the filename to the name
        std::transform(
            name_begin.base(),
            ext_begin,
            ref.name.begin(),
            [](char character)
            { return std::toupper(character); });

        // Only when volume is active -> it is assumed that volume keeps responding
        if (volume_responding)
        {
            // Read the file from memory
            auto id = volume_used.get_fileid(ref, ref.name);

            // When the file was not found but should be created
            if (not id and (mode & (Files::Mode::out | Files::Mode::app)))
            {
                // File not found -> create a new one
                volume_used.read_cluster(ref, ref.start_cluster);

                // Get empty file id
                auto new_id = volume_used.get_empty_id(ref);

                // Get next empty cluster and write its FAT entry
                auto new_cluster = volume_used.get_next_empty_cluster();
                volume_used.write_FAT_entry(new_cluster.value(), 0x0FFFFFFF);

                // Make the entry in the root directory
                volume_used.make_directory_entry(ref, new_id.value(), new_cluster.value(), ref.name, FAT32::Attribute::Archive, {0});
                id = new_id;
            }

            // When file was found
            if (id)
            {
                // Load the file information and content
                volume_used.get_file(ref, id.value());
                volume_used.read_cluster(ref, ref.start_cluster);
                
                // Set file state to open or read-only depending on open mode
                file_state = (mode & Files::Mode::in) ? Files::State::Read_only : Files::State::Open;
            }
        }
        // Create the file and return it
        return File<Volume_t>{ref, volume_used, file_state};
    };
};

#endif
