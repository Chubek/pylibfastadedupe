#include "../include/fastadedupe.hpp"

ImmX all_threes(true);

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

ImmX ImmX::operator>>(int immi)
{
    ImmX res_kmer;

    for (int i = 0; i < SIZE_IMM; i++)
    {
        res_kmer.loaded[i] = _mm256_srai_epi16(loaded[i], immi);
    }

    return res_kmer;
}

ImmX ImmX::operator<<(int immi)
{
    ImmX res_kmer;

    for (int i = 0; i < SIZE_IMM; i++)
    {
        res_kmer.loaded[i] = _mm256_slli_epi16
        
        
        
        
        (loaded[i], immi);
    }

    return res_kmer;
}

void ImmX::initZero() {
    for (int i = 0; i < SIZE_IMM; i++) {
        loaded[i] = _mm256_set1_epi8(0);
    }
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

void ImmX::initLoadU(strtype_t &str, seqsize_t size)
{
    chartype_t buffer[32];
    memset(buffer, 0, 32 * sizeof(chartype_t));

    strtype_t sstr = "";

    int j = 0;
    for (int i = 0; i < size; i += 32)
    {
        sstr = str.substr(i, 32);
        memset(buffer, 0, 32 * sizeof(chartype_t));
        strncpy(buffer, sstr.c_str(), 32);

        loaded[j++] = _mm256_loadu_si256((__m256i *)&buffer[0]);
    }
}

void ImmX::initLoadU64(uint64_t *arr, seqsize_t size)
{
    uint64_t buffer[4];
    memset(buffer, 0, sizeof(uint64_t));

    int j = 0;
    for (int i = 0; i < size; i += 4)
    {
        memset(buffer, 0, sizeof(uint64_t));
        memcpy(buffer, &arr[i], 4 * sizeof(uint64_t));

        loaded[j++] = _mm256_loadu_si256((__m256i *)&buffer[0]);
    }  
}

void ImmX::saveStoreU8()
{
    uint8_t buffer[32];
    memset(buffer, 0, 32 * sizeof(uint8_t));

    int j = 0;
    for (int i = 0; i < SIZE_IMM; i++)
    {
        memset(buffer, 0, 32 * sizeof(uint8_t));
        _mm256_storeu_si256((__m256i *)&buffer, loaded[i]);
        memcpy(&stored_u8[j], buffer, 32 * sizeof(uint8_t));

        j += 32;
    }
}

void ImmX::saveStoreU64()
{
    uint64_t buffer[4];
    memset(buffer, 0, 4 * sizeof(uint8_t));
    
    int j = 0;
    for (int i = 0; i < SIZE_IMM; i++)
    {
        memset(buffer, 0, 4 * sizeof(uint8_t));
        _mm256_storeu_si256((__m256i *)&buffer, loaded[i]);
        memcpy(&stored_u64[j], buffer, 4 * sizeof(uint64_t));

        j += 4;
    }
}



void ImmX::reverseU64Stored()
{
    auto swap = [](uint64_t &a, uint64_t &b)
    {
        uint64_t tmp = a;
        a = b;
        b = a;
    };

    for (int i = 0; i < SIZE_IMM_EXPANDED_U64; i++)
    {
        swap(stored_u64[i], stored_u64[SIZE_IMM_EXPANDED_U64 - i - 1]);
    }
}

int ImmX::hammingWith(ImmX &against, const uint8_t *ones_lut)
{
    ImmX packed_imm_a;
    ImmX packed_imm_b;

    packed_imm_a.initLoadU64(stored_u64, SIZE_IMM_EXPANDED_U64);
    packed_imm_b.initLoadU64(against.stored_u64, SIZE_IMM_EXPANDED_U64);

    ImmX xord = packed_imm_a ^ packed_imm_b;
    xord.saveStoreU8();

    int diff = 0;
    for (int i = 0; i < SIZE_IMM_EXPANDED_U8; i++)
    {
        diff += ones_lut[xord.stored_u8[i]];
    }

    return diff;
}

ImmX::ImmX() {
    memset(stored_u64, 0, SIZE_IMM_EXPANDED_U64 * sizeof(uint64_t));
    memset(stored_u8, 0, SIZE_IMM_EXPANDED_U8 * sizeof(uint8_t));

    initZero();
}

ImmX::ImmX(bool all_threes) {
    this->initOne(3);
}

PackedKmer::PackedKmer():  all_threes(all_threes) { }

void PackedKmer::loadSequence(strtype_t seq)
{
    loaded.initLoadU(seq, seq.length());
}

void PackedKmer::packLoad()
{
    ImmX shift_right_one = loaded >> 1;
    ImmX shift_right_two = loaded >> 2;
    shift_left_zero = shift_right_one ^ shift_right_two;
    shift_left_zero = shift_left_zero & all_threes;
    shift_left_two = shift_left_zero << 2;
    shift_left_four = shift_left_zero << 4;
    shift_left_six = shift_left_zero << 6;

    storeAll();    
}

void PackedKmer::storeAll() {
    shift_left_zero.saveStoreU8();
    shift_left_two.saveStoreU8();
    shift_left_four.saveStoreU8();
    shift_left_six.saveStoreU8();
}

void PackedKmer::revComp()
{
    rev_comp = packed ^ all_threes;

    rev_comp.reverseU64Stored();
}

void PackedKmer::init(strtype_t seq)
{
    loadSequence(seq);
    packLoad();
    packFin();
    revComp();
}

void PackedKmer::packFin() {
    uint64_t arr_fin[SIZE_IMM_PACKED];
    memset(arr_fin, 0, SIZE_IMM_PACKED * sizeof(uint64_t));

    int j = 0;
    for (int i = 0; i < SIZE_IMM_EXPANDED_U8; i += 32) {
        uint64_t or_all = 0;

        for (int k = i; k < i + 32; k += 4) {
            uint8_t curr_or = shift_left_zero.stored_u8[i + k] |
                            shift_left_two.stored_u8[i + k + 1] |
                            shift_left_four.stored_u8[i + k + 2] |
                            shift_left_six.stored_u8[i + k + 3];

            or_all <<= 8;
            or_all |= curr_or;
        }
        
        arr_fin[j++] = or_all;
    }

    packed.initLoadU64(arr_fin, SIZE_IMM_PACKED);
    packed.saveStoreU64();
}


ImmX getPacked(strtype_t &str) {
    ImmX loaded;
    loaded.initLoadU(str, str.size());

    ImmX shift_right_one = loaded >> 1;
    ImmX shift_right_two = loaded >> 2;
    ImmX shift_left_zero = shift_right_one ^ shift_right_two;
    shift_left_zero = shift_left_zero & all_threes;
    ImmX shift_left_two = shift_left_zero << 2;
    ImmX shift_left_four = shift_left_zero << 4;
    ImmX shift_left_six = shift_left_zero << 6;

    uint64_t arr_fin[SIZE_IMM_PACKED];
    memset(arr_fin, 0, SIZE_IMM_PACKED * sizeof(uint64_t));

    int j = 0;
    for (int i = 0; i < SIZE_IMM_EXPANDED_U8; i += 32) {
        uint64_t or_all = 0;

        for (int k = i; k < i + 32; k += 4) {
            uint8_t curr_or = shift_left_zero.stored_u8[i + k] |
                            shift_left_two.stored_u8[i + k + 1] |
                            shift_left_four.stored_u8[i + k + 2] |
                            shift_left_six.stored_u8[i + k + 3];

            or_all <<= 8;
            or_all |= curr_or;
        }
        
        arr_fin[j++] = or_all;
    }

    ImmX packed;

    packed.initLoadU64(arr_fin, SIZE_IMM_PACKED);
    packed.saveStoreU64();

    return packed;
}