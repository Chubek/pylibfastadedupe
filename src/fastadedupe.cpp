#include "../include/fastadedupe.hpp"

ImmX all_threes;
ImmX all_twos;
ImmX all_ones;
ImmX pack_shifts;

void initSeqGlobal() {
    all_threes.initOne(3);
    all_twos.initOne(2);
    all_ones.initOne(1);
    pack_shifts.initR64(2, 4, 6, 8);
}

vector<Seq> convertSeqs(chartype_t **seqs_char, int size, OutArray& out) {
    vector<Seq> seqs_vec;

    for (int i = 0; i < size; i++) {
        Seq curr_seq(seqs_char[i], i, out, all_ones, all_twos, all_threes, pack_shifts);
        seqs_vec.push_back(curr_seq);    
    }

    return seqs_vec;
}

HashMap<Seq&, vector<Seq&>, Seq&> clusterSeqs(vector<Seq>& seqs) {
    HashMap<Seq&, vector<Seq&>, Seq&> hashmap(true);

    for (int i = 0; i < seqs.size(); i++) {
        hashmap.insertElement(seqs[i], seqs[i]);
    }

    return hashmap;
}

void pylibfastadedupe_interface(chartype_t **seqs, int size, int *out, int *out_revcomp, int num_threads, int num_workers, bool do_revcomp) {
    cout << "Initiated libfastadedupe through Python interface...";    
    OutArray out_arr(out, out_revcomp, size);
    
    cout << "Converting seqs to usable form...";
    auto seqs_vec = convertSeqs(seqs, size, out_arr);
    cout << "Clustering seqs...";
    auto hashmap = clusterSeqs(seqs_vec);

    cout << "Getting minirmd-style kmers and doing hamming...";
    assignClustersToThreads(hashmap, num_threads, num_workers, do_revcomp);

    cout << "Done! Results should be reported by Python.";
}