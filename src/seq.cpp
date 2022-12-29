#include "../include/fastadedupe.hpp"


ImmX all_threes;
ImmX all_twos;
ImmX all_ones;
ImmX pack_shifts;

void initSeqGlobal() {
    all_threes.initOne(3);
    all_twos.initOne(2);
    all_ones.initOne(1);
    pack_shifts.initR64(2, 4, 6, 8);
}


Seq::Seq(chartype_t *seqin): packed(all_ones, all_twos, all_threes, pack_shifts) {
    setSeq(seqin);

    encodeSeqOTTF();
    reverseComplement();
}

void Seq::setSeq(chartype_t *seqin) {
    string s = seqin;
    seq = s;
}


void Seq::encodeSeqOTTF() {


}