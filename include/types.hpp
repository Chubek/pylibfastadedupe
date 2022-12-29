#ifndef TYPES_H
#define TYPES_H

#define SIZE_IMM 16
#define SIZE_IMM_EXPANDED_U64 SIZE_IMM * 4
#define SIZE_IMM_EXPANDED_U8 SIZE_IMM * 32

typedef uint8_t stored_u8_t[SIZE_IMM_EXPANDED_U8];
typedef uint64_t stored_u64_t[SIZE_IMM_EXPANDED_U64];
typedef uint32_t seqsize_t;
typedef char chartype_t;
typedef string strtype_t;
typedef uint64_t keytype_t;
typedef __m256i immx_t[SIZE_IMM];


template <typename T>
concept Hashable = requires(T o) {
    { o.hashSelf() } -> convertible_to<keytype_t>;
};

template <typename V>
struct KVPair {
    keytype_t key;
    V& value;
};


#endif