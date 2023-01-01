#include "../include/fastadedupe.hpp"

std::vector<Sequence> convertSequences(chartype_t **seqs_char, int size, OutArray &out_arr)
{
    std::vector<Sequence> seqs_vec;

    for (int i = 0; i < size; i++)
    {
      //  auto curr_seq = Sequence(const_cast<chartype_t *>(seqs_char[i]), i, out_arr);
        //seqs_vec.push_back(curr_seq);
    }

    return seqs_vec;
}

HashMap clusterSequences(std::vector<Sequence> &seqs)
{
    auto hashmap = HashMap();

    for (int i = 0; i < seqs.size(); i++)
    {
        hashmap.insertElement(seqs[i], seqs[i]);
    }

    return hashmap;
}

void fastadedupe(chartype_t *seqs[], int size, int out[], int out_revcomp[], int num_threads, int num_workers, int do_revcomp)
{
    std::cout << "Initiated libfastadedupe through Python interface...";
    auto out_arr = OutArray(out, out_revcomp, size);
    std::shared_ptr<OutArray> out_arr_ptr = std::make_shared<OutArray>(out_arr);

    std::cout << "Converting seqs to usable form...";
    //auto seqs_vec = convertSequences(seqs, size, out_arr);
    
    std::vector<Sequence> seqs_vec;

    for (int i = 0; i < size; i++)
    {
        auto curr_seq = Sequence(seqs[i], i, out_arr_ptr);
        seqs_vec.push_back(curr_seq);
    }

    
    std::cout << "Clustering seqs...";
  //  auto hashmap = clusterSequences(seqs_vec);

    auto hashmap = HashMap();

    for (int i = 0; i < size; i++)
    {
        hashmap.insertElement(seqs_vec[i], seqs_vec[i]);
    }

    std::vector<std::reference_wrapper<Node>> vec_ref_vals;

    for (hashtype_t key : hashmap.hashes_list)
    {
        std::reference_wrapper<Node> rwrapper = std::reference_wrapper<Node>(hashmap.map.at(key));
        vec_ref_vals.push_back(rwrapper);
    }


    std::cout << "Getting minirmd-style kmers and doing hamming...";
    assignClustersToThreads(vec_ref_vals, num_threads, num_workers, (bool)do_revcomp);

    std::cout << "Found " << out_arr.dups << " dups.";

    if (do_revcomp)
    {
        std::cout << "Found " << out_arr.dups_revcomp << " revcomp dups.";
    }
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