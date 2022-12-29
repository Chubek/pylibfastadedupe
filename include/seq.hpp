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
        Seq(chartype_t *seqin, int index_in_array);        
        keytype_t hashSelf();
        int compareHammingWith(Seq& other);
        void markOut(int lead_index);

    private: 
        void setSeq(chartype_t *seqin); 
        void packAndRevComp();
};


void initSeqGlobal();

#endif