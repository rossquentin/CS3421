//
// Created by quent on 9/28/2021.
//

#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class IMem {
private:
    struct _iMem {
        int *instr_mem;
        int size;
    } iMemDev;

    /**
     * Creates an array for the instruction memory
     * @param size The size of the array
     */
    void create(int size);

    /**
     * Resets the instruction memory to zero out all instructions
     */
    void reset() const;

    /**
     * Prints all instructions in range [address, address+count)
     * @param address   The address to start
     * @param count     The number of instructions
     */
    void dump(int address, int count) const;

    /**
     * Sets the instruction memory to the contents inside the infile
     * @param address   The memory address to start at
     * @param infile    The file stream containing the instructions
     */
    void set(int address, ifstream &infile) const;

public:
    /**
     * Parses the given infile stream
     * @param infile The stream to parse
     */
    void parse(ifstream &infile);

    /**
     * Returns the integer value of the instruction
     * @param address   The address in instruction memory
     * @return          The instruction found at the given address
     */
    int iMemGetInstruction(int address) const;

    ~IMem();
};

/**
 * Gets the current instruction memory device
 * @return The current instruction memory device
 */
IMem &getIMem();