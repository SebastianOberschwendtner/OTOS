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
#include "volumes.h"
namespace Files
{
    // === Enums ===
    // File states
    enum class State: unsigned char
    {
        Closed, Open, Changed, Not_Found, Duplicate_File
    };

    // === Interface ===
    struct File_Interface
    {
        State state{State::Closed};
        virtual unsigned long size(void) const = 0;
        // virtual bool seek(const unsigned long position) = 0;
        // virtual unsigned long tell() const = 0;
        // virtual bool write() = 0;
        // virtual bool write_line() = 0;
        // virtual bool read() = 0;
        // virtual bool read_line() = 0;
        // virtual bool save() = 0;
        // virtual bool close() = 0;
    };

};

namespace FAT32
{
    template<class Volume_t>
    class File: public Files::File_Interface
    {
    private:
        FAT32::Filehandler handle;
        Volume_t* volume;

    public:

        // *** Constructors ***
        File(Filehandler& file, Volume_t& volume_used) : handle{file} , volume{&volume_used} { };
        File(Filehandler& file, Volume_t& volume_used, Files::State file_state)
        : File{file, volume_used} { this->state = file_state; };

        // *** Methods ***
        unsigned long size(void) const final;
        // seek();
        // tell();
        // write();
        // write_line();
        // read();
        // read_line();
        // save();
        // close();
    };

    template<class Volume_t>
    File<Volume_t> open(Volume_t& volume_used, const char* path_to_file )
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
        const auto ext_begin = std::find( path_begin, path_end, '.');
        const auto name_begin = std::find(
            std::reverse_iterator(path_end - 1),
            std::reverse_iterator(path_begin),
            '/'
        );

        // Copy the file extension to the name without the point
        std::transform(
            ext_begin + 1,
            path_end,
            ref.name.begin() + 8,
            [](char character){return std::toupper(character);}
        );

        // Copy the filename to the name
        std::transform(
            name_begin.base(),
            ext_begin,
            ref.name.begin(),
            [](char character){return std::toupper(character);}
        );

        // Only when volume is active
        if (volume_responding)
        {
            // Read the file from memory
            auto id = volume_used.get_fileid(ref, ref.name);

            // When file was found
            if ( id )
            {
                volume_used.get_file(ref, id.value());
                volume_used.read_last_sector_of_file(ref);
                file_state = Files::State::Open;
            }
        }
        // Create the file and return it
        return File<Volume_t>{ref, volume_used, file_state};
    };
};

#endif

