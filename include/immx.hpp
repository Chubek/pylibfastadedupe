#ifndef IMMX_H
#define IMMX_H

struct ImmX
{
    immx_t loaded;
    stored_u8_t stored_u8;
    stored_u64_t stored_u64;


    ImmX operator^(ImmX const &against);
    ImmX operator&(ImmX const &against);
    ImmX operator>>(int immi);
    ImmX operator<<(int immi);

    void initZero();
    void initOne(uint8_t imm);
    void initR64(uint64_t one, uint64_t two, uint64_t three, uint64_t four);
    void initLoadU(strtype_t &str, seqsize_t size);
    void initLoadU64(uint64_t *arr, seqsize_t size);
    void saveStoreU8();
    void saveStoreU64();
    void reverseU64Stored();
    int hammingWith(ImmX &against, const uint8_t *ones_lut);
    ImmX();
    ImmX(bool all_threes);
};

struct PackedKmer
{
    ImmX loaded;
    ImmX packed;
    ImmX rev_comp;
    ImmX& all_threes;
    ImmX shift_left_zero;
    ImmX shift_left_two;
    ImmX shift_left_four;
    ImmX shift_left_six;

    PackedKmer();
    void loadSequence(strtype_t seq);
    void storeAll();
    void packLoad();
    void packFin();
    void revComp();
    void init(strtype_t seq);
};

ImmX getPacked(strtype_t &str);

#endif