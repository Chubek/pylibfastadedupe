#ifndef UTILS_H
#define UTILS_H

#define INIT_SWAP(TYPE)     TYPE temp

#define SWAP(A, B)          temp = A;\
                            A = B;  \
                            B = TEMP   

template <typename T>
void reverseArray(T *arr, seqsize_t len);
void* reallocZero(void* p_buffer, size_t old_size, size_t new_size);

#endif