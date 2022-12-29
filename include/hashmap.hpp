#ifndef HASHMNAP_H
#define HASHMAP_H

template <Hashable K, typename V>
class HashMap {
    public:
        V *arr;
        V getElementByKey(K key);
        void insertElement(K key, V value);
        vector<KVPair<V>> getAllKeyValues();
        HashMap();
        ~HashMap();
    private:
        vector<keytype_t> hashes_list;
        keytype_t size;
        keytype_t next_round;
        void resizeArrayToNextRound();
};

void nextRound(size_t& curr_size) {
    curr_size--;
    curr_size |= curr_size >> 1;
    curr_size |= curr_size >> 2;
    curr_size |= curr_size >> 4;
    curr_size |= curr_size >> 8;
    curr_size++;
};

#endif