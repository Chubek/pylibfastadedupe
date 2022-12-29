#ifndef INTR_H
#define INTR_H


struct ImmX {
    immx_t loaded;
    stored_u8_t stored_u8;
    stored_u8_t stored_u8_reverse;
    stored_u64_t stored_u64;
    stored_u64_t stored_u64_reverse;

    ImmX operator ^ (ImmX const &against) {
        ImmX res_imm;
        
        for (int i = 0; i < SIZE_IMM; i++) {
            res_imm.loaded[i] = _mm256_xor_si256(loaded[i], against.loaded[i]);       
        }

        return res_imm;
    }


    ImmX operator & (ImmX const &against) {
        ImmX res_imm;
        
        for (int i = 0; i < SIZE_IMM; i++) {
            res_imm.loaded[i] = _mm256_and_si256(loaded[i], against.loaded[i]);       
        }

        return res_imm;
    }

    ImmX operator >> (ImmX const &against) {
        ImmX res_kmer;
        
        for (int i = 0; i < SIZE_IMM; i++) {
            res_kmer.loaded[i] = _mm256_srav_epi16(loaded[i], against.loaded[i]);       
        }

        return res_kmer;
    }

    ImmX operator << (ImmX const &against) {
        ImmX res_kmer;
        
        for (int i = 0; i < SIZE_IMM; i++) {
            res_kmer.loaded[i] = _mm256_sllv_epi16(loaded[i], against.loaded[i]);       
        }

        return res_kmer;
    }

    void initOne(uint8_t imm) {
        for (int i = 0; i < SIZE_IMM; i++) {
            loaded[i] = _mm256_set1_epi8(imm);
        }
    }

    void initR64(uint64_t one, uint64_t two, uint64_t three, uint64_t four) {
        for (int i = 0; i < SIZE_IMM; i++) {
            loaded[i] = _mm256_setr_epi64x(one, two, three, four);
        }
    }

    void initLoadU(strtype_t str,  seqsize_t size) {
        chartype_t *buffer = new chartype_t[32];
        strtype_t sstr = "";

        int j = 0;
        for (int i = 0; i < size; i += 32) {
            sstr = str.substr(i, 32);
            strncpy(buffer, sstr.c_str(), 32);

            loaded[j++] = _mm256_loadu_si256((__m256i *)buffer[0]);
        }
    }
    

    void saveStoreU64() {
        uint64_t *buffer = new uint64_t[4];

        for (int i = 0; i < SIZE_IMM; i++) {
            _mm256_storeu_si256((__m256i *)&buffer, loaded[i]);
            memcpy(&stored_u64[i], buffer, 4 * sizeof(uint64_t));
        }
    }
    
    void saveStoreU8() {
        uint8_t *buffer = new uint8_t[32];

        for (int i = 0; i < SIZE_IMM; i++) {
            _mm256_storeu_si256((__m256i *)&buffer, loaded[i]);
            memcpy(&stored_u8[i], buffer, 32 * sizeof(uint8_t));
        }

    }

    void reverseU8Stored() {
        reverseArray<uint8_t>(stored_u8, SIZE_IMM_EXPANDED_U8);
    }

    void reverseU64Stored() {
        reverseArray<uint64_t>(stored_u64, SIZE_IMM_EXPANDED_U64);
    }
    
};

struct PackedKmer {
    ImmX loaded;
    ImmX packed;   
    ImmX rev_comp; 
    ImmX& all_ones;
    ImmX& all_twos;
    ImmX& all_threes;
    ImmX& pack_shifts;

    PackedKmer(ImmX& ones, ImmX& twos, ImmX& threes, ImmX& pack_shifts): all_ones(ones), all_twos(twos), all_threes(threes), pack_shifts(pack_shifts) {}

    void loadSeq(strtype_t seq) {
        loaded.initLoadU(seq, seq.length());
    }

   void packLoad() {
        ImmX shift_right_one = loaded >> all_ones;
        ImmX shift_right_two = loaded >> all_twos;
        packed = shift_right_one ^ shift_right_two;
        packed = packed & all_threes;
        packed = packed << pack_shifts;

        packed.saveStoreU8();
        packed.saveStoreU64();
   }

   void revComp() {
        rev_comp = packed ^ all_threes;

        rev_comp.reverseU8Stored();
        rev_comp.reverseU64Stored();        
   }

   void init(strtype_t seq) {
        loadSeq(seq);
        packLoad();
        revComp();
   }
};


#endif