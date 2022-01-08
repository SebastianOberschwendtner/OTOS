# Changelog for OTOS
## [v3.0.0](https://github.com/knuffel-v2/OTOS/releases/tag/v3.0.0) *(2022-xx-xx)*

>Released by `SO`

### Release Notes:
- Fixes bug in `types.h` for STM32L0 devices.
- Adds STM32 peripheral drivers for:
    - SPI *(tx only)*
    - USART *(tx only)*
    - SDIO *(STM32F4 devices only)*
    - Basic interface for System Clock and PLL settings.
- Adds chip drivers for:
    - Display Controller *ILI9341*
    - Display Controller *UC1611S*
- Adds timing and wait functions to tasks.
- Adds basic driver for *SDHC*<sup>&trade;</sup> memory cards.
- Adds driver for interfacing with *FAT32* volumes.

### Fixed Issues:

## [v2.0.0](https://github.com/knuffel-v2/OTOS/releases/tag/v2.0.0) *(2021-12-21)*

>Released by `SO`

### Release Notes:
- Adds font for fontsize 'Normal'. [&rdca; v1.2.0](https://github.com/knuffel-v2/OTOS/releases/tag/v1.2.0)
- Adds a mock interface for the CMSIS drivers of ARM<sup>&reg;</sup>.
- Adds the method `enable_interrupt` to the GPIO driver. This methods takes care of everything
  which is necessary to enable an edge interrupt on a specific IO pin. [&rdca; v1.3.0](https://github.com/knuffel-v2/OTOS/releases/tag/v1.3.0)
- Adds driver for IC which can be controlled via a bus system. The IC groups are:
    - Battery Management ICs
    - Display Controllers
- Adds priority and time based scheduling using a round-robin scheme. Still no preemption, see #1 for more information.

### Fixed Issues:

- #3: Add ms Timer for Threads


## [v1.1.0](https://github.com/knuffel-v2/OTOS/releases/tag/v1.1.0) *(2021-09-21)*

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

## [v1.0.0](https://github.com/knuffel-v2/OTOS/releases/tag/v1.0.0) *(2021-05-29)*

>Released by `SO`

### Release Notes:
- Initial release of *OTOS*.
- The kernel can switch the context of different tasks, but there is **no** preemptive scheduling.

> :warning: The scheduling relies on the tasks to **yield** their execution!

### Fixed Issues:

- n/a
