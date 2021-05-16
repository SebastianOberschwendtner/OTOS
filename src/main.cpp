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
/**
 ******************************************************************************
 * @file    main.c
 * @author  SO
 * @version v1.0.0
 * @date    09-March-2021
 * @brief   Main function for the OTOS. Mainly used to demonstrate how the OTOS
 *          functions work and should be used.
 ******************************************************************************
 */

// ****** Includes ******
#include "main.h"

// ****** Variables ******
unsigned int Value = 0;
// OTOS::kernel_t Stack;

// ****** Main ******
int main(void)
{
    // Value = myass((unsigned int)Stack.Stack + OTOS_STACK_SIZE);
    // OTOS::Kernel OS;
    // // Init OS with the maximum execution time each thread gets per execution slot.
    // ///@todo Should the slot time be calculated automatically? Or be defined separately.
    // OTOS_Init(SlotTime);

    // // Schedule threads
    // // Low priority, no timing requirements
    // OTOS_ScheduleThread(&MyFunc1, LOW);
    // OS.ScheduleThread();
    // // Mid priority, execute with 10 Hz
    // OTOS_ScheduleThread_Hz(&MyFunc2, MID, 10);
    // OS.ScheduleThread_Hz();

    // // Start execution, also pets the watchdog periodically
    // OTOS_Start();
    // OS.Start();
    if(Value)
        Value = 1;
    else
        Value =2;

    while(1);

    // Never reached
    return 0;
}

// ****** Functions ******
// void MyFunc1(void)
// {
//     // Init stuff here
//     OTOS::Thread MyThread;
    
//     while(1)
//     {
//         //Loop Here...

//         //Get elapsed time since boot in ms
//         // time = OTOS_Time_ms();

//         /* Lock this thread, meaning disabling interrupt for SysTick
//          * so that the thread cannot be interrupted. Only via the watchdog.
//          */
//         MyThread.Lock();
//         // Enable Systick interrupt again
//         MyThread.Unlock();

//         // Wait for condition to become true
//         MyThread.WaitFor(true);

//         // Sleep and do other stuff in the meantime
//         MyThread.Sleep(10);

//         //Yield execution and give control back to the kernel
//         MyThread.Yield();
//     }

// };

// void MyFunc2(void)
// {
//         // Get items from IPC queue
//         items = Thread_GetQueue();

//         // Push to IPC queue
//         Thread_PushQueue(Target);

//         // => KISS: Communication stays in ipc.c
// };