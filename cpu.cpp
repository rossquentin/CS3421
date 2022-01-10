//
// Created by quent on 9/3/2021.
//

#include "cpu.h"
#include "iMem.h"
#include "mem.h"
#include "cache.h"


void Cpu::reset() {
    for (char &i: cpuDev.reg) {
        i = 0;
    }
    cpuDev.state = IDLE;
    cpuDev.PC = 0;
    cpuDev.TC = 0;
}

void Cpu::setReg(const string &reg, char byte) {
    if (reg == "PC") {
        cpuDev.PC = byte;
    } else {
        cpuDev.reg[reg[1] - 'A'] = byte;
    }
}

void Cpu::dump() {
    printf("PC: 0x%02X\n", (unsigned char) cpuDev.PC);
    for (char i = 0; i < 8; i++) {
        printf("R%c: 0x%02X\n", 'A' + i, (unsigned char) cpuDev.reg[i]);
    }
    printf("TC: %d\n", (unsigned char) cpuDev.TC);
    printf("\n");
}

void Cpu::parse(ifstream &infile) {
    string argument;
    infile >> argument;

    if (argument == "reset") {
        reset();
    } else if (argument == "set") {
        infile >> argument;
        if (argument == "reg") {
            string reg;
            unsigned int byte;
            infile >> reg;
            infile >> hex >> byte;
            setReg(reg, byte);
        }
    } else if (argument == "dump") {
        dump();
    }
}

void Cpu::cpuDoCycleWork() {
    IMem &iMem = getIMem();
    Cache &cache = getCache();
    static int instruction;
    if (cpuDev.state != HALT_CPU) {
        if (cpuDev.state == FETCH) {
            instruction = iMem.iMemGetInstruction(cpuDev.PC);
            cpuDev.state = DECODE;
        } else if (cpuDev.state == DECODE) {
            cpuDecode(instruction);
        } else if (cpuDev.state == MEM_REQ) {
            cpuDev.state = WAIT;
        } else if (cpuDev.state == WAIT && memDone) {
            cpuDev.PC++;
            cpuDev.state = IDLE;
        } else if (cpuDev.state == BRANCHING) {
            cpuDev.PC = cpuDev.branchTo;
            cpuDev.state = IDLE;
        } else if (cpuDev.state == MULTIPLYING) {
            cpuDev.reg[cpuDev.mulReg] = cpuDev.mulHighLow.high * cpuDev.mulHighLow.low;
            cpuDev.PC++;
            cpuDev.state = IDLE;
        }
    }
}

void Cpu::cpuDecode(int command) {
    /*
     * All instructions have the following 20-bit form:
     * NNN DDD SSS TTT IIIIIIII
     *
     * NNN: 3-bit instruction encoding
     * DDD: 3-bit destination register selector
     * SSS: 3-bit source register selector
     * TTT: 3-bit target register selector
     * IIIIIIII: 8-bit immediate value
     */

    int instruction = (command >> S_INSTRUCTION) & M_INSTRUCTION;
    char destination = (command >> S_DESTINATION) & M_DESTINATION;
    char source = (command >> S_SOURCE) & M_SOURCE;
    char target = (command >> S_TARGET) & M_TARGET;
    char immediate = (command >> S_IMMEDIATE) & M_IMMEDIATE;;

    Mem &mem = getMem();
    Cache &cache = getCache();
    switch (instruction) {
        /*
         * Instruction encoding:
         * 000 DDD SSS TTT --------
         *
         * Description:
         * Adds registers S and T and stores in register D
         *
         * Timing: 1 cycle
         */
        case ADD:
            add(cpuDev.reg[source], cpuDev.reg[target], &cpuDev.reg[destination]);
            cpuDev.PC++;
            break;

            /*
             * Instruction encoding:
             * 001 DDD SSS --- IIIIIIII
             *
             * Description:
             * Adds registers I and S and stores in register D
             *
             * Timing: 1 cycle
             */
        case ADDI:
            add(cpuDev.reg[source], immediate, &cpuDev.reg[destination]);
            cpuDev.PC++;
            break;

            /*
             * Instruction encoding:
             * 010 DDD SSS --- --------
             *
             * Description:
             * Multiplies the unsigned high and low 4-bits of register S together
             * and stores the signed product in register D
             *
             * Timing: 2 cycles
             */
        case MUL:
            cpuDev.mulHighLow.high = (cpuDev.reg[source] >> 4) & 0xF;
            cpuDev.mulHighLow.low = cpuDev.reg[source] & 0xF;
            cpuDev.mulReg = destination;
            cpuDev.state = MULTIPLYING;
            break;

            /*
             * Instruction encoding:
             * 011 DDD SSS --- --------
             *
             * Description:
             * Inverts the bits of register S and stores in register D
             *
             * Timing: 1 cycle
             */
        case INV:
            cpuDev.reg[destination] = ~(cpuDev.reg[source]);
            cpuDev.state = WAIT;
            cpuDev.PC++;
            break;

            /*
             * Instruction encoding:
             * 100 DDD SSS TTT IIIIIIII
             *
             * Description:
             * D = 000: PC changes to I if S = T
             * D = 001: PC changes to I if S != T
             * D = 010: PC changes to I if S < T
             *
             * Timing: 1 cycle if branch is not taken, 2 cycles otherwise
             */
        case BRANCH:
            checkBranch(destination, source, target, immediate);
            break;

            /*
             * Instruction encoding:
             * 101 DDD --- TTT --------
             *
             * Description:
             * Loads a word into register D from data memory at address specified in register T
             *
             * Timing: Greater of 1 cycle or speed of data memory
             */

        case LW:
            cpuDev.state = MEM_REQ;
            cache.cacheRead(cpuDev.reg[target], &cpuDev.reg[destination], &memDone);
            cpuDev.PC++;
            break;

            /*
             * Instruction encoding:
             * 110 --- SSS TTT --------
             *
             * Description:
             * Stores the word in register S in data memory at the address specified in register T
             *
             * Timing: Greater of 1 cycle or speed of data memory
             */
        case SW:
            cpuDev.state = MEM_REQ;
            cache.cacheWrite(cpuDev.reg[target], &cpuDev.reg[source], &memDone);
            cpuDev.PC++;
            break;

            /*
             * Instruction encoding:
             * 111 --- --- --- --------
             *
             * Description:
             * Increments PC and halts execution of processor. After halting, processor will ignore
             * all future clock ticks
             *
             * Timing: 1 cycle
             */
        case HALT:
            cpuDev.state = HALT_CPU;
            cpuDev.PC++;
            break;

        default:
            exit(1);
    }
}

bool Cpu::cpuWorkToDo() const {
    return !isDoneState();
}

void Cpu::cpuStartTick() {
    if (cpuDev.state != HALT_CPU) {
        if (cpuDev.state == WAIT && memDone) {
            cpuDev.state = IDLE;
        }
        if (cpuDev.state == IDLE && cpuDev.state != HALT_CPU) {
            cpuDev.state = FETCH;
        }
        cpuDev.TC++;
    }

}

void Cpu::add(char first, char second, char *destination) {
    *destination = first + second;
    cpuDev.state = WAIT;
}

bool Cpu::isDoneState() const {
    return cpuDev.state == IDLE ||
           cpuDev.state == WAIT ||
           cpuDev.state == BRANCHING ||
           cpuDev.state == MULTIPLYING ||
           cpuDev.state == HALT_CPU;
}

void Cpu::checkBranch(char type, char source, char target, char immediate) {
    switch (type) {
        case 0:
            if (cpuDev.reg[source] == cpuDev.reg[target]) {
                cpuDev.branchTo = immediate;
                cpuDev.state = BRANCHING;
            } else {
                cpuDev.PC++;
                cpuDev.state = WAIT;
            }
            break;
        case 1:
            if (cpuDev.reg[source] != cpuDev.reg[target]) {
                cpuDev.branchTo = immediate;
                cpuDev.state = BRANCHING;
            } else {
                cpuDev.PC++;
                cpuDev.state = WAIT;
            }
            break;
        case 2:
            if (cpuDev.reg[source] < cpuDev.reg[target]) {
                cpuDev.branchTo = immediate;
                cpuDev.state = BRANCHING;
            } else {
                cpuDev.PC++;
                cpuDev.state = WAIT;
            }
            break;
    }
}

Cpu &getCPU() {
    static Cpu cpu;
    return cpu;
}
