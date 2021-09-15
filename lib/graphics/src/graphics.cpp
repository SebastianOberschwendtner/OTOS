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
 * @version v1.0.10
 * @date    13-September-2021
 * @brief   Graphics interface for the OTOS.
 ==============================================================================
 */

// === Includes ===
#include "graphics.h"

// === Functions ===

/**
 * @brief Draw a pixel with the defined color in the assigned buffer.
 * @param x_pos The x coordinate of the pixel
 * @param y_pos The y coordinate of the pixel
 * @param color The new color of the pixel
 */
void Graphics::Canvas_BW::draw_pixel(const unsigned int x_pos, const unsigned int y_pos,
    const Color_BW color)
{
    if (x_pos < this->width && y_pos < this->height)
    {
        unsigned int page = y_pos / 8;
        unsigned char y_page = (y_pos - page);
        unsigned char bit_mask = (1 << y_page);
        switch(color)
        {
            case Black: this->buffer[x_pos + (this->width) * page] &= ~bit_mask; break;
            case White: this->buffer[x_pos + (this->width) * page] |= bit_mask; break;
        };
    };
};

/**
 * @brief Fill the complete canvas with one color
 * @param color The color for filling the canvas
 */
void Graphics::Canvas_BW::fill(const Color_BW color)
{
    for (unsigned int iByte = 0; iByte < this->pixels/8; iByte++)
        this->buffer[iByte] = static_cast<unsigned char>(color);
};

/**
 * @brief Add a horizontal line on the canvas
 * @param start Coordinate where line begins
 * @param length Length of the line in pixels
 */
void Graphics::Canvas_BW::add_line_h(const Coordinate start, const unsigned int length)
{
    // Get the repeating bit which will be set
    unsigned int page = start.y_pos / 8;
    unsigned char y_page = (start.y_pos - page);
    unsigned char bit_mask = (1 << y_page);

    // Set every bit for the length of the line
    for (unsigned int iPixel = 0; iPixel < length; iPixel++)
        this->buffer[start.x_pos + iPixel + this->width*page] |= bit_mask;
};

/**
 * @brief Add a vertical line on the canvas
 * @param start Coordinate where line begins
 * @param length Length of the line in pixels
 */
void Graphics::Canvas_BW::add_line_v(const Coordinate start, const unsigned int length)
{
    // Get the page where the line starts
    unsigned int page = start.y_pos / 8;

    // Get the first and last bitmask
    unsigned char y_page = start.y_pos - page;
    unsigned char bitmask_first = (0xFF << (y_page));
    unsigned char bitmask_last = 0xFF;

    // Get the number of affected pages
    unsigned int n_pages =  (length-1)/8;
    unsigned char bit_modulo = (start.y_pos + length)%8;
    if (bit_modulo > 0)
    {
        // the line does not fit in an even multiple of pages
        // so last bitmask is not all ones
        bitmask_last = (0xFF >> (8 - bit_modulo));
    }

    // Set the bitmask when only one page is involved
    if (n_pages == 0)
        this->buffer[start.x_pos + this->width*page] |= bitmask_first & bitmask_last;

    // Set the bitmask when there are multiple pages involved
    if (n_pages > 0)
    {
        this->buffer[start.x_pos + this->width*page] |= bitmask_first;
        this->buffer[start.x_pos + this->width*(page + n_pages)] |= bitmask_last;
    }

    // Set the in between bits, when there are in between bits
    if (n_pages > 1)
        for (unsigned int iPage = 1; iPage < n_pages; iPage++)
            this->buffer[start.x_pos + this->width*(page + iPage)] |= 0xFF;
};