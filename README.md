# Limited_Software_Stacks_on_Arduino
Lightweight concurrency and synchronization on memory-constrained Arduino hardware using Protothreads and semaphores.


# Limited Software Stacks on Arduino

The main idea of the project implementation is to handle multiple concurrent tasks —specifically, reading button inputs, reading potentiometer values, managing LED states and updating an OLED display. This in achieved with protothreads and semaphores and later on with the use of FreeRTOS.

The project evaluates pure cooperative **Protothreads**, **Protothreads synchronized via Semaphores**, and a stress-test case study of preemptive **FreeRTOS** to determine real-world performance and memory boundaries

## Hardware & System Setup

The project was implemented on the Arduino Uno Grove Beginners Kit. The modules used were: a button, a potentiometer, a LED light and an OLED display.

| Component | Pin / Interface | Task Role |
| :--- | :--- | :--- |
| **Push Button** | `D6` (Digital In) | User input toggle with software debouncing |
| **Potentiometer** | `A0` (Analog In) | Continuous ADC sampling (0–1023) |
| **LED** | `D4` (PWM Out) | Actuates duty cycle scaled to potentiometer position |
| **0.96" OLED (SSD1315)** | `I2C` (`Wire`) | Renders state strings and ADC readouts via `U8x8` |

## Libraries Included

For the implementation of the project we used the following list of libraries on our Arduino IDE. 

- pt.h   // Implements protothreads
- semphr.h   // Provides semaphore functionality for synchronization in FreeRTOS
- pt-sem.h    // Offers semaphore support tailored for use with Protothreads

- Arduino_FreeRTOS.h   // Enables Arduino boards to run the FreeRTOS real-time operating system

- Wire.h   // Allows to communicate with I2C devices (for the Arduino)

- U8x8lib.h  // Library for the OLED Display of the Arduino


## Time Measurements

All the time measurements that were implemented during this project have Unit of measurement the μs (microseconds) 

## References
- Adam Dunkels, Protothreads:The Protothreads Library 1.4 Reference Manual, 2006

- Adam Dunkels,Oliver Schmidt, Thiemo Voigt, Muneeb Ali , Protothreads: Simplifying Event-Driven Programming of Memory-Constrained Embedded Systems, 2006

- Stoyanov, Y,  RTOS: Mutex and semaphore basics, Open4Tech , (2020, March 16)

- Embedds,  A comprehensive guide to RTOS in embedded systems: Types, benefits, choosing the right RTOS, and more, Embedds, Retrieved June 9, 2024
