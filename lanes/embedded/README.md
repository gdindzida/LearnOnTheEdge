# Roadmap

## Stage 1: Bare Metal & Real-Time Systems

### EMBD-0: What happens after reset?

<details>
- Practical:
    - write simple firmware that
        - defines vector table
        - provides a reset handler
        - reaches main()
        - toggles a GPIO
    - use vendor tools to generate final goals
    - inspect generated ELF file
- Theoretical:
    - what is vector table?
    - what does cpu do immidiately after reset?
    - what are initial stack pointer and reset vectors?
    - what happens before main()?
    - what is exception on Cortex-M
    - read hardware datasheets
- Hardware:
    - weact dev board stm32g431cbu6
    - weact dev board stm32h743 - do the excercise on this one as well to set it up and test
</details>

### EMBD-1: Read and write a hardware register

<details>
- Practical:
    - blink an LED
    - first with HAL then manually configure GPIO directly through registers
    - compare
- Theoretical:
    - what is memory-mapped I/O?
    - what is peripheral register?
    - why does `volatile` matter?
    - what happens when C executes: `GPIOx->ODR |= ...`
    - Why can the compiler not treat a hardware register like normal RAM?
- Hardware:
    - weact dev board stm32g431cbu6
    - weact dev board stm32h743 - do the excercise on this one as well to set it up and test
</details>

### EMBD-2: Understand the memory

<details>
- Practical:
    - create program containing:
        - global initialized variable
        - global zero-initialized variable
        - `const` data
        - local variables
        - static variables
        - functions
    - inspect ELF/map file and determine where each part ended up
    - use gcc tools
- Theoretical:
    - `.text`, `.rodata`, `.data`, `.bss`, stack, heap...
    - flash vs sram vs ...
    - load address vs runtime address
    - understand structure of elf file
- Hardware:
    - weact dev board stm32g431cbu6
    - weact dev board stm32h743 - do the excercise on this one as well to set it up and test
</details>

### EMBD-3: Write your own startup

<details>
- Practical:
    - remove as much vendor (HAL) startup code as you can
    - build:
        - vector table -> Reset_Handler -> copy .data from Flash to RAM -> zer .bss -> main()
- Theoretical:
        - Why does `.data` need copying?
        - Why doesn't `.bss` need to be stored in the executable?
        - What does the linker know versus what startup code must do?
        - What is the relationship between the linker script and startup code?
- Hardware:
    - weact dev board stm32g431cbu6
    - weact dev board stm32h743 - do the excercise on this one as well to set it up and test
</details>

### EMBD-4: First interrupt

<details>
- Practical:
    - configure a timer
    - generate interrupt periodically
    - toggle a gpio inside the ISR
    - change frequency
    - then create experiment where you can measure:
        - hardware event -> interrupt -> GPIO toggle
    - measure delay
    - then make system busier by:
        - longer main loop work
        - another interrupt
        - different interrupt priorities
- Theoretical:
    - What causes interrupt?
    - What does the CPU do when entering an ISR?
    - What get saved?
    - What is interrupt priority?
    - What is interrupt latency?
        - latency vs exec time
        - jitter
        - priority
        - nested interrupts?
        - deterministic vs average behaviour?
    - What is the difference between polling and interrupts?
    - Other kinds of interrupts?
- Hardware:
    - weact dev board stm32g431cbu6
    - weact dev board stm32h743 - do the excercise on this one as well to set it up and test
</details>

### EMBD-5+: Remaining work 

#### Midterm

- interrupt safe communication
    - Practical:
        - ISR produces events → main loop consumes them.
    - Theoretical:
        - race conditions, critical sections, atomic operations, ISR restrictions.

- UART driver from regs

- SPI driver

- I2C driver

- DMA
    - UART/SPI/ADC -> DMA -> RAM
    - DMA channels, desrcriptors, synchronization, buffers, cache considerations

- Build tiny scheduler
    - Before using an RTOS, make a deliberately primitive scheduler.
    - run tasks according to timers/events.
    - scheduling, cooperative vs preemptive execution, context switching, deadlines.

- First RTOS app
    - tasks, schedulers, queues, mutexes, semaphores, task notifications
    - build a small application with several tasks communicating.
    - what the RTOS is actually doing underneath your application.

- RTOS concurrency failure
    - deliberately create a race condition.
    - Then fix it
    - Do the same with:
        - deadlock
        - priority inversion
        - excessive locking

- Measure RTOS
    - task execution time
    - scheduling latency
    - CPU utilization
    - interrupt load
    - stack usage

- Break the firmware
    - invalid memory access
    - stack overflow
    - bad pointer
    - corrupted return address
    - HardFault
    - fault status registers
    - stack frame
    - exception entry
    - post-mortem debugging

#### Longterm

- Embedded Linux and SoC arch
    - arm64 and modern CPU arch
    - linux process/thread memory model
    - linux kernel arch
    - boot process and embedded linux construction
    - device tree and hardware description
    - linux device drivers
    - linux dma and memora arch
    - linux scheduling and performance
    - linux tracing and profiling
    - SoC arch
    - heterogeneous memory and communication
    - embedded linux performance engineering

- DSP, SIMD and Data-Oriented optimization
    - performance measurement methodology
    - compiler optimization
    - cpu cache and memory optimization
    - simd fundamentals
    - arm neon
    - numerical representation
    - DSP fundamentals
    - DSP optimization
    - Accelerator oriented DSP
    - Dataflow and streaming structures
    - Algorithm to hardware mapping