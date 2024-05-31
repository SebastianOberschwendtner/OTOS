/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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

/* === Includes === */
#include "font.h"
#include <array>
#include <iostream.h>

namespace graphics
{
    /* === Enumerations === */
    enum Color_BW : uint8_t
    {
        Black = 0x00,
        White = 0xFF
    };

    /**
     * @class Buffer_BW
     * @brief Buffer containing black and white pixels.
     *
     * @tparam Width_px The width of the buffer in pixels.
     * @tparam Height_px The height of the buffer in pixels.
     */
    template <uint32_t Width_px, uint32_t Height_px>
    class Buffer_BW
    {
      public:
        /* the height of the display should be divisible by 8 */
        static_assert((Height_px % 8) == 0, "Pixel height of the display is not a multiple of 8!");

        /* === Constructors === */
        Buffer_BW() { this->data.fill(0); };

        /* === Properties === */
        std::array<uint8_t, Width_px * Height_px / 8> data;      /**< The array containing the pixel data. */
        constexpr static uint32_t width_px = Width_px;           /**< [px] The width of the buffer. */
        constexpr static uint32_t height_px = Height_px;         /**< [px] The height of the buffer. */
        constexpr static uint32_t pixels = Width_px * Height_px; /**< [px] The total number of pixels. */
    };

    /**
     * @struct Coordinate
     * @brief A coordinate in the 2D space.
     */
    struct Coordinate
    {
        /* === Constructor === */
        Coordinate(const uint32_t x, const unsigned y) : x_pos(x), y_pos(y){};

        /* === Setters === */
        /**
         * @brief Set the x and y position of the coordinate.
         *
         * @param new_x [px] The new x position.
         * @param new_y [px] The new y position.
         */
        void set(const uint32_t new_x, const uint32_t new_y)
        {
            this->x_pos = new_x;
            this->y_pos = new_y;
        };

        /* === Methods === */
        Coordinate operator+(const Coordinate &rhs) const { return Coordinate(this->x_pos + rhs.x_pos, this->y_pos + rhs.y_pos); };
        Coordinate operator-(const Coordinate &rhs) const { return Coordinate(this->x_pos - rhs.x_pos, this->y_pos - rhs.y_pos); };
        Coordinate operator+=(const Coordinate &rhs)
        {
            this->x_pos += rhs.x_pos;
            this->y_pos += rhs.y_pos;
            return *this;
        };
        Coordinate operator-=(const Coordinate &rhs)
        {
            this->x_pos -= rhs.x_pos;
            this->y_pos -= rhs.y_pos;
            return *this;
        };

        /* === Properties === */
        uint32_t x_pos = 0; /**< [px] The x position of the coordinate. */
        uint32_t y_pos = 0; /**< [px] The y position of the coordinate. */
    };

    /**
     * @class Canvas_BW
     * @brief A canvas to draw black and white pixels.
     */
    class Canvas_BW : public OTOS::ostream<Canvas_BW>
    {
      public:
        /* === Constructor === */
        Canvas_BW() = delete;
        Canvas_BW(uint8_t *const buffer, const uint32_t width, const uint32_t height)
            : OTOS::ostream<Canvas_BW>{*this}, buffer(buffer), width(width), height(height), pixels(height * width), cursor(0, 0){};

        /* === Setters === */
        /**
         * @brief Set the cursor position according to the current font size.
         * Does not exceed the display limits
         * @param x_pos The character x position
         * @param y_pos The character y position
         * @return Canvas_BW& The reference to the canvas
         */
        auto set_cursor(uint32_t x_pos, uint32_t y_pos) -> Canvas_BW &;

        /**
         * @brief Set the font type to use for characters
         * @param type The new font type
         * @param scale The scale factor the font will be scaled with
         * @return Canvas_BW& The reference to the canvas
         */
        auto set_font(const font::Base_t &type, uint8_t scale = 1) -> Canvas_BW &;

        /* === Methods === */
        /**
         * @brief Add a circle on the canvas
         *
         * @param center Coordinate of the center of the circle
         * @param radius The radius of the circle
         * @param color The color of the circle. Defaults is White.
         * @attention
         * The implementation of this function is taken from the ADAfruit GFX library
         * with its license:
         *
         *     Software License Agreement (BSD License)
         *
         *     Copyright (c) 2012 Adafruit Industries.  All rights reserved.
         * @return Canvas_BW& The reference to the canvas
         */
        auto add_circle(Coordinate center, uint32_t radius, Color_BW color = White) -> Canvas_BW &;

        /**
         * @brief Add an arbitrary line on the canvas
         * using the Bresenham's algorithm.
         *
         * @param start Coordinate where line begins
         * @param end Coordinate where line ends
         * @param color The color of the line. Default is White.
         * @attention
         * The implementation of this function is taken from the ADAfruit GFX library
         * with its license:
         *
         *     Software License Agreement (BSD License)
         *
         *     Copyright (c) 2012 Adafruit Industries.  All rights reserved.
         * @return Canvas_BW& The reference to the canvas
         */
        auto add_line(Coordinate start, Coordinate end, Color_BW color = White) -> Canvas_BW &;

        /**
         * @brief Add a horizontal line on the canvas
         * @param start Coordinate where line begins
         * @param length Length of the line in pixels
         * @param color The color of the line. Default is White.
         * @param dotted (Optional) Distance of dots in line.
         * @return Canvas_BW& The reference to the canvas
         */
        auto add_line_h(Coordinate start, uint32_t length, uint8_t dotted = 0, Color_BW color = White) -> Canvas_BW &;

        /**
         * @brief Add a vertical line on the canvas
         * @param start Coordinate where line begins
         * @param color The color of the line. Default is White.
         * @param length Length of the line in pixels
         * @return Canvas_BW& The reference to the canvas
         */
        auto add_line_v(Coordinate start, uint32_t length, Color_BW color = White) -> Canvas_BW &;

        /**
         * @brief Add a number to the canvas using
         * the number font.
         * @param number The number to display.
         * @return Canvas_BW& The reference to the canvas
         */
        auto add_number(uint8_t number) -> Canvas_BW &;

        /**
         * @brief Draw a pixel with the defined color in the assigned buffer.
         * @param x_pos The x coordinate of the pixel
         * @param y_pos The y coordinate of the pixel
         * @param color The new color of the pixel
         * @return Canvas_BW& The reference to the canvas
         */
        auto draw_pixel(uint32_t x_px, uint32_t y_px, Color_BW color) -> Canvas_BW &;

        /**
         * @brief Fill the complete canvas with one color
         * @param color The color for filling the canvas
         * @return Canvas_BW& The reference to the canvas
         */
        auto fill(Color_BW color) -> Canvas_BW &;

        /**
         * @brief Fill a circle on the canvas
         *
         * @param center Coordinate of the center of the circle
         * @param radius The radius of the circle
         * @param color The color of the circle. Defaults is White.
         * @attention
         * The implementation of this function is taken from the ADAfruit GFX library
         * with its license:
         *
         *     Software License Agreement (BSD License)
         *
         *     Copyright (c) 2012 Adafruit Industries.  All rights reserved.
         * @return Canvas_BW& The reference to the canvas
         */
        auto fill_circle(Coordinate center, uint32_t radius, Color_BW color = White) -> Canvas_BW &;

        /**
         * @brief Provide the flush method as required by the ostream interface.
         * The function does nothing as of now. This may change in the future.
         */
        void flush();

        /**
         * @brief Set the cursor to a newline
         * @return Canvas_BW& The reference to the canvas
         */
        auto newline() -> Canvas_BW &;

        /**
         * @brief Add a character to the canvas
         * @param character The character to display.
         * @return Canvas_BW& The reference to the canvas
         */
        auto put(char character) -> Canvas_BW &;

        /**
         * @brief Add a string to the canvas
         *
         * Newline characters are supported.
         * @param string The pointer to the string to display.
         * @param len The number of bytes to write.
         * @return Canvas_BW& The reference to the canvas
         */
        auto write(const char *str, std::size_t len) -> Canvas_BW &;

        /* === Properties === */
        Coordinate cursor; /**< The current cursor position. */

      private:
        /* === Methods === */
        /**
         * @brief Draw a pixel with the defined color in the assigned buffer.
         * This does respect the scaling factor.
         * 
         * @param base The base coordinate of the pixel
         * @param color The new color of the pixel
         */
        void draw_pixel_with_scaling(Coordinate base, Color_BW color);

        /* === Properties === */
        uint8_t *buffer;                                /**< Pointer to the buffer containing the pixel data. */
        uint32_t width;                                 /**< [px] The width of the canvas. */
        uint32_t height;                                /**< [px] The height of the canvas. */
        uint32_t pixels;                                /**< The total number of pixels. */
        const font::Base_t *font{&font::_8px::Default}; /**< The font to use for drawing text. */
        uint8_t scaling{1};                             /**< The scaling factor for the font. */
    };
};
#endif
