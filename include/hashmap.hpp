#ifndef HASHMNAP_H
#define HASHMAP_H

template <Hashable K, typename V, typename VS>
class HashMap {
    public:
        V *arr;
        V getElementByKey(K key);
        bool is_vector;
        void insertElement(K key, VS value);
        vector<KVPair<V>> getAllKeyValues();
        vector<V&> getAllValues();
        bool containsKey(hashtype_t key);
        HashMap(bool is_vector): is_vector(is_vector) {
            arr = NULL;
            size = 0;
            next_round = 0;
        }
        ~HashMap() {
            free(arr);
        }
    private:
        vector<hashtype_t> hashes_list;
        hashtype_t size;
        hashtype_t next_round;
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