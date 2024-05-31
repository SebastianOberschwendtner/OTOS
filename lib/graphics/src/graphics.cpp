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
/**
 ==============================================================================
 * @file    graphics.c
 * @author  SO
 * @version v5.0.0
 * @date    13-September-2021
 * @brief   Graphics interface for the OTOS.
 ==============================================================================
 */

/* === Includes === */
#include "graphics.h"
#include <cstdint>
#include <cstdlib>
#include <utility>

namespace graphics
{
    /* === Methods === */
    auto Canvas_BW::set_cursor(const uint32_t x_pos, const uint32_t y_pos) -> Canvas_BW &
    {
        /* Get the new cursor positions */
        this->cursor.x_pos = x_pos * this->font->width_px * this->scaling;
        this->cursor.y_pos = y_pos * this->font->height_px * this->scaling;

        /* Limit the cursor to the display bounds */
        if (this->cursor.x_pos >= this->width)
            this->cursor.x_pos = 0;
        if (this->cursor.y_pos >= this->height)
            this->cursor.y_pos = 0;

        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::set_font(const font::Base_t &type, const uint8_t scale) -> Canvas_BW &
    {
        /* Set the scaling parameter */
        this->scaling = scale;

        /* Add the font lookup */
        this->font = &type;

        /* Return a the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::add_circle(
        const Coordinate center, const uint32_t radius, const Color_BW color) -> Canvas_BW &
    {
        std::int16_t x0 = center.x_pos, y0 = center.y_pos;
        std::int16_t f = 1 - radius;
        std::int16_t ddF_x = 1;
        std::int16_t ddF_y = -2 * radius;
        std::int16_t x = 0;
        std::int16_t y = radius;

        this->draw_pixel(static_cast<uint32_t>(x0), static_cast<uint32_t>(y0 + radius), color);
        this->draw_pixel(static_cast<uint32_t>(x0), static_cast<uint32_t>(y0 - radius), color);
        this->draw_pixel(static_cast<uint32_t>(x0 + radius), static_cast<uint32_t>(y0), color);
        this->draw_pixel(static_cast<uint32_t>(x0 - radius), static_cast<uint32_t>(y0), color);

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

            this->draw_pixel(static_cast<uint32_t>(x0 + x), static_cast<uint32_t>(y0 + y), color);
            this->draw_pixel(static_cast<uint32_t>(x0 - x), static_cast<uint32_t>(y0 + y), color);
            this->draw_pixel(static_cast<uint32_t>(x0 + x), static_cast<uint32_t>(y0 - y), color);
            this->draw_pixel(static_cast<uint32_t>(x0 - x), static_cast<uint32_t>(y0 - y), color);
            this->draw_pixel(static_cast<uint32_t>(x0 + y), static_cast<uint32_t>(y0 + x), color);
            this->draw_pixel(static_cast<uint32_t>(x0 - y), static_cast<uint32_t>(y0 + x), color);
            this->draw_pixel(static_cast<uint32_t>(x0 + y), static_cast<uint32_t>(y0 - x), color);
            this->draw_pixel(static_cast<uint32_t>(x0 - y), static_cast<uint32_t>(y0 - x), color);
        }

        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::add_line(
        const Coordinate start, const Coordinate end, const Color_BW color) -> Canvas_BW &
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
                this->draw_pixel(static_cast<uint32_t>(y0), static_cast<uint32_t>(x0), color);
            else
                this->draw_pixel(static_cast<uint32_t>(x0), static_cast<uint32_t>(y0), color);
            err -= dy;
            if (err < 0)
            {
                y0 += ystep;
                err += dx;
            }
        }
        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::add_line_h(
        const Coordinate start,
        const uint32_t length,
        const uint8_t dotted,
        const Color_BW color) -> Canvas_BW &
    {
        /* Get the repeating bit which will be set */
        uint32_t page = start.y_pos / 8;
        uint8_t y_page = start.y_pos - 8 * page;
        uint8_t bit_mask = (1 << y_page);

        /* Set every bit for the length of the line */
        for (uint32_t iPixel = 0; iPixel < length; iPixel += 1 + dotted)
        {
            if (color == Color_BW::White)
                this->buffer[start.x_pos + iPixel + this->width * page] |= bit_mask;
            else
                this->buffer[start.x_pos + iPixel + this->width * page] &= ~bit_mask;
        }
        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::add_line_v(
        const Coordinate start, const uint32_t length, const Color_BW color) -> Canvas_BW &
    {
        /* Get the page where the line starts */
        uint32_t page = start.y_pos / 8;

        /* Get the first and last bitmask */
        uint8_t y_page = start.y_pos - 8 * page;
        uint8_t bitmask_first = (0xFF << (y_page));
        uint8_t bitmask_last = 0xFF;

        /* Get the number of affected pages */
        uint32_t n_pages = (length - 1) / 8;
        uint8_t bit_modulo = (start.y_pos + length) % 8;
        if (bit_modulo > 0)
        {
            /* the line does not fit in an even multiple of pages */
            /* so last bitmask is not all ones */
            bitmask_last = (0xFF >> (8 - bit_modulo));

            /* when the line is shorter than one page, but still does occupy two pages */
            if (((length - 1) % 8 + y_page) > 7)
                n_pages++;
        }

        /* Set the bitmask when only one page is involved */
        if (n_pages == 0)
        {
            if (color == Color_BW::White)
                this->buffer[start.x_pos + this->width * page] |= bitmask_first & bitmask_last;
            else
                this->buffer[start.x_pos + this->width * page] &= ~(bitmask_first & bitmask_last);
        }

        /* Set the bitmask when there are multiple pages involved */
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

        /* Set the in between bits, when there are in between bits */
        if (n_pages > 1)
        {
            for (uint32_t iPage = 1; iPage < n_pages; iPage++)
            {
                if (color == Color_BW::White)
                    this->buffer[start.x_pos + this->width * (page + iPage)] = 0xFF;
                else
                    this->buffer[start.x_pos + this->width * (page + iPage)] = 0x00;
            }
        }
        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::add_number(const uint8_t number) -> Canvas_BW &
    {
        /* Temporary variables to get font size */
        constexpr uint8_t width = 16;
        constexpr uint8_t height = 20;

        /* Write the number using the number font */
        /* Write the pixels, the number font has 20 pixels in x direction */
        for (uint8_t iY = 0; iY < width; iY++)
        {
            for (uint8_t iX = 0; iX < height; iX++)
            {
                /* Get the color of the pixel */
                Color_BW color = (font::Font_Number[number][2 * iX + (iY / 8)] & (1 << (iY % 8))) ? Color_BW::White : Color_BW::Black;

                /* Draw the scaled pixel */
                this->draw_pixel_with_scaling({iY, iX}, color);
            }
        }

        /* append the cursor */
        this->cursor.x_pos += width * this->scaling;
        
        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::draw_pixel(
        const uint32_t x_px, const uint32_t y_px, const Color_BW color) -> Canvas_BW &
    {
        if (x_px < this->width && y_px < this->height)
        {
            uint32_t page = y_px / 8;
            uint8_t y_page = y_px % 8;
            uint8_t bit_mask = (1 << y_page);
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

        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::fill(const Color_BW color) -> Canvas_BW &
    {
        for (uint32_t iByte = 0; iByte < this->pixels / 8; iByte++)
            this->buffer[iByte] = static_cast<uint8_t>(color);

        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::fill_circle(
        const Coordinate center, const uint32_t radius, const Color_BW color) -> Canvas_BW &
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
                    {static_cast<uint32_t>(x0 + x), static_cast<uint32_t>(y0 - y)},
                    static_cast<uint32_t>(2 * y + delta),
                    color);
                this->add_line_v(
                    {static_cast<uint32_t>(x0 - x), static_cast<uint32_t>(y0 - y)},
                    static_cast<uint32_t>(2 * y + delta),
                    color);
            }
            if (y != py)
            {
                this->add_line_v(
                    {static_cast<uint32_t>(x0 + py), static_cast<uint32_t>(y0 - px)},
                    static_cast<uint32_t>(2 * px + delta),
                    color);
                this->add_line_v(
                    {static_cast<uint32_t>(x0 - py), static_cast<uint32_t>(y0 - px)},
                    static_cast<uint32_t>(2 * px + delta),
                    color);
                py = y;
            }
            px = x;
        }
        this->add_line_v(
            {static_cast<uint32_t>(x0), static_cast<uint32_t>(y0 - radius)},
            static_cast<uint32_t>(2 * radius),
            color);
        /* Return the reference to the canvas object */
        return *this;
    };

    void Canvas_BW::flush()
    {
        return;
    };

    auto Canvas_BW::newline() -> Canvas_BW &
    {
        /* x is always 0 */
        this->cursor.x_pos = 0;

        /* increase y */
        this->cursor.y_pos += this->font->height_px * this->scaling;

        /* Check bounds of y */
        if (this->cursor.y_pos >= this->height)
            this->cursor.y_pos = 0;

        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::put(const char character) -> Canvas_BW &
    {
        /* get pixel size of font */
        uint8_t width = this->font->width_px;
        uint8_t height = this->font->height_px;
        uint8_t height_pages = this->font->stride;

        /* Check whether scaling is active */
        if (this->scaling < 2)
        {
            /* Write the pixels */
            for (uint8_t iY = 0; iY < height_pages; iY++)
                for (uint8_t iX = 0; iX < width; iX++)
                    this->buffer[iX + this->cursor.x_pos + (this->width * (iY + (this->cursor.y_pos / 8)))] =
                        this->font->data[(character * width * height_pages) + ((height_pages)*iX) + (height_pages - 1) - iY];
        }
        else /* Use font scaling */
        {
            /* Variable for color of the pixel */
            Color_BW color = White;

            /* loop through every pixel and set it according to the scaling */
            for (uint8_t iX = 0; iX < width; iX++)
            {
                for (uint8_t iY = 0; iY < height; iY++)
                {
                    /* Get the color of the pixel */
                    color = (this->font->data[(character * width * height_pages) + (height_pages * iX) + (height_pages - 1) - (iY / 8)] & (1 << (iY % 8))) ? Color_BW::White : Color_BW::Black;

                    /* Draw the scaled pixel                                                */
                    this->draw_pixel_with_scaling({iX, iY}, color);
                }
            }
        }
        /* append the cursor */
        auto next_x = this->cursor.x_pos + width * this->scaling;
        if (next_x > this->width)
        {
            this->cursor.x_pos = 0;
            this->cursor.y_pos += this->font->height_px * this->scaling;
        }
        else
        {
            this->cursor.x_pos = next_x;
        }
        /* Return the reference to the canvas object */
        return *this;
    };

    auto Canvas_BW::write(const char *str, const std::size_t len) -> Canvas_BW &
    {
        char *character = const_cast<char *>(str);

        for (std::size_t iByte = 0; iByte < len; iByte++)
        {
            if (*character == '\n')
                this->newline();
            else
                this->put(*character);
            character++;
        }
        /* Return the reference to the canvas object */
        return *this;
    };

    void Canvas_BW::draw_pixel_with_scaling(const Coordinate base, const Color_BW color)
    {
        /* Set the pixel in the buffer, the number of pixels is the square of the scaling */
        for (uint8_t iScaleX = 0; iScaleX < this->scaling; iScaleX++)
        {
            for (uint8_t iScaleY = 0; iScaleY < this->scaling; iScaleY++)
            {
                this->draw_pixel(this->cursor.x_pos + base.x_pos * this->scaling + iScaleX,
                                 this->cursor.y_pos + base.y_pos * this->scaling + iScaleY,
                                 color);
            }
        }
    }
}; // namespace graphics
