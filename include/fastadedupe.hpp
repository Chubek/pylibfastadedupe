#ifndef FASTADEDUPE_H
#define FASTADEDUPE_H


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <queue>
#include <assert.h>
#include <chrono>
#include <cstring>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <unistd.h>
#include <stdint.h>
#include <immintrin.h>

using namespace std;

#include "types.hpp"
#include "utils.hpp"
#include "hashmap.hpp"
#include "intr.hpp"
#include "seq.hpp"

struct OutArray {
    int *out_array;
    int size;
    mutex lock;

    void lock() {
        lock.lock();
    }

    void unlock() {
        lock.unlock();
    }

    void setArr(int *out) {
        out_array = out;
    }

    void setArrAt(int index, int value) {
        out_array[index] = value;
    }

};

#endif