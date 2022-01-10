//
// Created by quent on 9/3/2021.
//

#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class Clock {
private:
    /**
     * The number of clock cycles since the last reset.
     */
    int clock;

    /**
     * Resets the clock to 0.
     */
    void reset();

    /**
     * Increments the clock and completes that number of cycles on the CPU.
     * @param num   The number of clock cycles to complete.
     */
    void tick(int num);

    /**
     * Dumps the clock to stdout.
     */
    void dump() const;

    /**
     * Completes a single cycle of work.
     */
    void doCycleWork() const;

public:
    /**
     * Parses the given infile stream.
     * @param infile The stream to parse.
     */
    void parse(ifstream &infile);
};

/**
 * Gets the current Clock.
 * @return  The current Clock.
 */
Clock &getClock();
