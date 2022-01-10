//
// Created by quent on 11/24/2021.
//

#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class Cache {
private:
    enum Flag {
        INVALID,
        VALID,
        WRITTEN
    };

    enum State {
        IDLE,
        WAIT,
        MOVE_DATA
    };

    struct _cache {
        char cache[8];
        Flag flags[8];
        State state = IDLE;
        char CLO;
        bool isEnabled = false;
        bool *memDonePtr;
        unsigned char cacheAddress;
        char *dataPtr;
    } cacheDev;

    /**
     * Resets the cache
     */
    void reset();

    /**
     * Enables the cache for read/write
     */
    void enable();

    /**
     * Passes all cache operations directly to data memory
     */
    void disable();

    /**
     * Prints current data in Cache, CLO, and flags to standard output
     */
    void dump();

    /**
     * Write all written elements to data memory. Invalidate cache
     * @param memDonePtr The memory done pointer
     */
    void flush(bool *memDonePtr);

    /**
     * Write all written elements to data memory. Validate written elements, invalidate otherwise
     * @param memDonePtr The memory done pointer
     */
    void flushAndValidate(bool *memDonePtr);

    /**
     * Write to the data pointer the value of the read operation on cache miss
     * @param address The cache address storing the value
     * @param dataPtr The pointer to assign the value to
     */
    void cacheFetch(unsigned int address, char *dataPtr);

public:
    /**
     * Reads a value from cache. Will read directly to data memory if disabled
     * @param address       The address from data memory to retrieve from
     * @param dataPtr       The pointer to assign the value to
     * @param memDonePtr    The memory done pointer
     */
    void cacheRead(int address, char *dataPtr, bool *memDonePtr);

    /**
     * Writes a value to cache. Will write directly to data memory if disabled
     * @param address       The address in data memory to write to
     * @param dataPtr       The pointer which contains the value to write to memory
     * @param memDonePtr    The memory done pointer
     */
    void cacheWrite(int address, char *dataPtr, bool *memDonePtr);

    /**
     * Parses the given infile stream
     * @param infile The stream to parse
     */
    void parse(ifstream &infile);

    /**
     * Completes a single cycle of cache work
     */
    void cacheDoCycleWork();

    /**
     * Tests if the cache has any work to do in this cycle
     * @return True if there is still work to be done this cycle, false otherwise
     */
    bool cacheWorkToDo() const;
};

/**
 * Gets the current cache
 * @return The current cache
 */
Cache &getCache();
