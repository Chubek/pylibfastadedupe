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
#include "concurrent.hpp"
#include "hash.hpp"

struct OutArray {
    int *out_array;
    int *out_array_revcomp;
    int size;
    mutex this_lock;

    OutArray(int *out_array, int *out_array_revcomp, int size): out_array(out_array), out_array_revcomp(out_array_revcomp), size(size) {};

    void setArrAt(int index, int value) {
        const std::lock_guard<mutex> lock(this_lock);
        out_array[index] = value;
    }

    void setArrRCAt(int index, int value) {
        const std::lock_guard<mutex> lock(this_lock);
        out_array[index] = value;
    }

};

vector<Seq> convertSeqs(chartype_t *seqs_char, int size, OutArray& out);
HashMap<Seq&, vector<Seq&>, Seq&> clusterSeqs(vector<Seq>& seqs);

// main python interface for libfastadedupe
// seqs must be null-terminated of int8 type (not signed)
// out and out_revcomp must be the same size as the seq array
// number of workers and number of threads should not exceed std::thread::hardware_concurrency
// pass true to do_revcomp to calculate revcomps, otherwise it will be empty.
void pylibfastadedupe_interface(chartype_t **seqs, int size, int *out, int *out_revcomp, int num_threads, int num_workers, bool do_revcomp);

#endif