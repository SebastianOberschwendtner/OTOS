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

#ifndef DMA_STM32_H_
#define DMA_STM32_H_

// === Includes ===
#include "vendors.h"
#include "interface.h"
#include <array>

namespace DMA
{
    // === Enums ===
    enum class Flag
    {
        All = 0,
        TransferComplete,
        HalfTransfer,
        TransferError
    };

    // === Declarations ===
    struct Stream_t
    {
        unsigned char DMA{0}, Stream{0}, Channel{0};
    };

    class Stream
    {
    private:
        // *** Properties ***
        volatile DMA_Stream_TypeDef *Instance; // Pointer to the DMA stream
        volatile std::uintptr_t *Flags;        // Pointer to the DMA stream flags
        volatile std::uintptr_t *ClearFlags;   // Pointer to the DMA stream clear flags
        unsigned char flag_offset{0};          // Offset of interrupt flag in the flag register

    public:
        // *** Constructors ***
        Stream() = delete;
        Stream( Stream &stream) = default;
        Stream( Stream &&stream) = default;
        Stream& operator=( Stream &stream) = default;
        Stream& operator=( Stream &&stream) = default;
        Stream(const Stream_t &stream);

        // *** Templated Methods ***

        /**
         * @brief Assign a peripheral to the DMA stream.
         *
         * @tparam peripheral_t The type of the peripheral.
         * @param peripheral The peripheral to assign.
         * @param enable_increment Enable incrementing of the peripheral address.
         */
        template <typename peripheral_t>
        void assign_peripheral(peripheral_t &peripheral, const bool enable_increment = false)
        {
            // Set the peripheral address
            this->Instance->PAR = reinterpret_cast<std::uintptr_t>(&peripheral);

            // Clear flag and enable incrementing of the peripheral address
            this->Instance->CR &= ~DMA_SxCR_PINC;
            this->Instance->CR |= (enable_increment ? DMA_SxCR_PINC : 0);
            return;
        };

        /**
         * @brief Assign memory to the DMA stream.
         *
         * @tparam memory_t The type of the memory.
         * @param memory The memory to assign.
         * @param enable_increment Enable incrementing of the memory address.
         */
        template <typename memory_t>
        void assign_memory(memory_t &memory, const bool enable_increment = false)
        {
            // Set the peripheral address
            this->Instance->M0AR = reinterpret_cast<std::uintptr_t>(&memory);

            // Clear flag and enable incrementing of the memory address
            this->Instance->CR &= ~DMA_SxCR_MINC;
            this->Instance->CR |= (enable_increment ? DMA_SxCR_MINC : 0);

            // Get the memory size of the template parameter and assign the memory size to the DMA stream
            this->Instance->CR &= ~DMA_SxCR_MSIZE_Msk;
            if constexpr (sizeof(memory_t) == 2)
                this->Instance->CR |= DMA_SxCR_MSIZE_0;
            else if constexpr (sizeof(memory_t) == 4)
                this->Instance->CR |= DMA_SxCR_MSIZE_1;
            return;
        };

        /**
         * @brief Assign an array as the memory to the DMA stream.
         * This automatically sets the memory size to the size of the array.
         *
         * @tparam memory_t The data type of the array.
         * @tparam size The size of the array.
         * @param array The array to assign.
         * @param enable_increment Enable incrementing of the memory address.
         */
        template <typename memory_t, std::size_t size>
        void assign_memory(std::array<memory_t, size> &array, const bool enable_increment = true)
        {
            // DMA can only handle arrays with a maximum length of 65535.
            static_assert(size <= 65535, "Array size is too large for DMA.");

            // Assign the memory to the DMA stream
            this->assign_memory(array[0], enable_increment);

            // Set number of memory transfers
            this->Instance->NDTR = size;
        };

        // *** Methods ***
        void set_direction(const Direction &direction);
        void clear_interrupt_flag(const Flag &flag);
        bool enable(void);
        bool is_enabled(void) const;
        bool is_transfer_complete(void) const;
    };
}; // namespace DMA

#endif
