# Changelog for OTOS
## [v5.1.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v5.1.0) *(2024-06-30)*

>Released by `SO`

### Release Notes:
- `drivers`:
    - Timer channel can be configured to input capture mode.
    - Adds enabling and disabling of timer interrupts.
    - Adds readout and clearing of timer interrupt status flags.
    - Adds **atomic** access functions to GPIO and Timer drivers, which can be called within interrupt handlers without overhead by the driver interface abstractions.
- Adds the define `OTOS_REDUCE_MEMORY_USAGE` which sets the implementation strategy of the functions to focus and memory usage rather than number of clock cycles.

### Fixed Issues:
- n/a

## [v5.0.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v5.0.0) *(2024-05-31)*

>Released by `SO`

### Release Notes:
- Major refactoring of comments.
- 🔥 Implements a builder method for all peripheral controller classes.
- Replaces integral types with types which explicitly specify the bit size.

### Fixed Issues:
- n/a

## [v4.2.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v4.2.0) *(2024-04-29)*

>Released by `SO`

### Release Notes:
- `misc` :
    - Adds library `units` for custom unit conversion.
    - Adds *frequency* unit to `units` library.
- `battery` :
    - Refactors *MAX17205* driver to use the new units and register classes.
    - Refactors *BQ25700* driver to use register classes.
- `vendors`:
    - Adds *STM32L071xx* devices to compatible MCUs.
- `drivers`:
    - Adds first version of *timer* interface.
    - Adds function overload to set the alternate function of an GPIO pin by directly specifying the alternate function code.
- Adjusts available peripheral to pass all builds of different devices.

### Fixed Issues:
- #16: Add support for STM32L071xx devices

## [v4.1.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v4.1.0)

>Released by `SO`

### Release Notes:
- `Battery` :
    - Adds function to handle USB PD contracts to *TPS65987* driver.
    - Adds handling of system configuration registers to *TPS65987* driver.
- `Kernel` :
    - Adds convenience function to `ipc` to yield wait until data becomes available.
- `misc` :
    - Adds library `bits` for manipulating bits.

## [v4.0.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v4.0.0) *(2022-11-15)*

>Released by `SO`

### Release Notes:
- `Graphics` :
    - Canvas inherits from `OTOS::ostream` and satisfies its interface requirements.
- `Kernel` :
    - Adds *endl* manipulator for ostreams.

## [v3.4.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.4.0)

>Released by `SO`

### Release Notes:
- `Filesystem` :
    - Files can be opened using a specific mode.
    - Files can be written to.
    - Files inherit from `OTOS::iostream`.
- New Examples which demonstrate how to use the OTOS.
- Adds *iostreams* to `Kernel`.
- *iostream* has overloads for adding:
    - Null terminated strings
    - `std::string_view`
    - Integer numbers
    - `const` null terminated strings

## [v3.3.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.3.0)

>Released by `SO`

### Release Notes:
- `Drivers` :
    - SPI can create RX and TX DMA Streams.
    - Adjusts PLL defines for L0 devices.
- `Graphics` :
    - Adds << operator to display canvas for strings.

### Fixed Issues:
- #12: Update core_stm32 for STM32L0 Devices

## [v3.2.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.2.0)

>Released by `SO`

### Release Notes:
- `Drivers` :
    - Improves DMA driver for sending data.
    - Adds 16bit mode for SPI driver.

## [v3.1.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.1.0)

>Released by `SO`

### Release Notes:
- `Graphics` :
    - Moves fonts to separate header files and makes adding fonts easier.
    - Adds custom mono spaced fonts.
    - Coordinate class has overloads for basic arithmetic operators.


## [v3.0.1](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.0.1) *(2022-10-07)*

>Released by `SO`

### Release Notes:
- Fixes bug in `types.h` for STM32L0 devices.
- Adds STM32 peripheral drivers for:
    - SPI *(tx and rx)*
    - USART *(tx only)*
    - SDIO *(STM32F4 devices only)*
    - Basic interface for System Clock and PLL settings.
    - DMA
- Adds chip drivers for:
    - Display Controller *ILI9341*
    - Display Controller *UC1611S*
    - Display Controller *GC9A01A* (&rArr; Used in the round 1.28" LCD display from Waveshare)
- Adds timing and wait functions to tasks.
- Adds basic driver for *SDHC*<sup>&trade;</sup> memory cards.
- Adds driver for interfacing with *FAT32* volumes.
- Extends the *MAX17205* driver.
- Adds blocking function to tasks for quick timing inside applications.
- Improves graphics library:
    - Fixes bug when drawing vertical lines.
    - Adds font scaling.
    - Adds Bresenhams' algorithm to draw lines. :tada:
    - Adds circles. :tada:
    - **Breaking Change**: Font size is now called font type.

### Fixed Issues:
- #8: Add driver for SPI communication
- #9: Add driver for SDIO communication
- #14: SysTick and SVCall Interrupt Priority

## [v2.0.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v2.0.0) *(2021-12-21)*

>Released by `SO`

### Release Notes:
- Adds font for fontsize 'Normal'. [&rdca; v1.2.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v1.2.0)
- Adds a mock interface for the CMSIS drivers of ARM<sup>&reg;</sup>.
- Adds the method `enable_interrupt` to the GPIO driver. This methods takes care of everything
  which is necessary to enable an edge interrupt on a specific IO pin. [&rdca; v1.3.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v1.3.0)
- Adds driver for IC which can be controlled via a bus system. The IC groups are:
    - Battery Management ICs
    - Display Controllers
- Adds priority and time based scheduling using a round-robin scheme. Still no preemption, see #1 for more information.

### Fixed Issues:

- #3: Add ms Timer for Threads


## [v1.1.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v1.1.0) *(2021-09-21)*

>Released by `SO`

### Release Notes:
- Adds separate type defines for unit testing with the environment `native`.
- Adds separate processor function to call the kernel, which can be used in interrupts. This function uses the *gcc* `__attribute__((always_inline))` to force inlining, which is important for robust context switching.
- Adds peripheral drivers for:
    - GPIO
    - I2C
- Adds a graphic interface class.
- Adds an inter process communication manager for exchanging data addresses.

### Fixed Issues:

- #5: Fix Unit Test on Linux Native

## [v1.0.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v1.0.0) *(2021-05-29)*

>Released by `SO`

### Release Notes:
- Initial release of *OTOS*.
- The kernel can switch the context of different tasks, but there is **no** preemptive scheduling.

> :warning: The scheduling relies on the tasks to **yield** their execution!

### Fixed Issues:

- n/a
