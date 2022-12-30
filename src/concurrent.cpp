#include "../include/fastadedupe.hpp"

void assignClustersToThreads(HashMap<Seq&, vector<Seq&>, Seq&> hashmap, uint32_t num_threads, uint32_t num_workers, bool do_revcomp) {
    bool exceeds_num_threads = checkThreadNumExceeds(num_workers, num_threads);

    if (exceeds_num_threads) {
        cout << "Error: number of total threads: `" << (num_threads + num_workers + 1)  << "` exceeds maximum number of threads in your implementation of `" << thread::hardware_concurrency() << "`.";
        exit(1);
    }
    
    auto hm_values = hashmap.getAllValues();
    MainQueue queue;
    vector<thread> threads_main;
    vector<thread> threads_worker;
    vector<BranchThread> main_threads_objs;
    vector<WorkerThread> worker_threads_objs;


    for (int i = 0; i < hm_values.size(); i += num_threads) {
        vector<vector<Seq&>&> sub_vec;

        for (int j = i; i < j + num_threads; j++) {
            sub_vec.push_back(hm_values[j]);
        }

        BranchThread curr_brancher(sub_vec, queue);
        main_threads_objs.push_back(curr_brancher);
    }

    for (int i = 0; i < num_workers; i++) {
        WorkerThread curr_worker(queue, do_revcomp);
        worker_threads_objs.push_back(curr_worker);
    }

    for (int i = 0; i < num_threads; i++) {
        threads_main.push_back(main_threads_objs[i].spwanThread());
    }

    for (int i = 0; i < num_workers; i++) {
        threads_worker.push_back(worker_threads_objs[i].spwanThread());
    }

    std::for_each(threads_worker.begin(), threads_worker.end(), [](thread thrd) { thrd.join(); });
    std::for_each(threads_main.begin(), threads_main.end(), [](thread thrd) { thrd.join(); });

}

bool checkThreadNumExceeds(int num_workers, int num_threads) {
    return (num_workers + num_threads + 1) > thread::hardware_concurrency();
}