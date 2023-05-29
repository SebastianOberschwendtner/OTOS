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
     * @warning
     * This class is mainly intended for sensors which uses custom units and treat those
     * values as SI units.
     *
     * @tparam ratio The ratio between the target unit and the raw value.
     */
    template <typename ratio, typename base = uint32_t>
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
        template <typename T>
        Unit(const T &rhs) : value(static_cast<base>(rhs)){}; // NOLINT(google*)

        // Default constructor, assignment and destructor
        Unit() = default;
        Unit(const Unit &) = default;
        Unit(Unit &&) noexcept = default;
        Unit &operator=(const Unit &) = default;
        Unit &operator=(Unit &&) noexcept = default;
        ~Unit() = default;

        /**
         * @brief Cast operator to use the object like a value of the target unit.
         *
         * @tparam T The type to cast to.
         * @return T The value in the target unit.
         */
        template <typename T>
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
        template <typename T>
        Unit &operator=(const T &rhs)
        {
            value = static_cast<base>(rhs); // Convert to base type
            // Use larger temporary variable to prevent overflow
            int64_t temp = value * ratio::num;
            temp /= ratio::den;              // Convert to target unit
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
        template <typename T>
        Unit &set_value(const T &new_val)
        {
            value = static_cast<base>(new_val); // Convert to to base type
            return *this;
        }

        /**
         * @brief Access the underlying base value.
         */
        [[nodiscard]] auto get() const -> base { return value; }
    };

    /**
     * @brief A class template to define a frequency unit.
     *
     * @tparam _Repr The type of the raw value.
     * @tparam _Ratio The ratio multiplier of the target unit.
     */
    template <typename _Repr, typename _Ratio>
    class frequency
    {
      public:
        static_assert(_Ratio::num > 0, "Frequency Ratio must be > 0!");
        static_assert(_Ratio::den > 0, "Frequency Denominator must be > 0!");
        using rep = _Repr;
        using ratio = typename _Ratio::type;

        /* Constructors */
        constexpr frequency() = default;
        constexpr explicit frequency(const rep &rhs) : value(rhs) {}
        frequency(const frequency &) = default;
        frequency(frequency &&) noexcept = default;
        frequency &operator=(const frequency &) = default;
        frequency &operator=(frequency &&) noexcept = default;
        ~frequency() = default;

        /**
         * @brief Construct a new frequency object from a frequency object with a different ratio.
         * 
         * @tparam _Ratio2 The ratio of the source frequency object.
         * @param rhs The source frequency object.
         */
        template <typename _Ratio2>
        frequency(const frequency<rep, _Ratio2> &rhs)
        {
            // Calculate the value in the target unit
            value = rhs.count() * _Ratio2::num / _Ratio2::den;

            // Calculate the value in the base unit
            value *= ratio::den;
            value /= ratio::num;
        }

        /**
         * @brief Cast operator to use the object like a value of the target unit.
         * 
         * @tparam T The type to cast to.
         * @return T The value in the target unit.
         */
        template <typename T>
        explicit operator T() const { return static_cast<T>(value); }

        /**
         * @brief Assignment operator to assign a raw value.
         * 
         * @param rhs The raw value to assign.
         * @return frequency& Reference to the object.
         */
        constexpr frequency &operator=(const rep &rhs)
        {
            value = rhs;
            return *this;
        }

        /**
         * @brief Access the underlying base value.
         */
        [[nodiscard]] auto count() const -> rep { return value; }

      private:
        rep value{0}; // The value in the target unit
    };

    /* Frequency types */
    typedef frequency<uint32_t, std::ratio<1, 1>> hertz;
    typedef frequency<uint32_t, std::kilo> kilohertz;
    typedef frequency<uint32_t, std::mega> megahertz;

    namespace literals
    {
        // Frequency literals
        constexpr hertz operator"" _Hz(unsigned long long val) { return hertz(val); }
        constexpr kilohertz operator"" _kHz(unsigned long long val) { return kilohertz(val); }
        constexpr megahertz operator"" _MHz(unsigned long long val) { return megahertz(val); }

    }; // namespace literals
};     // namespace OTOS
#endif // UNITS_H_
