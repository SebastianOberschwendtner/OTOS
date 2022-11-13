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

#ifndef IOSTREAM_H_
#define IOSTREAM_H_

// === Includes ===

// === IO Streams ===
namespace OTOS
{
    /**
     * @brief Output stream class to write to output devices.
     * 
     * @tparam o_device The device which holds the output buffer/interface.
     */
    template <class o_device>
    class ostream
    {
    private:
        o_device* const pimpl; // Pointer to the implementation of the output device

    public:
        // *** Constructors ***
        ostream(void) = delete;
        ostream(const ostream&) = delete;
        ostream(ostream&&) = delete;
        ostream(o_device& device): pimpl(&device) {};

        // *** Destructor ***
        ~ostream(void) {};

        // *** Operators ***
        ostream& operator=(const ostream&) = delete;
        ostream& operator=(ostream&&) = delete;

        // *** Methods ***
        ostream& put(char c) { pimpl->put(c); return *this;}; 
        ostream& write(const char* str, std::size_t n) { pimpl->write(str, n); return *this;};
        ostream& flush(void) { pimpl->flush(); return *this;};

        // *** Overloaded stream operators ***
        /**
         * @brief Add a string to the stream.
         * 
         * @param str The pointer to the  null terminated (!) string to add.
         * @return ostream& Returns a reference to the stream.
         */
        ostream& operator<<(char * str)
        {
            // Loop as long as the string is not terminated
            while(*str)
                this->put(*str++);
            return *this;
        }; 
    };

    /**
     * @brief Input stream class to read from input devices.
     * 
     * @tparam i_device The device which holds the input buffer/interface.
     */
    template <class i_device>
    class istream
    {
    };

    /**
     * @brief Input and output stream class to read and write from input and output devices.
     * 
     * @tparam io_device The device which holds the input and output buffer/interface.
     */
    template <class io_device>
    class iostream: public istream<io_device>, public ostream<io_device>
    {
    public:
        // *** Constructors ***
        iostream(void) = delete;
        iostream(const iostream&) = delete;
        iostream(iostream&&) = delete;
        iostream(io_device& device): ostream<io_device>(device) {};

        // *** Destructor ***
        ~iostream(void) {};

        // *** Operators ***
        iostream& operator=(const iostream&) = delete;
        iostream& operator=(iostream&&) = delete;

        // *** Methods ***
        // iostream& get(char& c) { this->pimpl->get(c); return *this;};
        // iostream& read(char* str, std::size_t n) { this->pimpl->read(str, n); return *this;};
    };

}; // !namespace OTOS

#endif
