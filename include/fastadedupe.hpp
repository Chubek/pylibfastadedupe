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
#include <string>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <time.h>
#include <unordered_map>
#include <algorithm>
#include <unistd.h>
#include <stdint.h>
#include <immintrin.h>

#define SIZE_IMM 16
#define SIZE_IMM_EXPANDED_U64 SIZE_IMM * 4
#define SIZE_IMM_EXPANDED_U8 SIZE_IMM * 32
#define SIZE_IMM_PACKED SIZE_IMM

typedef uint8_t stored_u8_t[SIZE_IMM_EXPANDED_U8];
typedef uint64_t stored_u64_t[SIZE_IMM_EXPANDED_U64];
typedef uint32_t seqsize_t;
typedef char chartype_t;
typedef std::string strtype_t;
typedef uint64_t hashtype_t;
typedef __m256i immx_t[SIZE_IMM];

struct OutArray
{
    int *out_array_main;
    int *out_array_revcomp;
    int size;
    int dups;
    int dups_revcomp;
    std::mutex *this_lock;

    OutArray() = default;
    OutArray(int *out_array_main, int *out_array_revcomp, int size);
    void setArrAt(int index, int value);
    void setArrRCAt(int index, int value);
};

#include "immx.hpp"
#include "hash.hpp"
#include "seqmap.hpp"
#include "concurrent.hpp"

// main python interface for libfastadedupe
// seqs must be null-terminated of int8 type (not signed)
// out and out_revcomp must be the same size as the seq array
// number of workers and number of threads should not exceed std::thread::hardware_concurrency
// pass true to do_revcomp to calculate revcomps, otherwise it will be empty.
void fastadedupe(chartype_t *seqs[], int size, int out[], int out_revcomp[], int num_threads, int num_workers, int do_revcomp);

extern "C" {
    extern void pylibfastadedupe_ffi(char *seqs[], int size[1], int out[], int out_revcomp[], int num_threads[1], int num_workers[1], int do_revcomp[1]);
}

#endif