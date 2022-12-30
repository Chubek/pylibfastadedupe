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




void Seq::setSeq(chartype_t *seqin) {
    string s = seqin;
    seq = s;
}

void Seq::packAndRevComp() {
    packed.init(seq);
}

bool Seq::compareHammingWith(Seq& other) {
    int diff = packed.packed.hammingWith(other.packed.packed, lookup_num_diffs);

    if (diff < 1) {
        out.setArrAt(index_in_array, other.index_in_array);
        is_dup = true;

        return true;
    }   

    return false;
}

bool Seq::revCompcompareHammingWith(Seq& other) {
    int diff_revcomp = packed.rev_comp.hammingWith(other.packed.rev_comp, lookup_num_diffs);

    if (diff_revcomp < 1) {
        out.setArrRCAt(index_in_array, other.index_in_array);
        is_dup = true;

        return true;
    }

    return false;
}

hashtype_t Seq::hashSelf() {
    hashtype_t hash1 = 0;
    hashtype_t hash2 = 0;

    uint64_t first_letters = packed.packed.stored_u64[0];
    uint64_t last_letters = packed.packed.stored_u64[SIZE_IMM_EXPANDED_U64 - 1];

    string substr = seq.substr(seq.length() / 3, 3 * (seq.length() / 4));

    HASH64(first_letters, hash1, last_letters);
    HASHSTR(substr, hash2);

    hashtype_t hash2_shifted = hash2 >> 32;
    hashtype_t final_hash = hash1 ^ hash2_shifted;

    return MASKU64_32(final_hash);
}   