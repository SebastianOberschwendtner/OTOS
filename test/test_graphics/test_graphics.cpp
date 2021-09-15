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
 ******************************************************************************
 * @file    test_graphics.c
 * @author  SO
 * @version v1.0.10
 * @date    16-March-2021
 * @brief   Unit tests to test the graphics driver.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include "graphics.h"

/** === Test List ===
 * ✓ Static buffer with variable size can be created
 * ✓ Graphics canvas can be created and the buffer assigned to it.
 * ✓ Canvas can write single pixel
 * ✓ Canvas can fill complete buffer with black and white
 * ▢ Canvas can write line to buffer:
 *      ▢ vertical line
 *      ▢ horizontal line
 *      ▢ sloped line
 * ▢ Canvas can write rectangle to buffer
 * ▢ Canvas can write character to buffer
 * ▢ Canvas can write string to buffer
 */

// === Fixtures ===
void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// === Define Tests ===

/// @brief test the buffer template
void test_buffer(void)
{
    // Test initialization of buffer
    Graphics::Buffer_BW<16, 8> UUT;
    TEST_ASSERT_EQUAL(16, UUT.width_px);
    TEST_ASSERT_EQUAL(8, UUT.height_px);
    TEST_ASSERT_EQUAL(16*8, UUT.pixels);

    // test writing to buffer
    UUT.data[10] = 0xAA;
    TEST_ASSERT_EQUAL(0, UUT.data[0]);
    TEST_ASSERT_EQUAL(0xAA, UUT.data[10]);

    UUT.data[12] = 0xBB;
    TEST_ASSERT_EQUAL(0, UUT.data[0]);
    TEST_ASSERT_EQUAL(0xBB, UUT.data[12]);
};

/// @brief test the constructor of a canvas object
void test_canvas_init(void)
{
    // Create buffer and object
    Graphics::Buffer_BW<8, 8> buffer;
    Graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);
};

/// @brief test writting a pixel
void test_canvas_write_pixel(void)
{
    // Create buffer and object
    Graphics::Buffer_BW<8, 16> buffer;
    Graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    // Write pixel
    UUT.draw_pixel(0, 0, Graphics::White);
    TEST_ASSERT_EQUAL(0x01, buffer.data[0]);
    UUT.draw_pixel(0, 0, Graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[0]);
    UUT.draw_pixel(1, 0, Graphics::White);
    TEST_ASSERT_EQUAL(0x01, buffer.data[1]);
    UUT.draw_pixel(1, 0, Graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[1]);
    UUT.draw_pixel(0, 1, Graphics::White);
    TEST_ASSERT_EQUAL(0x02, buffer.data[0]);
    UUT.draw_pixel(0, 1, Graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[0]);
    UUT.draw_pixel(1, 2, Graphics::White);
    TEST_ASSERT_EQUAL(0x04, buffer.data[1]);
    UUT.draw_pixel(1, 2, Graphics::Black);
    TEST_ASSERT_EQUAL(0x00, buffer.data[1]);

    // Test when pixel position is out of bounds
    UUT.draw_pixel(8, 0, Graphics::White);
    TEST_ASSERT_EQUAL(0, buffer.data[8]);
};

/// @brief test the filling of the canvas
void test_canvas_fill(void)
{
    // Create buffer and object
    Graphics::Buffer_BW<8, 8> buffer;
    Graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    // Write pixel white
    UUT.fill(Graphics::White);
    for (unsigned int iPixel = 0; iPixel < buffer.pixels/8; iPixel++)
        TEST_ASSERT_EQUAL(0xFF, buffer.data[iPixel]);

    // Write pixel black
    UUT.fill(Graphics::Black);
    for (unsigned int iPixel = 0; iPixel < buffer.pixels/8; iPixel++)
        TEST_ASSERT_EQUAL(0x00, buffer.data[iPixel]);
};

/// @brief test adding horizontal lines on the canvas
void test_canvas_add_horizontal_line(void)
{
    // Create buffer and object
    Graphics::Buffer_BW<8, 16> buffer;
    Graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    // draw a horizontal line
    Graphics::Coordinate Start(2,0);
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
};

/// @brief test adding vertical lines on the canvas
void test_canvas_add_vertical_line(void)
{
    // Create buffer and object
    Graphics::Buffer_BW<8, 16> buffer;
    Graphics::Canvas_BW UUT(buffer.data.data(), buffer.width_px, buffer.height_px);

    // draw a vertical line
    Graphics::Coordinate Start(0,0);
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
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_buffer();
    test_canvas_init();
    test_canvas_write_pixel();
    test_canvas_fill();
    test_canvas_add_horizontal_line();
    test_canvas_add_vertical_line();
    UNITY_END();
    return 0;
};