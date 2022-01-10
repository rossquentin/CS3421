//
// Created by quent on 9/3/2021.
//

#include "mem.h"

void Mem::create(int size) {
    memDev.memory = new unsigned char[size]();
    memDev.size = size;
}

void Mem::reset() const {
    for (int i = 0; i < memDev.size; ++i) {
        memDev.memory[i] = 0;
    }
}

void Mem::dump(int startAddress, int count) const {
    int endAddress = startAddress + count;
    int startRow = startAddress / 16;
    int endRow = endAddress % 16 != 0 ? endAddress / 16 : endAddress / 16 - 1;

    printf("Addr 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");

    for (int i = startRow; i <= endRow; ++i) {
        int row = 16 * i;
        printf("0x%02X ", 16 * i);
        for (int j = 0; j < 16; ++j) {
            if ((i <= startRow && j < startAddress - (row)) || (row) + j >= endAddress) {
                printf("   ");
            } else {
                printf("%02X ", memDev.memory[(row) + j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void Mem::set(int address, unsigned char byte) const {
    memDev.memory[address] = byte;
}

void Mem::parse(ifstream &infile) {
    string argument;
    infile >> argument;

    if (argument == "reset") {
        reset();
    } else if (argument == "create") {
        int num;
        infile >> hex >> num;
        create(num);
    } else if (argument == "dump") {
        int fileAddress;
        int fileCount;
        infile >> hex >> fileAddress;
        infile >> hex >> fileCount;
        dump(fileAddress, fileCount);
    } else if (argument == "set") {
        int fileAddress;
        int fileCount;
        unsigned int byte;
        infile >> hex >> fileAddress;
        infile >> hex >> fileCount;
        for (int i = fileAddress; i < fileAddress + fileCount; ++i) {
            infile >> hex >> byte;
            set(i, byte);
        }
    }
}

void Mem::memStartFetch(unsigned int address, unsigned int count, char *dataPtr, bool *memDonePtr) {
    memDev.state = FETCH;
    memDev.address = address;
    memDev.count = count;
    memDev.dataPtr = dataPtr;
    memDev.memDonePtr = memDonePtr;

}

void Mem::memStartStore(unsigned int address, unsigned int count, char *dataPtr, const bool writeToMem[8],
                        bool *memDonePtr) {
    memDev.state = STORE;
    memDev.address = address;
    memDev.count = count;
    memDev.dataPtr = dataPtr;
    for (int i = 0; i < 8; ++i) {
        memDev.writeToMem[i] = writeToMem[i];
    }
    memDev.memDonePtr = memDonePtr;

}

void Mem::memDoCycleWork() {
    if (memDev.state == IDLE) {
        return;
    } else if (memDev.state == FETCH) {
        *memDev.memDonePtr = false;
        if (memDev.completedTicks == memDev.speed) {
            memDev.state = MOVE_DATA;
            memDev.completedTicks = 1;
            memFetch(memDev.address, memDev.count, memDev.dataPtr);
        }
    } else if (memDev.state == STORE) {
        *memDev.memDonePtr = false;
        if (memDev.completedTicks == memDev.speed) {
            memDev.state = MOVE_DATA;
            memDev.completedTicks = 1;
            memStore(memDev.address, memDev.count, memDev.dataPtr);
        }
    } else if (memDev.state == MOVE_DATA) {
        *memDev.memDonePtr = true;
        memDev.state = IDLE;
    }
}

void Mem::memStore(unsigned int address, unsigned int count, char *dataPtr) {
    for (unsigned int i = address; i < address + count; i++) {
        if (memDev.writeToMem[i]) {
            memDev.memory[i] = *dataPtr;

        }
        dataPtr++;
    }
}

void Mem::memFetch(unsigned int address, unsigned int count, char *dataPtr) const {
    for (unsigned int i = address; i < address + count; i++) {
        *dataPtr = memDev.memory[i];
        dataPtr++;
    }
}

bool Mem::memWorkToDo() const {
    return memDev.state == MOVE_DATA;
}

void Mem::memStartTick() {
    if (memDev.state != IDLE) {
        memDev.completedTicks++;
    }
}

Mem::~Mem() {
    delete[] memDev.memory;
}

Mem &getMem() {
    static Mem mem;
    return mem;
}
