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
/**
 ==============================================================================
 * @file    graphics.c
 * @author  SO
 * @version v3.0.0
 * @date    13-September-2021
 * @brief   Graphics interface for the OTOS.
 ==============================================================================
 */

// === Includes ===
#include "graphics.h"
#include <cstdint>
#include <cstdlib>
#include <utility>

// === Functions ===

/**
 * @brief Set the cursor position according to the current font size.
 * Does not exceed the display limits
 * @param x_pos The character x position
 * @param y_pos The character y position
 */
void Graphics::Canvas_BW::set_cursor(const unsigned int x_pos, const unsigned int y_pos)
{
    // Get the new cursor positions
    this->cursor.x_pos = x_pos * Font::x_pixels(this->font_type) * this->scaling;
    this->cursor.y_pos = y_pos * Font::y_pixels(this->font_type) * this->scaling;

    // Limit the cursor to the display bounds
    if (this->cursor.x_pos >= this->width)
        this->cursor.x_pos = 0;
    if (this->cursor.y_pos >= this->height)
        this->cursor.y_pos = 0;
};

/**
 * @brief Set the font size to use for characters
 * @param size The new font size
 * @param scale The scale factor the font will be scaled with
 */
void Graphics::Canvas_BW::set_font(const Font::Type size, const unsigned char scale)
{
    this->font_type = size;
    this->scaling = scale;
};

/**
 * @brief Set the cursor to a newline
 */
void Graphics::Canvas_BW::newline(void)
{
    // x is always 0
    this->cursor.x_pos = 0;

    // increase y
    this->cursor.y_pos += Font::y_pixels(this->font_type) * this->scaling;

    // Check bounds of y
    if (this->cursor.y_pos >= this->height)
        this->cursor.y_pos = 0;
};

/**
 * @brief Draw a pixel with the defined color in the assigned buffer.
 * @param x_pos The x coordinate of the pixel
 * @param y_pos The y coordinate of the pixel
 * @param color The new color of the pixel
 */
void Graphics::Canvas_BW::draw_pixel(const unsigned int x_px, const unsigned int y_px,
                                     const Color_BW color)
{
    if (x_px < this->width && y_px < this->height)
    {
        unsigned int page = y_px / 8;
        unsigned char y_page = y_px % 8;
        unsigned char bit_mask = (1 << y_page);
        switch (color)
        {
        case Black:
            this->buffer[x_px + (this->width) * page] &= ~bit_mask;
            break;
        case White:
            this->buffer[x_px + (this->width) * page] |= bit_mask;
            break;
        };
    };
};

/**
 * @brief Fill the complete canvas with one color
 * @param color The color for filling the canvas
 */
void Graphics::Canvas_BW::fill(const Color_BW color)
{
    for (unsigned int iByte = 0; iByte < this->pixels / 8; iByte++)
        this->buffer[iByte] = static_cast<unsigned char>(color);
};

/**
 * @brief Add a horizontal line on the canvas
 * @param start Coordinate where line begins
 * @param length Length of the line in pixels
 * @param color The color of the line. Default is White.
 * @param dotted (Optional) Distance of dots in line.
 */
void Graphics::Canvas_BW::add_line_h(
    const Coordinate start,
    const unsigned int length,
    unsigned char dotted,
    const Color_BW color)
{
    // Get the repeating bit which will be set
    unsigned int page = start.y_pos / 8;
    unsigned char y_page = start.y_pos - 8 * page;
    unsigned char bit_mask = (1 << y_page);

    // Set every bit for the length of the line
    for (unsigned int iPixel = 0; iPixel < length; iPixel += 1 + dotted)
    {
        if(color == Color_BW::White)
            this->buffer[start.x_pos + iPixel + this->width * page] |= bit_mask;
        else
            this->buffer[start.x_pos + iPixel + this->width * page] &= ~bit_mask;
    }
};

/**
 * @brief Add a vertical line on the canvas
 * @param start Coordinate where line begins
 * @param color The color of the line. Default is White.
 * @param length Length of the line in pixels
 */
void Graphics::Canvas_BW::add_line_v(
    const Coordinate start,
    const unsigned int length,
    const Color_BW color)
{
    // Get the page where the line starts
    unsigned int page = start.y_pos / 8;

    // Get the first and last bitmask
    unsigned char y_page = start.y_pos - 8 * page;
    unsigned char bitmask_first = (0xFF << (y_page));
    unsigned char bitmask_last = 0xFF;

    // Get the number of affected pages
    unsigned int n_pages = (length - 1) / 8;
    unsigned char bit_modulo = (start.y_pos + length) % 8;
    if (bit_modulo > 0)
    {
        // the line does not fit in an even multiple of pages
        // so last bitmask is not all ones
        bitmask_last = (0xFF >> (8 - bit_modulo));

        // when the line is shorter than one page, but still does occupy two pages
        if (((length - 1) % 8 + y_page) > 7)
            n_pages++;
    }

    // Set the bitmask when only one page is involved
    if (n_pages == 0)
    {
        if (color == Color_BW::White)
            this->buffer[start.x_pos + this->width * page] |= bitmask_first & bitmask_last;
        else
            this->buffer[start.x_pos + this->width * page] &= ~(bitmask_first & bitmask_last);
    }

    // Set the bitmask when there are multiple pages involved
    if (n_pages > 0)
    {
        if (color == Color_BW::White)
        {
            this->buffer[start.x_pos + this->width * page] |= bitmask_first;
            this->buffer[start.x_pos + this->width * (page + n_pages)] |= bitmask_last;
        }
        else
        {
            this->buffer[start.x_pos + this->width * page] &= ~bitmask_first;
            this->buffer[start.x_pos + this->width * (page + n_pages)] &= ~bitmask_last;
        }
    }

    // Set the in between bits, when there are in between bits
    if (n_pages > 1)
    {
        for (unsigned int iPage = 1; iPage < n_pages; iPage++)
        {
            if ( color == Color_BW::White)
                this->buffer[start.x_pos + this->width * (page + iPage)] = 0xFF;
            else
                this->buffer[start.x_pos + this->width * (page + iPage)] = 0x00;
        }
    }
};

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
 */
void Graphics::Canvas_BW::add_line(
    const Coordinate start, const Coordinate end,
    const Color_BW color)
{
    std::int16_t x0 = start.x_pos;
    std::int16_t y0 = start.y_pos;
    std::int16_t x1 = end.x_pos;
    std::int16_t y1 = end.y_pos;

    std::int16_t steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    std::int16_t dx, dy;
    dx = x1 - x0;
    dy = std::abs(y1 - y0);

    std::int16_t err = dx / 2;
    std::int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
            this->draw_pixel(static_cast<unsigned int>(y0), static_cast<unsigned int>(x0), color);
        else
            this->draw_pixel(static_cast<unsigned int>(x0), static_cast<unsigned int>(y0), color);
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
};

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
 */
void Graphics::Canvas_BW::add_circle(
    const Coordinate center, const unsigned int radius,
    const Color_BW color)
{
    std::int16_t x0 = center.x_pos, y0 = center.y_pos;
    std::int16_t f = 1 - radius;
    std::int16_t ddF_x = 1;
    std::int16_t ddF_y = -2 * radius;
    std::int16_t x = 0;
    std::int16_t y = radius;

    this->draw_pixel(static_cast<unsigned int>(x0), static_cast<unsigned int>(y0 + radius), color);
    this->draw_pixel(static_cast<unsigned int>(x0), static_cast<unsigned int>(y0 - radius), color);
    this->draw_pixel(static_cast<unsigned int>(x0 + radius), static_cast<unsigned int>(y0), color);
    this->draw_pixel(static_cast<unsigned int>(x0 - radius), static_cast<unsigned int>(y0), color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        this->draw_pixel(static_cast<unsigned int>(x0 + x), static_cast<unsigned int>(y0 + y), color);
        this->draw_pixel(static_cast<unsigned int>(x0 - x), static_cast<unsigned int>(y0 + y), color);
        this->draw_pixel(static_cast<unsigned int>(x0 + x), static_cast<unsigned int>(y0 - y), color);
        this->draw_pixel(static_cast<unsigned int>(x0 - x), static_cast<unsigned int>(y0 - y), color);
        this->draw_pixel(static_cast<unsigned int>(x0 + y), static_cast<unsigned int>(y0 + x), color);
        this->draw_pixel(static_cast<unsigned int>(x0 - y), static_cast<unsigned int>(y0 + x), color);
        this->draw_pixel(static_cast<unsigned int>(x0 + y), static_cast<unsigned int>(y0 - x), color);
        this->draw_pixel(static_cast<unsigned int>(x0 - y), static_cast<unsigned int>(y0 - x), color);
    }
};

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
 */
void Graphics::Canvas_BW::fill_circle(
    const Coordinate center, const unsigned int radius,
    const Color_BW color)
{
    std::int16_t x0 = center.x_pos, y0 = center.y_pos;

    std::int16_t f = 1 - radius;
    std::int16_t ddF_x = 1;
    std::int16_t ddF_y = -2 * radius;
    std::int16_t x = 0;
    std::int16_t y = radius;
    std::int16_t px = x;
    std::int16_t py = y;

    std::int16_t delta = 1;

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (x < (y + 1))
        {
            this->add_line_v(
                {static_cast<unsigned int>(x0 + x), static_cast<unsigned int>(y0 - y)},
                static_cast<unsigned int>(2 * y + delta),
                color);
            this->add_line_v(
                {static_cast<unsigned int>(x0 - x), static_cast<unsigned int>(y0 - y)},
                static_cast<unsigned int>(2 * y + delta),
                color);
        }
        if (y != py)
        {
            this->add_line_v(
                {static_cast<unsigned int>(x0 + py), static_cast<unsigned int>(y0 - px)},
                static_cast<unsigned int>(2 * px + delta),
                color);
            this->add_line_v(
                {static_cast<unsigned int>(x0 - py), static_cast<unsigned int>(y0 - px)},
                static_cast<unsigned int>(2 * px + delta),
                color);
            py = y;
        }
        px = x;
    }
    this->add_line_v(
        {static_cast<unsigned int>(x0), static_cast<unsigned int>(y0 - radius)},
        static_cast<unsigned int>(2 * radius + 1),
        color);
};

/**
 * @brief Add a character to the canvas
 * @param character The character to display.
 */
void Graphics::Canvas_BW::add_char(const unsigned char character)
{
    // get pixelsize of font
    unsigned char width = Font::x_pixels(this->font_type);
    unsigned char height = Font::y_pixels(this->font_type);
    unsigned char height_pages = height / 8;

    // Check whether scaling is active
    if (this->scaling < 2)
    {
        // Write character depending in font since there is no scaling active
        if (this->font_type == Font::Type::Small)
        {
            // Write the pixels, the small font has 6 pixels in x direction
            for (unsigned char iX = 0; iX < width; iX++)
                this->buffer[iX + this->cursor.x_pos + (this->width * this->cursor.y_pos / 8)] =
                    Font::Font_Small[character][iX];
        }
        else
        {
            // Write the pixels, the normal font has 12 pixels in x direction
            for (unsigned char iY = 0; iY < height_pages; iY++)
                for (unsigned char iX = 0; iX < width; iX++)
                    this->buffer[iX + this->cursor.x_pos + (this->width * (iY + (this->cursor.y_pos / 8)))] =
                        Font::Font_Normal[character][((height_pages) * iX) + 1 - iY];
        }
        // append the cursor
        this->cursor.x_pos += width;
    }
    else // Use font scaling
    {
        // Variable for color of the pixel
        Color_BW color = White;

        // loop through every pixel and set it according to the scaling
        for (unsigned char iX = 0; iX < width; iX++)
        {
            for (unsigned char iY = 0; iY < height; iY++)
            {
                // Get the color of the pixel
                if (this->font_type == Font::Type::Small)
                    color = (Font::Font_Small[character][iX] & (1 << iY)) ? Color_BW::White : Color_BW::Black;
                else
                    color = (Font::Font_Normal[character][2 * iX + 1 - (iY / 8)] & (1 << (iY % 8))) ? Color_BW::White : Color_BW::Black;

                // Draw the scaled pixel
                this->draw_pixel_with_scaling({iX, iY}, color);
            }
        }
        // append the cursor
        this->cursor.x_pos += width * this->scaling;
    }
};

/**
 * @brief Add a string to the canvas
 * String has to be null terminated!
 * Newline characters are supported.
 * @param string The pointer to the string to display.
 */
void Graphics::Canvas_BW::add_string(const char *string)
{
    char *character = const_cast<char *>(string);

    while (*character != 0)
    {
        if (*character == '\n')
            this->newline();
        else
            this->add_char(*character);
        character++;
    }
};

/**
 * @brief Add a number to the canvas using
 * the number font.
 * @param number The number to display.
 */
void Graphics::Canvas_BW::add_number(const unsigned char number)
{
    // Temporary variables to get font size
    constexpr unsigned char width = 16;
    constexpr unsigned char height = 20;

    // Write the number using the number font
    // Write the pixels, the number font has 20 pixels in x direction
    for (unsigned char iY = 0; iY < width; iY++)
    {
        for (unsigned char iX = 0; iX < height; iX++)
        {
            // Get the color of the pixel
            Color_BW color = (Font::Font_Number[number][2 * iX + (iY / 8)] & (1 << (iY % 8))) ? Color_BW::White : Color_BW::Black;

            // Draw the scaled pixel
            this->draw_pixel_with_scaling({iY, iX}, color);
        }
    }

    // append the cursor
    this->cursor.x_pos += width * this->scaling;
};

void Graphics::Canvas_BW::draw_pixel_with_scaling(const Coordinate base, const Color_BW color)
{
    // Set the pixel in the buffer, the number of pixels is the square of the scaling
    for (unsigned char iScaleX = 0; iScaleX < this->scaling; iScaleX++)
    {
        for (unsigned char iScaleY = 0; iScaleY < this->scaling; iScaleY++)
        {
            this->draw_pixel(this->cursor.x_pos + base.x_pos * this->scaling + iScaleX,
                             this->cursor.y_pos + base.y_pos * this->scaling + iScaleY,
                             color);
        }
    }
}