#ifndef CONCURRENT_H
#define CONCURRENT_H

#define SLEEP_THREAD std::this_thread::sleep_for(std::chrono::milliseconds(500))

struct MainQueue
{
    std::queue<std::vector<seqsize_t>> q;
    std::mutex *this_lock;

    MainQueue();

    std::vector<seqsize_t> pop();
    void push(std::vector<seqsize_t> s);
    bool isEmpty();
};


void runWorkerThread(std::reference_wrapper<MainQueue> queue_ref, std::vector<std::reference_wrapper<Sequence>> ref_seqs, bool do_revcomp);
void runBranchThread(std::vector<std::reference_wrapper<MainQueue>> &queue, const HashMap &map, int start, int end);

bool checkThreadNumExceeds(int num_workers, int num_threads);
void assignClustersToThreads(HashMap &map, uint32_t num_threads_given, uint32_t num_workers_given, bool do_revcomp);

#endif