/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2023 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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

#ifndef UNITS_H_
#define UNITS_H_

// === Includes ===
#include <cstdint>
#include <ratio>
namespace OTOS
{
    // === Classes ===
    /**
     * @brief A class template to define a unit.
     * You can use the instantiated objects like values of the target unit.
     * When assigning a integral raw value the value is converted to the target unit.
     * 
     * @tparam ratio The ratio between the target unit and the raw value.
     */
    template< typename ratio, typename base = uint32_t>
    class Unit
    {
	static_assert(ratio::num > 0, "Unit Ratio must be positive!");
    static_assert(ratio::den > 0, "Unit Denominator must be > 0!");
    private:
        base value{0}; // The value in the target unit

    public:
        /**
         * @brief Construct a new Unit object.
         * 
         * @tparam T The type of the raw value. Should fit into uint16_t!
         * @param rhs The raw value to assign. The value is not converted!
         */
        template<typename T>
        Unit(const T& rhs): value(static_cast<base>(rhs)) {}; // NOLINT(google*)

        // Default constructor, assignment and destructor
        Unit() = default;
        Unit(const Unit&) = default;
        Unit(Unit&&) noexcept = default;
        Unit& operator=(const Unit&) = default;
        Unit& operator=(Unit&&) noexcept = default;
        ~Unit() = default;

        /**
         * @brief Cast operator to use the object like a value of the target unit.
         * 
         * @tparam T The type to cast to.
         * @return T The value in the target unit.
         */
        template<typename T>
        explicit operator T() const { return static_cast<T>(value); }

        /**
         * @brief Assignment operator to assign a raw value.
         * This value is converted to the target unit.
         * 
         * @tparam T The type of the raw value. Should fit into 16bits!
         * @param rhs The raw value to assign.
         * @return Unit& Reference to the object.
         * @todo Select the calculation type based on the size of the raw value
         */
        template<typename T>
        Unit& operator=(const T& rhs)
        {
            value = static_cast<base>(rhs); // Convert to base type
            // Use larger temporary variable to prevent overflow
            int64_t temp = value * ratio::num;
            temp /= ratio::den; // Convert to target unit
            value = static_cast<base>(temp); // Convert to base type
            return *this;
        }

        /**
         * @brief Set the value of the object without conversion.
         * 
         * @tparam T The type of the raw value.
         * @param new_val The raw value to assign.
         * @return Unit& Reference to the object.
         */
        template<typename T>
        Unit& set_value(const T& new_val)
        {
            value = static_cast<base>(new_val); // Convert to to base type
            return *this;
        }

        /**
         * @brief Access the underlying base value.
         */
        [[nodiscard]] auto get() const -> base { return value; }
    };
}; // namespace OTOS
#endif // UNITS_H_
