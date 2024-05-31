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
 * @file    test_graphics.c
 * @author  SO
 * @version v5.0.0
 * @date    16-March-2021
 * @brief   Unit tests to test the graphics driver.
 ==============================================================================
 */

/* === Includes === */
#include <unity.h>
#include "graphics.h"

/** === Test List ===
 * ✓ Static buffer with variable size can be created
 * ✓ Graphics canvas can be created and the buffer assigned to it.
 * ✓ Canvas can write single pixel
 * ✓ Canvas can fill complete buffer with black and white
 * ▢ Canvas can write line to buffer:
 *      ✓ vertical line
 *      ✓ horizontal line
 *      ▢ sloped line
 * ▢ Canvas can write rectangle to buffer
 * ▢ Canvas can set cursor:
 *      ▢ set its coordinates
 *      ▢ get its current coordinates
 *      ▢ increment by font size
 *      ▢ does roll over at limits
 * ▢ Canvas can write character to buffer
 * ▢ Canvas can write string to buffer
 * ▢ Canvas can write number to buffer
 */

/* === Fixtures === */
void setUp() {
/* set stuff up here */
};

void tearDown() {
/* clean stuff up here */
};

/* === Define Tests === */

/** 
 * @brief test the buffer template
 */
void test_buffer()
{
    /* Test initialization of buffer */
    graphics::Buffer_BW<16, 8> UUT;
    TEST_ASSERT_EQUAL(16, UUT.width_px);
    TEST_ASSERT_EQUAL(8, UUT.height_px);
    TEST_ASSERT_EQUAL(16*8, UUT.pixels);

    /* test writing to buffer */
    UUT.data[10] = 0xAA;
    TEST_ASSERT_EQUAL(0, UUT.data[0]);
    TEST_ASSERT_EQUAL(0xAA, UUT.data[10]);

    UUT.data[12] = 0xBB;
    TEST_ASSERT_EQUAL(0, UUT.data[0]);
    TEST_ASSERT_EQUAL(0xBB, UUT.data[12]);
};

/** 
 * @brief test the constructor of a canvas object
 */
void test_canvas_init()
{
    /* Create buffer and object */
    graphics::Buffer_BW<8, 8> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);
};

/** 
 * @brief test writing a pixel
 */
void test_canvas_write_pixel()
{
    /* Create buffer and object */
    graphics::Buffer_BW<8, 16> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* Write pixel */
    UUT.draw_pixel(0, 0, graphics::White);
    TEST_ASSERT_EQUAL(0x01, buffer.data[0]);
    UUT.draw_pixel(0, 0, graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[0]);
    UUT.draw_pixel(1, 0, graphics::White);
    TEST_ASSERT_EQUAL(0x01, buffer.data[1]);
    UUT.draw_pixel(1, 0, graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[1]);
    UUT.draw_pixel(0, 1, graphics::White);
    TEST_ASSERT_EQUAL(0x02, buffer.data[0]);
    UUT.draw_pixel(0, 1, graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[0]);
    UUT.draw_pixel(1, 2, graphics::White);
    TEST_ASSERT_EQUAL(0x04, buffer.data[1]);
    UUT.draw_pixel(1, 2, graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[1]);

    /* Test when pixel position is out of bounds */
    UUT.draw_pixel(8, 0, graphics::White);
    TEST_ASSERT_EQUAL(0, buffer.data[8]);

    /* Test when width is greater than 8, fixes a bug in write_pixel */
    graphics::Buffer_BW<16, 16> buffer2;
    graphics::Canvas_BW UUT2(buffer2.data.data(), buffer2.width_px, buffer2.height_px);

    /* Write pixel */
    UUT2.draw_pixel(0, 8, graphics::White);
    TEST_ASSERT_EQUAL(0x01, buffer2.data[16]);
    UUT2.draw_pixel(0, 8, graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer2.data[16]);
    UUT2.draw_pixel(1, 8, graphics::White);
    TEST_ASSERT_EQUAL(0x01, buffer2.data[17]);
    UUT2.draw_pixel(1, 8, graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer2.data[17]);
    UUT2.draw_pixel(0, 9, graphics::White);
    TEST_ASSERT_EQUAL(0x02, buffer2.data[16]);
    UUT2.draw_pixel(0, 9, graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer2.data[16]);
    UUT2.draw_pixel(1, 10, graphics::White);
    TEST_ASSERT_EQUAL(0x04, buffer2.data[17]);
    UUT2.draw_pixel(1, 10, graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer2.data[17]);
};

/** 
 * @brief test the filling of the canvas
 */
void test_canvas_fill()
{
    /* Create buffer and object */
    graphics::Buffer_BW<8, 8> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* Write pixel white */
    UUT.fill(graphics::White);
    for (uint32_t iPixel = 0; iPixel < buffer.pixels/8; iPixel++)
        TEST_ASSERT_EQUAL(0xFF, buffer.data[iPixel]);

    /* Write pixel black */
    UUT.fill(graphics::Black);
    for (uint32_t iPixel = 0; iPixel < buffer.pixels/8; iPixel++)
        TEST_ASSERT_EQUAL(0x00, buffer.data[iPixel]);
};

/** 
 * @brief test adding horizontal lines on the canvas
 */
void test_canvas_add_horizontal_line()
{
    /* Create buffer and object */
    graphics::Buffer_BW<8, 32> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* draw a horizontal line */
    graphics::Coordinate Start(2,0);
    UUT.add_line_h(Start, 3);
    TEST_ASSERT_EQUAL(0x00, buffer.data[0]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[1]);
    TEST_ASSERT_EQUAL(0x01, buffer.data[2]);
    TEST_ASSERT_EQUAL(0x01, buffer.data[3]);
    TEST_ASSERT_EQUAL(0x01, buffer.data[4]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[5]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[6]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[7]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[8]);

    /* draw a horizontal line */
    Start.set(2,25);
    UUT.add_line_h(Start, 3);
    TEST_ASSERT_EQUAL(0x00, buffer.data[23]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[24]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[25]);
    TEST_ASSERT_EQUAL(0x02, buffer.data[26]);
    TEST_ASSERT_EQUAL(0x02, buffer.data[27]);
    TEST_ASSERT_EQUAL(0x02, buffer.data[28]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[29]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[30]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[31]);

    /* draw a horizontal line */
    buffer.data.fill(0);
    Start.set(2,0);
    UUT.add_line_h(Start, 5, 1);
    TEST_ASSERT_EQUAL(0x00, buffer.data[0]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[1]);
    TEST_ASSERT_EQUAL(0x01, buffer.data[2]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[3]);
    TEST_ASSERT_EQUAL(0x01, buffer.data[4]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[5]);
    TEST_ASSERT_EQUAL(0x01, buffer.data[6]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[7]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[8]);

    /* draw a horizontal line in Black */
    buffer.data.fill(0);
    Start.set(0,0);
    UUT.add_line_h(Start, 5);
    UUT.add_line_h(Start, 3, 0, graphics::Color_BW::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[0]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[1]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[2]);
    TEST_ASSERT_EQUAL(0x01, buffer.data[3]);
    TEST_ASSERT_EQUAL(0x01, buffer.data[4]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[5]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[6]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[7]);
    TEST_ASSERT_EQUAL(0x00, buffer.data[8]);
};

/** 
 * @brief test adding vertical lines on the canvas
 */
void test_canvas_add_vertical_line()
{
    /* Create buffer and object */
    graphics::Buffer_BW<8, 16> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* draw a vertical line */
    graphics::Coordinate Start(0,0);
    UUT.add_line_v(Start, 5);
    TEST_ASSERT_EQUAL(0b00011111, buffer.data[0]);

    Start.set(1,2);
    UUT.add_line_v(Start, 5);
    TEST_ASSERT_EQUAL(0b01111100, buffer.data[1]);

    Start.set(2,2);
    UUT.add_line_v(Start, 9);
    TEST_ASSERT_EQUAL(0b11111100, buffer.data[2]);
    TEST_ASSERT_EQUAL(0b00000111, buffer.data[10]);

    Start.set(3,0);
    UUT.add_line_v(Start, 16);
    TEST_ASSERT_EQUAL(0b11111111, buffer.data[3]);
    TEST_ASSERT_EQUAL(0b11111111, buffer.data[11]);

    Start.set(4,3);
    UUT.add_line_v(Start, 13);
    TEST_ASSERT_EQUAL(0b11111000, buffer.data[4]);
    TEST_ASSERT_EQUAL(0b11111111, buffer.data[12]);

    Start.set(0,0);
    UUT.add_line_v(Start, 3, graphics::Color_BW::Black);
    TEST_ASSERT_EQUAL(0b00011000, buffer.data[0]);
};

/** 
 * @brief Test adding a line using Bresenham's algorithm
 */
void test_add_line()
{
    /* Create buffer and object */
    graphics::Buffer_BW<8, 8> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* draw a vertical line */
    UUT.add_line({0,0}, {0,5});
    TEST_ASSERT_EQUAL(0b00111111, buffer.data[0]);

    /* draw a vertical line in black */
    UUT.add_line({0,0}, {0,3}, graphics::Color_BW::Black);
    TEST_ASSERT_EQUAL(0b00110000, buffer.data[0]);
};

/** 
 * @brief Test adding circles to the canvas
 */
void test_add_circle()
{
    /* Create buffer and object */
    graphics::Buffer_BW<8, 8> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* draw a circle */
    UUT.add_circle({4,4}, 4);
    TEST_ASSERT_EQUAL(0b00111000, buffer.data[0]);
    TEST_ASSERT_EQUAL(0b11000110, buffer.data[1]);
    TEST_ASSERT_EQUAL(0b10000010, buffer.data[2]);
    TEST_ASSERT_EQUAL(0b00000001, buffer.data[3]);
    TEST_ASSERT_EQUAL(0b00000001, buffer.data[4]);
    TEST_ASSERT_EQUAL(0b00000001, buffer.data[5]);
    TEST_ASSERT_EQUAL(0b10000010, buffer.data[6]);
    TEST_ASSERT_EQUAL(0b11000110, buffer.data[7]);

    /* draw a black circle */
    buffer.data.fill(0xFF);
    UUT.add_circle({4,4}, 4, graphics::Color_BW::Black);
    TEST_ASSERT_EQUAL(0b11000111, buffer.data[0]);
    TEST_ASSERT_EQUAL(0b00111001, buffer.data[1]);
    TEST_ASSERT_EQUAL(0b01111101, buffer.data[2]);
    TEST_ASSERT_EQUAL(0b11111110, buffer.data[3]);
    TEST_ASSERT_EQUAL(0b11111110, buffer.data[4]);
    TEST_ASSERT_EQUAL(0b11111110, buffer.data[5]);
    TEST_ASSERT_EQUAL(0b01111101, buffer.data[6]);
    TEST_ASSERT_EQUAL(0b00111001, buffer.data[7]);
};

/** 
 * @brief Test adding filled circles to the canvas
 */
void test_fill_circle()
{
    /* Create buffer and object */
    graphics::Buffer_BW<8, 16> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* draw a full circle */
    UUT.fill_circle({4,4}, 4);
    TEST_ASSERT_EQUAL(0b00111000, buffer.data[0]);
    TEST_ASSERT_EQUAL(0b11111110, buffer.data[1]);
    TEST_ASSERT_EQUAL(0b11111110, buffer.data[2]);
    TEST_ASSERT_EQUAL(0b11111111, buffer.data[3]);
    TEST_ASSERT_EQUAL(0b11111111, buffer.data[4]);
    TEST_ASSERT_EQUAL(0b11111111, buffer.data[5]);
    TEST_ASSERT_EQUAL(0b11111110, buffer.data[6]);
    TEST_ASSERT_EQUAL(0b11111110, buffer.data[7]);

    /* draw a full circle in black */
    buffer.data.fill(0xFF);
    UUT.fill_circle({4,4}, 4, graphics::Color_BW::Black);
    TEST_ASSERT_EQUAL(0b11000111, buffer.data[0]);
    TEST_ASSERT_EQUAL(0b00000001, buffer.data[1]);
    TEST_ASSERT_EQUAL(0b00000001, buffer.data[2]);
    TEST_ASSERT_EQUAL(0b00000000, buffer.data[3]);
    TEST_ASSERT_EQUAL(0b00000000, buffer.data[4]);
    TEST_ASSERT_EQUAL(0b00000000, buffer.data[5]);
    TEST_ASSERT_EQUAL(0b00000001, buffer.data[6]);
    TEST_ASSERT_EQUAL(0b00000001, buffer.data[7]);
};

/** 
 * @brief test the cursor functionality of the canvas
 */
void test_cursor()
{
    /* Create buffer and object */
    graphics::Buffer_BW<32, 32> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* Test the initial cursor position */
    TEST_ASSERT_EQUAL(0, UUT.cursor.x_pos);
    TEST_ASSERT_EQUAL(0, UUT.cursor.y_pos);

    /* Test setting the cursor position */
    UUT.set_cursor(2,3);
    TEST_ASSERT_EQUAL(12, UUT.cursor.x_pos);
    TEST_ASSERT_EQUAL(24, UUT.cursor.y_pos);

    /* Test the limit of the cursor position */
    UUT.set_cursor(6,5);
    TEST_ASSERT_EQUAL(0, UUT.cursor.x_pos);
    TEST_ASSERT_EQUAL(0, UUT.cursor.y_pos);

    /* Test the newline */
    UUT.newline();
    TEST_ASSERT_EQUAL(0, UUT.cursor.x_pos);
    TEST_ASSERT_EQUAL(8, UUT.cursor.y_pos);

    /* Test the newline when at the end of the buffer */
    UUT.set_cursor(5,3);
    UUT.newline();
    TEST_ASSERT_EQUAL(0, UUT.cursor.x_pos);
    TEST_ASSERT_EQUAL(0, UUT.cursor.y_pos);
};

/** 
 * @brief test writing a character
 */
void test_add_character()
{
    /* Create buffer and object */
    graphics::Buffer_BW<16, 16> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* add the character */
    UUT.put('B');
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['B'*6 + 0], buffer.data[0]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['B'*6 + 1], buffer.data[1]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['B'*6 + 2], buffer.data[2]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['B'*6 + 3], buffer.data[3]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['B'*6 + 4], buffer.data[4]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['B'*6 + 5], buffer.data[5]);

    /* add another character */
    UUT.put('D');
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 0], buffer.data[6]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 1], buffer.data[7]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 2], buffer.data[8]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 3], buffer.data[9]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 4], buffer.data[10]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 5], buffer.data[11]);

    /* add another character */
    UUT.set_cursor(0,1);
    UUT.put('F');
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['F'*6 + 0], buffer.data[16]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['F'*6 + 1], buffer.data[17]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['F'*6 + 2], buffer.data[18]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['F'*6 + 3], buffer.data[19]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['F'*6 + 4], buffer.data[20]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['F'*6 + 5], buffer.data[21]);
};

/** 
 * @brief test adding a string
 */
void test_add_string()
{
    /* Create buffer and object */
    graphics::Buffer_BW<16, 16> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* add the character */
    UUT.write("GD", 2);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 0], buffer.data[0]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 1], buffer.data[1]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 2], buffer.data[2]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 3], buffer.data[3]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 4], buffer.data[4]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 5], buffer.data[5]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 0], buffer.data[6]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 1], buffer.data[7]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 2], buffer.data[8]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 3], buffer.data[9]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 4], buffer.data[10]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 5], buffer.data[11]);

    /* add character with line break */
    buffer.data.fill(0);
    UUT.set_cursor(0,0);
    UUT.write("G\nD", 3);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 0], buffer.data[0]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 1], buffer.data[1]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 2], buffer.data[2]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 3], buffer.data[3]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 4], buffer.data[4]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['G'*6 + 5], buffer.data[5]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 0], buffer.data[16]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 1], buffer.data[17]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 2], buffer.data[18]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 3], buffer.data[19]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 4], buffer.data[20]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_8px['D'*6 + 5], buffer.data[21]);
};

/** 
 * @brief Test the normal font size
 */
void test_font_normal()
{
    /* Create buffer and object */
    graphics::Buffer_BW<16, 16> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* Change the fontsize */
    UUT.set_font(font::_16px::Default);

    /* Test writing new characters */
    UUT.put('A');
    
    /* Perform testing */
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  1], buffer.data[ 0]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  3], buffer.data[ 1]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  5], buffer.data[ 2]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  7], buffer.data[ 3]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  9], buffer.data[ 4]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 11], buffer.data[ 5]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 13], buffer.data[ 6]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 15], buffer.data[ 7]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 17], buffer.data[ 8]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 19], buffer.data[ 9]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 21], buffer.data[10]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 23], buffer.data[11]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  0], buffer.data[16]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  2], buffer.data[17]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  4], buffer.data[18]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  6], buffer.data[19]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 +  8], buffer.data[20]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 10], buffer.data[21]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 12], buffer.data[22]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 14], buffer.data[23]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 16], buffer.data[24]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 18], buffer.data[25]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 20], buffer.data[26]);
    TEST_ASSERT_EQUAL(font::Lookup_Default_16px['A'*12*2 + 22], buffer.data[27]);
};

/** 
 * @brief Test the number font
 */
void test_font_number()
{
    /* Create buffer and object */
    graphics::Buffer_BW<16, 24> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* Test writing new characters */
    UUT.add_number(2);
    
    /* Perform testing */
    TEST_ASSERT_EQUAL(0, buffer.data[ 0]);
    TEST_ASSERT_EQUAL(0, buffer.data[ 1]);
    TEST_ASSERT_EQUAL(62, buffer.data[ 2]);
    TEST_ASSERT_EQUAL(62, buffer.data[ 3]);
    TEST_ASSERT_EQUAL(62, buffer.data[ 4]);
    TEST_ASSERT_EQUAL(14, buffer.data[ 5]);
    TEST_ASSERT_EQUAL(14, buffer.data[ 6]);
    TEST_ASSERT_EQUAL(14, buffer.data[ 7]);
    TEST_ASSERT_EQUAL(14, buffer.data[ 8]);
    TEST_ASSERT_EQUAL(14, buffer.data[ 9]);
    TEST_ASSERT_EQUAL(14, buffer.data[10]);
    TEST_ASSERT_EQUAL(254, buffer.data[11]);
    TEST_ASSERT_EQUAL(0, buffer.data[16]);
    TEST_ASSERT_EQUAL(0, buffer.data[17]);
    TEST_ASSERT_EQUAL(252, buffer.data[18]);
    TEST_ASSERT_EQUAL(252, buffer.data[19]);
    TEST_ASSERT_EQUAL(252, buffer.data[20]);
    TEST_ASSERT_EQUAL(28, buffer.data[21]);
    TEST_ASSERT_EQUAL(28, buffer.data[22]);
    TEST_ASSERT_EQUAL(28, buffer.data[23]);
    TEST_ASSERT_EQUAL(28, buffer.data[24]);
    TEST_ASSERT_EQUAL(28, buffer.data[25]);
};

/** 
 * @brief Test scaling the fonts
 */
void test_font_scaling()
{
    /* Create buffer and object */
    graphics::Buffer_BW<16, 24> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* Change the font size and scale */
    UUT.set_font(font::_8px::Default, 2);

    /* Test setting the cursor position */
    UUT.set_cursor(1,1);
    TEST_ASSERT_EQUAL(font::_8px::Default.width_px * 2, UUT.cursor.x_pos);
    TEST_ASSERT_EQUAL(font::_8px::Default.height_px * 2, UUT.cursor.y_pos);


    /* Test the newline when the font is scaled */
    UUT.set_cursor(0,0);
    UUT.newline();
    TEST_ASSERT_EQUAL(0, UUT.cursor.x_pos);
    TEST_ASSERT_EQUAL(font::_8px::Default.height_px * 2, UUT.cursor.y_pos);

    /* Test writing new characters */
    UUT.set_cursor(0,0);
    UUT.put('A');
    
    /* Perform testing */
    TEST_ASSERT_EQUAL(0, buffer.data[ 0]);
    TEST_ASSERT_EQUAL(0, buffer.data[ 1]);
    TEST_ASSERT_EQUAL(252, buffer.data[ 2]);
    TEST_ASSERT_EQUAL(252, buffer.data[ 3]);
    TEST_ASSERT_EQUAL(3, buffer.data[ 4]);
    TEST_ASSERT_EQUAL(3, buffer.data[ 5]);
    TEST_ASSERT_EQUAL(3, buffer.data[20]);
    TEST_ASSERT_EQUAL(3, buffer.data[21]);
    TEST_ASSERT_EQUAL(3, buffer.data[22]);
};

/** 
 * @brief Test the coordinate overloading
 */
void test_coordinate_operators()
{
    /* Arrange */
    graphics::Coordinate coord1(1,2);
    graphics::Coordinate coord2(3,4);

    /* Test Adding two coordinates */
    graphics::Coordinate coord3 = coord1 + coord2;
    TEST_ASSERT_EQUAL(4, coord3.x_pos);
    TEST_ASSERT_EQUAL(6, coord3.y_pos);

    /* Test Subtracting two coordinates */
    graphics::Coordinate coord4 = coord2 - coord1;
    TEST_ASSERT_EQUAL(2, coord4.x_pos);
    TEST_ASSERT_EQUAL(2, coord4.y_pos);

    /* Test the += operator */
    coord1 += coord2;
    TEST_ASSERT_EQUAL(4, coord1.x_pos);
    TEST_ASSERT_EQUAL(6, coord1.y_pos);

    /* Test the -= operator */
    coord1 -= coord2;
    TEST_ASSERT_EQUAL(1, coord1.x_pos);
    TEST_ASSERT_EQUAL(2, coord1.y_pos);
};

/** 
 * @brief Test the flush method as required by OTOS::ostream
 */
void test_flush()
{
    /* Create buffer and object */
    graphics::Buffer_BW<16, 24> buffer;
    graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    /* Test the flush method */
    UUT.flush();
};

/** 
 * === Run Tests ===
 */
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_buffer);
    RUN_TEST(test_canvas_init);
    RUN_TEST(test_canvas_write_pixel);
    RUN_TEST(test_canvas_fill);
    RUN_TEST(test_canvas_add_horizontal_line);
    RUN_TEST(test_canvas_add_vertical_line);
    RUN_TEST(test_add_line);
    RUN_TEST(test_add_circle);
    RUN_TEST(test_fill_circle);
    RUN_TEST(test_cursor);
    RUN_TEST(test_add_character);
    RUN_TEST(test_add_string);
    RUN_TEST(test_font_normal);
    RUN_TEST(test_font_number);
    RUN_TEST(test_font_scaling);
    RUN_TEST(test_coordinate_operators);
    RUN_TEST(test_flush);
    return UNITY_END();
};
