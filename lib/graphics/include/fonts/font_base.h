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

#ifndef FONT_BASE_H_
#define FONT_BASE_H_

namespace Font
{
    /**
     * @brief Struct to store the properties of a font.
     * 
     * @property data: Pointer to the font data.
     * @property width_px: Width of the font in pixels.
     * @property height_px: Height of the font in pixels.
     * @property stride: The of occupied pages assuming one page has 8 pixels.
     */
    struct Base_t
    {
        const unsigned char * const data;
        const unsigned char width_px;
        const unsigned char height_px;
        const unsigned char stride;
    };

};

#endif