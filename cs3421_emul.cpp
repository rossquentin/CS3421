//
// Created by quent on 9/8/2021.
//
#include "cpu.h"
#include "mem.h"
#include "clock.h"
#include "iMem.h"
#include "cache.h"
#include "iodevice.h"

using namespace std;

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "usage: cs3421_emul <data_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);

    if (!ifstream()) {
        cerr << "error opening input file" << argv[1] << endl;
        return 1;
    }

    Cpu &cpu = getCPU();
    Mem &mem = getMem();
    Clock &clock = getClock();
    IMem &iMem = getIMem();
    Cache &cache = getCache();
    IODevice &ioDevice = getIODevice();


    string deviceName;

    while (infile >> deviceName) {
        if (deviceName == "cpu") {
            cpu.parse(infile);
        } else if (deviceName == "memory") {
            mem.parse(infile);
        } else if (deviceName == "clock") {
            clock.parse(infile);
        } else if (deviceName == "imemory") {
            iMem.parse(infile);
        } else if (deviceName == "cache") {
            cache.parse(infile);
        } else if (deviceName == "iodev") {
            ioDevice.parse(infile);
        } else {
            return 1;
        }
    }
}
