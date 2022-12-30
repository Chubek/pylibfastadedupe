#ifndef SEQ_H
#define SEQ_H

class Seq {
    public:
        strtype_t seq;
        size_t len;
        PackedKmer packed;
        OutArray& out;
        int index_in_array;
        bool is_dup;
        Seq(chartype_t *seqin, int index_in_array, OutArray& out_array, ImmX all_ones, ImmX all_twos, ImmX all_threes, ImmX pack_shifts): out(out_array), packed(all_ones, all_twos, all_threes, pack_shifts) {
        setSeq(seqin);
        packAndRevComp();
        index_in_array = index_in_array;
        is_dup = false;
}
        hashtype_t hashSelf();
        bool compareHammingWith(Seq& other);
        bool revCompcompareHammingWith(Seq& other);


    private: 
        void setSeq(chartype_t *seqin); 
        void packAndRevComp();
};

#endif