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

namespace Font {
    constexpr unsigned char font_8x16[5] = {
        't', 'e', 's', 't', 0
    };
};

// === Declarations ===
namespace Graphics {
    // === Enumerations ===
    enum Color_BW: bool
    {
        Black = false,
        White = true
    };    

    // === Classes ===
    template<typename T, unsigned int Width_px, unsigned int Height_px>
    class Buffer
    {
    public:
        union buffer_union
        {
        T array[Width_px * Height_px];
        T coordinates[Height_px][Width_px];
        };
        buffer_union data;
        constexpr static unsigned int width = Width_px;
        constexpr static unsigned int height = Height_px;
        constexpr static unsigned int pixels = Width_px * Height_px;
    };

    class Coordinate
    {
    public:
        // *** Properties ***
        unsigned int x_pos;
        unsigned int y_pos;

        // *** Constructor ***
        Coordinate(const unsigned int x, const unsigned y):
            x_pos(x), y_pos(y) {};

        // *** Methods ***
        void set(const unsigned int new_x, const unsigned int new_y) { this->x_pos = new_x; this->y_pos = new_y; };
    };

    class Canvas_BW
    {
    private:
        bool* const         buffer;
        const unsigned int  width;
        const unsigned int  height;
        const unsigned int  pixels;

    public:
        // *** Constructor ***
        Canvas_BW(bool* const buffer, const unsigned int width, const unsigned int height):
            buffer(buffer), width(width), height(height), pixels(height * width) {};

        // *** Methods ***
        void        draw_pixel  (const unsigned int x_pos, const unsigned int y_pos, const Color_BW color);
        void        fill        (const Color_BW color);
        void        draw        (const Coordinate start, const Coordinate stop);

    };
};
#endif