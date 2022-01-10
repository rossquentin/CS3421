cmake_minimum_required(VERSION 3.20)
project(CS3421)

set(CMAKE_CXX_STANDARD 14)

add_executable(CS3421 cs3421_emul.cpp cpu.cpp cpu.h clock.cpp clock.h mem.cpp mem.h iMem.cpp iMem.h cache.cpp cache.h iodevice.cpp iodevice.h)
