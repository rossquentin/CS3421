//
// Created by quent on 9/28/2021.
//

#include "iMem.h"

void IMem::create(int size) {
    iMemDev.instr_mem = new int[size];
    iMemDev.size = size;
}

void IMem::reset() const {
    for (int i = 0; i < iMemDev.size; i++) {
        iMemDev.instr_mem[i] = 0;
    }
}

void IMem::dump(int address, int count) const {
    int width = 8;
    int endAddress = address + count;
    int startRow = address / width;
    int endRow = endAddress % width != 0 ? endAddress / width : endAddress / width - 1;

    printf("Addr     0     1     2     3     4     5     6     7\n");

    for (int i = startRow; i <= endRow; ++i) {
        int row = width * i;
        printf("0x%02X ", width * i);
        for (int j = 0; j < width; ++j) {
            if ((i <= startRow && j < address - (row)) || (row) + j >= endAddress) {
                printf("      ");
            } else {
                printf("%05X ", iMemDev.instr_mem[(row) + j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void IMem::set(int address, ifstream &infile) const {
    int command;
    int i = address;
    while (infile >> hex >> command) {
        iMemDev.instr_mem[i] = command;
        i++;
    }
}

int IMem::iMemGetInstruction(int address) const {
    return iMemDev.instr_mem[address];
}

void IMem::parse(ifstream &infile) {
    string argument;
    infile >> dec >> argument;

    if (argument == "create") {
        int size;
        infile >> hex >> size;
        create(size);
    } else if (argument == "reset") {
        reset();
    } else if (argument == "dump") {
        int address;
        int count;
        infile >> hex >> address;
        infile >> hex >> count;
        dump(address, count);
    } else if (argument == "set") {
        int address;
        infile >> hex >> address;
        infile >> dec >> argument;
        if (argument == "file") {
            string data;
            infile >> dec >> data;
            ifstream datafile(data);

            if (!ifstream()) {
                cerr << "error opening instruction memory file: " << data[1] << endl;
                exit(1);
            }

            set(address, datafile);
        }
    }
}

IMem::~IMem() {
    delete[] iMemDev.instr_mem;
}

IMem &getIMem() {
    static IMem _iMem;
    return _iMem;
}
