# Changelog for OTOS
## [v1.1.0](https://github.com/knuffel-v2/OTOS/releases/tag/v1.1.0) *(2021-xx-xx)*

>Released by `SO`

### Release Notes:
- Adds separate type defines for unit testing with the environment `native`.
- Adds static kernel timer with *ms* resolution using the SysTick timer.
- Adds separate processor function to call the kernel, which can be used in interrupts. This function uses the *gcc* `__attribute__((always_inline))` to force inlining, which is important for robust context switching.
- Adds peripheral drivers for:
    - GPIO
    - I2C
- Adds a graphic interface class.

### Fixed Issues:

- #5: Fix Unit Test on Linux Native
- #3: Add ms Timer for Threads


## [v1.0.0](https://github.com/knuffel-v2/OTOS/releases/tag/v1.0.0) *(2021-05-29)*

>Released by `SO`

### Release Notes:
- Initial release of *OTOS*.
- The kernel can switch the context of different tasks, but there is **no** preemptive scheduling.

> :warning: The scheduling relies on the tasks to **yield** their execution!

### Fixed Issues:

- n/a
