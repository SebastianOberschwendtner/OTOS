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
 * @version v1.0.9
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
        this->buffer[x_pos + (this->width) * y_pos] = static_cast<bool>(color);
};

/**
 * @brief Fill the complete canvas with one color
 * @param color The color for filling the canvas
 */
void Graphics::Canvas_BW::fill(const Color_BW color)
{
    for (unsigned int iPixel = 0; iPixel < this->pixels; iPixel++)
        this->buffer[iPixel] = static_cast<bool>(color);
};

/**
 * @brief Draw a line between two points
 * @param start Coordinate where line begins
 * @param end   Coordinate where line ends
 */
void Graphics::Canvas_BW::draw(const Coordinate start, const Coordinate end)
{
    for (unsigned int iPixel_y = start.y_pos; iPixel_y <= end.y_pos; iPixel_y++)
        for (unsigned int iPixel_x = start.x_pos; iPixel_x <= end.x_pos; iPixel_x++)
            this->draw_pixel(iPixel_x, iPixel_y, White);
};