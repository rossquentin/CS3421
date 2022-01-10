//
// Created by quent on 12/11/2021.
//

#include "iodevice.h"
#include "mem.h"


void IODevice::read(unsigned char address) {
    Mem &mem = getMem();
    mem.memStartFetch(address,
                      1,
                      &ioDev.reg,
                      &ioDev.memDonePtr);
    ioDev.currentTask++;
}

void IODevice::write(unsigned char address, char value) {
    Mem &mem = getMem();
    ioDev.writingValue = value;
    bool writeToMem[8] = {true, true, true, true, true, true, true, true};
    mem.memStartStore(address,
                      1,
                      &ioDev.writingValue,
                      writeToMem,
                      &ioDev.memDonePtr);
    ioDev.currentTask++;
}

void IODevice::reset() {
    ioDev.reg = 0;
    ioDev.currentTask = 0;
    ioDev.writingValue = 0;
    for (Event & i : ioDev.schedule) {
        i.tick = 0;
        i.location.address = 0;
        i.location.value = 0;
        i.operation = READ;
    }
}

void IODevice::load(ifstream &infile) {
    int clock;
    string operation;
    int address;
    int value;
    char addedTasks = 0;

    while (!infile.eof()) {
        infile >> dec >> clock;
        infile >> dec >> operation;
        infile >> hex >> address;
        ioDev.schedule[ioDev.currentTask].tick = clock;
        ioDev.schedule[ioDev.currentTask].operation = (operation == "read") ? READ : WRITE;
        ioDev.schedule[ioDev.currentTask].location.address = address;
        if (operation == "write") {
            infile >> hex >> value;
            ioDev.schedule[ioDev.currentTask].location.value = value;
        }
        ioDev.currentTask++;
        addedTasks++;
    }
    ioDev.currentTask -= addedTasks;
}

void IODevice::dump() const {
    printf("IO Device: 0x%02X\n\n", ioDev.reg);
}

void IODevice::parse(ifstream &infile) {
    string argument;
    infile >> dec >> argument;

    if (argument == "reset") {
        reset();
    } else if (argument == "load") {
        string data;
        infile >> dec >> data;
        ifstream datafile(data);

        if (!ifstream()) {
            cerr << "error opening io device data: " << data[1] << endl;
            exit(1);
        }
        load(datafile);
    } else if (argument == "dump") {
        dump();
    }
}

void IODevice::iodeviceAttemptScheduledTask(int clock) {
    Event attemptedEvent = ioDev.schedule[ioDev.currentTask];
    if (attemptedEvent.tick == clock) {
        if (attemptedEvent.operation == READ) {
            read(attemptedEvent.location.address);
        } else if (attemptedEvent.operation == WRITE) {
            write(attemptedEvent.location.address, attemptedEvent.location.value);
        } else {
            exit(1);
        }
    }
}

IODevice &getIODevice() {
    static IODevice ioDevice;
    return ioDevice;
}
