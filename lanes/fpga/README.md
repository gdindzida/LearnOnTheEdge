# Roadmap

## FPGA-0: Set up RTL enviroment

<details>
- Practical:
    - Tools: SystemVerilog, Verilator, GTKWave, VS Code
    - Creat first simulation
    - Write tiny testbench
- Theoretical:
    - why these tools?
    - what are they used for?
    - basic usage of these tools?
- Hardware:
    - no hardware only simulator - Verilator
</details>

## FPGA-1: Combinatorial hardware

<details>
- Practical:
    - build combinatorial modules:
        - and/or/xor
        - mux
        - decoder
        - comparator
        - adder
        - ...?
    - for each do: RTL -> testbench -> simulation -> waveform
- Theoretical:
    - basic combinatorial circuits
    - system verilog
    - optimal design?
- Hardware:
    - no hardware only simulator - Verilator
</details>

## FPGA-2: Clocked logic

<details>
- Practical:
    - build a counter on rising edge
    - synchronous reset
    - enable
    - overflow
- Theoretical:
    - concepts: clock, rising edge, registers, synchronous reset, counter, clock enables
- Hardware:
    - no hardware only simulator - Verilator
</details>

## FPGA-3: FSM + valid/ready

<details>
- Practical:
    - build a small FSM - just process data
    - then add valid/ready
- Theoretical:
    - What is FSM?
- Hardware:
    - no hardware only simulator - Verilator
</details>

## FPGA-4: Build a hardware multiplier peripheral

<details>
- Practical:
    - receive two numbers
    - recevie start
    - perform the op
    - produce result
    - assert done
    - write testbench
- Theoretical:
    - what is hardware system
- Hardware:
    - no hardware only simulator - Verilator
</details>

## FPGA-5+: Remaining work 

### Midterm

- Sipeed Tang Nano 20k
    - Setup Sipeed toolchain
    - blink LED
    - counter -> LED
    - Button -> LED
    - UART transmitter
    - UART receiver
    - put simulated multiplier on the FPGA
    - compare simulation with hardware

- QMTECH Zynq-7000
    - understand PS vs PL
    - boot linux on ARM
    - build a simple PL peripheral
    - map its regs into ARM address space
    - write C/C++ program that controls it
    - Add interrupt
    - Measure communication overhead
    - move your multiplier/vector operation into PL
    - benchmark ARM vs FPGA

### Longterm

- FPGA accelerator 

- Tiny Tapeout / ASIC