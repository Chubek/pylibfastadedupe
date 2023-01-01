#ifndef IMMX_H
#define IMMX_H

struct ImmX
{
    immx_t loaded;
    stored_u8_t stored_u8;
    stored_u64_t stored_u64;
    packed_u64_t packed_u64;

    ImmX operator^(ImmX const &against);
    ImmX operator&(ImmX const &against);
    ImmX operator>>(ImmX const &against);
    ImmX operator<<(ImmX const &against);

    void initOne(uint8_t imm);
    void initR64(uint64_t one, uint64_t two, uint64_t three, uint64_t four);
    void initLoadU(strtype_t str, seqsize_t size);
    void initLoadU64(uint64_t *arr, seqsize_t size);
    void saveStoreU64();
    void saveStoreU8();
    void packU8ToU64();
    void reverseU64Packed();
    int hammingWith(ImmX &against, const uint8_t *ones_lut);
    ImmX();
};

struct PackedKmer
{
    ImmX loaded;
    ImmX packed;
    ImmX rev_comp;
    ImmX all_ones;
    ImmX all_twos;
    ImmX all_threes;
    ImmX pack_shifts;

    PackedKmer();
    void loadSequence(strtype_t seq);
    void packLoad();
    void revComp();
    void init(strtype_t seq);
};


#endif