//
// Created by quent on 9/3/2021.
//

#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class Cpu {
private:
    enum MaskShift {
        S_IMMEDIATE = 0,
        S_TARGET = 8,
        S_SOURCE = 11,
        S_DESTINATION = 14,
        S_INSTRUCTION = 17,

    };

    enum Mask {
        M_IMMEDIATE = 0xFF,
        M_TARGET = 0x7,
        M_SOURCE = 0x7,
        M_DESTINATION = 0x7,
        M_INSTRUCTION = 0x7,
    };

    enum Instruction {
        ADD = 0x0,
        ADDI = 0x1,
        MUL = 0x2,
        INV = 0x3,
        BRANCH = 0x4,
        LW = 0x5,
        SW = 0x6,
        HALT = 0x7
    };

    enum State {
        IDLE,
        FETCH,
        DECODE,
        MEM_REQ,
        WAIT,
        BRANCHING,
        MULTIPLYING,
        HALT_CPU
    };

    struct _cpu {
        enum State state = IDLE;
        char reg[8];
        struct _hilo {
            char high;
            char low;
        } mulHighLow;
        char mulReg;
        char PC;
        int TC;
        char branchTo;
    } cpuDev;

    /**
     * The status of memory fetching
     */
    bool memDone = true;

    /**
     * Resets the CPU, PC, and TC to 0.
     */
    void reset();

    /**
    * Sets the given registry to the given hex value.
    * @param reg 	The registry to set.
    * @param byte 	The hex value to set the registry to.
    */
    void setReg(const string &reg, char byte);

    /**
     * Dumps all CPU registry and PC to stdout.
     */
    void dump();

    /**
     * Decodes an instruction from instruction memory
     * @param command The instruction to decode
     */
    void cpuDecode(int command);

    /**
     * Adds two 8-bit twos-complement values together.
     * @param first         The first value to add.
     * @param second        The second value to add.
     * @param destination   The destination address.
     */
    void add(char first, char second, char *destination);

    /**
     * Tests if the CPU can move to the next tick.
     * @return True if the CPU can move to the next tick, false otherwise.
     */
    bool isDoneState() const;

    /**
     * Tests if the branch type can be taken.
     * @param type      The type of branch, either lt, eq, or neq.
     * @param source    The first word to check.
     * @param target    The second work to check.
     * @param immediate The value to set the PC to if branching.
     */
    void checkBranch(char type, char source, char target, char immediate);

public:
    /**
     * Parses the given infile stream.
     * @param infile The stream to parse.
     */
    void parse(ifstream &infile);

    /**
     * Attempts to complete a single cycle of CPU work.
     */
    void cpuDoCycleWork();

    /**
     * Tests if the CPU has any work to do in this cycle.
     * @return True if there is still work to be done this cycle, false otherwise.
     */
    bool cpuWorkToDo() const;

    /**
     * Starts a new tick for the CPU.
     */
    void cpuStartTick();
};

/**
 * Gets the current CPU.
 * @return  The current CPU.
 */
Cpu &getCPU();
