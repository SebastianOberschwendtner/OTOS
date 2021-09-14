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
 * @version v1.0.9
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
    Graphics::Buffer<bool, 16, 9> UUT;
    TEST_ASSERT_EQUAL(16, UUT.width);
    TEST_ASSERT_EQUAL(9, UUT.height);
    TEST_ASSERT_EQUAL(16*9, UUT.pixels);

    // test writing to buffer
    UUT.data.array[10] = true;
    TEST_ASSERT_FALSE(UUT.data.array[0]);
    TEST_ASSERT_TRUE(UUT.data.array[10]);
    TEST_ASSERT_TRUE(UUT.data.coordinates[0][10]);

    UUT.data.array[16] = true;
    TEST_ASSERT_FALSE(UUT.data.array[0]);
    TEST_ASSERT_TRUE(UUT.data.array[16]);
    TEST_ASSERT_TRUE(UUT.data.coordinates[1][0]);
};

/// @brief test the constructor of a canvas object
void test_canvas_init(void)
{
    // Create buffer and object
    Graphics::Buffer<bool, 8, 8> buffer;
    Graphics::Canvas_BW UUT(&buffer.data.array[0], buffer.width, buffer.height);
};

/// @brief test writting a pixel
void test_canvas_write_pixel(void)
{
    // Create buffer and object
    Graphics::Buffer<bool, 8, 8> buffer;
    Graphics::Canvas_BW UUT(&buffer.data.array[0], buffer.width, buffer.height);

    // Write pixel
    UUT.draw_pixel(0, 0, Graphics::White);
    TEST_ASSERT_TRUE(buffer.data.coordinates[0][0]);
    UUT.draw_pixel(0, 0, Graphics::Black);
    TEST_ASSERT_FALSE(buffer.data.coordinates[0][0]);
    UUT.draw_pixel(1, 0, Graphics::White);
    TEST_ASSERT_TRUE(buffer.data.coordinates[0][1]);
    UUT.draw_pixel(1, 0, Graphics::Black);
    TEST_ASSERT_FALSE(buffer.data.coordinates[0][1]);
    UUT.draw_pixel(0, 1, Graphics::White);
    TEST_ASSERT_TRUE(buffer.data.coordinates[1][0]);
    UUT.draw_pixel(0, 1, Graphics::Black);
    TEST_ASSERT_FALSE(buffer.data.coordinates[1][0]);
    UUT.draw_pixel(1, 2, Graphics::White);
    TEST_ASSERT_TRUE(buffer.data.coordinates[2][1]);
    UUT.draw_pixel(1, 2, Graphics::Black);
    TEST_ASSERT_FALSE(buffer.data.coordinates[2][1]);

    // Test when pixel position is out of bounds
    UUT.draw_pixel(8, 0, Graphics::White);
    TEST_ASSERT_FALSE(buffer.data.array[8]);
};

/// @brief test the filling of the canvas
void test_canvas_fill(void)
{
    // Create buffer and object
    Graphics::Buffer<bool, 8, 8> buffer;
    Graphics::Canvas_BW UUT(&buffer.data.array[0], buffer.width, buffer.height);

    // Write pixel white
    UUT.fill(Graphics::White);
    for (unsigned int iPixel = 0; iPixel < buffer.pixels; iPixel++)
        TEST_ASSERT_TRUE(buffer.data.array[iPixel]);

    // Write pixel black
    UUT.fill(Graphics::Black);
    for (unsigned int iPixel = 0; iPixel < buffer.pixels; iPixel++)
        TEST_ASSERT_FALSE(buffer.data.array[iPixel]);
};

/// @brief test drawing lines on the canvas
void test_canvas_draw_line(void)
{
    // Create buffer and object
    Graphics::Buffer<bool, 8, 8> buffer;
    Graphics::Canvas_BW UUT(&buffer.data.array[0], buffer.width, buffer.height);

    // draw a horizontal line
    Graphics::Coordinate Start(2,0);
    Graphics::Coordinate End(5,0);
    UUT.draw(Start, End);
    TEST_ASSERT_FALSE(buffer.data.coordinates[0][0]);
    TEST_ASSERT_FALSE(buffer.data.coordinates[0][1]);
    TEST_ASSERT_TRUE(buffer.data.coordinates[0][2]);
    TEST_ASSERT_TRUE(buffer.data.coordinates[0][3]);
    TEST_ASSERT_TRUE(buffer.data.coordinates[0][4]);
    TEST_ASSERT_TRUE(buffer.data.coordinates[0][5]);
    TEST_ASSERT_FALSE(buffer.data.coordinates[0][6]);
    TEST_ASSERT_FALSE(buffer.data.coordinates[0][7]);

    // draw a horizontal line
    Start.set(0, 2);
    End.set(0, 5);
    UUT.draw(Start, End);
    TEST_ASSERT_FALSE(buffer.data.coordinates[0][0]);
    TEST_ASSERT_FALSE(buffer.data.coordinates[1][0]);
    TEST_ASSERT_TRUE(buffer.data.coordinates[2][0]);
    TEST_ASSERT_TRUE(buffer.data.coordinates[3][0]);
    TEST_ASSERT_TRUE(buffer.data.coordinates[4][0]);
    TEST_ASSERT_TRUE(buffer.data.coordinates[5][0]);
    TEST_ASSERT_FALSE(buffer.data.coordinates[6][0]);
    TEST_ASSERT_FALSE(buffer.data.coordinates[7][0]);
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_buffer();
    test_canvas_init();
    test_canvas_write_pixel();
    test_canvas_fill();
    test_canvas_draw_line();
    UNITY_END();
    return 0;
};