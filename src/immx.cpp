#include "../include/fastadedupe.hpp"

ImmX ImmX::operator^(ImmX const &against)
{
    ImmX res_imm;

    for (int i = 0; i < SIZE_IMM; i++)
    {
        res_imm.loaded[i] = _mm256_xor_si256(loaded[i], against.loaded[i]);
    }

    return res_imm;
}

ImmX ImmX::operator&(ImmX const &against)
{
    ImmX res_imm;

    for (int i = 0; i < SIZE_IMM; i++)
    {
        res_imm.loaded[i] = _mm256_and_si256(loaded[i], against.loaded[i]);
    }

    return res_imm;
}

ImmX ImmX::operator>>(ImmX const &against)
{
    ImmX res_kmer;

    for (int i = 0; i < SIZE_IMM; i++)
    {
        res_kmer.loaded[i] = _mm256_srlv_epi64(loaded[i], against.loaded[i]);
    }

    return res_kmer;
}

ImmX ImmX::operator<<(ImmX const &against)
{
    ImmX res_kmer;

    for (int i = 0; i < SIZE_IMM; i++)
    {
        res_kmer.loaded[i] = _mm256_sllv_epi64(loaded[i], against.loaded[i]);
    }

    return res_kmer;
}

void ImmX::initOne(uint8_t imm)
{
    for (int i = 0; i < SIZE_IMM; i++)
    {
        loaded[i] = _mm256_set1_epi8(imm);
    }
}

void ImmX::initR64(uint64_t one, uint64_t two, uint64_t three, uint64_t four)
{
    for (int i = 0; i < SIZE_IMM; i++)
    {
        loaded[i] = _mm256_setr_epi64x(one, two, three, four);
    }
}

void ImmX::initLoadU(strtype_t str, seqsize_t size)
{
    chartype_t *buffer = new chartype_t[32];
    strtype_t sstr = "";

    int j = 0;
    for (int i = 0; i < size; i += 32)
    {
        sstr = str.substr(i, 32);
        strncpy(buffer, sstr.c_str(), 32);

        loaded[j++] = _mm256_loadu_si256((__m256i *)&buffer[0]);
    }
}

void ImmX::initLoadU64(uint64_t *arr, seqsize_t size)
{
    uint64_t *buffer = new uint64_t[4];

    int j = 0;
    for (int i = 0; i < size; i += 4)
    {
        memcpy(buffer, &arr[i], 4 * sizeof(uint64_t));

        loaded[j++] = _mm256_loadu_si256((__m256i *)&buffer[0]);
    }
}

void ImmX::saveStoreU64()
{
    uint64_t *buffer = new uint64_t[4];

    for (int i = 0; i < SIZE_IMM; i++)
    {
        _mm256_storeu_si256((__m256i *)&buffer, loaded[i]);
        memcpy(&stored_u64[i], buffer, 4 * sizeof(uint64_t));
    }
}

void ImmX::saveStoreU8()
{
    uint8_t *buffer = new uint8_t[32];

    for (int i = 0; i < SIZE_IMM; i++)
    {
        _mm256_storeu_si256((__m256i *)&buffer, loaded[i]);
        memcpy(&stored_u8[i], buffer, 32 * sizeof(uint8_t));
    }
}

void ImmX::packU8ToU64()
{
    int j = 0;
    for (int i = 0; i < SIZE_IMM_EXPANDED_U64; i += 4)
    {
        packed_u64[j++] = stored_u64[i] | stored_u64[i + 1] | stored_u64[i + 2] | stored_u64[i + 3];
    }
}

void ImmX::reverseU64Packed()
{
    auto swap = [](uint64_t &a, uint64_t &b)
    {
        uint64_t tmp = a;
        a = b;
        b = a;
    };

    for (int i = 0; i < SIZE_IMM_PACKED; i++)
    {
        swap(packed_u64[i], packed_u64[SIZE_IMM_PACKED - i - 1]);
    }
}

int ImmX::hammingWith(ImmX &against, const uint8_t *ones_lut)
{
    ImmX packed_imm_a;
    ImmX packed_imm_b;

    packed_imm_a.initLoadU64(packed_u64, SIZE_IMM_PACKED);
    packed_imm_b.initLoadU64(against.packed_u64, SIZE_IMM_PACKED);

    ImmX xord = packed_imm_a ^ packed_imm_b;
    xord.saveStoreU8();

    int diff = 0;
    for (int i = 0; i < SIZE_IMM_PACKED; i++)
    {
        diff += ones_lut[xord.stored_u8[i]];
    }

    return diff;
}

ImmX::ImmX() {}

PackedKmer::PackedKmer() {
    all_ones.initOne(1);
    all_twos.initOne(2);
    all_threes.initOne(3);
    pack_shifts.initR64(2, 4, 6, 8);
}

void PackedKmer::loadSequence(strtype_t seq)
{
    loaded.initLoadU(seq, seq.length());
}

void PackedKmer::packLoad()
{
    ImmX shift_right_one = loaded >> all_ones;
    ImmX shift_right_two = loaded >> all_twos;
    packed = shift_right_one ^ shift_right_two;
    packed = packed & all_threes;
    packed = packed << pack_shifts;

    packed.saveStoreU8();
    packed.saveStoreU64();
    packed.packU8ToU64();
}

void PackedKmer::revComp()
{
    rev_comp = packed ^ all_threes;

    rev_comp.reverseU64Packed();
}

void PackedKmer::init(strtype_t seq)
{
    loadSequence(seq);
    packLoad();
    revComp();
}