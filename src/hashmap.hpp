#include "../include/fastadedupe.hpp"

template <Hashable K, typename V>
HashMap<K, V>::HashMap() {
    arr = NULL;
    size = 0;
    next_round_32 = 0;
}

template <Hashable K, typename V>
HashMap<K, V>::~HashMap() {
    free(arr);
}

template <Hashable K, typename V>
void HashMap<K, V>::resizeArrayToNextRound() {
    size_t new_round = nextRound(size);

    if (new_round > next_round_32) {
        next_round = new_round;

        size_t old_size = size * sizeof(T);
        size_t new_size = next_round * sizeof(T);

        T *nptr = (T *)reallocZero(arr, old_size, new_size);

        if (!nptr) {
            cout << "Error reallocating hashmap array.";
            exit(139);
        }

        arr = nptr;
    }
}

template <Hashable K, typename V>
void HashMap<K, V>::insertElement(K key, V value) {
    keytype_t hash = key.hashSelf();

    if (hash > size) {
        size = hash;
        resizeArrayToNextRound();
    }

    arr[hash - 1] = value;
    hashes_list.push_back(hash - 1);
}


template <Hashable K, typename V>
V HashMap<K, V>::getElementByKey(K key) {
    keytype_t hash = key.hashSelf();

    if (hash > size) {
        cout << "Error: Element is not contained in hashmap.";
        exit(139);
    }

    return arr[hash - 1];
}


template<Hashable K, typename V>
vector<KVPair<V>> HashMap<K, V>::getAllKeyValues() {
    vector<KVPair<V>> all_vector;
 
    for (int i = 0; i < size; i++) {
        all_vector.push_back(KVPair<V>{ hashes_list[i], arr[hashes_list[i]] });
    }

    return all_vector;
}