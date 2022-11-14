/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

// === Includes ===
#include <array>
#include "font.h"
#include "iostream.h"

// === Declarations ===
namespace Graphics {
    // === Enumerations ===
    enum Color_BW: unsigned char
    {
        Black = 0x00,
        White = 0xFF
    };    

    // === Classes ===
    template<unsigned int Width_px, unsigned int Height_px>
    class Buffer_BW
    {
    public:
        // the height of the display should be divisible by 8
        static_assert((Height_px%8) == 0, "Pixel height of the display is not a multiple of 8!");

        // Properties buffer + sizes
        std::array<unsigned char, Width_px * Height_px / 8> data;
        constexpr static unsigned int width_px = Width_px;
        constexpr static unsigned int height_px = Height_px;
        constexpr static unsigned int pixels = Width_px * Height_px;

        // Constructor
        Buffer_BW() { this->data.fill(0); };
    };

    struct Coordinate
    {
        // *** Properties ***
        unsigned int x_pos = 0;
        unsigned int y_pos = 0;

        // *** Constructor ***
        Coordinate(const unsigned int x, const unsigned y):
            x_pos(x), y_pos(y) {};

        // *** Methods ***
        void set(const unsigned int new_x, const unsigned int new_y) { this->x_pos = new_x; this->y_pos = new_y; };
        Coordinate operator+(const Coordinate& rhs) const { return Coordinate(this->x_pos + rhs.x_pos, this->y_pos + rhs.y_pos); };
        Coordinate operator-(const Coordinate& rhs) const { return Coordinate(this->x_pos - rhs.x_pos, this->y_pos - rhs.y_pos); };
        Coordinate operator+=(const Coordinate& rhs) { this->x_pos += rhs.x_pos; this->y_pos += rhs.y_pos; return *this; };
        Coordinate operator-=(const Coordinate& rhs) { this->x_pos -= rhs.x_pos; this->y_pos -= rhs.y_pos; return *this; };
    };

    class Canvas_BW: public OTOS::ostream<Canvas_BW>
    {
    private:
        // *** properties
        unsigned char* const    buffer;
        const unsigned int      width;
        const unsigned int      height;
        const unsigned int      pixels;
        const Font::Base_t*     font{&Font::_8px::Default};
        unsigned char           scaling{1};

        // *** Methods ***
        void draw_pixel_with_scaling(const Coordinate base, Color_BW color);

    public:
        // *** properties
        Coordinate cursor;

        // *** Constructor ***
        Canvas_BW() = delete;
        Canvas_BW(unsigned char* const buffer, const unsigned int width, const unsigned int height)
            : OTOS::ostream<Canvas_BW>{*this}
            , buffer(buffer)
            , width(width)
            , height(height)
            , pixels(height * width)
            , cursor(0,0){};

        // *** Methods ***

        void        set_cursor  (const unsigned int x_pos, const unsigned int y_pos);
        void        set_font(const Font::Base_t& type, const unsigned char scale = 1);
        void        newline     (void);
        void        draw_pixel  (const unsigned int x_px, const unsigned int y_px, const Color_BW color);
        void        fill        (const Color_BW color);
        void        add_line_h  (const Coordinate start, const unsigned int length, const unsigned char dotted = 0, const Color_BW color = White);
        void        add_line_v  (const Coordinate start, const unsigned int length, const Color_BW color = White);
        void        add_line    (const Coordinate start, const Coordinate end, const Color_BW color = White);
        void        add_circle  (const Coordinate center, const unsigned int radius, const Color_BW color = White);
        void        fill_circle (const Coordinate center, const unsigned int radius, const Color_BW color = White);
        void        put         (const char character);
        void        write       (const char* str, const std::size_t len);
        void        add_number  (const unsigned char number);
    };
};
#endif
