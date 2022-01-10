//
// Created by quent on 11/24/2021.
//

#include "cache.h"
#include "mem.h"

void Cache::parse(ifstream &infile) {
    string argument;
    infile >> argument;

    if (argument == "reset") {
        reset();
    } else if (argument == "on") {
        enable();
    } else if (argument == "off") {
        disable();
    } else if (argument == "dump") {
        dump();
    }
}

void Cache::reset() {
    cacheDev.isEnabled = false;
    cacheDev.CLO = 0;
    for (Flag &flag: cacheDev.flags) {
        flag = INVALID;
    }
}

void Cache::enable() {
    cacheDev.isEnabled = true;
}

void Cache::disable() {
    cacheDev.isEnabled = false;
}

void Cache::dump() {
    printf("CLO        : 0x%02X\n", cacheDev.CLO);
    printf("cache data : ");
    for (unsigned char i: cacheDev.cache) {
        printf("0x%02X ", i);
    }
    printf("\n");
    printf("Flags      : ");
    for (Flag &flag: cacheDev.flags) {
        switch (flag) {
            case INVALID:
                printf("  I  ");
                break;
            case VALID:
                printf("  V  ");
                break;
            case WRITTEN:
                printf("  W  ");
                break;
        }
    }
    printf("\n\n");
}

void Cache::flush(bool *memDonePtr) {
    Mem &mem = getMem();
    bool writeToMem[8];
    for (int i = 0; i < 8; ++i) {
        writeToMem[i] = (cacheDev.flags[i] == WRITTEN);
        cacheDev.flags[i] = INVALID;
    }
    mem.memStartStore(cacheDev.CLO * 8, 8, cacheDev.cache, writeToMem, memDonePtr);
}

void Cache::cacheRead(int address, char *dataPtr, bool *memDonePtr) {
    Mem &mem = getMem();

    char computedCLO = address / 8;
    char cacheAddress = address % 8;
    *memDonePtr = false;

    if (cacheDev.isEnabled) {
        // If address = 0xFF, invalidate cache.
        if (address == -1) {
            for (Flag &flag: cacheDev.flags) {
                flag = INVALID;
                *memDonePtr = true;
            }
            return;
        }
        // Cache hit, return the value in cache.
        if (computedCLO == cacheDev.CLO && cacheDev.flags[cacheAddress] != INVALID) {
            *dataPtr = cacheDev.cache[cacheAddress];
            *memDonePtr = true;
        } else {
            cacheDev.state = WAIT;
            cacheDev.memDonePtr = memDonePtr;
            cacheDev.dataPtr = dataPtr;
            cacheDev.cacheAddress = cacheAddress;
            mem.memStartFetch(address - (address % 8), 8, cacheDev.cache, cacheDev.memDonePtr);
            for (Flag &flag: cacheDev.flags) {
                flag = VALID;
            }
            *dataPtr = *(cacheDev.cache + cacheAddress);
        }
    } else {
        mem.memStartFetch(address, 1, dataPtr, memDonePtr);
    }

}

void Cache::cacheWrite(int address, char *dataPtr, bool *memDonePtr) {
    Mem &mem = getMem();

    char computedCLO = address / 8;
    char cacheAddress = address % 8;
    *memDonePtr = false;

    // If address = 0xFF, write 'written' data to memory. Written data is now valid
    if (address == -1) {
        flushAndValidate(memDonePtr);
        return;
    }
    if (cacheDev.isEnabled) {
        bool noValid = true;
        bool hasWritten = false;
        for (Flag &flag: cacheDev.flags) {
            if (noValid) {
                noValid = flag == INVALID;
            }
            if (!hasWritten) {
                hasWritten = flag == WRITTEN;
            }
        }
        // Cache hit, write to cache.
        if (computedCLO == cacheDev.CLO || noValid) {
            cacheDev.cache[cacheAddress] = *dataPtr;
            cacheDev.flags[cacheAddress] = WRITTEN;
            *memDonePtr = true;
            return;
        }
            // Cache miss. Flush memory if memory is written. Invalidate cache
        else {
            if (hasWritten) {
                flush(memDonePtr);
            }
            for (Flag &flag: cacheDev.flags) {
                flag = INVALID;
            }
            cacheDev.CLO = computedCLO;
            cacheDev.cache[cacheAddress] = *dataPtr;
            cacheDev.flags[cacheAddress] = WRITTEN;
            if (!hasWritten) {
                *memDonePtr = true;
            }
            return;
        }
    } else {
        cacheDev.cache[cacheAddress] = *dataPtr;
        cacheDev.flags[cacheAddress] = WRITTEN;
        bool writeToMem[8] = {true, true, true, true, true, true, true, true};
        mem.memStartStore(address, 1, dataPtr, writeToMem, memDonePtr);
    }
}

void Cache::cacheDoCycleWork() {
    if (cacheDev.state == WAIT && *cacheDev.memDonePtr) {
        cacheDev.state = MOVE_DATA;
    }
    if (cacheDev.state == MOVE_DATA) {
        cacheFetch(cacheDev.cacheAddress, cacheDev.dataPtr);

        cacheDev.state = IDLE;
    }
}

bool Cache::cacheWorkToDo() const {
    return cacheDev.state == MOVE_DATA;
}

void Cache::cacheFetch(unsigned int address, char *dataPtr) {
    *dataPtr = *(cacheDev.cache + address);
}

void Cache::flushAndValidate(bool *memDonePtr) {
    Mem &mem = getMem();
    bool writeToMem[8];
    for (int i = 0; i < 8; ++i) {
        writeToMem[i] = (cacheDev.flags[i] == WRITTEN);
        if (cacheDev.flags[i] == WRITTEN) {
            cacheDev.flags[i] = VALID;
        }
    }
    mem.memStartStore(cacheDev.CLO * 8, 8, cacheDev.cache, writeToMem, memDonePtr);
}


Cache &getCache() {
    static Cache cache;
    return cache;
}
