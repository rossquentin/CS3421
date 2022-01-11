## Description
This program is the final result during CS3421 - Computer Organization. 
It runs by feeding an input set of commands to setup the components as desired, then loads a program into data memory, which the CPU will then run.

## Building
This project is a CMake project. Simply building the CMake project will compile the program

This project uses a custom entropy assembler which can be used to create instructions for the CPU. For details, see [Entropy Assembler](#entropy-assembler).

## Usage
The program takes the following parameters:

`cs3421_emul <inputfile>`

`<inputfile>` (Required) The location of the input file.

The input file can have the following commands, sorted by component:

### Clock

Usage: `clock <command>`

Note: Some commands require additional arguments.

- `reset` Sets the internal counter to zero.

- `tick <decimal integer>` Accepts a positive integer indicating how many clock ticks should be issued to attached devices. Internal counter is incremented by the given number.

- `dump` Prints the internal clock to standard output.

### CPU

Usage `cpu <command>`

Note: Some commands require additional arguments. Hex arguments must be prepended by `0x`.

- `reset` Resets all registers to zero, including the program counter (PC).

- `set reg <reg> <hex byte>` Sets the value of the given register to the given value. `<reg>` can be `RA-RH` or `PC`.

- `dump` Prints the values of all registers to standard output.

### Data Memory

Contains any data stored during the program execution.

Usage: `memory <command> [args]`

Note: Some commands require additional arguments. Hex arguments must be prepended by `0x`.

- `create <size in hex bytes>` Allocates a block of data memory of the given size.

- `reset` Sets all allocated data memory values to zero.
 
- `dump <hex address> <hex count>` Prints the values of data memory within the interval `[address, address + count)` to standard output.
 
- `set <hex address> <hex count> <hex byte 1> <hex byte 2> ... <hex byte N>` Sets the values of addresses within the interval `[address, address + count)` to the given values.

### Instruction Memory

Contains the instructions of the program to run.

Usage: `imemory <command>`

Note: Some commands require additional arguments. Hex arguments must be prepended by `0x`.

- `create <size in hex bytes>` Allocates a block of instruction memory of the given size in 20 bit words.

- `reset` Sets all allocated instruction memory values to zero.

- `dump <hex address> <hex count>` Prints the values of instruction memory within the interval `[address, address + count)` to standard output.

- `set <hex address> file <datafile>` Initializes instruction memory starting from the given address with the values found in the data file. `<datafile>` represents the location of the file genetated by the [Entropy Assembler](#entropy-assembler).

### Cache

A faster version of data memory limited to 8 bits. On a cache hit, instruction takes 1 cycle. On a cache miss, instruction fetches data from data memory, which takes 5 cycles.

Usage: `cache <command>`

- `reset` Disables cache, set to cache line offset (CLO) to zero, and all data is invalidated.

- `on` Enables the cache. Only to be enabled after completion of previous instruction and no new instruction has begun to be decoded.

- `off` Disables the cache and writes all data to data memory. Only to be enabled after completion of previous instruction and no new instruction has begun to be decoded.

- `dump` Shows contents of cache including CLO, values of the cache, and any flags for each of the cache addresses.

### I/O Device

A simple device which reads and writes directly to data memory according to a given schedule. Has a single 8-bit register.

Usage: `iodev <command>`

Note: Some commands require additional arguments. Hex arguments must be prepended by `0x`.

- `reset` Sets the register to zero.

- `load <filename>` Loads a file with I/O events into the schedule of the I/O device. Each event has the following form:
  - `<clock tick> <read | write> <address> [hex value]`
    - `<clock tick>` The clock tick where the event happens.
    - `<read | write>` The operation on data memory.
    - `<address>` Address in data memory to perform the operation.
    - `[hex value]` If writing, the value to write to data memory.
    
- `dump` Prints the value of the register to standard output.

## Entropy Assembler

This is a custom assembler which turns MIPS-like instructions into machine code. 

Usage: `java EntropyCompiler <inputfile> > <outputfile>`

The following instructions are implemented:

- `add $<regdest> $<reg1> $<reg2>` Adds `<reg1>` and `<reg2>` and stores the result in register `<regdest>`. 
  - Takes 1 cycle.

- `addi $<regdest> $<reg1> <decimal integer>` Adds `<reg1>` and the immediate 8-bit value in `<decimal integer>` and stores the result in register `<regdest>`. 
  - Takes 1 cycle.

- `mul $<regdest> $<reg1>` Multiplies the high and low 4-bits of `<reg1>` together and stores the result in register `<regdest>`. 
  - Takes 2 cycles.

- `inv $<regdest> $<reg1>` Inverts the bit of `<reg1>` and stores the result in register `<regdest>`. 
  - Takes 1 cycle.

- `beq $<reg1> $<reg2> <label>` Tests if value in `<reg1>` equals `<reg2>`. If true, set the PC to the address specified by the label. Otherwise, increment the PC. 
  - Takes 1 cycle if false, 2 cycles if true.

- `bneq $<reg1> $<reg2> <label>` Tests if value in `<reg1>` does not equal `<reg2>`. If true, set the PC to the address specified by the label. Otherwise, increment the PC. 
  - Takes 1 cycle if false, 2 cycles if true.

- `blt $<reg1> $<reg2> <label>` Tests if value in `<reg1>` is less than `<reg2>`. If true, set the PC to the address specified by the label. Otherwise, increment the PC. 
  - Takes 1 cycle if false, 2 cycles if true.

- `lw $<regdest> $<reg1>` Loads a word into `<regdest>` the value from data memory at address specified in `<reg1>`. 
  - Takes the greater of 1 cycle or the speed of data memory (5 cycles).

- `sw $<regsrc> $<reg1>` Stores a word from `<regsrc>` into data memory at address specified in `<reg1>`. 
  - Takes the greater of 1 cycle or the speed of data memory (5 cycles).

- `halt` Prevents further execution on the CPU after incrementing PC. Will still cooperate in parser commands such as `cpu dump`. 
  - Takes 1 cycle.
