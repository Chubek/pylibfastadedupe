#ifndef SEQ_H
#define SEQ_H

class Seq {
    public:
        strtype_t seq;
        size_t len;
        PackedKmer packed;
        Seq(chartype_t *seqin);
        
        keytype_t hashSelf();
        int compareHammingWith(Seq& other);
        void markOut();

    private:
        void encodeSeqOTTF();
        void reverseComplement();   
        void setSeq(chartype_t *seqin); 
        void initPacked(); 
};


void initSeqGlobal();

#endif