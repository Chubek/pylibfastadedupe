#include "../include/fastadedupe.hpp"


void fastadedupe(chartype_t *seqs[], int size, int out[], int out_revcomp[], int num_threads, int num_workers, int do_revcomp)
{
    std::cout << "Initiated libfastadedupe through Python interface...";
    auto out_arr = OutArray(out, out_revcomp, size);
    std::shared_ptr<OutArray*> out_arr_ptr = std::make_shared<OutArray*>(&out_arr);

    std::cout << "Converting seqs to usable form...\n";
    
    std::vector<Sequence> seqs_vec;

    for (int i = 0; i < size; i++)
    {
        auto curr_seq = Sequence(seqs[i], i, out_arr_ptr);
        curr_seq.index_in_array = i;
        curr_seq.is_dup = false;
        curr_seq.initPacked();
        seqs_vec.push_back(curr_seq);
    }
    
    std::cout << "Clustering seqs...\n";

    std::vector<std::reference_wrapper<Sequence>> ref_seqs;

    for (int i = 0; i < seqs_vec.size(); i++) {
        auto ref = std::reference_wrapper<Sequence>(seqs_vec[i]);
        ref_seqs.push_back(ref);
    }

    auto hashmap = HashMap(ref_seqs);
    
    std::vector<std::reference_wrapper<Node>> vec_ref_vals;

   
    std::cout << "Getting minirmd-style kmers and doing hamming...\n";
    assignClustersToThreads(hashmap, num_threads, num_workers, (bool)do_revcomp);

    std::cout << "Found " << out_arr.dups << " dups. ";

    if (do_revcomp)
    {
        std::cout << "Found " << out_arr.dups_revcomp << " revcomp dups.";
    }

    std::cout << "\n";
}

OutArray::OutArray(int *out_array_main, int *out_array_revcomp, int size) : out_array_main(out_array_main), out_array_revcomp(out_array_revcomp), size(size)
{
    dups = dups_revcomp = 0;
    this_lock = new std::mutex();
};

void OutArray::setArrAt(int index, int value)
{
    this_lock->lock();
    out_array_main[index] = value;
    dups++;
    this_lock->unlock();
}

void OutArray::setArrRCAt(int index, int value)
{
    this_lock->lock();
    out_array_revcomp[index] = value;
    dups_revcomp++;
    this_lock->unlock();
}

extern "C"
{
    extern void pylibfastadedupe_ffi(char *seqs[], int size[1], int out[], int out_revcomp[], int num_threads[1], int num_workers[1], int do_revcomp[1])
    {
        fastadedupe(seqs, size[0], out, out_revcomp, num_threads[0], num_workers[0], do_revcomp[0]);
    }
}