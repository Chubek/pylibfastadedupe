#include "../include/fastadedupe.hpp"

const uint8_t lookup_num_diffs[256] = {
    0, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2,
    1, 2, 2, 2, 1, 2, 2, 2, 2, 3, 3, 3,
    2, 3, 3, 3, 2, 3, 3, 3, 1, 2, 2, 2,
    2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
    1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3,
    2, 3, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3,
    2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
    3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
    2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4,
    3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4,
    3, 4, 4, 4, 3, 4, 4, 4, 1, 2, 2, 2,
    2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
    2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4,
    3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4,
    3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3,
    3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
    1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3,
    2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4,
    3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3,
    3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
    2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4,
    3, 4, 4, 4};

ImmX all_threes;
ImmX all_twos;
ImmX all_ones;
ImmX pack_shifts;
OutArray out_array;

void initSeqGlobal() {
    all_threes.initOne(3);
    all_twos.initOne(2);
    all_ones.initOne(1);
    pack_shifts.initR64(2, 4, 6, 8);
}


Seq::Seq(chartype_t *seqin, int index_in_array): out(out_array), packed(all_ones, all_twos, all_threes, pack_shifts) {
    setSeq(seqin);
    packAndRevComp();
    index_in_array = index_in_array;
    is_dup = false;
}

void Seq::setSeq(chartype_t *seqin) {
    string s = seqin;
    seq = s;
}

void Seq::packAndRevComp() {
    packed.init(seq);
}

bool Seq::compareHammingWith(Seq& other) {
    int diff = packed.packed.hammingWith(other.packed.packed, lookup_num_diffs);
    int diff_revcomp = packed.rev_comp.hammingWith(other.packed.rev_comp, lookup_num_diffs);

    if (diff < 1) {
        out.setArrAt(index_in_array, other.index_in_array);
    }

    if (diff_revcomp < 1) {
        out.setArrRCAt(index_in_array, other.index_in_array);
    }

    return diff < 1 ? true : false;
}

keytype_t Seq::hashSelf() {
    keytype_t hash1 = 0;
    keytype_t hash2 = 0;

    uint64_t first_letters = packed.packed.stored_u64[0];
    uint64_t last_letters = packed.packed.stored_u64[SIZE_IMM_EXPANDED_U64 - 1];

    string substr = seq.substr(seq.length() / 3, 3 * (seq.length() / 4));

    HASH64(first_letters, hash1, last_letters);
    HASHSTR(substr, hash2);

    keytype_t hash2_shifted = hash2 >> 32;
    keytype_t final_hash = hash1 ^ hash2_shifted;

    return MASKU64_32(final_hash);
}   