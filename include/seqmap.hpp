#ifndef SEQMAP_H
#define SEQMAP_H

class Sequence
{
public:
    chartype_t *raw_seq;
    strtype_t seq;
    size_t len;
    ImmX packed;
    std::shared_ptr<OutArray*> out_array;
    seqsize_t index_in_array;
    bool is_dup;
    hashtype_t hashSelf();
    Sequence(chartype_t *seqin, int index_in_array, std::shared_ptr<OutArray*> out_array);
    bool compareHammingWith(Sequence &other);
    bool revCompcompareHammingWith(Sequence &other);
    Sequence& getSelf();
    void initPacked();

private:
    void setSequence();
};

struct Node {
    std::vector<std::reference_wrapper<Sequence>> seq_vec;
    int size;

    void insertIntoNode(std::reference_wrapper<Sequence> seq); 
};

class HashMap
{
public:
    std::unordered_map<hashtype_t, std::vector<seqsize_t>> map;
    std::vector<std::reference_wrapper<Sequence>> ref_seqs;
    void mapValues();
    std::vector<hashtype_t> hashes_list;

    HashMap(std::vector<std::reference_wrapper<Sequence>> ref_seqs);

private:
    bool containsKey(hashtype_t key);
};

#endif