//
// Created by quent on 9/3/2021.
//

#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class Mem {
private:
    enum State {
        FETCH,
        STORE,
        MOVE_DATA,
        IDLE
    };
    struct _mem {
        unsigned char *memory;
        int size;

        enum State state = IDLE;
        unsigned int address;
        unsigned int count;
        char *dataPtr;
        bool *memDonePtr;
        bool writeToMem[8];

        int speed = 5;
        int completedTicks = 1;
    } memDev;

    /**
     * Creates a block of memory of a given size.
     * @param size The size of the memory block to create.
     */
    void create(int size);

    /**
     * Sets all memory blocks to 0.
     */
    void reset() const;

    /**
     * Dumps the contents of memory between [startAddress, startAddress + count] to stdout.
     * @param startAddress  The starting location to dump.
     * @param count         The number of elements to dump.
     */
    void dump(int startAddress, int count) const;

    /**
     * Sets the memory address to the given byte.
     * @param address   The address to set.
     * @param byte      The byte to set the address to.
     */
    void set(int address, unsigned char byte) const;

    /**
     * Stores count bytes in range of [address, address+count) with value in dataPtr
     * @param address   The address to store the value into
     * @param count     The number of bytes to store
     * @param dataPtr   The pointer which has the bytes to store
     */
    void memStore(unsigned int address, unsigned int count, char *dataPtr);

    /**
     * Fetches count bytes in range of [address, address+count) into dataPtr
     * @param address   The address to fetch the value from
     * @param count     The number of bytes to fetch
     * @param dataPtr   The pointer to store the bytes into
     */
    void memFetch(unsigned int address, unsigned int count, char *dataPtr) const;

public:
    /**
     * Parses the given infile stream.
     * @param infile The stream to parse.
     */
    void parse(ifstream &infile);

    /**
     * Starts the fetch operation
     * @param address       The address to fetch the bytes from
     * @param count         The number of bytes to fetch
     * @param dataPtr       The pointer to store the bytes into
     * @param memDonePtr    The memory done pointer
     */
    void memStartFetch(unsigned int address, unsigned int count, char *dataPtr, bool *memDonePtr);

    /**
     * Starts the store operation
     * @param address       The address to store the value into
     * @param count         The number of bytes to store
     * @param dataPtr       The pointer which has the bytes to store
     * @param writeToMem    An array of size count which determines which bytes to write
     * @param memDonePtr    The memory done pointer
     */
    void
    memStartStore(unsigned int address, unsigned int count, char *dataPtr, const bool *writeToMem, bool *memDonePtr);

    /**
     * Tests if memory has any work to do in this cycle
     * @return True if there is still work to be done this cycle, false otherwise
     */
    bool memWorkToDo() const;

    /**
     * Attempts to complete a single cycle of memory work
     */
    void memDoCycleWork();

    /**
     * Starts a new tick for memory
     */
    void memStartTick();

    ~Mem();
};

/**
 * Gets the current Memory.
 * @return  The current Memory.
 */
Mem &getMem();