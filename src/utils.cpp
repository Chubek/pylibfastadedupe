#include "../include/fastadedupe.hpp"

template <typename T>
void reverseArray(T *arr, seqsize_t len) {
    INIT_SWAP(T);

    for (int i = 0; i < len; i++) {
        SWAP(arr[i], arr[len - i - 1]);
    }
}

void* reallocZero(void* p_buffer, size_t old_size, size_t new_size) {
  void* p_new = realloc(p_buffer, new_size);
  if ( new_size > old_size && p_new ) {
    size_t diff = new_size - old_size;
    void* p_start = ((char*)p_new) + old_size;
    memset(p_start, 0, diff);
  }
  return p_new;
}
