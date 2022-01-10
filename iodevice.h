//
// Created by quent on 12/11/2021.
//

#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class IODevice {
private:
    enum Operation {
        READ,
        WRITE
    };

    struct Location {
        unsigned char address;
        char value;
    };

    struct Event {
        int tick;
        Operation operation;
        Location location;
    };

    struct _iodevice {
        Event schedule[100];
        char reg;
        char currentTask;
        char writingValue;
        bool memDonePtr;
    } ioDev;

    /**
     * Reads from data memory the value at the given address. Sets the IO device register to this value
     * @param address The address in data memory to read from
     */
    void read(unsigned char address);

    /**
     * Write to data memory the value at the given address
     * @param address   The address in data memory to write to
     * @param value     The value to write
     */
    void write(unsigned char address, char value);

    /**
     * Resets the io device
     */
    void reset();

    /**
     * Loads the schedule from the given infile stream
     * @param infile The stream which contains the schedule
     */
    void load(ifstream &infile);

    /**
     * Prints the hex value of the IO device's register
     */
    void dump() const;

public:
    /**
     * Parses the given infile stream
     * @param infile The stream to parse
     */
    void parse(ifstream &infile);

    /**
     * Attempts to start a scheduled task
     * @param clock The current clock value
     */
    void iodeviceAttemptScheduledTask(int clock);

};

/**
 * Gets the current IO device
 * @return The current IO device
 */
IODevice &getIODevice();
