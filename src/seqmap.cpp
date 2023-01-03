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


HashMap::HashMap(std::vector<std::reference_wrapper<Sequence>> ref_seqs): ref_seqs(ref_seqs) {}

void HashMap::mapValues()
{
    for (auto seq: ref_seqs) {
        auto s = seq.get();
        auto hash = s.hashSelf();

        if (!containsKey(hash)) {
            std::vector<seqsize_t> v_bin;
            v_bin.push_back(s.index_in_array);

            map[hash] = v_bin;
            hashes_list.push_back(hash);
        } else {
            map[hash].push_back(s.index_in_array);
        }
    }
}


bool HashMap::containsKey(hashtype_t key)
{
    return find(hashes_list.begin(), hashes_list.end(), key) != hashes_list.end();
}

void Sequence::setSequence()
{
    std::string s = raw_seq;
    seq = s;
}

Sequence::Sequence(chartype_t *seqin, int index_in_array, std::shared_ptr<OutArray*> out_array)
    : out_array(out_array)
{
    raw_seq = seqin;
    index_in_array = index_in_array;
    setSequence();
    initPacked();
}

bool Sequence::compareHammingWith(Sequence &other)
{
    int diff = packed.hammingWith(other.packed, lookup_num_diffs);

    if (diff <= 1)
    {
        auto out = *out_array.get();
        out->setArrAt(index_in_array, other.index_in_array);
        is_dup = true;

        return true;
    }

    return false;
}

bool Sequence::revCompcompareHammingWith(Sequence &other)
{
    ImmX rev_comp = packed;
    ImmX other_rev_comp = other.packed;
    rev_comp.reverseU64Stored();
    other_rev_comp.reverseU64Stored();
    int diff_revcomp = rev_comp.hammingWith(other_rev_comp, lookup_num_diffs);

    if (diff_revcomp <= 1)
    {
        auto out = *out_array.get();
        out->setArrRCAt(index_in_array, other.index_in_array);
        is_dup = true;

        return true;
    }

    return false;
}

hashtype_t Sequence::hashSelf()
{
    hashtype_t hash1 = 0;
    hashtype_t hash2 = 0;

    uint64_t first_letters = packed.stored_u64[0];
    uint64_t last_letters = packed.stored_u64[SIZE_IMM_EXPANDED_U64 - 1];

    std::string substr = seq.substr(seq.length() / 3, 3 * (seq.length() / 4));

    hash1 = (~first_letters + (first_letters << 21)) & last_letters;                  
    hash1 = hash1 ^ hash1 >> 24;                          
    hash1 = ((hash1 + (hash1 << 3)) + (hash1 << 8)) & last_letters; 
    hash1 = hash1 ^ hash1 >> 14;                          
    hash1 = ((hash1 + (hash1 << 2) + (hash1 << 4))) & last_letters; 
    hash1 = hash1 ^ hash1 >> 28;                          
    hash1 = (hash1 + (hash1 << 31)) & last_letters; 
    
    
    hash2 = 5381;                      
    for (int i = 0; i < substr.length();  i++)   
            hash2 = ((hash2 << 5) + hash2) + substr[i]; 

    hashtype_t hash2_shifted = hash2 >> 32;
    hashtype_t final_hash = hash1 ^ hash2_shifted;

    return final_hash;
}

void Node::insertIntoNode(std::reference_wrapper<Sequence> seq) {
    seq_vec.push_back(seq);
}

Sequence& Sequence::getSelf() {
    return *this;
}

void Sequence::initPacked() {
    packed = getPacked(seq);
}