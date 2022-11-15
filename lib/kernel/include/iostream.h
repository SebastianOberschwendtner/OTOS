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
#include <array>
#include <string_view>
#include <charconv>

// === Details ===
namespace detail
{
    constexpr std::size_t max_int_decimals = 10;
}; // !namespace detail

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
        ostream& put(const char c) { pimpl->put(c); return *this;}; 
        ostream& write(const char* str, const std::size_t n) { pimpl->write(str, n); return *this;};
        ostream& flush(void) { pimpl->flush(); return *this;};

        // *** Overloaded stream operators ***
        /**
         * @brief Add a string to the stream.
         *
         * => Prefer using `std::string_views` instead of null terminated strings.
         * String views can be implemented more efficient.
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

        /**
         * @brief Add a const string to the stream.
         *
         * => Prefer using `std::string_views` instead of null terminated strings.
         * String views can be implemented more efficient.
         * 
         * @param str The pointer to the  null terminated (!) string to add.
         * @return ostream& Returns a reference to the stream.
         */
        ostream& operator<<(const char * str)
        {
            // Get the temporary non-const pointer to the data
            auto ptr = const_cast<char *>(str);

            // Loop as long as the string is not terminated
            while(*ptr)
                this->put(*ptr++);
            return *this;
        }; 

        /**
         * @brief Add a string view to the stream.
         * 
         * @param str_view The reference to the string view to add.
         * @return ostream& Returns a reference to the stream.
         */
        ostream& operator<<(const std::string_view & str_view)
        {
            // Write all characters of the string view
            this->write(str_view.data(), str_view.size());
            return *this;
        };

        /**
         * @brief  Interface for manipulators.
         *
         * Manipulators such as OTOS::endl use these
         * functions in constructs like "out << OTOS::endl".  For more
         * more information refer to the C++ standard as this construct
         * is taken from there.
         * @param __pf The function pointer to the manipulator.
         * @return Returns a reference to the stream.
         */
        ostream & operator<<(ostream &(*__pf)(ostream &))
        {
            return __pf(*this);
        };

        /**
         * @brief Add an integer number to the stream.
         *
         * The maximum number of decimals which can be 
         * converted is defined by `detail::max_int_decimals`.
         * 
         * @tparam int_type The integral type of the number to add.
         * @param num The number to add.
         * @return ostream& Returns a reference to the stream.
         */
        template <typename int_type>
        ostream& operator<<(const int_type num)
        {
            // Check if the template type is an integral type
            static_assert(std::is_integral_v<int_type>, "Only integral types are allowed.");

            // Temporary container for result
            std::array<char, detail::max_int_decimals> result;

            // convert the number to a string
            auto [ptr, ec] = std::to_chars(
                    result.data(),
                    result.data() + result.size(),
                    num);
            // @TODO Check for errors in the string conversion
            
            // Calculate the length of the result
            auto len = std::distance(result.begin(), ptr);

            // Write the string to the stream
            this->write(result.begin(), len);
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

    // === Manipulators ===

    /**
     * @brief Function template to end the current line.
     * In the C++ standard this manipulator also flushes the stream.
     * This behavior might be added in the future.
     * 
     * @tparam o_device The output device which holds the output buffer/interface.
     * @param stream The reference to the output stream.
     * @return Returns a reference to the output stream.
     */
    template <class o_device>
    inline ostream<o_device>& endl(ostream<o_device>& stream)
    {
        // write one character, since put() does not always check for newlines
        stream.write("\n", 1);
        return stream;
    };

}; // !namespace OTOS

#endif
