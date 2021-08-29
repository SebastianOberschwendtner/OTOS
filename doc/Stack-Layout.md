# Layout of the Stacks:
##  Layout 1
>:heavy_check_mark: Valid for: `cortex-m4-nofpu` `cortex-m0plus-nofpu`

>:x: **Without** FPU!

|Offset|Content n=`msp`|Content n=`psp`|
|:---:|:---|:---|
| *StackSize* | Top Stack||
|||...|
| *n* + 16 |*Kernel:* **LR**| *Thread:* **PSR**<sup>1</sup> |
| *n* + 15 |*Kernel:* **R7**|*Thread:* **PC**<sup>1</sup> |
| *n* + 14 |*Kernel:* **R6**|*Thread:* **LR**<sup>1</sup> |
| *n* + 13 |*Kernel:* **R5**|*Thread:* **R12**<sup>1</sup> |
| *n* + 12 |*Kernel:* **R4**|*Thread:* **R3**<sup>1</sup> |
| *n* + 11 |*Kernel:* **R3**|*Thread:* **R2**<sup>1</sup> |
| *n* + 10 |*Kernel:* **R2**|*Thread:* **R1**<sup>1</sup> |
| *n* + 9 |*Kernel:* **R1**|*Thread:* **R0**<sup>1</sup> |
| *n* + 8 |*Kernel:* **R12**|*Thread:* **LR**<sup>2</sup> |
| *n* + 7 |*Kernel:* **R11**|*Thread:* **R11** |
| *n* + 6 |*Kernel:* **R10**|*Thread:* **R10** |
| *n* + 5 |*Kernel:* **R9**|*Thread:* **R9** |
| *n* + 4 |*Kernel:* **R8**|*Thread:* **R8** |
| *n* + 3 |*Kernel:* **PSR**|*Thread:* **R7** |
| *n* + 2 ||*Thread:* **R6** |
| *n* + 1 |-|*Thread:* **R5** |
| *n* + 0 |-|*Thread:* **R4** |
|||...|
| 0 | End of Stack||

<sup>1</sup>Part of exception frame which is saved by hardware.

<sup>2</sup>Used to capture the correct return mode from handler mode, when the kernel exits its handler execution. Should usually be *0xFFFFFFFD*.