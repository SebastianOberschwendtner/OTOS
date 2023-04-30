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

#ifndef OTOS_BITS_H_
#define OTOS_BITS_H_

// === includes ===
#include <stdint.h>

namespace bits
{
    // === classes ===
    /**
     * @brief Class to define a bit field.
     */
    template <typename T>
    struct BitField
    {
        uint8_t mask{0xFF};
        uint8_t shift{0};
        T value{0};
    };

    // === functions ===
    /**
     * @brief Get the a subset of bits from an integral type.
     *
     * @tparam T The integral type to extract the bits from.
     * @param data The data to extract the bits from.
     * @param field The bit field to extract.
     * @return constexpr T The extracted bits.
     */
    template <typename T>
    [[nodiscard]] constexpr inline T get(const T data, const BitField<T> &field)
    {
        return (data >> field.shift) & field.mask;
    }

    /**
     * @brief Set a subset of bits in an integral type.
     * 
     * @tparam T The integral type to set the bits in.
     * @param data The data to set the bits in.
     * @param field The bit field to extract.
     * @return constexpr T The data with the updated bits.
     */
    template <typename T>
    [[nodiscard]] constexpr inline T set(const T data, const BitField<T> &field)
    {
        return (data & ~(field.mask << field.shift)) | ((field.value & field.mask) << field.shift);
    }

}; // namespace bits

#endif /* OTOS_BITS_H_ */