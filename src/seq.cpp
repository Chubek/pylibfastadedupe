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

int Seq::compareHammingWith(Seq& other) {
    int diff = packed.packed.hammingWith(other.packed.packed, lookup_num_diffs);
    int diff_revcomp = packed.rev_comp.hammingWith(other.packed.rev_comp, lookup_num_diffs);

    if (diff < 1) {
        out.lock();
        out.setArrAt(index_in_array, other.index_in_array);
        out.unlock();
    }

    if (diff_revcomp < 1) {
        out.lock();
        out.setArrRCAt(index_in_array, other.index_in_array);
        out.unlock();
    }
}