# Changelog for OTOS
## [v5.0.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v5.0.0) *(2022-xx-xx)*

>Released by `SO`

### Release Notes:
#### [&rarrb; v4.1.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v4.1.0)
- `Battery` :
    - Adds function to handle USB PD contracts to *TPS65987* driver.
    - Adds handling of system configuration registers to *TPS65987* driver.
- `Kernel` :
    - Adds convenience function to `ipc` to yield wait until data becomes available.
- `misc` :
    - Adds library `bits` for manipulating bits.
#### [&rarrb; v4.2.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v4.2.0)

### Fixed Issues:
- n/a

## [v4.0.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v4.0.0) *(2022-11-15)*

>Released by `SO`

### Release Notes:
#### [&rarrb; v3.1.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.1.0)
- `Graphics` :
    - Moves fonts to separate header files and makes adding fonts easier.
    - Adds custom mono spaced fonts.
    - Coordinate class has overloads for basic arithmetic operators.
#### [&rarrb; v3.2.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.2.0)
- `Drivers` :
    - Improves DMA driver for sending data.
    - Adds 16bit mode for SPI driver.
#### [&rarrb; v3.3.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.3.0)
- `Drivers` :
    - SPI can create RX and TX DMA Streams.
    - Adjusts PLL defines for L0 devices.
- `Graphics` :
    - Adds << operator to display canvas for strings.

#### [&rarrb; v3.4.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v3.4.0)
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
#### [&rarrb; v4.0.0](https://github.com/SebastianOberschwendtner/OTOS/releases/tag/v4.0.0)
- `Graphics` :
    - Canvas inherits from `OTOS::ostream` and satisfies its interface requirements.
- `Kernel` :
    - Adds *endl* manipulator for ostreams.

### Fixed Issues:
- #12: Update core_stm32 for STM32L0 Devices

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
