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
 * @version v1.2.0
 * @date    13-September-2021
 * @brief   Graphics interface for the OTOS.
 ==============================================================================
 */

// === Includes ===
#include "graphics.h"

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
    this->cursor.x_pos = x_pos * Font::x_pixels(this->current_size);
    this->cursor.y_pos = y_pos * Font::y_pixels(this->current_size);

    // Limit the cursor to the display bounds
    if (this->cursor.x_pos >= this->width) this->cursor.x_pos = 0;
    if (this->cursor.y_pos >= this->height) this->cursor.y_pos = 0;
};

/**
 * @brief Set the font size to use for characters
 * @param size The new fontsize
 */
void Graphics::Canvas_BW::set_fontsize( const Font::Size size)
{
    this->current_size = size;
};

/**
 * @brief Set the cursor to a newline
 */
void Graphics::Canvas_BW::newline(void)
{
    // x is always 0
    this->cursor.x_pos = 0;

    // increase y
    this->cursor.y_pos += Font::y_pixels(this->current_size);

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
        unsigned char y_page = (y_px - page);
        unsigned char bit_mask = (1 << y_page);
        switch(color)
        {
            case Black: this->buffer[x_px + (this->width) * page] &= ~bit_mask; break;
            case White: this->buffer[x_px + (this->width) * page] |= bit_mask; break;
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
 * @param dotted (Optional) Distance of dots in line.
 */
void Graphics::Canvas_BW::add_line_h(const Coordinate start, const unsigned int length,
    unsigned char dotted)
{
    // Get the repeating bit which will be set
    unsigned int page = start.y_pos / 8;
    unsigned char y_page = start.y_pos - 8*page;
    unsigned char bit_mask = (1 << y_page);

    // Set every bit for the length of the line
    for (unsigned int iPixel = 0; iPixel < length; iPixel += 1 + dotted)
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

/**
 * @brief Add a character to the canvas
 * @param character The character to display.
 */
void Graphics::Canvas_BW::add_char(const unsigned char character)
{
    // Temporary variables to get font size
    unsigned char width = 0;
    unsigned char height = 0;

    // Write character depending in font
    switch(this->current_size)
    {

    case Font::Size::Small:
        // get pixelsize of font
        width = Font::x_pixels(Font::Size::Small);
        height = Font::y_pixels(Font::Size::Small);

        // Write the pixels, the small font has 6 pixels in x direction
        for (unsigned char iX = 0; iX < width; iX++)
            this->buffer[iX + this->cursor.x_pos + (this->width * this->cursor.y_pos/8)] 
            = Font::Font_Small[character][iX];
        break;

    case Font::Size::Normal:
        // get pixelsize of font
        width = Font::x_pixels(Font::Size::Normal);
        height = Font::y_pixels(Font::Size::Normal);

        // Write the pixels, the normal font has 12 pixels in x direction
        for (unsigned char iX = 0; iX < width; iX++)
            for (unsigned char iY = 0; iY < height/8; iY++)
                this->buffer[iX + this->cursor.x_pos + (this->width * (iY + (this->cursor.y_pos/8)))] 
                = Font::Font_Normal[character][2*iX + 1 - iY];
        break;

    default:
        break;
    };
    // append the cursor
    this->cursor.x_pos += width;
};

/**
 * @brief Add a string to the canvas
 * String has to be null terminated!
 * Newline characters are supported.
 * @param string The pointer to the string to display.
 */
void Graphics::Canvas_BW::add_string(const char* string)
{
    char* character = const_cast<char*>(string);

    while(*character != 0)
    {
        if(*character == '\n')
            this->newline();
        else
            this->add_char(*character);
        character++;
    }
};