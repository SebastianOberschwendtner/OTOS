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
/**
 ******************************************************************************
 * @file    file.cpp
 * @author  SO
 * @version v2.8.0
 * @date    04-January-2022
 * @brief   Interface for files.
 ******************************************************************************
 */

// === Includes ===
#include "file.h"

// Provide valid template instanciations
template class FAT32::File<FAT32::Volume<SDHC::Card>>;

// === Class Methods ===

template<class Volume_t>
unsigned long FAT32::File<Volume_t>::size(void) const
{
    return this->handle.size;
};