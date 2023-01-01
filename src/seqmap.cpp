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


HashMap::HashMap() { }

void HashMap::insertElement(Sequence key, Sequence value)
{
    hashtype_t hash = key.hashSelf();

    if (!containsKey(hash)) {
        Node node;
        node.insertIntoNode(value);
        map[hash] = node; 
        hashes_list.push_back(hash);

        return;
    }   

    map.at(hash).insertIntoNode(value);   
}

std::vector<std::reference_wrapper<Node>> HashMap::getAllValues()
{
    std::vector<std::reference_wrapper<Node>> vec_ref_vals;

    for (hashtype_t key : hashes_list)
    {
        std::reference_wrapper<Node> rwrapper = std::reference_wrapper<Node>(map.at(key));
        vec_ref_vals.push_back(rwrapper);
    }

    return vec_ref_vals;
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

Sequence::Sequence(chartype_t *seqin, int index_in_array, std::shared_ptr<OutArray> out_array)
    : out_array(out_array)
{
    raw_seq = seqin;
    index_in_array = index_in_array;
    setSequence();
}

void Sequence::packAndRevComp()
{
    packed.init(seq);
}

bool Sequence::compareHammingWith(Sequence &other)
{
    int diff = packed.packed.hammingWith(other.packed.packed, lookup_num_diffs);

    if (diff < 1)
    {
        out_array.get()->setArrAt(index_in_array, other.index_in_array);
        is_dup = true;

        return true;
    }

    return false;
}

bool Sequence::revCompcompareHammingWith(Sequence &other)
{
    int diff_revcomp = packed.rev_comp.hammingWith(other.packed.rev_comp, lookup_num_diffs);

    if (diff_revcomp < 1)
    {
        out_array.get()->setArrRCAt(index_in_array, other.index_in_array);
        is_dup = true;

        return true;
    }

    return false;
}

hashtype_t Sequence::hashSelf()
{
    hashtype_t hash1 = 0;
    hashtype_t hash2 = 0;

    uint64_t first_letters = packed.packed.stored_u64[0];
    uint64_t last_letters = packed.packed.stored_u64[SIZE_IMM_EXPANDED_U64 - 1];

    std::string substr = seq.substr(seq.length() / 3, 3 * (seq.length() / 4));

    HASH64(first_letters, hash1, last_letters);
    HASHSTR(substr, hash2);

    hashtype_t hash2_shifted = hash2 >> 32;
    hashtype_t final_hash = hash1 ^ hash2_shifted;

    return MASKU64_32(final_hash);
}

void Node::insertIntoNode(Sequence seq) {
    seq_vec.push_back(seq);
}

Sequence& Sequence::getSelf() {
    return *this;
}