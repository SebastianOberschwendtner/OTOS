![OTOS Logo Header](doc/img/OTOS_Header.png)

# OTOS
![tag](https://img.shields.io/github/v/tag/SebastianOberschwendtner/otos?color=green)
![release](https://img.shields.io/github/v/release/SebastianOberschwendtner/otos?color=green)
![issues](https://img.shields.io/github/issues-raw/SebastianOberschwendtner/otos)
![bugs](https://img.shields.io/github/issues/SebastianOberschwendtner/otos/bug?color=red)
![Status](https://img.shields.io/badge/Status-Developing-yellowgreen)

Bear bones realtime operating system for embedded systems. Focus lies on KISS (**K**eep-**I**t-**S**uper-**S**imple).

> Read the [&rarrb; Getting Started](#getting-started) when you want to use the OS in your project.

## Software Framework
![GitHub Build Status](https://img.shields.io/github/actions/workflow/status/SebastianOberschwendtner/otos/Build.yml)
![GitHub Test Status](https://img.shields.io/github/actions/workflow/status/SebastianOberschwendtner/otos/Test.yml?label=test)
![IDE2](https://img.shields.io/static/v1?label=IDE&message=VSCode&color=yellowgreen)
![GitHub top language](https://img.shields.io/github/languages/top/SebastianOberschwendtner/otos?color=brightgreen)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/SebastianOberschwendtner/otos)
![GitHub](https://img.shields.io/github/license/SebastianOberschwendtner/otos)

> Supported Microcontrollers:
- STM<sup>&reg;</sub>

|Device | Core | FPU | Header |
|---|---|:---:|---|
|STM32F429|*ARM* Cortex M4<sup>&reg;</sub>|:x:|`arm_cm4_nofpu.h`|
|STM32F405|*ARM* Cortex M4<sup>&reg;</sub>|:x:|`arm_cm4_nofpu.h`|
|STM32L073|*ARM* Cortex M0+<sup>&reg;</sub>|:x:|`arm_cm0plus_nofpu.h`|
|STM32L072|*ARM* Cortex M0+<sup>&reg;</sub>|:x:|`arm_cm0plus_nofpu.h`|
|STM32L053|*ARM* Cortex M0+<sup>&reg;</sub>|:x:|`arm_cm0plus_nofpu.h`|

- Atmel<sup>&reg;</sub>

|Device | Core | FPU | Header |
|---|---|:---:|---|
|-|||

## Branches
|Name|Status|Description|
|---|---|---|
|**main**|`active`| Main branch for working on **releases**|
|**dev**|`active`| Main branch for **developing**|

## Getting Started
You want to use the *OTOS* operating system in your project? Awesome! :sunglasses:

Here are the main steps to get up and running:

### Initialize the Kernel
The kernel is automatically initialized when the *kernel* object is created:
```cpp
// Create the kernel object
OTOS::Kernel OS;
```
- Only create the kernel **once**! (The kernel class is not yet implemented as a *singleton*...)
- The kernel should be created in your **main** function.

### Schedule the Threads
Each thread handles **one** task. You can bundle multiple actions in one task, or use separate tasks for each action.
The tasks are scheduled using their *function name*:
```cpp
// Schedule task with the function name MyTask and a stack size of 128 bytes
OS.scheduleTask<128>(&MyTask, OTOS:PrioNormal);
```
- You have to schedule the thread in the file where your *kernel* object is created.
- The task can be defined in a separate file.

When you do not give an execution rate, the task will run whenever it is possible to run it.
That means:
- No higher priority task can be run.
- No other task with a defined execution frequency can run.

To define a execution frequency the task is allowed to execute use this task constructor:
```cpp
// Schedule task with the function name MyTask and run it with 10 Hz
OS.scheduleTask<128>(&MyTask, OTOS:PrioNormal, 10);
```

### Control within Thread/Task
The OS **does not** implement a *preemptive* scheduling (yet). So your scheduled task has
to periodically yield its execution and tell the OS that another task can be executed.
```cpp
// Tell the OS that it can give the control to another task
OTOS::Thread::yield();
```

### Timing within Tasks
You can use `Timed_Task` for timing within tasks.
The function will use the `yield()` function for waiting and timing. 
Currently, only the *SysTick* timer is used as a time base, but you can give the task any function handle which returns a time as an integer type.
To create a task using the *SysTick* timer use this:
```cpp
// Create a timed task using SysTick as the timebase
OTOS::Timed_Task MyTask(OTOS::get_time_ms);
```

The function `get_time_ms()` is a static class method of the OTOS kernel and can be called from anywhere to get the current time in *[ms]* since the system was started.

>:warning: The SysTick Interrupt has to be enabled!

Within the *SysTick* interrupt execute the following kernel timing functions:
```cpp
/** 
 * @brief Provide a Interrupt handler for the systick timer,
 * which gets called every 1 ms.
 */
extern "C" void SysTick_Handler(void)
{
    OS.count_time_ms();
    OS.update_schedule();
};
```
In the main function configure the interrupt using the following function:
```cpp
// Configure SysTick timer for interrupts every 1 ms
Timer::SysTick_Configure();
```
The function takes the configured CPU clock into account and configures the *SysTick* interrupt for an interrupt every 1ms.

### Start Executing the Threads
Once all threads are scheduled, you can start the kernel execution with:
```cpp
// Start the kernel and execute the threads
OS.start();
```
- This starts an *infinite* loop inside the kernel, which switches the context of each thread according to the schedule.
