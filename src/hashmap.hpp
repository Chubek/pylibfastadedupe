#include "../include/fastadedupe.hpp"

template <Hashable K, typename V, typename VS>
void HashMap<K, V, VS>::resizeArrayToNextRound() {
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

template <Hashable K, typename V, typename VS>
void HashMap<K, V, VS>::insertElement(K key, VS value) {
    hashtype_t hash = key.hashSelf();

    if (hash > size) {
        size = hash;
        resizeArrayToNextRound();
    }

    switch (is_vector) {
        case true:
            if containsKey(hash - 1) {
                arr[hash - 1].push_back(value);
            } else {
                vector<V> v;
                v.push_back(value);

                arr[hash - 1] = v;
                hashes_list.push_back(hash - 1);
            }
            
            break;
        case false:
            arr[hash - 1] = value;

            if (!containsKey(hash - 1)) {
                hashes_list.push_back(hash - 1);
            }

            break;
    }
}


template <Hashable K, typename V, typename VS>
V HashMap<K, V, VS>::getElementByKey(K key) {
    hashtype_t hash = key.hashSelf();

    if (hash > size) {
        cout << "Error: Element is not contained in hashmap.";
        exit(139);
    }

    return arr[hash - 1];
}


template <Hashable K, typename V, typename VS>
vector<KVPair<V>> HashMap<K, V, VS>::getAllKeyValues() {
    vector<KVPair<V>> all_vector;
 
    for (int i = 0; i < size; i++) {
        all_vector.push_back(KVPair<V>{ hashes_list[i], arr[hashes_list[i]] });
    }

    return all_vector;
}

template <Hashable K, typename V, typename VS>
vector<V&> HashMap<K, V, VS>::getAllValues() {
    vector<V&> vec_ref_vals;

    for (hashtype_t key: hashes_list) {
        vec_ref_vals.push_back(arr[key]);
    }

    return vec_ref_vals;
}

template <Hashable K, typename V, typename VS>
bool HashMap<K, V, VS>::containsKey(hashtype_t key) {
    return find(hashes_list.begin(), hashes_list.end(), key) != hashes_list.end();
}