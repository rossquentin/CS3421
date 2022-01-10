//
// Created by quent on 9/3/2021.
//

#include "clock.h"
#include "cpu.h"
#include "mem.h"
#include "cache.h"
#include "iodevice.h"

void Clock::tick(int num) {
    for (int i = 1; i <= num; i++) {
        clock++;
        doCycleWork();
    }

}

void Clock::reset() {
    clock = 0;
}

void Clock::dump() const {
    printf("Clock: %d\n\n", clock);
}

void Clock::parse(ifstream &infile) {
    string argument;
    infile >> argument;

    if (argument == "reset") {
        reset();
    } else if (argument == "tick") {
        int num;
        infile >> dec >> num;
        tick(num);
    } else if (argument == "dump") {
        dump();
    }
}

void Clock::doCycleWork() const {
    bool workToDo = true;
    Cpu &cpu = getCPU();
    Mem &mem = getMem();
    Cache &cache = getCache();
    IODevice &ioDevice = getIODevice();

    cpu.cpuStartTick();
    mem.memStartTick();

    while (workToDo) {

        cpu.cpuDoCycleWork();
        mem.memDoCycleWork();
        cache.cacheDoCycleWork();
        ioDevice.iodeviceAttemptScheduledTask(clock);

        workToDo = cpu.cpuWorkToDo() || mem.memWorkToDo() || cache.cacheWorkToDo();
    }


}


Clock &getClock() {
    static Clock clock;
    return clock;
}
