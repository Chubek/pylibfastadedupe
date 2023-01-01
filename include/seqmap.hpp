#ifndef SEQMAP_H
#define SEQMAP_H

class Sequence
{
public:
    chartype_t *raw_seq;
    strtype_t seq;
    size_t len;
    PackedKmer packed;
    std::shared_ptr<OutArray> out_array;
    int index_in_array;
    bool is_dup;
    hashtype_t hashSelf();
    Sequence(chartype_t *seqin, int index_in_array, std::shared_ptr<OutArray> out_array);
    bool compareHammingWith(Sequence &other);
    bool revCompcompareHammingWith(Sequence &other);
    Sequence& getSelf();

private:
    void setSequence();
    void packAndRevComp();
};

struct Node {
    std::vector<Sequence> seq_vec;
    int size;

    void insertIntoNode(Sequence seq); 
};

class HashMap
{
public:
    std::unordered_map<hashtype_t, Node> map;
    void insertElement(Sequence key, Sequence value);
    std::vector<std::reference_wrapper<Node>> getAllValues();
    std::vector<hashtype_t> hashes_list;

    HashMap();

private:
    bool containsKey(hashtype_t key);
};

#endif